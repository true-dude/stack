#include "stack.h"


const char log_file_name[]  = "log.txt";
const char dump_file_name[] = "dump.txt";
FILE* log_file              = NULL;
FILE* dump_file             = NULL;

#ifdef LOGGING

    int OPEN_LOG_FILE = openLogFile();

#endif

#ifdef STACK_DUMPING

    int OPEN_DUMP_FILE = openDumpFile();

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
    "Left canary in stack.data[] is incorrect.",
    "Right canary in stack.data[] is incorrect.",
    "Stack hash is invalid.",

};


void closeLogFile()
{

    assert(log_file != NULL);

    char finish_message[] = "-----------------------PROGRAM END------------------------\n";
    fprintf(log_file, "%s", finish_message);

    fclose(log_file);
}


int openLogFile()
{

    log_file = fopen(log_file_name, "w");
    assert(log_file != NULL);

    setvbuf(log_file, NULL, _IONBF, 0);
    
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


int openDumpFile()
{

    dump_file = fopen(dump_file_name, "w");
    assert(dump_file != NULL);

    setvbuf(dump_file, NULL, _IONBF, 0);
    
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

        case STACK_ERROR:
            fprintf(log_file, "Error with stack in file %s  in function %s at line %d\n", file_name, func_name, line);
            break;
            

        default:
            fprintf(log_file, "Error code (%d) is not correct in file %s in function %s at line %d, \n", error, file_name, func_name, line);
    }


}


Hash hash13(void* _data, size_t size)
{

    char* data = (char*) _data;

    Hash new_hash = (Hash) DEF_HASH;

    for (size_t i = 0; i < size; i++)
    {

        new_hash = ((new_hash << 3) + data[i]) ^ DEF_HASH;

    }

    return new_hash;

}


void recalcStackHash(Stack* stk)
{

    Hash hash1 = hash13(stk, sizeof(Stack) - sizeof(Canary) - sizeof(Hash));

    #ifdef CANARY_PROTECT

        Hash hash2 = hash13(((char*) stk->data) - sizeof(Canary), stk->size + 2 * sizeof(Canary));
    
    #else

        Hash hash2 = hash13(stk->data, stk->size);

    #endif

    stk->hash = hash1 ^ hash2;

}


void set_error_bit(StackErrorSet* error, int bit_number)
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


StackErrorSet stackError(Stack* stk)
{

    StackErrorSet error = STK_OK;
    
    if (stk == NULL)                              set_error_bit(&error, STK_PTR_ERROR);

    if ((long long) stk->size < 0)                set_error_bit(&error, NEGATIVE_SIZE);

    if ((long long) stk->capacity < 0)            set_error_bit(&error, NEGATIVE_SIZE);
    
    if ( stk->capacity != 0 && stk->data == NULL) set_error_bit(&error, DATA_PTR_ERROR);
    
    if (stk->size > stk->capacity)                set_error_bit(&error, SIZE_OVER_CAPACITY_ERROR);

    if ((int) stk->capacity > 0)
    {
        
        for (size_t coun = 0; coun < stk->size; coun++)
        {

            if (!checkPoisonElem(stk->data + coun, sizeof(Elem), 0, (unsigned char) POISON_ELEM))
            {
                
                set_error_bit(&error, ACTIVE_ELEM_IS_POISON);

            }

        }
        
        for (size_t coun = stk->size; coun < stk->capacity; coun++)
        {

            if (!checkPoisonElem(stk->data + coun, sizeof(Elem), 1, (unsigned char) POISON_ELEM))
            {

                set_error_bit(&error, NON_ACTIVE_ELEM_IS_NOT_POISON);

            }

        }

    }

    #ifdef CANARY_PROTECT

        if (stk->left_canary  != LEFT_CANARY)  set_error_bit(&error, STACK_L_CANARY_INCORRECT);
        if (stk->right_canary != RIGHT_CANARY) set_error_bit(&error, STACK_R_CANARY_INCORRECT);

        if (*((Canary*)  ((char*)  stk->data - sizeof(Canary))) != LEFT_CANARY)  set_error_bit(&error, DATA_L_CANARY_INCORRECT);
        if (*((Canary*)  ((char*) (stk->data + stk->capacity))) != RIGHT_CANARY) set_error_bit(&error, DATA_R_CANARY_INCORRECT);


    #endif

    #ifdef HASH_PROTECT

        Hash prev_hash = stk->hash;

        recalcStackHash(stk);

        if (stk->hash != prev_hash) set_error_bit(&error, BAD_HASH);

    #endif
    
return error;

}


void fprintStackErrors(FILE* fp, int error, const char** error_messages)
{
   
    for (int i = 0; i <= NUM_OF_ERRORS; i++)
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

        fprintf(fp, "%x", *((((unsigned char*) ptr) + i)));

    }

    fprintf(fp, "\n");

}


int stackDump(Stack* stk, int error, const char* file, int line, const char* function)
{
    
    fprintf(dump_file, "At %s at %s(%d):\n", function, file, line);

    fprintf(dump_file, "This stack (%s) was born in %s in %s at %d line\n", stk->info.var_name, stk->info.file_name, stk->info.func_name, stk->info.line);
    
    if (error != 0)
    {
        
        fprintStackErrors(dump_file, error, stack_error_messages);
    
    }

    fprintf(dump_file, "stack[%p] (%s) \n{\n\tsize     = %lu \n\tcapacity = %lu \n",
                       stk,(error==0)?"ok":"ERR",stk->size,                 stk->capacity);
    
    #ifdef CANARY_PROTECT

        fprintf(dump_file, "\tstack.LEFT_CANARY = %llx\n\tstack.RIGHT_CANARY = %llx\n",
                                                stk->left_canary,         stk->right_canary);

        fprintf(dump_file, "\tdata.LEFT_CANARY = %llx\n\tdata.RIGHT_CANARY = %llx\n",
                                               *((Canary*) ((char*) stk->data - sizeof(Canary))),
                                                                        *((Canary*) ((char*) (stk->data + stk->capacity))));

    #endif

    #ifdef HASH_PROTECT

        fprintf(dump_file, "\tstack.hash = %llx\n", stk->hash);

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
            
            fprintf(dump_file, "\t\t ");
            fprintElementBits(dump_file, &stk->data[i], sizeof(Elem));

        }

    }

    fprintf(dump_file, "\t}\n\n");

    fprintf(dump_file, "\n");

    fprintf(dump_file, "---------------------------------------------------------------\n\n");


    return SUCCES;
}


void fillPoison(void* ptr_1st_poison, size_t elem_size, size_t left, size_t right, unsigned char poison_val)
{

    assert(ptr_1st_poison != NULL);

    memset((char*) ptr_1st_poison + left * elem_size, poison_val, elem_size * (right - left));
    
}


ReturnCode _stackCtor(Stack* stk, size_t capacity, const char* stk_name,
                                            const char* stk_file, 
                                            int stk_line, 
                                            const char* stk_func)
{

    StackErrorSet error = 0;

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

    
    #ifdef CANARY_PROTECT

        stk->left_canary  = LEFT_CANARY;
        stk->right_canary = RIGHT_CANARY;


        stk->data = (Elem*) calloc(new_capacity * sizeof(Elem) + 2 * sizeof(Canary), 1);

        if (stk->data == NULL)
        {

            log_error(MEMORY_ERROR);
            return MEMORY_ERROR;

        }

        ((Canary*) stk->data) [0] = LEFT_CANARY;
        
        stk->data = (Elem*) ((char*) stk->data + sizeof(Canary));

        ((Canary*) (stk->data + new_capacity)) [0] = RIGHT_CANARY;

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

    RECALC_STACK_HASH(stk);
    
    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);

    return SUCCES;
}


ReturnCode stackDtor(Stack* stk)
{

    StackErrorSet error = 0;

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


ReturnCode stackResize(Stack* stk, size_t new_size)
{
    StackErrorSet error = 0;

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);
    

    #ifdef CANARY_PROTECT

        stk->data = (Elem*) ((char*) stk->data - sizeof(Canary));

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

    RECALC_STACK_HASH(stk);

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);

    return SUCCES;
}


ReturnCode stackPush(Stack* stk, Elem value)
{
    
    StackErrorSet error = 0;

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);

        
    if (stk->size >= stk->capacity)
    {

        if (stackResize(stk, stk->capacity * coef_size_up) == MEMORY_ERROR)
        {
            
            return MEMORY_ERROR;

        }
    }

    stk->data[stk->size] = value;

    stk->size++;

    RECALC_STACK_HASH(stk);

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);
    
     return SUCCES;

}


ReturnCode stackPop(Stack* stk, Elem* val)
{

    StackErrorSet error = 0;

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);


    if (stk->size < 1)
    {

        log_error(STACK_ERROR);
        return STACK_ERROR;

    }
    
    stk->size--;

    *val = stk->data[stk->size];

    stk->data[stk->size] = 0;

    fillPoison(stk->data, sizeof(Elem), stk->size, stk->size + 1, (unsigned char) POISON_ELEM);


    if (stk->capacity > 8 && stk->size <= stk->capacity / 4)
    {
        
        if (stackResize(stk, stk->capacity / 4) == MEMORY_ERROR)
        {
            return MEMORY_ERROR;
        }
    }


    RECALC_STACK_HASH(stk);

    ASSERT_STK(stk, error);
    STACK_DUMP(stk, error);

    return SUCCES;
    
}




