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

#ifndef __LOGAGENT_CONFIG_H__
#define __LOGAGENT_CONFIG_H__

enum ENUM_CONFIG_LOCATION {
	CONFIG_LOCAL,
	CONFIG_NETWORK,
	CONFIG_ALIYUN
};

int logagent_config_load_from_file(const char *filename, char *json);

int logagent_config_load_from_network(const char *url, char *json);

int logagent_config_load_from_aliyun(const char *url, char *json);

#endif
