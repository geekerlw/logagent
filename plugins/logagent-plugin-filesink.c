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

#include <stdio.h>
#include <string.h>
#include <json.h>

#include "logagent-plugin-api.h"

#define FILESINK_BUF_SIZE	256

typedef struct {
	char filepath[FILESINK_BUF_SIZE];
}filesink_t;

static const char *filesink_text = "welcome to filesink";

static int filesink_work(filesink_t *filesink, struct list_head *log_list)
{
	log_t *pos, *n;
	list_for_each_entry_safe(pos, log_t, n, log_list, list) {
		printf("debug: get log: %s\n", pos->log);
		logagent_log_remove(&pos->list);
		pos = n;
	}
	
	return 0;
}

static int filesink_init(const char *json, void **context)
{
	filesink_t *filesink = (filesink_t *) malloc(sizeof(filesink_t));
	if (!filesink)
		return -1;

	memset(filesink, 0, sizeof(filesink));
	
	struct json_object *plugin_obj = json_tokener_parse(json);
	if (!plugin_obj)
		return -2;

	/* get filepath key-value */
	struct json_object *filepath_obj;
	json_bool ret = json_object_object_get_ex(plugin_obj, "path", &filepath_obj);
	if (ret) {
		const char *filepath = json_object_get_string(filepath_obj);
		memcpy(filesink->filepath, filepath, strlen(filepath) + 1);
	}

	json_object_put(plugin_obj);

	*context = (void *)filesink;

	return 0;
}

static int filesink_exit(filesink_t *filesink)
{
	if (filesink)
		free(filesink);
}

int logagent_plugin_work(void *gconfig, void *pconfig, struct list_head *log_list)
{
	filesink_t *filesink = (filesink_t *)pconfig;

	return filesink_work(filesink, log_list);

}

int logagent_plugin_init(void *gconfig, void **context)
{
	char *json = (char *)context;

	return filesink_init(json, context);
}

int logagent_plugin_exit(void *gconfig, void **connext)
{
	filesink_t *filesink = (filesink_t *)*connext;

	return filesink_exit(filesink);
}

int logagent_plugin_env_init(void **context)
{
	return 0;
}

int logagent_plugin_env_destroy(void **context)
{

	return 0;
}
