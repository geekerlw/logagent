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
#include <string.h>
#include <pthread.h>
#include <sys/signal.h>

#include "logagent-list.h"
#include "logagent-config.h"
#include "logagent-pipeline.h"
#include "logagent-log.h"
#include "logagent.h"

static struct list_head pipeline_list;
static struct list_head plugin_list;

static void logagent_help()
{
	printf("usage: logagent [option] [...]\n");
	printf("\t logagent -f <config file>\n");

	return;
}

static void logagent_work()
{
	pipeline_t *pipeline;
	list_for_each_entry(pipeline, pipeline_t, &pipeline_list, list) {
		pthread_create(&pipeline->thread_id, NULL,
			       (void *)logagent_pipeline_work, pipeline);
	}

	list_for_each_entry(pipeline, pipeline_t, &pipeline_list, list) {
		pthread_join(pipeline->thread_id, NULL);
	}

	return;
}

static int logagent_init(const char *filename)
{
	char json_config[MAX_JSON_BUF] = { 0 };

	logagent_need_exit = false;

	int ret = logagent_config_load_from_file(filename, json_config);

	if (ret != 0) {
		LOGAGENT_LOG_ERROR("failed load json config from: %s\n",
				   filename);
		return -1;
	}

	logagent_plugin_config_load(&plugin_list, json_config);

	logagent_plugin_env_init_all(&plugin_list);

	logagent_pipeline_config_load(&pipeline_list, json_config);

	logagent_pipeline_element_config_load(&plugin_list, &pipeline_list);

	return 0;
}

static void logagent_exit()
{
	logagent_pipeline_element_config_unload(&pipeline_list);

	logagent_pipeline_config_unload(&pipeline_list);

	logagent_plugin_env_destroy_all(&plugin_list);

	logagent_plugin_config_unload(&plugin_list);

	return;
}

static void logagent_signal_handler(int sig)
{
	switch (sig) {
	case SIGTERM:
	case SIGINT:
		logagent_need_exit = true;
		break;
	default:
		break;
	}

	return;
}

int main(int argc, char *argv[])
{
	char *filename = NULL;

	if (argc == 1) {
		printf("Try 'loagent --help' for more infomation\n");
		return 0;
	}

	if (strncmp(argv[1], "--help", sizeof("--help")) == 0
	    || strncmp(argv[1], "-h", sizeof("-h")) == 0) {
		logagent_help();
		return 0;
	} else if (strncmp(argv[1], "-f", sizeof("-f")) == 0) {
		filename = argv[2];
	} else {
		logagent_help();
		return 0;
	}

	logagent_init(filename);

	signal(SIGINT, logagent_signal_handler);
	signal(SIGTERM, logagent_signal_handler);

	logagent_work();

	logagent_exit();

	return 0;
}
