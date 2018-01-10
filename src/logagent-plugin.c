#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#include "logagent.h"
#include "logagent-list.h"
#include "logagent-plugin.h"

static void logagent_plugin_list_add(struct list_head *plugin_list, const char *json)
{
	if (!json)
		return;

	plugin_t *pdata = (plugin_t *) malloc(sizeof(plugin_t));
	if (!pdata)
		return;
	memset(pdata, 0, sizeof(pdata));

	memcpy(pdata->json_config, json, sizeof(json));

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

static void logagent_plugin_init(plugin_t *plugin, const char *json)
{
	plugin->init(json);

	return;
}

/* run init for each plugin in list */
void logagent_plugin_init_all(struct list_head *plugin_list)
{
	plugin_t *plugin;
	list_for_each_entry(plugin, plugin_t, plugin_list, list) {
		logagent_plugin_init(plugin, plugin->json_config);
	}

	return;
}

static void logagent_plugin_exit(plugin_t *plugin)
{
	plugin->exit();

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

	return;
}

/* plugin config unload */
void logagent_plugin_config_unload(struct list_head *plugin_list)
{
	logagent_plugin_list_destroy(plugin_list);
}
