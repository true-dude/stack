#ifndef H_STACK
#define H_STACK

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


int openLogFile  (const char* log_file_name);
int openDumpFile (const char* log_file_name);








int _stackCtor(Stack* stk, size_t capacity, const char* stk_name,
                                            const char* stk_file, 
                                            int stk_line, 
                                            const char* stk_func);
int stackDtor(Stack* stk);
int stackPush(Stack* stk, Elem value);
int stackPop(Stack* stk, Elem* val);
int stackResize(Stack* stk, size_t new_size);


#endif