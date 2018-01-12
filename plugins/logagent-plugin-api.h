#ifndef __LOGAGENT_PLUGIN_API_H__
#define __LOGAGENT_PLUGIN_API_H__

/* 
 * context take something in and take another out
 */

extern void logagent_plugin_work(void *config, void **context);

extern void logagent_plugin_init(void **context);

extern void logagent_plugin_exit(void **context);

#endif
