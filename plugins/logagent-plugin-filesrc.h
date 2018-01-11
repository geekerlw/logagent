#ifndef __LOGAGENT_PLUGIN_FILESRC_H__
#define __LOGAGENT_PLUGIN_FILESRC_H__

#define FILESRC_BUF_SIZE	256

typedef struct {
	char filepath[FILESRC_BUF_SIZE];
}filesrc_t;

extern void logagent_plugin_work(void *context);

extern void logagent_plugin_init(void *context);

extern void logagent_plugin_exit(void *context);

#endif
