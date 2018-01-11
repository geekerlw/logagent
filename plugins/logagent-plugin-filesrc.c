#include <stdio.h>
#include <string.h>
#include <json-c/json.h>

#include "logagent-plugin-filesrc.h"

static const char *filesrc_text = "welcome geek's world";

static void filesrc_work(filesrc_t *filesrc, void *context)
{
	printf("filesrc's filepath is: %s\n", filesrc->filepath);

	char *tmp = (char *)malloc(sizeof(char) * 100);
	memcpy(tmp, filesrc_text, sizeof(filesrc_work));

	context = (void *)tmp;

	return;
}

static void filesrc_init(const char *json, void *context)
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
	memcpy(filesrc->filepath, filepath, sizeof(filepath));

	json_object_put(plugin_obj);

	context = (void *)filesrc;

	return;
}

static void filesrc_exit(filesrc_t *filesrc)
{
	printf("filesrc struct's path is: %s\n", filesrc->filepath);
	free(filesrc);
}

/*
 * context -- config in
 * context -- log buffer out
 */
void logagent_plugin_work(void *context)
{
	filesrc_t *filesrc = (filesrc_t *)context;

	filesrc_work(filesrc, context);

	return;
}

/* 
 * context -- json in
 * context -- config out
 */
void logagent_plugin_init(void *context)
{
	char *json = (char *)context;

	filesrc_init(json, context);

	return;
}

/*
 * context -- config in
 * context -- null out
 */
void logagent_plugin_exit(void *context)
{
	filesrc_t *filesrc = (filesrc_t *)context;

	filesrc_exit(filesrc);

	return;
}
