#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "wasmer_ext.h"
#include "include/wasm.h"

/// Returns a pointer to the export's index, or a NULL pointer otherwise
size_t* get_export_by_name(const char* ext_name, wasm_exporttype_vec_t exports) {
	size_t current_name_size = 0;
	char* name_being_checked = NULL;

	for (size_t i = 0; i < exports.size; i += 1) {
		const wasm_name_t* name_data = wasm_exporttype_name(exports.data[i]);

		if (name_data == NULL) {
			printf("Error getting name data");
			exit(-1);

		}

		if (name_data->size > current_name_size) {
			current_name_size = name_data->size;
			name_being_checked = realloc(name_being_checked, name_data->size + 1);
		}

		memcpy(name_being_checked, name_data->data, name_data->size);

		// Add the null byte
		name_being_checked[name_data->size] = 0;

		if (strcmp(name_being_checked, ext_name) == 0) {
			free(name_being_checked);
			size_t* index_ptr = malloc(sizeof(size_t));
			*index_ptr = i;

			return index_ptr;

		}

	}

	if (name_being_checked != NULL) {
		free(name_being_checked);

	}

	// Didn't find the export :(
	return NULL;	

}
