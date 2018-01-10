#include <stdio.h>
#include <json-c/json.h>

void logagent_config_load_from_file(const char *filename, char *json)
{
	struct json_object *root = NULL;

	if (!filename || !json)
		return;

	root = json_object_from_file(filename);
	if(!root)
		return;

	sprintf(json, "%s", json_object_to_json_string(root));

	json_object_put(root);

	return;
}

void logagent_config_load_from_network(const char *url, char *json)
{

	return;
}
