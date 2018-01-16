#include <stdio.h>
#include <json.h>

#include "logagent-plugin-api.h"
#include "log_producer_config.h"
#include "log_producer_client.h"

#define ALIYUN_BUF_SIZE	256

typedef struct {
	char config[ALIYUN_BUF_SIZE];
	log_producer *producer;
	log_producer_client *client;
}aliyun_t;

void aliyun_log_work(aliyun_t *aliyun, struct list_head *log_list)
{
	log_t *pos, *n;
	printf("run into aliyun\n");
	list_for_each_entry_safe(pos, log_t, n, log_list, list) {
		LOG_PRODUCER_INFO(aliyun->client, "content", pos->log);
		logagent_log_remove(&pos->list);
		printf("remove log from list\n");
		pos = n;
	}
	
	return;
}

void aliyun_log_init(const char *json, void **context)
{
	if (log_producer_env_init() != LOG_PRODUCER_OK)
		return;

	aliyun_t *aliyun = (aliyun_t *) malloc(sizeof(aliyun_t));
	if (!aliyun)
		return;
	memset(aliyun, 0, sizeof(aliyun));

	struct json_object *plugin_obj = json_tokener_parse(json);
	if (!plugin_obj)
		return;

	struct json_object *config_obj = json_object_object_get(plugin_obj, "logstore_config");
	const char *config = json_object_get_string(config_obj);
	memcpy(aliyun->config, config, strlen(config) + 1);

	json_object_put(plugin_obj);

	log_producer *producer = create_log_producer_by_config_file(config, NULL);
	if (!producer)
		return;

	log_producer_client *client = get_log_producer_client(producer, NULL);
	if (!client)
		return;

	aliyun->producer = producer;
	aliyun->client = client;

	*context = (void *)aliyun;

	return;
}

void aliyun_log_exit(aliyun_t *aliyun)
{
	destroy_log_producer(aliyun->producer);
	log_producer_env_destroy();
}

void logagent_plugin_work(void *config, struct list_head *log_list)
{
	aliyun_t *aliyun = (aliyun_t *)config;

	aliyun_log_work(aliyun, log_list);

	return;
}

void logagent_plugin_init(void **context)
{
	char *json = (char *)context;

	aliyun_log_init(json, context);

	return;
}

void logagent_plugin_exit(void **context)
{
	aliyun_t *aliyun = (aliyun_t *)*context;

	aliyun_log_exit(aliyun);

	return;
}
