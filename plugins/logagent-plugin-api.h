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

#ifndef __LOGAGENT_PLUGIN_API_H__
#define __LOGAGENT_PLUGIN_API_H__

#include "logagent-plugin-list.h"

/* each line size */
#define MAX_LOG_SIZE	1024

typedef struct {
	char log[MAX_LOG_SIZE];
	struct list_head list;
} log_t;

/* add line to log list */
void logagent_log_add(struct list_head *log_list, const char *log);

/* remove log from log list */
void logagent_log_remove(struct list_head *pos);

/*
 * @brief:		plugin env init
 * @context:	take json in and plugin config out
 *
 *   logagent will maintained a plugin list. This
 * function is used for global init for plugin.
 */
extern int logagent_plugin_env_init(void **context);

/*
 * @brief:		plugin env destroy
 * @context:	take global config in
 *
 *   when logagent exit, it will call all destroy function
 * for all plugins in the plugin list.
 */
extern int logagent_plugin_env_destroy(void **context);

/*
 * @brief:		plugin work function
 * @gconfig:	plugin global config structure
 * @pconfig:	your own plugin config structure
 * @log_list: 	log list
 * 
 *   logagent will call work function one by one till
 * exit signal received, each plugin only need to add
 * or remove log from the log list. No need to malloc
 * or free, log_add and log_remove just do for you.
 *   
 * note: this function must be thread safety and noblocked.      
 */
extern int logagent_plugin_work(void *gconfig, void *pconfig,
				struct list_head *log_list);

/*
 * @brief:		plugin init function
 * @gconfig:	plugin global config structure
 * @context:	take json in and plugin pconfig out
 *
 *   logagent will save all plugin's config structure
 * in it's own list node, each time when logagent call
 * init function, context will take a json string in,
 * when init done, you should send back the config
 * structure's address.
 * 
 * note: this function must be thread safety and noblocked.
 */
extern int logagent_plugin_init(void *gconfig, void **context);

/*
 * @brief:		plugin exit function
 * @context:	take plugin pconfig in
 *
 *   logagent will call exit function for all plugins,
 * the context will take plugin's own config in, and
 * you need to do some clean work, and remember to
 * free the config structure.
 *
 * note: this function must be thread safety and noblocked.
 */
extern int logagent_plugin_exit(void *gconfig, void **context);

#endif
