#ifndef __LOGAGENT_PLUGIN_FILESINK_H__
#define __LOGAGENT_PLUGIN_FILESINK_H__

#define FILESINK_BUF_SIZE	256

typedef struct {
	char filepath[FILESINK_BUF_SIZE];
}filesink_t;

#endif
