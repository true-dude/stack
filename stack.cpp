#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
//#include "ASSERT_STK.h"

int openLogFile(const char* log_file_name);
int openDumpFile(const char* dump_file_name);

#define LOGGING
#define STACK_DUMPING
#define SUPER_STACK_DUMPING
#define CANARY_PROTECT

#define LOCATION __FILE__, __LINE__, __PRETTY_FUNCTION__

#define stackCtor(stk, capacity)                                      \
_stackCtor(stk, capacity, #stk, LOCATION)

#ifdef LOGGING

    #define log_error(error)                                           \
           _log_error(error, LOCATION)

    #define log_func(error) _log_func(error, __PRETTY_FUNCTION__, __LINE__)

#else

    #define log_error(error) 

#endif

#ifdef STACK_DUMPING  
                                                                            
    #define ASSERT_STK(stk, error)                                          \
        /*printf("error = %d \n", stackError(stk));*/                           \
        error = stackError(stk);                                            \
        if (error)                                                          \
        {                                                                  \
            printf("O_O err = %d\n", error);                                                            \
            stackDump(stk, error, LOCATION);                                \
                                                                            \
            return error;                                                   \
        } 

#else

    #define ASSERT_STK(stk, error)                      \
    error = stackError(stk)                             \
    if (error)                                          \
    {                                                   \
                                                        \
        log_error(error);  \
        return error;                                   \
                                                        \
    }                                                   


#endif

#ifdef SUPER_STACK_DUMPING

    #define STACK_DUMP(stk, error) stackDump(stk, error, LOCATION)

#else

    #define STACK_DUMP(stk, error)

#endif


#ifdef CANARY_PROTECT

    typedef unsigned long long Canary;

#endif

typedef double Elem;

const int coef_size_up      = 2;
const int min_size_capacity = 8;

const char log_file_name[]  = "log.txt";
const char dump_file_name[] = "dump.txt";
FILE* log_file              = NULL;
FILE* dump_file             = NULL;

#ifdef LOGGING

    int OPEN_LOG_FILE = openLogFile(log_file_name);

#endif

#ifdef STACK_DUMPING

    int OPEN_DUMP_FILE = openDumpFile(dump_file_name);

#endif

const char* stack_error_messages[] = 
{

    "Ok.",
    "Stack pointer if null.",
    "Data poinet is null, but size > 0.",
    "Size over capacity.",
    "Size if negative.",
    "Capacity is negative.",
    "Active element is poisoned.",
    "Not active elemnt is not poisoned.",
    "Left canary in stack if incorrect.",
    "Right canary in stack if incorrect.",
    "Left canary in stack.data[] is incorrect",
    "Right canary in stack.data[] is incorrect.",

};

#ifdef STACK_DUMPING

    typedef struct _VarInfo
    {

        const char* var_name;
        const char* func_name;
        const char* file_name;

        int line;
    
    } VarInfo;

#endif




enum FUNC_ERRORS {

    SUCCES                        = 0,
    MEMORY_ERROR                  = 1,
    BAD_ARGS_ERROR                = 2,
    FILE_ERROR                    = 3,
    POP_FROM_EMTY_STACK           = 4,

};

enum STACK_ERORRS {

    STK_OK                        = 0,
    STK_PTR_ERROR                 = 1,
    DATA_PTR_ERROR                = 2,
    SIZE_OVER_CAPACITY_ERROR      = 3,
    NEGATIVE_SIZE                 = 4,
    NEGATIVE_CAPACITY             = 5,
    ACTIVE_ELEM_IS_POISON         = 6,
    NON_ACTIVE_ELEM_IS_NOT_POISON = 7,
    STACK_L_CANARY_INCORRECT      = 8,
    STACK_R_CANARY_INCORRECT      = 9,
    DATA_L_CANARY_INCORRECT       = 10,
    DATA_R_CANARY_INCORRECT       = 11,
    
};

const int NUM_OF_ERRORS = 12;

enum POISON_VALS {

    POISON_ELEM     = unsigned(-243),
    POISON_SIZE     = -1,
    POISON_CAPACITY = -1

};

enum Protect {

    LEFT_CANARY  = 0xDADADADA,
    RIGHT_CANARY = 0xFEFEFEFE

};

typedef struct _Stack
{
    #ifdef CANARY_PROTECT

    Canary left_canary = LEFT_CANARY;

    #endif

    Elem* data;
    
    size_t size = POISON_SIZE;
    size_t capacity = POISON_CAPACITY;

    char is_ctor = 0;

    #ifdef STACK_DUMPING

    VarInfo info;

    #endif

    #ifdef CANARY_PROTECT

    Canary right_canary = RIGHT_CANARY;

    #endif

} Stack;

void closeLogFile()
{
    assert(log_file != NULL);

    char finish_message[] = "-----------------------PROGRAM END------------------------\n";
    fprintf(log_file, "%s", finish_message);

    fclose(log_file);
}

int openLogFile(const char* log_file_name)
{
    log_file = fopen(log_file_name, "w");
    assert(log_file != NULL);

    setvbuf               (log_file, NULL, _IONBF, 0);
    
    char start_message[] = "--------------------PROGRAM STARTED--------------------\n";
    fprintf(log_file, "%s", start_message);
    
    atexit(closeLogFile);

    return 1;
}

void closeDumpFile()
{
    assert(dump_file != NULL);

    char finish_message[] = "-----------------------DUMPING END------------------------\n";
    fprintf(dump_file, "%s", finish_message);

    fclose(dump_file);
}

int openDumpFile(const char* log_file_name)
{
    dump_file = fopen(dump_file_name, "w");
    assert(dump_file != NULL);

    setvbuf               (dump_file, NULL, _IONBF, 0);
    
    char start_message[] = "--------------------DUMPING STARTED--------------------\n";
    fprintf(dump_file, "%s", start_message);
    
    atexit(closeDumpFile);

    return 1;
}


void _log_func(int error, const char* func_name, int line)
{
    fprintf(log_file, "%s returned %d (line %d)\n\n", func_name, error, line);
}

void _log_error(int error, const char* file_name, int line, const char* func_name)
{

    assert(log_file != NULL);

    switch (error)
    {

        case SUCCES:
            fprintf(log_file, "All is ok in %s in function %s at line %d.\n", file_name, func_name, line);
            break;

        
        case MEMORY_ERROR:
            fprintf(log_file, "Memory error in file %s in function %s at line %d.\n", file_name, func_name, line);
            break;

        
        case BAD_ARGS_ERROR:
            fprintf(log_file, "Bad parameters given in function %s in file %s at line %d\n", func_name, file_name, line);
            break;

        
        case FILE_ERROR:
            fprintf(log_file, "File pointer error in file %s  in function %s at line %d\n", file_name, func_name, line);
            break;

        case POP_FROM_EMTY_STACK:
            fprintf(log_file, "Pop from empty stackin file %s  in function %s at line %d\n", file_name, func_name, line);
            

        default:
            fprintf(log_file, "Error code (%d) is not correct in file %s in function %s at line %d, \n", error, file_name, func_name, line);
    }


}

void set_error_bit(int* error, int bit_number)
{

    *error |= (1 << bit_number);

}

int checkPoisonElem(void* _elem, size_t elem_size, int is_poison, unsigned char correct_poison)
{

    unsigned char* elem = (unsigned char*) _elem;

    for (size_t coun = 0; coun < elem_size; coun++)
    {

        int correct = is_poison ? elem[coun] == correct_poison :
                             elem[coun] != correct_poison;

        if (!correct)
            return 0;
    
    }

    return 1;

}

int stackError(Stack* stk)
{
    int error = 0;
    
    if (stk == NULL)                              set_error_bit(&error, STK_PTR_ERROR);

    if ((long long) stk->size < 0)                set_error_bit(&error, NEGATIVE_SIZE);

    if ((long long) stk->capacity < 0)            set_error_bit(&error, NEGATIVE_SIZE);
    
    if ( stk->capacity != 0 && stk->data == NULL) set_error_bit(&error, DATA_PTR_ERROR);
    
    if (stk->size > stk->capacity)                set_error_bit(&error, SIZE_OVER_CAPACITY_ERROR);

    if ((int) stk->capacity > 0)
    {
        //printf("%d\n", stk->capacity);
        for (size_t coun = 0; coun < stk->size; coun++)
        {

            if (!checkPoisonElem(stk->data + coun, sizeof(Elem), 0, (unsigned char) POISON_ELEM))
            {
                //sprintf("%d\n", coun);
                set_error_bit(&error, ACTIVE_ELEM_IS_POISON);

            }

        }
        //printf("stk_size = %lu\n", stk->size);
        for (size_t coun = stk->size; coun < stk->capacity; coun++)
        {

            if (!checkPoisonElem(stk->data + coun, sizeof(Elem), 1, (unsigned char) POISON_ELEM))
            {

                set_error_bit(&error, NON_ACTIVE_ELEM_IS_NOT_POISON);

            }

        }

    }

    #ifdef CANARY_PROTECT
        printf("check1 %x\n", *((Canary*) stk->data));
        printf("check2 %x\n", *((Canary*) (stk->data + stk->capacity)));

        if (stk->left_canary  != LEFT_CANARY)  set_error_bit(&error, STACK_L_CANARY_INCORRECT);
        if (stk->right_canary != RIGHT_CANARY) set_error_bit(&error, STACK_R_CANARY_INCORRECT);

        if (*((Canary*)  ((char*)  stk->data - sizeof(Canary))) != LEFT_CANARY)  set_error_bit(&error, DATA_L_CANARY_INCORRECT);
        if (*((Canary*)  ((char*) (stk->data + stk->capacity)))     != RIGHT_CANARY) set_error_bit(&error, DATA_R_CANARY_INCORRECT);


    #endif
    
return error;

}

void fprintStackErrors(FILE* fp, int error, const char** error_messages)
{
   
    for (int i = 0; i < NUM_OF_ERRORS; i++)
    {
        if (error & (1 << i))
        {
            fprintf(fp, "ERROR: %s\n", error_messages[i]);
        }
    }

}

void fprintElementBits(FILE* fp, void* ptr, size_t size)
{

    for (size_t i = 0; i < size; i++)
    {
        //printf("%lu\n", i);
        fprintf(fp, "%x", *((((char*) ptr) + i)));

    }

    fprintf(fp, "\n");

}

int stackDump(Stack* stk, int error, const char* file, int line, const char* function)
{
    //printf("Dump %d\n", error);
    FILE* dump_file = fopen(dump_file_name, "a");

    //printf("DUMPPP@@\n");

    fprintf(dump_file, "At %s at %s(%d):\n", function, file, line);

    fprintf(dump_file, "This stack (%s) was born in %s in %s at %d line\n", stk->info.var_name, stk->info.file_name, stk->info.func_name, stk->info.line);
    
    if (error != 0)
    {
        //printf("printfError\n");
        fprintStackErrors(dump_file, error, stack_error_messages);
    }

    fprintf(dump_file, "stack[%p] (%s) \n{\n\tsize     = %lu \n\tcapacity = %lu \n",
                       stk,(error==0)?"ok":"ERR",stk->size,                 stk->capacity);
    
    #ifdef CANARY_PROTECT

        fprintf(dump_file, "\tstack.LEFT_CANARY = %x\n\tstack.RIGHT_CANARY = %x\n",
                                                stk->left_canary,         stk->right_canary);

        fprintf(dump_file, "\tdata.LEFT_CANARY = %x\n\tdata.RIGHT_CANARY = %x\n",
                                               *((Canary*) ((char*) stk->data - sizeof(Canary))),
                                                                        *((Canary*) ((char*) (stk->data + stk->capacity))));

    #endif
    
    fprintf(dump_file, "\tdata[%p]\n", stk->data);

    fprintf(dump_file, "\t{\n");

    for (size_t i = 0; i < stk->capacity; i++)
    {
        if (i < stk->size)
        {
            
            fprintf(dump_file, "\t\t*");
            fprintf(dump_file, " %lg\n", stk->data[i]);
        
        }
        else
        {
            //printf("p1 = %x v1 = %X\n", &stk->data[i], stk->data[i]);
            fprintf(dump_file, "\t\t ");
            fprintElementBits(dump_file, &stk->data[i], sizeof(Elem));

        }

    }

    fprintf(dump_file, "\t}\n\n");

    fprintf(dump_file, "\n");

    fprintf(dump_file, "---------------------------------------------------------------\n\n");


    return 1;
}

void fillPoison(void* ptr_1st_poison, size_t elem_size, size_t left, size_t right, unsigned char poison_val)
{

    assert(ptr_1st_poison != NULL);

    //memset((char*) ptr_1st_poison + left * elem_size, (unsigned char) poison_val, elem_size * (right - left));
    
    for (size_t idx = left * elem_size; idx < right * elem_size; idx++)
    {

        //printf("ptr = %x\n", (char*) ptr_1st_poison + idx);
        *((char*) ptr_1st_poison + idx) = poison_val; 
        //printf("val == %x\n", *((char*) ptr_1st_poison + idx));
    }
}


int _stackCtor(Stack* stk, size_t capacity, const char* stk_name,
                                            const char* stk_file, 
                                            int stk_line, 
                                            const char* stk_func)
{
    printf("dcsd\n");
    int error = 0;

    //ASSERT_STK(stk, error);
    //STACK_DUMP(stk, error);

    size_t new_capacity = 1;
    
    if (capacity <= 8)
    {

        new_capacity = 8;    
    
    }
    else
    {
        while (new_capacity < capacity)
        {

            new_capacity *= 2;
        
        }
    }

    printf("capacccity = %d\n", new_capacity);

    
    #ifdef CANARY_PROTECT

        stk->left_canary  = LEFT_CANARY;
        stk->right_canary = RIGHT_CANARY;

        printf("uvu = %lu\n", new_capacity * sizeof(Elem) + 2 * sizeof(Canary));

        printf("new cap = %llu\n", new_capacity);

        stk->data = (Elem*) calloc(new_capacity * sizeof(Elem) + 2 * sizeof(Canary), 1);

        if (stk->data == NULL)
        {

            log_error(MEMORY_ERROR);
            return MEMORY_ERROR;

        }

        fprintElementBits(stdout, stk->data, sizeof(Canary));

        ((Canary*) stk->data) [0] = LEFT_CANARY;

        printf("UWU %x\n", *((Canary*) stk->data));

        fprintElementBits(stdout, stk->data, sizeof(Canary));

        printf("nc + sc = %lu\n", new_capacity + sizeof(Canary));
        
        stk->data = (Elem*) ((char*) stk->data + sizeof(Canary));
        
        fprintElementBits(stdout, stk->data + new_capacity, sizeof(Canary));

        ((Canary*) (stk->data + new_capacity)) [0] = RIGHT_CANARY;

        printf("III %x\n", *((Canary*) (stk->data + new_capacity)));
    
        fprintElementBits(stdout, stk->data + new_capacity, sizeof(Canary));

    #else

        stk->data = (Elem*) calloc(new_size, sizeof(Elem));

        if (stk->data == NULL)
        {
            
            log_error(MEMORY_ERROR);
            return MEMORY_ERROR;

        }

    #endif
    
    stk->size     = 0;
    stk->capacity = new_capacity;

    fillPoison(stk->data, sizeof(Elem), 0, new_capacity, (unsigned char) POISON_ELEM);

    #ifdef STACK_DUMPING

    stk->info.var_name  = stk_name;
    stk->info.file_name = stk_file;
    stk->info.func_name = stk_func;
    stk->info.line      = stk_line;

    #endif

    printf("AWA %x\n", *((Canary*) stk->data));
    printf("LML %x\n", *((Canary*) (stk->data + new_capacity)));
    
    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);

    return SUCCES;
}

int stackDtor(Stack* stk)
{
    int error = 0;

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);

    #ifdef CANARY_PROTECT

        free(((char*) stk->data) - sizeof(Canary));

    #else

        free(stk->data);

    #endif

    stk->size     = POISON_SIZE;
    stk->capacity = POISON_CAPACITY;

    return SUCCES;
}

int stackResize(Stack* stk, size_t new_size)
{
    int error = 0;

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);

    //printf("YTYTYTY\n");
    

    #ifdef CANARY_PROTECT

        stk->data = (Elem*) ((char*) stk->data - sizeof(Canary));

        //printf("EUEUE\n");

        stk->data = (Elem*) realloc(stk->data, new_size * sizeof(Elem) + 2 * sizeof(Canary));

        if (stk->data == NULL) return MEMORY_ERROR;

        stk->data = (Elem*) ((char*) stk->data + sizeof(Canary));

        *((Canary*) (stk->data + new_size)) = RIGHT_CANARY;

    #else

        stk->data = (Elem*) realloc(stk->data, new_size * sizeof(Elem));

        if (stk->data == NULL) return MEMORY_ERROR;

    #endif

    stk->capacity = new_size;

    fillPoison(stk->data, sizeof(Elem), stk->size, stk->capacity, (unsigned char) POISON_ELEM); 

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);

    return SUCCES;
}


int stackPush(Stack* stk, Elem value)
{
    int error = 0;

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);

        
    if (stk->size >= stk->capacity)
    {
        error = stackResize(stk, stk->capacity * coef_size_up);

        if (error != 0)
        {

            return error;

        }
    }

    stk->data[stk->size] = value;

    stk->size++;

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);
    
    return SUCCES;
}

int stackPop(Stack* stk, Elem* val)
{
    int error = 0;

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);


    if (stk->size < 1)
    {

        log_error(POP_FROM_EMTY_STACK);
        return POP_FROM_EMTY_STACK;

    }
    
    stk->size--;

    *val = stk->data[stk->size];

    stk->data[stk->size] = 0;

    fillPoison(stk->data, sizeof(Elem), stk->size, stk->size + 1, (unsigned char) POISON_ELEM);


    if (stk->capacity > 8 && stk->size <= stk->capacity / 4)
    {
        error = stackResize(stk, stk->capacity / 4);
        if (error != 0)
        {
            return error;
        }
    }


    //printf("I TUTA!!\n");
    //printf("I ZDESSS!!\n");
    error |= stackError(stk);

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);

    return SUCCES;
    
}

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


