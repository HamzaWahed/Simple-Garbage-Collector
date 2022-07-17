#include <stdio.h>
#include <stdlib.h>

#define STACK_MAX 256
#define INITIAL_GC_THRESHOLD 12

typedef enum
{
    OBJ_INT,
    OBJ_PAIR
} ObjectType;

typedef struct sObject
{
    unsigned char marked;
    ObjectType type;
    struct sObject* next;

    union 
    {
        int value;

        struct
        {
            struct sObject* head;
            struct sObject* tail;
        };
    };
} Object;

typedef struct
{
    int numObjects;
    int maxObjects;
    Object* stack[STACK_MAX];
    Object* firstObject;
    int stackSize;
} VM;

VM* newVM()
{
    VM* vm = malloc(sizeof(VM));
    vm->stackSize=0;
    vm->numObjects=0;
    vm->firstObject = NULL;
    vm->maxObjects=INITIAL_GC_THRESHOLD;
    return vm;
}

void push(VM* vm, Object* value)
{
    assert(vm->stackSize < STACK_MAX, "Stack overflow!");
    vm->stack[vm->stackSize++]=value;
}

Object* pop(VM* vm)
{
    assert(vm->stackSize > 0, "Stack underflow!");
    return vm->stack[--vm->stackSize];
}

Object* newObject(VM* vm, ObjectType type)
{
    if(vm->numObjects == vm->maxObjects) gc(vm);

    Object* object = malloc(sizeof(Object));
    object->type = type;
    object->marked = 0;

    object->next = vm->firstObject;
    vm->firstObject = object;
    vm->numObjects++;
    return object;
}

void pushInt(VM* vm, int intValue)
{
    Object* object = newObject(vm, OBJ_INT);
    object->value = intValue;
    push(vm,object);
}

Object* pushPair(VM* vm)
{
    Object* object = newObject(vm, OBJ_PAIR);
    object->tail = pop(vm);
    object->head = pop(vm);

    push(vm,object);
    return object;
}

void markAll(VM* vm)
{
    for(int i=0;i<vm->stackSize;i++)
    {
        mark(vm->stack[i]);
    }
}

void mark(Object* object)
{
    if (object->marked) return;

    object->marked = 1;

    if(object->type == OBJ_PAIR)
    {
        mark(object->head);
        mark(object->tail);
    }
}

void sweep(VM* vm)
{
    Object** object = &vm->firstObject;
    while(*object){
        if(!((*object)->marked)){
            Object* unreached = *object;

            *object = unreached->next;
            free(unreached);
            vm->numObjects--;
        }else{
            (*object)->marked=0;
            object = &(*object)->next;
        }
    }
}

void gc(VM* vm)
{
    int numObjects = vm->numObjects;

    markAll(vm);
    sweep(vm);

    vm->maxObjects = vm->numObjects * 2;
}