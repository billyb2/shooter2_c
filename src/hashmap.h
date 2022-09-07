#ifndef HASHMAP_H
#define HASHMAP_H
#include <stdint.h>
#include <stdbool.h>

typedef struct HashMap {
	void* item_list;
	uint64_t num_items;

} HashMap;

HashMap new_hashmap();
bool hashmap_get(const void* key, const uint64_t key_size, const HashMap* hashmap, uint64_t* value_out);
bool hashmap_insert(const void* item, uint64_t key_size, HashMap* hashmap, uint64_t value);

#endif
