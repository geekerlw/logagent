#ifndef __LOGAGENT_PIPELINE_H__
#define __LOGAGENT_PIPELINE_H__

#define MAX_PIPELINE_NUMS	10
#define PIPELINE_JSON_SIZE	((PLUGIN_JSON_SIZE) * MAX_PIPELINE_NUMS)

typedef struct {
	char json[PIPELINE_JSON_SIZE];	/* pipeline json config */

	pthread_t thread_id;	/* each pipeline have a thread to run */

	struct list_head plugin_list;	/* each pipeline have a plugin list */

	struct list_head list; /* pipeline list each thread */
}pipeline_t;


void logagent_pipeline_work(pipeline_t *pipeline);

void logagent_pipeline_config_load(struct list_head *pipeline_list, const char *json);

void logagent_pipeline_config_unload(struct list_head *pipeline_list);

#endif
