#define TWO_LVL_PROTECT
#include "stk_header.h"

int main()
{
    Stack stk = {};

    StackCtor (&stk, BASE_STK_SIZE);
    //printf("%X\n", *(data_type*)stk.data);
    StackPush (&stk, 40);
    StackPush (&stk, 50);
    stk.size++; 
    StackPush (&stk, 60);
    StackPush (&stk, 70);
    StackPush (&stk, 80);

    StackDtor(&stk);

    return 0;
}