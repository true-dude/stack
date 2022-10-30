#ifndef H_STACK
#define H_STACK

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define LOGGING
#define STACK_DUMPING
#define SUPER_STACK_DUMPING
#define CANARY_PROTECT
#define HASH_PROTECT

#define LOCATION __FILE__, __LINE__, __PRETTY_FUNCTION__

#define stackCtor(stk, capacity)                       \
_stackCtor(stk, capacity, #stk, LOCATION)


#ifdef LOGGING

    #define log_error(error)                            \
           _log_error(error, LOCATION)

    #define log_func(error) _log_func(error, __PRETTY_FUNCTION__, __LINE__)

#else

    #define log_error(error) 

#endif


#ifdef STACK_DUMPING  
                                                                            
    #define ASSERT_STK(stk, error)                      \
        error = stackError(stk);                        \
        if (error)                                      \
        {                                               \
            stackDump(stk, error, LOCATION);            \
                                                        \
            return STACK_ERROR;                         \
        } 

#else

    #define ASSERT_STK(stk, error)                      \
    error = stackError(stk)                             \
    if (error)                                          \
    {                                                   \
                                                        \
        log_error(error);                               \
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


#ifdef HASH_PROTECT

    typedef unsigned long long int Hash;

    #define DEF_HASH ((Hash) 0xd1e3f5a7b9c1d3e5f7)

    #define RECALC_STACK_HASH(stk) recalcStackHash(stk)

#else

    define RECALC_STACK_HASH(stk)

#endif


typedef double Elem;

const int coef_size_up      = 2;
const int min_size_capacity = 8;


#ifdef STACK_DUMPING

    typedef struct _VarInfo
    {

        const char* var_name;
        const char* func_name;
        const char* file_name;

        int line;
    
    } VarInfo;

#endif


typedef enum RETURN_CODE {

    SUCCES                        = 0,
    MEMORY_ERROR                  = 1,
    BAD_ARGS_ERROR                = 2,
    FILE_ERROR                    = 3,
    STACK_ERROR                   = 4,

} ReturnCode;


typedef enum STACK_ERRORS {

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
    BAD_HASH                      = 12,
    
} StackError;

typedef int StackErrorSet;


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

    
    #ifdef HASH_PROTECT

        Hash hash = 0;

    #endif

    
    #ifdef CANARY_PROTECT

        Canary right_canary = RIGHT_CANARY;

     #endif

} Stack;


const int NUM_OF_ERRORS = 12;


int openLogFile   ();
void closeLogFile ();


int openDumpFile   ();
void closeDumpFile ();


ReturnCode _stackCtor  (Stack* stk, size_t capacity, const char* stk_name,
                                              const char* stk_file, 
                                              int stk_line, 
                                              const char* stk_func);
ReturnCode stackDtor   (Stack* stk);
ReturnCode stackPush   (Stack* stk, Elem value);
ReturnCode stackPop    (Stack* stk, Elem* val);
ReturnCode stackResize (Stack* stk, size_t new_size);


void _log_func  (int error, const char* func_name, int line);
void _log_error (int error, const char* file_name, int line, const char* func_name);


void          set_error_bit   (StackErrorSet* error, int bit_number);
int           checkPoisonElem (void* _elem, size_t elem_size, int is_poison, unsigned char correct_poison);
StackErrorSet stackError      (Stack* stk);


Hash hash13(void* _data, size_t size);
void recalcStackHash(Stack* stk);


void fprintStackErrors (FILE* fp, int error, const char** error_messages);
void fprintElementBits (FILE* fp, void* ptr, size_t size);


int stackDump          (Stack* stk, int error, const char* file, int line, const char* function);


void fillPoison        (void* ptr_1st_poison, size_t elem_size, size_t left, size_t right, unsigned char poison_val);


#endif