#ifndef WASMER_EXT_H
#define WASMER_EXT_H

#include "include/wasmer.h"

size_t* get_export_by_name(const char* name, wasm_exporttype_vec_t exports);
wasm_func_t* setup_wasm_func(const char* func_name,	wasm_exporttype_vec_t exports_type, wasm_extern_vec_t wasm_exports);
#endif
