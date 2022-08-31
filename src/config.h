#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>

/* Set a config option */
void set_config_option(const char* option_name, const char* value);
/* Get a config option. Returns the option's value or NULL if not found
 * SAFETY: It is the caller's responsibility to free() the returned value*/
char* get_config_option(const char* option_name);

#endif
