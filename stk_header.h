#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cstdint>

#ifndef NO_PROTECT

    #define LEFT_DATA_CAN_  *(can_type*)stk->data_canary
    #define RIGHT_DATA_CAN_ *(can_type*)(stk->data_canary + stk->capacity * sizeof(data_type) + sizeof(can_type))

#endif

#define CHECK_STK_  check_stk_err (stk, __LINE__, __func__)

typedef int data_type;
typedef uint64_t can_type;

const int BASE_STK_SIZE = 2;
const can_type const_r_stk_canary = 0xDEDADEAD;
const can_type const_l_stk_canary = 0xDEADBABA;

enum resize_mode {
    UNDER_MODE = -1,
    UPPER_MODE = 1
};

enum errors {
    NO_ERROR = 0,
    STACK_UNDERFLOW       = -1,
    STACK_OVERFLOW        = -2,
    DATA_SIZE_ERR         = -3,
    DATA_CAPACITY_ERR     = -4,
    NULL_STK_POINTER      = -5,
    DATA_LEFT_CANARY_ERR  = -6,
    DATA_RIGHT_CANARY_ERR = -7,
    STK_LEFT_CANARY_ERR   = -8,
    STK_RIGHT_CANARY_ERR  = -9, 
    HASH_ERR              = -10,
    RESIZE_ERR            = -11
};

typedef struct Stack_tag {
    #ifndef NO_PROTECT
        can_type left_stk_can;
    #endif

    #ifdef TWO_LVL_PROTECT
        int hash;
    #endif

    char* data_canary;
    data_type* data;
    int size;
    int capacity;


    #ifndef NO_PROTECT
        can_type right_stk_can;
    #endif
} Stack;

void       StackPush           (Stack *stk, const data_type value);
data_type  StackPop            (Stack *stk); // ToDo: data_type
data_type  StackPeek           (Stack* stk);
void       StackCtor           (Stack* stk, const int capacity );
void       StackDtor           (Stack* stk);
int        StackResize         (Stack* stk, const int new_capacity, const int mode);
void       StackDump           (Stack* stk, const int line, const int error, const char* func);
int        check_stk_err       (Stack* stk, const int line, const char* func);
data_type  MurMurHash          (Stack* stk);
int        stk_status          (Stack* stk);

