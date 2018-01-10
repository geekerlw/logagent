#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <sys/signal.h>

#include "logagent.h"

static struct list_head pipeline_list;

static void logagent_work()
{
	/* list each pipeline and create thread */
	pipeline_t *pipeline;
	list_for_each_entry(pipeline, pipeline_t, &pipeline_list, list) {
		pthread_create(&pipeline->thread_id, NULL,
				(void *)logagent_pipeline_work, pipeline);
	}

	/* wait till all thread exit */
	list_for_each_entry(pipeline, pipeline_t, &pipeline_list, list) {
		pthread_join(pipeline->thread_id, NULL);
	}


	return;
}

static void logagent_init()
{
	logagent_need_exit = false;

	char json_config[MAX_JSON_BUF] = { 0 };
	/* load config from local disk */
	logagent_config_load_from_file("./logagent.conf", json_config);

	logagent_pipeline_config_load(&pipeline_list, json_config);

	return;
}

static void logagent_exit()
{
	logagent_pipeline_config_unload(&pipeline_list);

	return;
}

static void logagent_signal_handler(int sig)
{
	switch (sig) {
	case SIGINT:
		logagent_need_exit = true;
		break;
	default:
		break;
	}

	return;
}

int main(int argc, char **argv)
{
	logagent_init();

	signal(SIGINT, logagent_signal_handler);

	logagent_work();

	logagent_exit();

	return 0;
}
