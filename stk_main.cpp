#include "stk_header.h"

int main()
{
    Stack stk = {};

    StackCtor (&stk, BASE_STK_SIZE);

    StackPush (&stk, 40);
    StackPush (&stk, 50);
    stk.data[1] = 1; 
    StackPush (&stk, 60);
    StackPush (&stk, 70);
    StackPush (&stk, 80);
    

    StackDtor(&stk);

    return 0;
}