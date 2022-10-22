#include <stdio.h>
#include "stack.h"

int main()
{
    Stack stk1;

    printf("%x\n", (unsigned(-13)));

    //double d[] = {0, 0, 0, 0, 0};

    //printf("tatat = %x\n", d[0]);

   /* for (int i = 0; i < sizeof(double); i++)
    {
        *((char*) d + i) = (char) 12;
    }

    printf("tatat = %lg\n", d[0]);*/

    stackCtor(&stk1, 5);

    //*((char*) stk1.data - sizeof(Canary)) = 1;

    //stackPop(&stk1);
    
    //stackPush(&stk1, 1);
    
    int value = 1;

    Elem val2 = 0;
    //stackPop(&stk1, &val2);

    for (int i = 0; i < 10; i++)
    {
        stackPush(&stk1, 7);
    }

    for (int i = 0; i < 20; i++)
    {
        Elem v = 0;
        stackPop(&stk1, &v);

    }

    for (int i = 0; i < 6; i++)
    {

        stackPush(&stk1, 9);

    }
    stackDtor(&stk1); 
}