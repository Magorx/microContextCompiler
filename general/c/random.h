#ifndef GENERAL_RANDOM
#define GENERAL_RANDOM

#include <time.h>

//=============================================================================
//<KCTF> Handmade_random ======================================================

long randlong() {
    long ret = rand();
    ret |= rand() << sizeof(int);
    return ret > 0 ? ret : -ret;
}

//=============================================================================
//<Babichev> Fast_random ======================================================

typedef struct FastRandom_s {
    unsigned long long rnd;
} FastRandom;

FastRandom *new_FastRandom(unsigned long long seed) {
  FastRandom *t = (FastRandom*) malloc(sizeof (FastRandom));
  t->rnd = seed;
  return t;
}

unsigned long long FastRandom_randomize(FastRandom *t) {
  t->rnd ^= t->rnd << 21;
  t->rnd ^= t->rnd >> 35;
  t->rnd ^= t->rnd << 4;
  return t->rnd;
}

void delete_FastRandom(FastRandom *r) {
  free(r);
}

FastRandom GENERAL_FAST_RANDOM = {1};

void FastRandom_set_seed(unsigned long long seed) {
	GENERAL_FAST_RANDOM.rnd = seed;
}

unsigned long long FastRandom_rand() {
	return FastRandom_randomize(&GENERAL_FAST_RANDOM);
}

#endif // GENERAL_RANDOM