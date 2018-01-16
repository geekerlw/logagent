#include <stdlib.h>
#include <string.h>

#include "logagent-plugin-api.h"

void logagent_log_add(struct list_head *log_list, const char *log)
{
	if (!log)
		return;

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
