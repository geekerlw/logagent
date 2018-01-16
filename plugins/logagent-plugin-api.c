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

#include <stdlib.h>
#include <string.h>

#include "logagent-plugin-api.h"

void logagent_log_add(struct list_head *log_list, const char *log)
{
	log_t *pdata = (log_t *) malloc(sizeof(log_t));
	if (!pdata)
		return;
	memset(pdata, 0, sizeof(pdata));

	memcpy(pdata->log, log, strlen(log) + 1);

	list_append(log_list, &pdata->list);

	return;
}

void logagent_log_remove(struct list_head *pos)
{
	if (pos == pos->next)
		return;

	log_t *pdata = list_entry(pos, log_t, list);

	list_remove(pos);

	free(pdata);

	return;
}
