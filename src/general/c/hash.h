#ifndef GENERAL_HASH
#define GENERAL_HASH

#include "common.h"

//=============================================================================
//<KCTF> Hash =================================================================

const long long HASH_MODULE = 1000000007;
const long long BASE = 257;

#define get_hash(struct) do_hash(struct, sizeof(struct))
long long do_hash(const void *memptr, size_t size_in_bytes);

long long do_hash(const void *memptr, size_t size_in_bytes) {
    VERIFY_T(memptr != NULL, long);
    long long ret = 1;
    const char *ptr = (const char*) memptr;
    for (size_t i = 0; i < size_in_bytes; ++i) {
        ret = (ret * BASE + ptr[i]) % HASH_MODULE;
    }
    return ret;
}

#endif // GENERAL_HASH