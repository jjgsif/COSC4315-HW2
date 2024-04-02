#include <cstdarg>
#include "vm.h"

static InterpretResult run();
void push(Value value);
Value pop();
Value peek(int distance);
static void resetStack();
static void runtimeError(const char *format, ...);
void initVM();
void freeVM();
void freeObjects();
void freeObject(Obj *object);

VM vm;

void compile(char *source)
{
    initScanner(source);
}

InterpretResult interpret(char *source)
{

    Chunk chunk;

    initChunk(&chunk);

    if (!compile(source, &chunk))
    {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}

void push(Value value)
{
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop()
{
    vm.stackTop--;
    return *vm.stackTop;
}

Value peek(int distance)
{
    return vm.stackTop[-1 - distance];
}

static void resetStack()
{
    vm.stackTop = vm.stack;
}

static void runtimeError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);
    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);

    resetStack();
}

void initVM()
{
    resetStack();
    vm.objects = nullptr;
}

void freeVM()
{
    freeObjects();
}

void freeObjects()
{
    Obj *object = vm.objects;
    while (object != nullptr)
    {
        Obj *next = object->next;
        freeObject(object);
        object = next;
    }
}

void freeObject(Obj *object)
{
    switch (object->type)
    {
    case OBJ_STRING:
    {
        ObjString *string = (ObjString *)object;
        FREE_ARRAY(char, string->chars, string->length + 1);
        FREE(ObjString, object);
        break;
    }
    }
}

static bool isFalsey(Value value)
{
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

void concatenate()
{
    ObjString *a, *b;
    b = AS_STRING(pop());
    a = AS_STRING(pop());

    int length = a->length + b->length;
    char *combined = ALLOCATE(char, length + 1);

    memcpy(combined, a->chars, a->length);
    memcpy(combined + a->length, b->chars, b->length);
    combined[length] = '\0';

    ObjString *result = takeString(combined, length);
    push(OBJ_VAL(result));
}

#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_SHORT() ((vm.ip += 2, (uint16_t)((vm.ip[-2] << 8) | vm.ip[-1])))
static InterpretResult run()
{

#define BINARY_OP(valueType, op)                        \
    do                                                  \
    {                                                   \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) \
        {                                               \
            runtimeError("Operands must be numbers.");  \
            return INTERPRET_RUNTIME_ERR;               \
        }                                               \
        double b = AS_NUMBER(pop());                    \
        double a = AS_NUMBER(pop());                    \
        push(valueType(a op b));                        \
    } while (false)

    for (;;)
    {
        uint8_t instruction;
        switch (instruction = READ_BYTE())
        {
        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            push(constant);
            break;
        }
        case OP_RETURN:
            return INTERPRET_OK;

        case OP_NEGATE:
            if (!IS_NUMBER(peek(0)))
            {
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERR;
            }
            push(NUMBER_VAL(-AS_NUMBER(pop())));
            break;
        case OP_TRUE:
            push(BOOL_VAL(true));
            break;
        case OP_FALSE:
            push(BOOL_VAL(false));
            break;
        case OP_EQUAL:
        {
            Value b = pop();
            Value a = pop();
            push(BOOL_VAL(valuesEqual(a, b)));
            break;
        }

        case OP_GREATER:
            BINARY_OP(BOOL_VAL, >);
            break;
        case OP_LESS:
            BINARY_OP(BOOL_VAL, <);
            break;
        case OP_ADD:
        {
            if (IS_STRING(peek(0)) && IS_STRING(peek(1)))
            {
                concatenate();
            }
            else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1)))
            {
                double b = AS_NUMBER(pop());
                double a = AS_NUMBER(pop());
                push(NUMBER_VAL(a + b));
            }
            else
            {
                runtimeError(
                    "Operands must be two numbers or two strings.");
                return INTERPRET_RUNTIME_ERR;
            }
            break;
        }
        case OP_SUBTRACT:
            BINARY_OP(NUMBER_VAL, -);
            break;
        case OP_MULTIPLY:
            BINARY_OP(NUMBER_VAL, *);
            break;
        case OP_DIVIDE:
            BINARY_OP(NUMBER_VAL, /);
            break;

        case OP_PRINT:
        {
            printValue(pop());
            printf("\n");
            break;
        }
        case OP_CONCAT:
        {
            Value b = pop();
            Value a = pop();
            printValues(a, b);
            instruction = READ_BYTE();
            cout << endl;
            break;
        }
        case OP_DEFINE_GLOBAL:
        {

            string c = AS_CSTRING(vm.chunk->constants.values[*vm.ip++]);

            try
            {

                vm.variables.at(c) = peek(0);
                // cout << "ASSIGNED " << c << ": " << AS_NUMBER(peek(0)) << endl;
                pop();
                break;
            }
            catch (exception e)
            {
                vm.variables.insert(pair<string, Value>(c, peek(0)));
                // cout << "ASSIGNED " << c << ": " << AS_NUMBER(peek(0)) << endl;
                break;
            }
        }
        case OP_GET_GLOBAL:
        {
            string v = AS_CSTRING(vm.chunk->constants.values[*vm.ip++]);
            try
            {
                push(vm.variables.at(v));
                break;
            }
            catch (exception e)
            {
                runtimeError("Undefined Variable name: ", v.c_str());
                return INTERPRET_RUNTIME_ERR;
            }
        }

        case OP_POP:
            pop();
            break;
        case OP_NOT:
            push(BOOL_VAL(isFalsey(pop())));
            break;

        case OP_JUMP_IF_FALSE:
        {
            uint16_t offset = READ_SHORT();
            if (isFalsey(peek(0)))
                vm.ip += offset;
            break;
        }

        case OP_JUMP:
        {
            uint16_t offset = READ_SHORT();
            vm.ip += offset;
            break;
        }
        }
    }
}

#undef READ_BYTE
#undef READ_CONSTANT
