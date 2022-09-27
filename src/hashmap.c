#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "hashmap.h"

HashMap new_hashmap(void) {
	HashMap hashmap = { 
		.num_items = 0,
		.item_list = NULL,

	};

	return hashmap;

}

#define ITEM_SIZE (key_size + sizeof(uint64_t))

/// Returns whether or not the hashmap contains the value
bool hashmap_get(const void* key, const uint64_t key_size, const HashMap* hashmap, uint64_t* value_out) {
	if (hashmap->num_items == 0) {
		return false;

	}

	for (uint64_t i = 0; i < hashmap->num_items; i += 1) {
		void* hashmap_item = hashmap->item_list + (ITEM_SIZE * i);

		if (memcmp(hashmap_item, key, key_size) == 0) {
			if (value_out != NULL) {
				memcpy(value_out, hashmap_item + key_size, sizeof(*value_out));

			}

			return true;

		}

	}

	return false;

}

/// Returns whether the hashmap contains the value
bool hashmap_contains(const void* item, const uint64_t key_size, const HashMap* hashmap) {
	for (uint64_t i = 0; i < hashmap->num_items; i += 1) {
		void* hashmap_item = hashmap->item_list + (ITEM_SIZE * i);

		if (memcmp(hashmap_item, item, key_size) == 0) {
			return true;	

		}

	}

	return false;

}

void* hashmap_find_item(const void* key, const uint64_t key_size, HashMap* hashmap) {
	for (uint64_t i = 0; i < hashmap->num_items; i += 1) {
		void* hashmap_item = hashmap->item_list + (ITEM_SIZE * i);

		if (memcmp(hashmap_item, key, key_size) == 0) {
			return hashmap_item;	

		}

	}

	return NULL;
}

/// Returns whether the value was newly inserted
bool hashmap_insert(const void* item, uint64_t key_size, HashMap* hashmap, uint64_t value) {
	void* hashmap_item = hashmap_find_item(item, key_size, hashmap);
	bool new_item = hashmap_item == NULL;

	// If the item exists, update the existing one
	if (new_item) {
		hashmap->num_items += 1;
		hashmap->item_list = realloc(hashmap->item_list, hashmap->num_items * ITEM_SIZE);

		hashmap_item = hashmap->item_list + ((hashmap->num_items - 1) * ITEM_SIZE);

		memcpy(hashmap_item, item, key_size);

	}


	memcpy(hashmap_item + key_size, &value, sizeof(value));

	return new_item;

}

/// TODO
bool hashmap_remove(const void* item, uint64_t key_size, HashMap* hashmap) {
	exit(-1);

}
