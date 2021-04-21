#ifndef GENERAL_COMMON
#define GENERAL_COMMON

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef uint8_t byte;

//=============================================================================
//<KCTF> Constants ============================================================

const size_t GENERAL_INIT_SIZE = 32;

const long KCTF_POISON = -7777777;

const double GENERAL_EPS = 1e-6;

//=============================================================================
//<KCTF> Handmade asserts =====================================================

const int KCTF_VERIFY_LOUDNESS = 1;
const int FATAL_ERROR = 2;
const int CHECK_ERROR = 1;                                                                 

#ifndef EXCLUDE_VERIFICATION
#define FULL_VERIFY(expr, err_name, loudness, cur_loudness, droptable, ERROR, ret_type)  \
    do {                                                                                 \
        const long long ret = (long long) (expr);                                        \
        if ((ERROR || !ret) && (loudness) >= (cur_loudness)) {                           \
            printf("[ERR]<verify>: [erro_erro](%s)\n", err_name);                        \
            if (ret == 0) {                                                              \
            printf("[   ]<      >: [erro_code](%X)\n", (unsigned)0xDED);                 \
            } else {                                                                     \
            printf("[   ]<      >: [erro_code](%lld)\n", ret);                           \
            }                                                                            \
            printf("[   ]<      >: [file_name](%s)\n", __FILE__);                        \
            printf("[   ]<      >: [func_name](%s)\n", __FUNCTION__);                    \
            printf("[   ]<      >: [line_indx](%d)\n", __LINE__);                        \
            }                                                                            \
        if (ERROR || !ret) {                                                             \
            if (droptable) { exit   (           ERROR_CHECK_UPPER_VERIFY); }             \
            else           { return ((ret_type) ERROR_CHECK_UPPER_VERIFY); }             \
        }                                                                                \
    } while(0)
#else
#define FULL_VERIFY(expr, err_name, loudness, cur_loudness, droptable, ERROR, ret_type)
#endif

#define VERIFY_YESDROP(expr, err_name, loudness, cur_loudness) FULL_VERIFY(expr, err_name, loudness, cur_loudness, 1, 0, int)
#define VERIFY_LOUDSET(expr, err_name, loudness) VERIFY_YESDROP(expr, err_name, loudness, KCTF_VERIFY_LOUDNESS)
#define VERIFY_ERRCDNM(expr, err_name) VERIFY_LOUDSET(expr, err_name, KCTF_VERIFY_LOUDNESS)

#define VERIFY(expr) VERIFY_ERRCDNM(expr, #expr)
#define VERIFY_OK(expr) VERIFY((expr) == OK)

#define VERIFY_T(expr, type) FULL_VERIFY(expr, #expr, KCTF_VERIFY_LOUDNESS, KCTF_VERIFY_LOUDNESS, 1, 0, type)

#define RETURN_ERROR_VERIFY  (expr) FULL_VERIFY(expr, #expr, KCTF_VERIFY_LOUDNESS, KCTF_VERIFY_LOUDNESS, 0, 1, int)
#define RETURN_ERROR_VERIFY_T(expr, type) FULL_VERIFY(expr, #expr, KCTF_VERIFY_LOUDNESS, KCTF_VERIFY_LOUDNESS, 0, 1, type)

#ifdef VERIFY_BOMB
#define RETURNING_VERIFY(expr) FULL_VERIFY(expr, #expr, KCTF_VERIFY_LOUDNESS, KCTF_VERIFY_LOUDNESS, 1, 0, int)
#else
#define RETURNING_VERIFY(expr) FULL_VERIFY(expr, #expr, KCTF_VERIFY_LOUDNESS, KCTF_VERIFY_LOUDNESS, 0, 1, int)
#endif

#define RETURNING_VERIFY_OK(expr) RETURNING_VERIFY((expr) == OK)

//=============================================================================
//<KCTF> Return_codes =========================================================

enum RETURN_CODES {
    ERROR_ERROR = -7777777,
    
    ERROR_FILE_NOT_FOUND = -50,
    ERROR_BIG_FILE,
    
    ERROR_MALLOC_FAIL,
    ERROR_REALLOC_FAIL,
    
    ERROR_NO_RET_CODE,
    ERROR_BAD_ARGS,

    ERROR_NULL_OBJECT,
    ERROR_NULL_BUFFER,

    ERROR_BAD_OBJECT,
    ERROR_BAD_LINKS,
    ERROR_BAD_NODE_INDEX,

    ERROR_BAD_CANARY,
    ERROR_BAD_HASH,

    ERROR_CHECK_UPPER_VERIFY = -1,

    NULL_OBJ_OK = 0,
    RET_OK = 0,
    OK = 0,

    NOT_OK = 1
};

#define PRINTF_ERROR() printf("EROR HAPANED\n")

#endif // GENERAL_COMMON