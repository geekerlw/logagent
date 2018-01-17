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
#include <stdbool.h>
#include <json.h>

#include "log_producer_config.h"
#include "log_producer_client.h"

#include "logagent-plugin-api.h"


#define ALIYUN_BUF_SIZE	256

typedef struct {
	char config[ALIYUN_BUF_SIZE];
	log_producer *producer;
	log_producer_client *client;
}aliyun_t;

static int aliyun_log_work(aliyun_t *aliyun, struct list_head *log_list)
{
	log_t *pos, *n;
	list_for_each_entry_safe(pos, log_t, n, log_list, list) {
		log_producer_client_add_log(aliyun->client, 2, "content", pos->log);
		logagent_log_remove(&pos->list);
		pos = n;
	}
	
	return 0;
}

static int aliyun_log_init(const char *json, void **context)
{
	if (log_producer_env_init() != LOG_PRODUCER_OK)
		return -1;

	aliyun_t *aliyun = (aliyun_t *) malloc(sizeof(aliyun_t));
	if (!aliyun)
		return -2;
	memset(aliyun, 0, sizeof(aliyun));

	struct json_object *plugin_obj = json_tokener_parse(json);
	if (!plugin_obj)
		return -3;

	struct json_object *config_obj;
	json_bool ret = json_object_object_get_ex(plugin_obj, "logstore_config", &config_obj);
	if (ret == false) {
		json_object_put(plugin_obj);
		return -4;
	}
	
	const char *config = json_object_get_string(config_obj);
	memcpy(aliyun->config, config, strlen(config) + 1);

	json_object_put(plugin_obj);

	aliyun->producer = create_log_producer_by_config_file(aliyun->config, NULL);
	if (!aliyun->producer)
		return -5;

	aliyun->client = get_log_producer_client(aliyun->producer, NULL);
	if (!aliyun->client)
		return -6;

	*context = (void *)aliyun;

	return 0;
}

static int aliyun_log_exit(aliyun_t *aliyun)
{
	if (aliyun->producer) {
		destroy_log_producer(aliyun->producer);
	}
	log_producer_env_destroy();

	return 0;
}

int logagent_plugin_work(void *config, struct list_head *log_list)
{
	aliyun_t *aliyun = (aliyun_t *)config;
	
	return aliyun_log_work(aliyun, log_list);
}

int logagent_plugin_init(void **context)
{
	char *json = (char *)context;

	return aliyun_log_init(json, context);
}

int logagent_plugin_exit(void **context)
{
	aliyun_t *aliyun = (aliyun_t *)*context;

	return aliyun_log_exit(aliyun);
}
