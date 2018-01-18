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
#include <stdbool.h>
#include <string.h>
#include <json.h>

#include "logagent-log.h"
#include "logagent-list.h"
#include "logagent-plugin.h"


static void logagent_element_list_add(struct list_head *element_list, const char *json)
{
	element_t *pdata = (element_t *) malloc(sizeof(element_t));
	if (!pdata)
		return;
	memset(pdata, 0, sizeof(pdata));

	memcpy(pdata->json, json, strlen(json) + 1);

	list_append(element_list, &pdata->list);

	return;
}

static void logagent_element_list_remove(struct list_head *pos)
{
	if (pos == pos->next)
		return;

	element_t *pdata = list_entry(pos, element_t, list);

	list_remove(pos);

	free(pdata);

	return;
}

static void logagent_element_list_init(struct list_head *element_list)
{
	list_init(element_list);
}

static void logagent_element_list_destroy(struct list_head *element_list)
{
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, element_list) {
		element_t *node = list_entry(pos, element_t, list);
		list_remove(pos);
		free(node);
		pos = n;
	}

	return;
}

static void logagent_element_init(element_t *element)
{
	int ret = -1;

	plugin_t *plugin = list_entry(element->pos, plugin_t, list);
	
	element->context = (void *)&element->json;

	ret = plugin->init(plugin->config, element->context);
	if (ret != 0) {
		LOGAGENT_LOG_ERROR("[%s]->init() call error, return: %d\n", plugin->name, ret);
	}
	
	element->config = *element->context;

	return;
}

void logagent_element_init_all(struct list_head *element_list)
{
	element_t *element;
	list_for_each_entry(element, element_t, element_list, list) {
		logagent_element_init(element);
	}

	return;
}

static void logagent_element_exit(element_t *element)
{
	int ret = -1;

	plugin_t *plugin = list_entry(element->pos, plugin_t, list);
	
	element->context = (void *)&element->config;
	
	ret = plugin->exit(plugin->config, element->context);
	if (ret != 0) {
		LOGAGENT_LOG_ERROR("[%s]->exit() call error, return: %d\n", plugin->name, ret);
	}

	return;
}

void logagent_element_exit_all(struct list_head *element_list)
{
	element *element;
	list_for_each_entry(element, element_t, element_list, list) {
		logagent_element_exit(element);
	}
	
	return;
}

static void logagent_element_work(element_t *element, struct list_head *log_list)
{
	int ret = -1;

	plugin_t *plugin = list_entry(element->pos, plugin_t, list);
	
	ret = plugin->work(plugin->config, element->config, log_list);
	if (ret != 0) {
		LOGAGENT_LOG_ERROR("[%s]->work() call error, return: %d\n", plugin->name, ret);
	}

	return;
}

void logagent_element_work_all(struct list_head *element_list)
{
	element_t *element;
	struct list_head log_list;

	list_init(&log_list);

	list_for_each_entry(element, element_t, element_list, list) {
		logagent_element_work(element, &log_list);
	}
	
	return;
}

static void logagent_element_load(struct list_head *plugin_list, element_t *element)
{
	char plugin_name[PLUGIN_LIB_NAME_SIZE] = { 0 };
	struct json_object *plugin_name_obj;
	json_bool ret;

	struct json_object *root_obj = json_tokener_parse(element->json);
	if (!root_obj) {
		LOGAGENT_LOG_FATAL("can't parse config json string: %s\n", element->json);
		return;
	}

	ret = json_object_object_get_ex(root_obj, "plugin_name", &plugin_name_obj);
	if (ret == false) {
		LOGAGENT_LOG_FATAL("can't get plugin name from json: %s\n", element->json);
		goto err_json_parse;
	}

	sprintf(plugin_name, "liblogagent-plugin-%s.so", json_object_get_string(plugin_name_obj));

	plugin_t *plugin;
	list_for_each_entry(plugin, plugin_t, &plugin_list, list) {
		if (strncmp(plugin->name, plugin_name, sizeof(plugin_name)) == 0) {
			element->pos = &plugin->list;
			break;
		}
	}
	

err_json_parse:
	json_object_put(root_obj);

	return;
}

static void logagent_element_load_all(struct list_head *plugin_list, struct list_head *element_list)
{
	element *element;
	list_for_each_entry(element, element_t, element_list, list) {
		logagent_element_load(plugin_list, element);
	}

	return;
}

static void logagent_element_unload(element_t *element)
{

	return;
}

static void logagent_element_unload_all(struct list_head *element_list)
{
	element_t *element;
	list_for_each_entry(element, element_t, element_list, list) {
		logagent_element_unload(element);
	}

	return;
}

void logagent_element_config_load(struct list_head *plugin_list, struct list_head *element_list, const char *json)
{
	struct json_object *element_nums_obj;
	struct json_object *element_obj;
	json_bool ret;
	
	logagent_element_list_init(element_list);

	struct json_object *root_obj = json_tokener_parse(json);
	if (root_obj == NULL) {
		LOGAGENT_LOG_FATAL("can't parse config json string: %s\n", json);
		return;
	}

	ret = json_object_object_get_ex(root_obj, "element_nums", &element_nums_obj);
	if (ret ==  false) {
		LOGAGENT_LOG_FATAL("can't get element nums from json: %s\n", json);
		goto err_json_parse;
	}
	
	int element_nums = json_object_get_int(element_nums_obj);

	for (int i = 0; i < element_nums; i++) {
		char element_name[20] = { 0 };
		sprintf(element_name, "element@%d", i);

		ret = json_object_object_get_ex(root_obj, element_name, &element_obj);
		if (ret == false) {
			LOGAGENT_LOG_ERROR("can't found %s in json config, please check\n", element_name);
		}else {
			logagent_element_list_add(element_list, json_object_to_json_string(element_obj));
		}
	}

	json_object_put(root_obj);

	logagent_element_load_all(plugin_list, element_list);

	return;

err_json_parse:
	json_object_put(root_obj);

	return;
}

void logagent_element_config_unload(struct list_head *element_list)
{
	logagent_element_unload_all(element_list);
	logagent_element_list_destroy(element_list);
}
