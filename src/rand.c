#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <time.h>

#include "wyhash.h"
#include "rand.h"

thread_local uint64_t RNG_SEED = 0;

void init_fast_rand() {
	RNG_SEED = time(NULL);

}

uint64_t fast_rand() {
	return wyrand(&RNG_SEED);

}

uint64_t rand_range_u64(uint64_t min, uint64_t max) {
	return min + (fast_rand() % (max - min));
}
