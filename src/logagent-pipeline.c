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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <json.h>

#include "logagent.h"
#include "logagent-log.h"
#include "logagent-list.h"
#include "logagent-pipeline.h"

static void logagent_pipeline_list_add(struct list_head *pipeline_list, const char *json)
{
	pipeline_t *pdata = (pipeline_t *) malloc(sizeof(pipeline_t));
	if (!pdata)
		return;
	memset(pdata, 0, sizeof(pdata));

	memcpy(pdata->json, json, strlen(json) + 1);

	list_append(pipeline_list, &pdata->list);

	return;
}

static void logagent_pipeline_list_remove(struct list_head *pos)
{
	if (pos == pos->next)
		return;

	pipeline_t *pdata = list_entry(pos, pipeline_t, list);

	list_remove(pos);

	free(pdata);

	return;
}


static void logagent_pipeline_list_init(struct list_head *pipeline_list)
{
	list_init(pipeline_list);
}

static void logagent_pipeline_list_destroy(struct list_head *pipeline_list)
{
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, pipeline_list) {
		pipeline_t *node = list_entry(pos, pipeline_t, list);
		list_remove(pos);
		free(node);
		pos = n;
	}

	return;
}

static void logagent_pipeline_init(pipeline_t *pipeline)
{
	logagent_plugin_init_all(&pipeline->plugin_list);

	return;
}

static void logagent_pipeline_init_all(struct list_head *pipeline_list)
{
	pipeline_t *pipeline;
	list_for_each_entry(pipeline, pipeline_t, pipeline_list, list) {
		logagent_pipeline_init(pipeline);
	}

	return;
}

static void logagent_pipeline_exit(pipeline_t *pipeline)
{
	logagent_plugin_exit_all(&pipeline->plugin_list);

	return;
}

static void logagent_pipeline_exit_all(struct list_head *pipeline_list)
{
	pipeline_t *pipeline;
	list_for_each_entry(pipeline, pipeline_t, pipeline_list, list) {
		logagent_pipeline_exit(pipeline);
	}

	return;
}

void logagent_pipeline_work(pipeline_t *pipeline)
{
	logagent_pipeline_init(pipeline);

	while (!logagent_need_exit) {
		usleep(PIPELINE_SLEEP_TIME * 1000);
		
		logagent_plugin_work_all(&pipeline->plugin_list);
	}

	logagent_pipeline_exit(pipeline);

	return;
}

static void logagent_pipeline_plugin_config_load(struct list_head *pipeline_list)
{
	pipeline_t *pipeline;
	list_for_each_entry(pipeline, pipeline_t, pipeline_list, list) {
		logagent_plugin_config_load(&pipeline->plugin_list, pipeline->json);
	}

	return;
}

static void logagent_pipeline_plugin_config_unload(struct list_head *pipeline_list)
{
	pipeline_t *pipeline;
	list_for_each_entry(pipeline, pipeline_t, pipeline_list, list) {
		logagent_plugin_config_unload(&pipeline->plugin_list);
	}

	return;
}

void logagent_pipeline_config_load(struct list_head *pipeline_list, const char *json)
{
	struct json_object *pipeline_nums_obj;
	struct json_object *pipeline_obj;
	json_bool ret;
	
	logagent_pipeline_list_init(pipeline_list);

	struct json_object *root_obj = json_tokener_parse(json);
	if(root_obj == NULL) {
		LOGAGENT_LOG_FATAL("can't parse config json string: %s\n", json);
		return;
	}
	
	ret = json_object_object_get_ex(root_obj, "pipeline_nums", &pipeline_nums_obj);
	if (ret == false) {
		LOGAGENT_LOG_FATAL("can't get pipeline nums from json: %s\n", json);
		goto err_json_parse;
	}
	
	int pipeline_nums = json_object_get_int(pipeline_nums_obj);

	for (int i = 0; i < pipeline_nums; i++) {
		char pipeline_name[20] = { 0 };
		sprintf(pipeline_name, "pipeline@%d", i);

		ret = json_object_object_get_ex(root_obj, pipeline_name, &pipeline_obj);
		if (ret == false) {
			LOGAGENT_LOG_ERROR("can't found %s in json config, please check\n", pipeline_name);
		} else {
			logagent_pipeline_list_add(pipeline_list, json_object_to_json_string(pipeline_obj));
		}
	}

	json_object_put(pipeline);
	
	logagent_pipeline_plugin_config_load(pipeline_list);

	return;

err_json_parse:
	json_object_put(root_obj);

	return;
}

void logagent_pipeline_config_unload(struct list_head *pipeline_list)
{
	logagent_pipeline_plugin_config_unload(pipeline_list);

	logagent_pipeline_list_destroy(pipeline_list);
}
