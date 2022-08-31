#include "file_ext.h"
#include <stddef.h>
#include <stdio.h>

size_t get_file_size(FILE* file) {
	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	rewind(file);

	return file_size;

}
