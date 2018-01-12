#include <stdio.h>
#include <string.h>
#include <json-c/json.h>

#include "logagent-plugin-api.h"
#include "logagent-plugin-filesink.h"

static const char *filesink_text = "welcome to filesink";

static void filesink_work(filesink_t *filesink, void **context)
{
	//printf("filesink filepath is : %s\n", filesink->filepath);

	char *p = (char *)*context;

	printf("bring into filesink: %s\n", p);

	*context = (void *)filesink_text;

	printf("bring out filesink: %s\n", (char *)*context);
	
	if (p)
		free(p);

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

void logagent_plugin_work(void *config, void **context)
{
	filesink_t *filesink = (filesink_t *)config;

	filesink_work(filesink, context);

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
