#include <stdio.h>
#include "stack.h"

int main()
{
    Stack stk1;

    stackCtor(&stk1, 5);

    for (int i = 0; i < 100; i++)
    {
        stackPush(&stk1, 7);
        printf("OU SHIT\n");

        stk1.data[0] = 13;
    }

    for (int i = 0; i < 200; i++)
    {
        Elem v = 0;
        stackPop(&stk1, &v);

    }

    for (int i = 0; i < 60; i++)
    {

        stackPush(&stk1, 9);

    }
    stackDtor(&stk1); 
}