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

#ifndef __LOGAGENT_PIPELINE_H__
#define __LOGAGENT_PIPELINE_H__

#include "logagent-plugin.h"

#define PIPELINE_SLEEP_TIME	1000
#define MAX_PIPELINE_NUMS	10
#define PIPELINE_JSON_SIZE	(MAX_PIPELINE_NUMS * PLUGIN_JSON_SIZE)

typedef struct {
	char json[PIPELINE_JSON_SIZE];	/**< pipeline json config */

	pthread_t thread_id;	/**< each pipeline have a thread to run */

	struct list_head element_list;	/**< each pipeline have a plugin list */

	struct list_head list;	/**< pipeline list header*/
} pipeline_t;

/**
 * pipeline thread work wrapper
 * @param pipeline one pipeline to run
 *
 * this function will run init, work and exit for all elements, just create
 * a thread and send a pipeline argument in.
 */
void logagent_pipeline_work(pipeline_t * pipeline);

/**
 * load pipeline's elements config
 * @param plugin_list logagent's plugin store
 * @param pipeline_list a pipeline list
 */
void logagent_pipeline_element_config_load(struct list_head *plugin_list,
					   struct list_head *pipeline_list);

/**
 * unload pipeline's elements config
 * @param pipeline_list a pipeline list
 */
void logagent_pipeline_element_config_unload(struct list_head *pipeline_list);

/**
 * load config for pipeline from json
 * @param pipeline_list a pipeline list
 * @param json the json config string
 */
void logagent_pipeline_config_load(struct list_head *pipeline_list,
				   const char *json);

/** unload config for pipeline
 * @param pipeline_list a pipeline list
 */
void logagent_pipeline_config_unload(struct list_head *pipeline_list);

#endif
