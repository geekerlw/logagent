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

#ifndef __LOGAGENT_ELEMENT_H__
#define __LOGAGENT_ELEMENT_H__


#define ELEMENT_JSON_SIZE		1024

typedef struct {
	char json[ELEMENT_JSON_SIZE];	/**< element json string */
	void *config;	/**< plugin independent config structure */

	void **context; /**< context for elements */

	struct list_head *pos;	/**< plugin position */

	struct list_head list; /**< list header */
} element_t;

/**
 * call work functions for all elements
 * @param element_list a element list of pipeline
 */
void logagent_element_work_all(struct list_head *element_list);

/**
 * call init functions for all elements
 * @param element_list a element list of pipeline
 */
void logagent_element_init_all(struct list_head *element_list);

/**
 * call exit functions for all elements
 * @param element_list a element list of pipeline
 */
void logagent_element_exit_all(struct list_head *element_list);

/**
 * load elements config from json string
 * @param plugin_list logagent's plugin store
 * @param element_list a element list of pipeline
 * @param json the pipeline's config json string
 */
void logagent_element_config_load(struct list_head *plugin_list,
				  struct list_head *element_list,
				  const char *json);

/**
 * unload elements config of a pipeline
 * @param element_list a element list of pipeline
 */
void logagent_element_config_unload(struct list_head *element_list);

#endif
