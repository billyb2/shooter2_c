#ifndef WASMER_EXT_H
#define WASMER_EXT_H

#include "include/wasmer.h"

size_t* get_export_by_name(const char* name, wasm_exporttype_vec_t exports);
#endif
