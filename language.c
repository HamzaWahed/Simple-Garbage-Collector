
typedef enum
{
    OBJ_INT,
    OBJ_PAIR
} ObjectType;

typedef struct sObject
{
    ObjectType type;

    union 
    {
        int value;

        struct
        {
            struct sObject* head;
            struct sOject* tail;
        };
    };
}; Object;


