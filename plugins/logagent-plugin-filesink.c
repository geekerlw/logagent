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

static void filesink_work(filesink_t *filesink, struct list_head *log_list)
{
	log_t *log;
	list_for_each_entry(log, log_t, log_list, list) {
		printf("steven: get log: %s\n", log);
		logagent_log_remove(&log->list);
	}
	
	return;
}

static void filesink_init(const char *json, void **context)
{
	if (!json)
		return;

	filesink_t *filesink = (filesink_t *) malloc(sizeof(filesink_t));
	if (!filesink)
		return;

	memset(filesink, 0, sizeof(filesink));
	
	struct json_object *plugin_obj = json_tokener_parse(json);
	if (!plugin_obj)
		return;

	/* get filepath key-value */
	struct json_object *filepath_obj = json_object_object_get(plugin_obj, "path");
	const char *filepath = json_object_get_string(filepath_obj);
	memcpy(filesink->filepath, filepath, strlen(filepath) + 1);

	json_object_put(plugin_obj);

	*context = (void *)filesink;

	return;
}

static void filesink_exit(filesink_t *filesink)
{
	//if (filesink)
	//	free(filesink);
}

void logagent_plugin_work(void *config, struct list_head *log_list)
{
	filesink_t *filesink = (filesink_t *)config;

	filesink_work(filesink, log_list);

	return;
}

void logagent_plugin_init(void **context)
{
	char *json = (char *)context;

	filesink_init(json, context);

	return;
}

void logagent_plugin_exit(void **connext)
{
	filesink_t *filesink = (filesink_t *)*connext;

	filesink_exit(filesink);

	return;
}
