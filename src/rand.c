#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wyhash.h"
#include "rand.h"

uint64_t RNG_SEED = 0;

void init_fast_rand() {
	uint32_t first_rand = rand();
	uint32_t second_rand = rand();

	uint32_t* RNG_SEED_LOWER_BITS = (uint32_t*)&RNG_SEED + 4;

	memcpy(&RNG_SEED, &first_rand, sizeof(uint32_t));
	memcpy(&RNG_SEED_LOWER_BITS, &second_rand, sizeof(uint32_t));


}

uint64_t fast_rand() {
	return wyrand(&RNG_SEED);

}
