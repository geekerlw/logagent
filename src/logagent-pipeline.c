#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <json.h>

#include "logagent.h"
#include "logagent-list.h"
#include "logagent-pipeline.h"

static void logagent_pipeline_list_add(struct list_head *pipeline_list, const char *json)
{
	if (!json)
		return;

	pipeline_t *pdata = (pipeline_t *) malloc(sizeof(pipeline_t));
	if (!pdata)
		return;
	memset(pdata, 0, sizeof(pdata));

	memcpy(pdata->json, json, sizeof(json));

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

/* pipeline init */
static void logagent_pipeline_init(pipeline_t *pipeline)
{
	logagent_plugin_init_all(&pipeline->plugin_list);

	return;
}

/* list each pipeline and call plugin init */
static void logagent_pipeline_init_all(struct list_head *pipeline_list)
{
	pipeline_t *pipeline;
	list_for_each_entry(pipeline, pipeline_t, pipeline_list, list) {
		logagent_pipeline_init(pipeline);
	}

	return;
}

/* pipeline exit */
static void logagent_pipeline_exit(pipeline_t *pipeline)
{
	logagent_plugin_exit_all(&pipeline->plugin_list);

	return;
}

/* list each pipeline and call plugin exit */
static void logagent_pipeline_exit_all(struct list_head *pipeline_list)
{
	pipeline_t *pipeline;
	list_for_each_entry(pipeline, pipeline_t, pipeline_list, list) {
		logagent_pipeline_exit(pipeline);
	}

	return;
}

/* pipeline main work flow */
void logagent_pipeline_work(pipeline_t *pipeline)
{
	logagent_pipeline_init(pipeline);

	while (!logagent_need_exit) {
		usleep(100);

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

/* pipeline config load */
void logagent_pipeline_config_load(struct list_head *pipeline_list, const char *json)
{
	if (!json)
		return;

	logagent_pipeline_list_init(pipeline_list);

	struct json_object *pipeline = json_tokener_parse(json);
	if(pipeline == NULL)
		return;
	
	struct json_object *pipeline_nums_obj = json_object_object_get(pipeline, "pipeline_nums");
	int pipeline_nums = json_object_get_int(pipeline_nums_obj);

	/* parse pipeline config and add to list */
	for (int i = 0; i < pipeline_nums; i++) {
		char pipeline_name[20] = { 0 };
		sprintf(pipeline_name, "pipeline@%d", i);

		/* get pipeline json string */
		struct json_object *pipeline_obj;
		pipeline_obj = json_object_object_get(pipeline, pipeline_name);

		const char *pipeline_config = json_object_to_json_string(pipeline_obj);

		logagent_pipeline_list_add(pipeline_list, pipeline_config);

	}

	json_object_put(pipeline);
	

	logagent_pipeline_plugin_config_load(pipeline_list);

	return;
}

void logagent_pipeline_config_unload(struct list_head *pipeline_list)
{
	logagent_pipeline_plugin_config_unload(pipeline_list);

	logagent_pipeline_list_destroy(pipeline_list);
}
