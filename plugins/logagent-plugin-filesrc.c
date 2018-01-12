#include <stdio.h>
#include <string.h>
#include <json-c/json.h>

#include "logagent-plugin-api.h"
#include "logagent-plugin-filesrc.h"

static const char *filesrc_text = "welcome to filesrc";

static void filesrc_work(filesrc_t *filesrc, void **context)
{
	char *tmp = (char *)malloc(sizeof(char) * 100);
	memcpy(tmp, filesrc_text, strlen(filesrc_text) + 1);
	
	char *p = (char *)context;

	printf("bring into filesrc: %s\n", p);

	*context =(void *)tmp;

	printf("bring out filesrc: %s\n", (char *)*context);

	return;
}

static void filesrc_init(const char *json, void **context)
{
	if (!json)
		return;

	filesrc_t *filesrc = (filesrc_t *) malloc(sizeof(filesrc_t));
	if (!filesrc)
		return;

	memset(filesrc, 0, sizeof(filesrc));
	
	struct json_object *plugin_obj = json_tokener_parse(json);
	if (!plugin_obj)
		return;

	/* get filepath key-value */
	struct json_object *filepath_obj = json_object_object_get(plugin_obj, "path");
	const char *filepath = json_object_get_string(filepath_obj);
	memcpy(filesrc->filepath, filepath, strlen(filepath) + 1);

	*context = (void *)filesrc;

	json_object_put(plugin_obj);

	return;
}

static void filesrc_exit(filesrc_t *filesrc)
{
	//if (filesrc)
	//	free(filesrc);
}

void logagent_plugin_work(void *config, void **context)
{
	filesrc_t *filesrc = (filesrc_t *)config;

	filesrc_work(filesrc, context);

	return;
}

void logagent_plugin_init(void **context)
{
	char *json = (char *)context;

	filesrc_init(json, context);

	return;
}

void logagent_plugin_exit(void **context)
{
	filesrc_t *filesrc = (filesrc_t *)*context;

	filesrc_exit(filesrc);

	return;
}
