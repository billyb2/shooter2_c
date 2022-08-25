#ifndef HASHSET_H
#define HASHSET_H
#include <stdint.h>
#include <stdbool.h>

typedef struct HashSet {
	void* item_list;
	uint64_t num_items;

} HashSet;

HashSet new_hashset();
bool hashset_insert(const void* item, uint64_t item_size, HashSet* hash_set);
bool hashset_contains(const void* item, uint64_t item_size, HashSet* hash_set); 

#endif
