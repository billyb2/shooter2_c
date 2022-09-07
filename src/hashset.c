#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "hashset.h"

HashSet new_hashset() {
	HashSet hashset = { 
		.num_items = 0,
		.item_list = NULL,

	};

	return hashset;

}

/// Returns whether the hash_set contains the value
bool hashset_contains(const void* item, uint64_t item_size, const HashSet* hash_set) {
	for (uint64_t i = 0; i < hash_set->num_items; i += 1) {
		void* hashset_item = hash_set->item_list + (item_size * i);
		
		if (memcmp(hashset_item, item, item_size) == 0) {
			return true;

		}

	}

	return false;

}

/// Returns whether the value was newly inserted
bool hashset_insert(const void* item, uint64_t item_size, HashSet* hash_set) {
	bool contains_item = hashset_contains(item, item_size, hash_set);

	if (!contains_item) {
		hash_set->num_items += 1;
		hash_set->item_list = realloc(hash_set->item_list, hash_set->num_items * item_size);

		memcpy(hash_set->item_list + ((hash_set->num_items - 1) * item_size), item, item_size);

	}

	return contains_item;

}


/// TODO
bool hashset_remove(const void* item, uint64_t item_size, HashSet* hash_set) {
	exit(-1);

}
