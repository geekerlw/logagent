#ifndef __LOGAGENT_PLUGIN_FILESRC_H__
#define __LOGAGENT_PLUGIN_FILESRC_H__

#define FILESRC_BUF_SIZE	256

typedef struct {
	char filepath[FILESRC_BUF_SIZE];
}filesrc_t;

#endif
