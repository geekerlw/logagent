#ifndef __LOGAGENT_PLUGIN_H__
#define __LOGAGENT_PLUGIN_H__

#define PLUGIN_JSON_SIZE	1024

#include "logagent-list.h"

typedef struct {
	char json_config[PLUGIN_JSON_SIZE];

	/* plugin functions */
	void (*init)(const char *json);
	void (*work)(void *context);
	void (*exit)();

	void *context;	/* context struct that plugin use */

	void *lib_handle; /* dynamic lib handle */

	struct list_head list; /* plugin list each pipeline */
}plugin_t;

void logagent_plugin_work_all(struct list_head *plugin_list);

void logagent_plugin_init_all(struct list_head *plugin_list);

void logagent_plugin_exit_all(struct list_head *plugin_list);

void logagent_plugin_config_load(struct list_head *plugin_list, const char *json);

void logagent_plugin_config_unload(struct list_head *plugin_list);

#endif
