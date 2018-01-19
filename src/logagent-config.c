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
#include <json.h>

#include "logagent-log.h"

int logagent_config_load_from_file(const char *filename, char *json)
{
	struct json_object *root = NULL;

	root = json_object_from_file(filename);
	if (root == NULL) {
		LOGAGENT_LOG_FATAL("can't load json config from file: %s\n",
				   filename);
		return -1;
	}

	sprintf(json, "%s", json_object_to_json_string(root));

	json_object_put(root);

	return 0;
}

int logagent_config_load_from_network(const char *url, char *json)
{

	return 0;
}

int logagent_config_load_from_aliyun(const char *url, char *json)
{
	return 0;

}
