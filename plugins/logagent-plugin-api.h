#ifndef __LOGAGENT_PLUGIN_API_H__
#define __LOGAGENT_PLUGIN_API_H__

#include "logagent-plugin-list.h"

#define MAX_LOG_SIZE	1024

typedef struct {
	char log[MAX_LOG_SIZE];
	struct list_head list;
}log_t;

void logagent_log_add(struct list_head *log_list, const char *log);

void logagent_log_remove(struct list_head *pos);

/* 
 * context take something in and take another out
 */

extern void logagent_plugin_work(void *config, struct list_head *log_list);

extern void logagent_plugin_init(void **context);

extern void logagent_plugin_exit(void **context);

#endif
