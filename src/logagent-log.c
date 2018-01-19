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
#include <stdarg.h>

#include "logagent-log.h"

void logagent_log_write(int level, const char *function, const char *format,
			...)
{
	va_list argv;

	char *log_level;
	switch (level) {
	case LOG_LEVEL_DEBUG:
		log_level = "DEBUG";
		break;
	case LOG_LEVEL_INFO:
		log_level = "INFO";
		break;
	case LOG_LEVEL_WARN:
		log_level = "WARN";
		break;
	case LOG_LEVEL_ERROR:
		log_level = "ERROR";
		break;
	default:
		log_level = "DEBUG";
		break;
	};

	va_start(argv, format);

	vfprintf(stderr, format, argv);

	va_end(argv);

	return;
}
