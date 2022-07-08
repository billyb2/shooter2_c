#include <stdint.h>

uint16_t saturating_sub(uint16_t x, uint16_t y) {
	if (y > x) {
		return 0;

	} else {
		return x - y;

	}

}

