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
}aliyun_env_t;

typedef struct {
	char logstore[ALIYUN_BUF_SIZE];
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

static int aliyun_log_init(aliyun_env_t *env, const char *json, void **context)
{
	aliyun_t *aliyun = (aliyun_t *) malloc(sizeof(aliyun_t));
	if (!aliyun)
		return -2;
	memset(aliyun, 0, sizeof(aliyun));

	struct json_object *root_obj = json_tokener_parse(json);
	if (!root_obj)
		return -3;

	struct json_object *logstore_obj;
	json_bool ret = json_object_object_get_ex(root_obj, "logstore_name", &logstore_obj);
	if (ret == false) {
		json_object_put(root_obj);
		return -4;
	}

	const char *logstore_name = json_object_get_string(logstore_obj);
	memcpy(aliyun->logstore, logstore_name, strlen(logstore_name) + 1);

	json_object_put(root_obj);

	aliyun->client = get_log_producer_client(env->producer, aliyun->logstore);
	if (!aliyun->client)
		return -5;

	*context = (void *)aliyun;

	return 0;
}

static int aliyun_log_exit(aliyun_t *aliyun)
{
	return 0;
}

int aliyun_log_env_init(const char *json, void **context)
{
	if (log_producer_env_init() != LOG_PRODUCER_OK)
		return -1;

	aliyun_env_t *aliyun_env = (aliyun_env_t *) malloc(sizeof(aliyun_env_t));
	if (!aliyun_env)
		return -2;
	memset(aliyun_env, 0, sizeof(aliyun_env));

	struct json_object *root_obj = json_tokener_parse(json);
	if (!root_obj)
		return -3;

	struct json_object *config_obj;
	json_bool ret = json_object_object_get_ex(root_obj, "logstore_config", &config_obj);
	if (ret == false) {
		json_object_put(root_obj);
		return -4;
	}

	const char *config = json_object_get_string(config_obj);
	memcpy(aliyun_env->config, config, strlen(config) + 1);

	json_object_put(root_obj);

	aliyun_env->producer = create_log_producer_by_config_file(aliyun_env->config, NULL);
	if (!aliyun_env->producer)
		return -5;

	*context = (void *)aliyun_env;

	return 0;
}

int aliyun_log_env_destroy(aliyun_env_t *aliyun_env)
{
	destroy_log_producer(aliyun_env->producer);

    log_producer_env_destroy();

	return 0;
}

int logagent_plugin_work(void *gconfig, void *pconfig, struct list_head *log_list)
{
	aliyun_t *aliyun = (aliyun_t *)pconfig;
	
	return aliyun_log_work(aliyun, log_list);
}

int logagent_plugin_init(void *gconfig, void **context)
{
	aliyun_env_t *env = (aliyun_env_t *)gconfig;
	char *json = (char *)context;

	return aliyun_log_init(env, json, context);
}

int logagent_plugin_exit(void *gconfig, void **context)
{
	aliyun_t *aliyun = (aliyun_t *)*context;

	return aliyun_log_exit(aliyun);
}

int logagent_plugin_env_init(void **context)
{
	char *json = (char *)context;
	
	return aliyun_log_env_init(json, context);
}

int logagent_plugin_env_destroy(void **context)
{
	aliyun_env_t *env = (aliyun_env_t *)*context;

	return aliyun_log_env_destroy(env);
}
