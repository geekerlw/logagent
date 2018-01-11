#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <json.h>

#include "logagent.h"
#include "logagent-list.h"
#include "logagent-plugin.h"

#define PLUGIN_LIB_PATH		"/usr/lib/logagent"

#define PLUGIN_LIB_PATH_SIZE	256
#define PLUGIN_LIB_NAME_SIZE	32

static void logagent_plugin_list_add(struct list_head *plugin_list, const char *json)
{
	if (!json)
		return;

	plugin_t *pdata = (plugin_t *) malloc(sizeof(plugin_t));
	if (!pdata)
		return;
	memset(pdata, 0, sizeof(pdata));

	memcpy(pdata->json, json, sizeof(json));

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

static void logagent_plugin_work(plugin_t *plugin, void *context)
{
	plugin->work(context);

	return;
}

/* run work for each plugin in list */
void logagent_plugin_work_all(struct list_head *plugin_list)
{
	plugin_t *plugin;
	list_for_each_entry(plugin, plugin_t, plugin_list, list) {
		logagent_plugin_work(plugin, plugin->context);
	}
	
	return;
}

static void logagent_plugin_init(plugin_t *plugin, char *json)
{
	plugin->context = json;

	plugin->init(plugin->context);

	plugin->config = plugin->context;

	return;
}

/* run init for each plugin in list */
void logagent_plugin_init_all(struct list_head *plugin_list)
{
	plugin_t *plugin;
	list_for_each_entry(plugin, plugin_t, plugin_list, list) {
		logagent_plugin_init(plugin, plugin->json);
	}

	return;
}

static void logagent_plugin_exit(plugin_t *plugin)
{
	plugin->context = plugin->config;

	plugin->exit(plugin->context);

	return;
}

/* run exit for each plugin in list */
void logagent_plugin_exit_all(struct list_head *plugin_list)
{
	plugin_t *plugin;
	list_for_each_entry(plugin, plugin_t, plugin_list, list) {
		logagent_plugin_exit(plugin);
	}
	
	return;
}

static void logagent_plugin_load(plugin_t *plugin)
{
	char plugin_lib_name[PLUGIN_LIB_NAME_SIZE] = { 0 };
	char plugin_lib_path[PLUGIN_LIB_PATH_SIZE] = { 0 };

	json_object *plugin_obj = json_tokener_parse(plugin->json);
	if (!plugin)
		return;

	json_object *plugin_name_obj = json_object_object_get(plugin_obj, "plugin_name");

	sprintf(plugin_lib_name, "lib%s.so", json_object_get_string(plugin_name_obj));

	memcpy(plugin_lib_path, PLUGIN_LIB_PATH, sizeof(PLUGIN_LIB_PATH));
	memcpy(plugin_lib_path + strlen(PLUGIN_LIB_PATH), plugin_lib_name, sizeof(plugin_lib_name));

	plugin->lib_handle = dlopen(plugin_lib_path, RTLD_LAZY);

	if ( plugin->lib_handle == NULL)
		return;

	plugin->init = dlsym(plugin->lib_handle, "logagent_plugin_init");
	plugin->work = dlsym(plugin->lib_handle, "logagent_plugin_work");
	plugin->exit = dlsym(plugin->lib_handle, "logagent_plugin_exit");

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
		logagent_plugin_load(plugin);
	}

	return;
}

/* plugin config load */
void logagent_plugin_config_load(struct list_head *plugin_list, const char *json)
{
	if (!json)
		return;
	
	logagent_plugin_list_init(plugin_list);

	struct json_object *plugin = json_tokener_parse(json);
	if (plugin == NULL)
		return;

	struct json_object *plugin_nums_obj = json_object_object_get(plugin, "plugin_nums");
	int plugin_nums = json_object_get_int(plugin_nums_obj);

	for (int i = 0; i < plugin_nums; i++) {
		char plugin_name[20] = { 0 };
		sprintf(plugin_name, "plugin@%d", i);

		struct json_object *plugin_obj = json_object_object_get(plugin, plugin_name);

		const char *plugin_config = json_object_to_json_string(plugin_obj);
		
		logagent_plugin_list_add(plugin_list, plugin_config);
	}

	json_object_put(plugin);

	logagent_plugin_load_all(plugin_list);

	return;
}

/* plugin config unload */
void logagent_plugin_config_unload(struct list_head *plugin_list)
{
	logagent_plugin_unload_all(plugin_list);
	logagent_plugin_list_destroy(plugin_list);
}
