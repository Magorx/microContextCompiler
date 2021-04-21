#ifndef GENERAL_OVERLOADS
#define GENERAL_OVERLOADS

//=============================================================================
//<KCTF> Handmade_overloads ===================================================

#define CONCAT(a, c) a ## _ ## c
#define OVERLOAD(func, type) CONCAT(func, type)

#endif // GENERAL_OVERLOADS
