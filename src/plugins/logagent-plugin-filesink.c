#include <stdio.h>
#include <string.h>
#include <json-c/json.h>

#include "logagent-plugin-filesink.h"

static void filesink_work(filesink_t *filesink, void *context)
{
	if (!context || filesink)
		return;

	printf("filesink's filepath is: %s\n", filesink->filepath);

	char *log = (char *)context;

	printf("filesink success get filesrc log buf: %s\n", log);
	
	if (log)
		free(log);

	return;
}

static void filesink_init(const char *json, void *context)
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
	memcpy(filesink->filepath, filepath, sizeof(filepath));

	json_object_put(plugin_obj);

	context = (void *)filesink;

	return;
}

static void filesink_exit(filesink_t *filesink)
{
	printf("filesink struct's path is: %s\n", filesink->filepath);

	if (filesink)
		free(filesink);
}

/*
 * context -- config in
 * context -- log buffer out
 */
void logagent_plugin_work(void *context)
{
	filesink_t *filesink = (filesink_t *)context;

	filesink_work(filesink, context);

	return;
}

/* 
 * context -- json in
 * context -- config out
 */
void logagent_plugin_init(void *context)
{
	char *json = (char *)context;

	filesink_init(json, context);

	return;
}

/*
 * context -- config in
 * context -- null out
 */
void logagent_plugin_exit(void *context)
{
	filesink_t *filesink = (filesink_t *)context;

	filesink_exit(filesink);

	return;
}
