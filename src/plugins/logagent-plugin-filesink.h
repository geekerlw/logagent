#ifndef __LOGAGENT_PLUGIN_FILESINK_H__
#define __LOGAGENT_PLUGIN_FILESINK_H__

#define FILESINK_BUF_SIZE	256

typedef struct {
	char filepath[FILESINK_BUF_SIZE];
}filesink_t;

extern void logagent_plugin_work(void *context);

extern void logagent_plugin_init(void *context);

extern void logagent_plugin_exit(void *context);

#endif
