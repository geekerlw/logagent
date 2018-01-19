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

#ifndef __LOGAGENT_LOG_H__
#define __LOGAGENT_LOG_H__

enum ENUM_LOG_LEVEL {
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
};

#define LOGAGENT_LOG_DEBUG(x, ...) 	logagent_log_write(LOG_LEVEL_DEBUG, __FUNCTION__, x, __VA_ARGS__)
#define LOGAGENT_LOG_INFO(x, ...) 	logagent_log_write(LOG_LEVEL_INFO, __FUNCTION__, x, __VA_ARGS__)
#define LOGAGENT_LOG_WARN(x, ...)	logagent_log_write(LOG_LEVEL_WARN, __FUNCTION__, x, __VA_ARGS__)
#define LOGAGENT_LOG_ERROR(x, ...)	logagent_log_write(LOG_LEVEL_ERROR, __FUNCTION__, x, __VA_ARGS__)
#define LOGAGENT_LOG_FATAL(x, ...)	logagent_log_write(LOG_LEVEL_FATAL, __FUNCTION__, x, __VA_ARGS__)

void logagent_log_write(int level, const char *function, const char *format,
			...);

#endif
