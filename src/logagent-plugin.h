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
#define PLUGIN_LIB_NAME_SIZE	64

typedef struct {
	char json[PLUGIN_JSON_SIZE]; /**< plugin's json config */
	char path[PLUGIN_LIB_PATH_SIZE]; /**< plugin lib path */
	char name[PLUGIN_LIB_NAME_SIZE]; /**< plugin lib short name */

	void *config;		/**<  plugin's global config */

	/* plugin functions */
	int (*env_init) (void **context);
	int (*init) (void *gconfig, void **context);
	int (*work) (void *gconfig, void *pconfig, struct list_head * log_list);
	int (*exit) (void *gconfig, void **context);
	int (*env_destroy) (void **context);

	void **context; /**< context for plugins */

	void *lib_handle;	/**< plugin dynamic lib handle */

	struct list_head list;	/**< plugin list header */
} plugin_t;

/**
 * plugin global environment init
 * @param plugin_list a plugin list
 */
void logagent_plugin_env_init_all(struct list_head *plugin_list);

/**
 * plugin global environment destory
 * @param plugin_list a plugin list
 */
void logagent_plugin_env_destroy_all(struct list_head *plugin_list);

/**
 * load plugin config from json string
 * @param plugin_list a plugin list
 * @param json the plugin's json config string
 */
void logagent_plugin_config_load(struct list_head *plugin_list,
				 const char *json);

/**
 * unload plugin config
 * @param plugin_list a plugin list
 */
void logagent_plugin_config_unload(struct list_head *plugin_list);

#endif
