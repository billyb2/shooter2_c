#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "file_ext.h"
#include "include/json-c/json_tokener.h"
#include "include/json-c/json.h"
#include "include/json-c/json_object.h"

#if defined(__unix__) || defined(__WIN32__)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int cross_mkdir(const char* path) {
	#ifdef __unix__
		// Read, write, execute
		return mkdir(path, 0700);
	#endif

	#ifdef __WIN32__
		return mkdir(path);

	#endif

}

size_t get_config_file(FILE** file, bool read_mode) {
	struct stat st = { 0 };


	#define CONFIG_RELATIVE_PATH_LEN 9 + 11 + 6 + 1

	#ifdef __unix__
	char* possible_config_home = getenv("XDG_CONFIG_HOME");
	int possible_config_home_len;

	if (possible_config_home == NULL) {
		possible_config_home_len = 0;

	} else {
		possible_config_home_len = strlen(possible_config_home);

	}
	#endif

	#ifdef __WIN32__
	char* possible_config_home = "./";
	int possible_config_home_len = strlen(possible_config_home);

	#endif

	char* config_home = malloc(possible_config_home_len + CONFIG_RELATIVE_PATH_LEN);
	int config_home_len = 0;

	if (possible_config_home == NULL) {
		char* home_dir = getenv("HOME");
		int home_dir_strlen = strlen(home_dir);

		config_home = realloc(config_home, home_dir_strlen + CONFIG_RELATIVE_PATH_LEN);

		memcpy(config_home, home_dir, home_dir_strlen);
		config_home_len += home_dir_strlen;


	} else {
		memcpy(config_home, possible_config_home, possible_config_home_len + 1);
		config_home_len += possible_config_home_len;

	}

	memcpy(config_home + config_home_len, "/.config/", 10);
	config_home_len += 9;

	if (stat(config_home, &st) == -1) {
		cross_mkdir(config_home);

	}

	memcpy(config_home + config_home_len, "shooter2_c/", 12);

	if (stat(config_home, &st) == -1) {
		cross_mkdir(config_home);

	}

	memcpy(config_home + config_home_len + 11, "config", 7);

	if (stat(config_home, &st) == -1) {
		fclose(fopen(config_home, "w+"));

	}

	FILE* tmp_file = fopen(config_home, "r");
	size_t file_size = get_file_size(tmp_file);
	fclose(tmp_file);

	if (read_mode) {
		*file = fopen(config_home, "r");

	} else {
		*file = fopen(config_home, "w");

	}

	return file_size;

}

#endif

void set_config_option(const char* option_name, const char* value) {
	FILE* config_file_ptr; 
	size_t config_file_size = get_config_file(&config_file_ptr, true);

	if (config_file_ptr == NULL) {
		fprintf(stderr, "Opening config file failed\n");
		exit(-1);

	}


	char* config_file_buffer = malloc(config_file_size);
	fread(config_file_buffer, config_file_size, 1, config_file_ptr);

	fclose(config_file_ptr);
	get_config_file(&config_file_ptr, false);

	struct json_object* json_obj;

	if (config_file_size > 0) {
		json_obj = json_tokener_parse(config_file_buffer);

	} else {
		json_obj = json_object_new_object();

	}
	
	struct json_object* value_as_json_string = json_object_new_string(value);

	if (value_as_json_string == NULL) {
		fprintf(stderr, "???\n");
		exit(-1);

	}

	struct json_object* tmp_obj = json_object_object_get(json_obj, option_name);

	if (tmp_obj != NULL) {
		json_object_object_del(json_obj, option_name);

	}

	json_object_object_add(json_obj, option_name, value_as_json_string);

	const char* json_string = json_object_to_json_string(json_obj);

	fwrite(json_string, strlen(json_string) + 1, 1, config_file_ptr);

	json_object_put(json_obj);

	free(config_file_buffer);
	fclose(config_file_ptr);

}

char* get_config_option(const char* option_name) {
	FILE* config_file_ptr;
	size_t config_file_size = get_config_file(&config_file_ptr, true);

	if (config_file_ptr == NULL) {
		fprintf(stderr, "Opening config file failed\n");
		exit(-1);

	}

	char* config_file_buffer = malloc(config_file_size);
	fread(config_file_buffer, config_file_size, 1, config_file_ptr);

	struct json_object* json_obj;

	if (config_file_size > 0) {
		json_obj = json_tokener_parse(config_file_buffer);

	} else {
		return NULL;

	}

	struct json_object* option_value;

	bool found_key = json_object_object_get_ex(json_obj, option_name, &option_value);

	if (!found_key) {
		return NULL;

	}

	const char* value_string = json_object_get_string(option_value);

	if (value_string == NULL) {
		return NULL;

	}
	
	const int value_string_len = strlen(value_string);


	// This is done so I don't have so the caller doesn't have to worry about freeing the json object
	char* value_to_return = malloc(value_string_len + 1);
	memcpy(value_to_return, value_string, value_string_len + 1);

	json_object_put(json_obj);
	free(config_file_buffer);
	fclose(config_file_ptr);

	return value_to_return;

}

