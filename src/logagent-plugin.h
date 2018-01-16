/*
 * Copyright (c) 2018 Steven Lee <geekerlw@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 */

#ifndef __LOGAGENT_PLUGIN_H__
#define __LOGAGENT_PLUGIN_H__

#define PLUGIN_JSON_SIZE		1024
#define PLUGIN_LIB_PATH_SIZE	256
#define PLUGIN_LIB_NAME_SIZE	32

typedef struct {
	char json[PLUGIN_JSON_SIZE];
	char path[PLUGIN_LIB_PATH_SIZE];
	char name[PLUGIN_LIB_NAME_SIZE];
	
	void *config; /* json parsed config struct */

	/* plugin functions */
	int (*init)(void **context);
	int (*work)(void *config, struct list_head *log_list);
	int (*exit)(void **context);

	void **context;	/* context for plugins */

	void *lib_handle; /* dynamic lib handle */

	struct list_head list; /* plugin list each pipeline */
}plugin_t;

void logagent_plugin_work_all(struct list_head *plugin_list);

void logagent_plugin_init_all(struct list_head *plugin_list);

void logagent_plugin_exit_all(struct list_head *plugin_list);

void logagent_plugin_config_load(struct list_head *plugin_list, const char *json);

void logagent_plugin_config_unload(struct list_head *plugin_list);

#endif
