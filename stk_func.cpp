#define TWO_LVL_PROTECT
#include "stk_header.h"
#include <math.h>

void StackPush (Stack* stk, const data_type value)
{
    CHECK_STK_;
   
    if (stk->size == stk->capacity)

        StackResize (stk, 2 * stk->capacity, UPPER_MODE);

    *(stk->data + stk->size++) = value;

    #ifdef TWO_LVL_PROTECT

        stk->hash = MurMurHash(stk);

    #endif

    CHECK_STK_;
}

int StackPop (Stack *stk)
{
    CHECK_STK_;

    if (stk->size <= 0)
        exit(STACK_UNDERFLOW);


    stk->size--;

    if (stk->size * 2 <= stk->capacity)
        StackResize (stk, stk->capacity / 2, UNDER_MODE);
    

    #ifdef TWO_LVL_PROTECT

        stk->hash = MurMurHash(stk);
        
    #endif

    CHECK_STK_;

    return stk->data[stk->size];
}

void StackCtor (Stack* stk, const int capacity)  
{
    assert(stk->data == NULL);

    stk->capacity = capacity;
    stk->size = 0;

    #ifdef NO_PROTECT

        stk->data = (data_type*) calloc(1, stk->capacity * sizeof(data_type));
    
    #else 

        stk->data_canary = (char*) calloc(1, stk->capacity * sizeof(data_type) + 2 * sizeof(can_type));
        stk->data = (data_type*)(stk->data_canary + sizeof(can_type));

        stk->left_stk_can  = const_l_stk_canary;
        stk->right_stk_can = const_r_stk_canary;

        LEFT_DATA_CAN_ = const_l_stk_canary;
        RIGHT_DATA_CAN_ = const_r_stk_canary;
    
    #endif

    #ifdef TWO_LVL_PROTECT

       stk->hash = MurMurHash(stk);

    #endif
   
    CHECK_STK_;
}

void StackDtor (Stack* stk)
{
    CHECK_STK_;

    free(stk->data_canary);
    stk->size = -1;
    stk->capacity = 0;
    stk->data = NULL;
    stk->data_canary = NULL;
}

int StackResize (Stack* stk, const int new_capacity, const int mode)
{
    CHECK_STK_;
    
    if (mode == UPPER_MODE && new_capacity <= stk->size)  
        return RESIZE_ERR;

    else if (mode == UNDER_MODE && new_capacity >= stk->size)
        return RESIZE_ERR;

    stk->capacity = new_capacity;

    #ifndef NO_PROTECT

        RIGHT_DATA_CAN_  = 0;

        stk->data_canary = (char*) realloc (stk->data_canary, new_capacity*sizeof(data_type) + 2 * sizeof(can_type));
        stk->data = (data_type*)(stk->data + sizeof(can_type));

        RIGHT_DATA_CAN_ = const_r_stk_canary;

    #endif

    #ifdef NO_PROTECT

        stk->data = (data_type*) realloc (stk->data, new_capacity*sizeof(data_type));

    #endif

    #ifdef TWO_LVL_PROTECT

       stk->hash = MurMurHash(stk);

    #endif

    CHECK_STK_;

    return 0;
}

int StackPeek (Stack* stk)
{
    CHECK_STK_;

    return stk->data[stk->size];
}

void StackDump (Stack* stk, const int line, const int error, const char* func)
{
    FILE* canary_file = fopen("canary_errors.txt", "w");
    fprintf(canary_file, "ERROR types:\n"
                         "NO ERROR              =  0\n"
                         "STACK UNDERFLOW       = -1\n"
                         "STACK OVERFLOW        = -2\n"
                         "DATA SIZE ERR         = -3\n"
                         "DATA CAPACITY ERR     = -4\n"
                         "NULL STK POINTER      = -5\n"
                         "DATA LEFT CANARY ERR  = -6\n"
                         "DATA RIGHT CANARY ERR = -7\n"
                         "STK LEFT CANARY ERR   = -8\n"
                         "STK RIGHT CANARY ERR  = -9\n"
                         "HASH ERR              = -10\n\n");

    #ifdef NO_PROTECT

        fprintf(canary_file, "<< No canary protect >>\n");

    #else 

        #ifdef TWO_LVL_PROTECT

            fprintf(canary_file, "<< Canary protection and hash protection >>\n");

        #else

            fprintf(canary_file, "<< Canary protection >>\n");

        #endif

        if (error < 0)
        {
            fprintf(canary_file, "Stack has error status type: %d on line %d in func %s\n\n", error, line, func);
            fprintf(canary_file, "*stk left canary = %X\t"
                                 " stk right canary = %X\n\n"
                                 "*data left canary = %X\t"
                                 " data right canary = %X\n\n"
                                 "*default left canary = %X\t"
                                 " default right canary = %X\n\n",
            stk->left_stk_can, stk->right_stk_can, LEFT_DATA_CAN_, RIGHT_DATA_CAN_, const_l_stk_canary, const_r_stk_canary);
        }
    #endif

    fprintf(canary_file, "*data pointer = %p; *stack size = %d; *stack capacity = %d\n\n", stk->data, stk->size, stk->capacity);
    
    #ifdef TWO_LVL_PROTECT

        fprintf(canary_file, "*stack hash = %d\n\n", stk->hash);
    
    #endif

    for (int counter = 0; counter < stk->size; counter++)
        fprintf(canary_file, "data[%d] = %d\n", counter, stk->data[counter]);

    fclose(canary_file);
}

int MurMurHash (Stack* stk)
{
    int sum = 0;

    if (stk == NULL || stk->data == NULL)
    {
        sum = 0xBADDED;
    }
    else
    {
        for (int counterj = 0; counterj < stk->size; counterj++)
        {
            for (int counteri = counterj; counteri < stk->size; counteri++)
            {
                sum += (stk->data[counteri] << counteri) * (stk->data[counterj] << counterj);
            }

            sum += (stk->data[counterj] | 10) * (counterj | 30);
        }

        sum += sqrt(stk->size * stk->size + stk->capacity * stk->capacity + stk->size * stk->capacity);
    }

    return sum;
}

int check_stk_err(Stack* stk, const int line, const char* func)
{
    const int error_status = stk_status(stk);

    if (error_status < 0)
        StackDump(stk, line, error_status, func);

    return error_status;
}

int stk_status(Stack* stk)
{
    if (stk->size <= -1)
        return DATA_SIZE_ERR;

    else if (stk == NULL)  
        return NULL_STK_POINTER;

    else if (stk->capacity <= 0)  
        return DATA_CAPACITY_ERR;

    #ifndef NO_PROTECT

        if (LEFT_DATA_CAN_ != const_l_stk_canary)   
            return DATA_LEFT_CANARY_ERR;

        else if (RIGHT_DATA_CAN_ != const_r_stk_canary)  
            return DATA_RIGHT_CANARY_ERR;

        else if (stk->left_stk_can != const_l_stk_canary)
            return STK_LEFT_CANARY_ERR;

        else if (stk->right_stk_can != const_r_stk_canary)   
            return STK_RIGHT_CANARY_ERR;

    #endif

    #ifdef TWO_LVL_PROTECT

        if (MurMurHash(stk) != stk->hash)
            return HASH_ERR;

    #endif

    return NO_ERROR;
}




/*
switch (check_stk_)
    {
        case DATA_LEFT_CANARY_ERR:
            print
            break;
        case DATA_RIGHT_CANARY_ERR:

        case STK_LEFT_CANARY_ERR:

        case STK_RIGHT_CANARY_ERR:


    }
    return;
*/

/*int check_canary (Stack* stk)
{
    if (left_data_can_ != const_l_stk_canary || 
        right_data_can_ != const_r_stk_canary || 
        stk->left_stk_can != const_l_stk_canary || 
        stk->right_stk_can != const_r_stk_canary)
    {
        return CANARY_ERROR;
    }
}
*/

/*int StackIsValid (Stack* stk)
{
    if (stk->data == NULL  || stk == NULL || stk->size <= -1)
        return 0;   
    return 1;
}*/

//FILE* canaryfile = fopen("canary_errors.txt", "w");
            //fprintf(canary_file, "CANARY ERROR on line %d", str_number)
            //printf("CANARY_ERROR\n %d", line);

/*
printf("size = %d; ", stk->size);
    printf("data_pointer = %p\n", stk->data);
    
    for (int counter = 0; counter < stk->size; counter++)
    {
        printf("%d; ", stk->data[counter]);
    }
    printf("END_OF_STK\n");
*/