#ifndef RAND_H
#define RAND_H
#include <stdint.h>

#define FAST_RAND_MAX UINT64_MAX

void init_fast_rand(); 
uint64_t fast_rand();

#endif 
