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
#include <dlfcn.h>
#include <json.h>

#include "logagent.h"
#include "logagent-log.h"
#include "logagent-list.h"
#include "logagent-plugin.h"

static void logagent_plugin_list_add(struct list_head *plugin_list, const char *json)
{
	plugin_t *pdata = (plugin_t *) malloc(sizeof(plugin_t));
	if (!pdata)
		return;
	memset(pdata, 0, sizeof(pdata));

	memcpy(pdata->json, json, strlen(json) + 1);

	list_append(plugin_list, &pdata->list);

	return;
}

static void logagent_plugin_list_remove(struct list_head *pos)
{
	if (pos == pos->next)
		return;

	plugin_t *pdata = list_entry(pos, plugin_t, list);

	list_remove(pos);

	free(pdata);

	return;
}

static void logagent_plugin_list_init(struct list_head *plugin_list)
{
	list_init(plugin_list);
}

static void logagent_plugin_list_destroy(struct list_head *plugin_list)
{
	struct list_head *pos, *n;
	list_for_each_safe(pos, n, plugin_list) {
		plugin_t *node = list_entry(pos, plugin_t, list);
		list_remove(pos);
		free(node);
		pos = n;
	}

	return;
}

static void logagent_plugin_env_init(plugin_t *plugin)
{
	int ret = -1;
	
	plugin->context = (void *)&plugin->json;

	ret = plugin->env_init(plugin->context);
	
	if (ret != 0) {
		LOGAGENT_LOG_ERROR("[%s]->init() call error, return: %d\n", plugin->name, ret);
	}
	
	plugin->config = *plugin->context;

	return;
}

void logagent_plugin_env_init_all(struct list_head *plugin_list)
{
	plugin_t *plugin;
	list_for_each_entry(plugin, plugin_t, plugin_list, list) {
		logagent_plugin_env_init(plugin);
	}

	return;
}

static void logagent_plugin_env_destroy(plugin_t *plugin)
{
	int ret = -1;
	plugin->context = (void *)&plugin->config;
	
	ret = plugin->env_destroy(plugin->context);
	if (ret != 0) {
		LOGAGENT_LOG_ERROR("[%s]->exit() call error, return: %d\n", plugin->name, ret);
	}

	return;
}

void logagent_plugin_env_destroy_all(struct list_head *plugin_list)
{
	plugin_t *plugin;
	list_for_each_entry(plugin, plugin_t, plugin_list, list) {
		logagent_plugin_env_destroy(plugin);
	}
	
	return;
}

static void logagent_plugin_load(plugin_t *plugin)
{
	char lib_path[PLUGIN_LIB_PATH_SIZE] = { 0 };
	json_object *plugin_path_obj, *plugin_name_obj;
	json_bool ret;

	json_object *plugin_obj = json_tokener_parse(plugin->json);
	if (!plugin_obj) {
		LOGAGENT_LOG_FATAL("can't parse config json string: %s\n", plugin->json);
		return;
	}

	ret = json_object_object_get_ex(plugin_obj, "plugin_path", &plugin_path_obj);
	if (ret == false) {
		LOGAGENT_LOG_FATAL("can't get plugin path from json: %s\n", plugin->json);
		goto err_json_parse;
	}
	
	ret = json_object_object_get_ex(plugin_obj, "plugin_name", &plugin_name_obj);
	if (ret == false) {
		LOGAGENT_LOG_FATAL("can't get plugin name from json: %s\n", plugin->json);
		goto err_json_parse;
	}

	sprintf(plugin->path, "%s", json_object_get_string(plugin_path_obj));
	sprintf(plugin->name, "liblogagent-plugin-%s.so", json_object_get_string(plugin_name_obj));

	json_object_put(plugin_obj);

	memcpy(lib_path, plugin->path, sizeof(plugin->path));
	memcpy(lib_path + strlen(plugin->path), plugin->name, sizeof(plugin->name));

	plugin->lib_handle = dlopen(lib_path, RTLD_LAZY);

	if (plugin->lib_handle == NULL) {
		LOGAGENT_LOG_FATAL("failed to open dynamic library: %s\n", lib_path);
		return;
	}

	plugin->env_init = dlsym(plugin->lib_handle, "logagent_plugin_env_init");
	plugin->init = dlsym(plugin->lib_handle, "logagent_plugin_init");
	plugin->work = dlsym(plugin->lib_handle, "logagent_plugin_work");
	plugin->exit = dlsym(plugin->lib_handle, "logagent_plugin_exit");
	plugin->env_destroy = dlsym(plugin->lib_handle, "logagent_plugin_env_destroy");

	return;

err_json_parse:
	json_object_put(plugin_obj);

	return;
}

static void logagent_plugin_load_all(struct list_head *plugin_list)
{
	plugin_t *plugin;
	list_for_each_entry(plugin, plugin_t, plugin_list, list) {
		logagent_plugin_load(plugin);
	}

	return;
}

static void logagent_plugin_unload(plugin_t *plugin)
{
	if (plugin->lib_handle) {
		dlclose(plugin->lib_handle);
	}

	return;
}

static void logagent_plugin_unload_all(struct list_head *plugin_list)
{
	plugin_t *plugin;
	list_for_each_entry(plugin, plugin_t, plugin_list, list) {
		logagent_plugin_unload(plugin);
	}

	return;
}

void logagent_plugin_config_load(struct list_head *plugin_list, const char *json)
{
	struct json_object *plugin_nums_obj;
	struct json_object *plugin_obj;
	json_bool ret;
	
	logagent_plugin_list_init(plugin_list);

	struct json_object *root_obj = json_tokener_parse(json);
	if (root_obj == NULL) {
		LOGAGENT_LOG_FATAL("can't parse config json string: %s\n", json);
		return;
	}

	ret = json_object_object_get_ex(root_obj, "plugin_nums", &plugin_nums_obj);
	if (ret ==  false) {
		LOGAGENT_LOG_FATAL("can't get plugin nums from json: %s\n", json);
		goto err_json_parse;
	}
	
	int plugin_nums = json_object_get_int(plugin_nums_obj);

	for (int i = 0; i < plugin_nums; i++) {
		char plugin_name[20] = { 0 };
		sprintf(plugin_name, "plugin@%d", i);

		ret = json_object_object_get_ex(root_obj, plugin_name, &plugin_obj);
		if (ret == false) {
			LOGAGENT_LOG_ERROR("can't found %s in json config, please check\n", plugin_name);
		}else {
			logagent_plugin_list_add(plugin_list, json_object_to_json_string(plugin_obj));
		}
	}

	json_object_put(root_obj);

	logagent_plugin_load_all(plugin_list);

	return;

err_json_parse:
	json_object_put(root_obj);

	return;
}

/* plugin config unload */
void logagent_plugin_config_unload(struct list_head *plugin_list)
{
	logagent_plugin_unload_all(plugin_list);
	logagent_plugin_list_destroy(plugin_list);
}
