#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <json.h>

#include "logagent-plugin-api.h"

#define FILESRC_INOTIFY_BUF_SIZE	1024 * 1024
#define FILESRC_BUF_SIZE	256

typedef struct {
	int fd;	/* inotify file discription */
	int wd; /* inotify file watch id */
	char filepath[FILESRC_BUF_SIZE];
}filesrc_t;

static long filesrc_file_offset_load(const char *filepath, const char *filename)
{
	long offset = 0;
	char buf[256] = { 0 };
	char offset_file[256] = { 0 };

	sprintf(offset_file, "%s/.%s.laoffset", filepath, filename);

	FILE *fd = fopen(offset_file, "r");
	if (!fd)
		return 0;

	if (fgets(buf, sizeof(buf), fd) != NULL) {
		offset = atol(buf);
	}

	fclose(fd);

	return offset;
}

static void filesrc_file_offset_save(const char *filepath, const char *filename, long offset)
{
	char offset_file[256] = { 0 };

	sprintf(offset_file, "%s/.%s.laoffset", filepath, filename);

	FILE *fd = fopen(offset_file, "w");
	if (!fd)
		return;

	fprintf(fd, "%ld", offset);

	fclose(fd);

	return;
}

static void filesrc_file_read(const char *filepath, const char *filename, struct list_head *log_list)
{
	long old_offset = 0;
	long new_offset;
	char log_file[256] = { 0 };
	char buf[1024] = { 0 };

	old_offset = filesrc_file_offset_load(filepath, filename);

	sprintf(log_file, "%s/%s", filepath, filename);

	FILE *fd = fopen(log_file, "r");
	if (!fd)
		return;

	fseek(fd, old_offset, SEEK_SET);

	while (!feof(fd)) {
		if (fgets(buf, 1024, fd)) {
			printf("get log buffer: %s", buf);
			logagent_log_add(log_list, buf);
		}
	}

	new_offset = ftell(fd);
	filesrc_file_offset_save(filepath, filename, new_offset);

	fclose(fd);

	return;
}

static bool filesrc_file_ismatch(const char *filename)
{
	char logname[256] = { 0 };
	char timestr[256] = { 0 };
	time_t now = time(NULL);
	struct tm *ptime = localtime(&now);
	strftime(timestr, sizeof(timestr), "%Y-%m-%d", ptime);
	sprintf(logname, "%s.log", timestr);

	if (strncmp(filename, logname, sizeof(logname)) == 0)
		return true;

	return false;
}

static void filesrc_inotify_event_parse(filesrc_t *filesrc, struct list_head *log_list, struct inotify_event *event)
{
	if (event->mask & IN_ISDIR)
		return;

	if (event->mask & IN_MODIFY) {
		printf("filename: %s is modify\n", event->name);

		if (filesrc_file_ismatch(event->name)) {
			printf("current log file is modify, filename: %s\n", event->name);
			filesrc_file_read(filesrc->filepath, event->name, log_list);
		}
	}

	if (event->mask & IN_CREATE) {
		printf("filename: %s is create\n", event->name);
	}

	if (event->mask & IN_DELETE) {
		printf("filename: %s is delete\n", event->name);
	}

	return;
}

static void filesrc_work(filesrc_t *filesrc, struct list_head *log_list)
{
	char buf[FILESRC_INOTIFY_BUF_SIZE] = { 0 };
	int len = 0;
	int ret = ioctl(filesrc->fd, FIONREAD, &len);
	if (ret  == -1 || len == 0)
		return;

	int read_size = read(filesrc->fd, buf, sizeof(buf));
	if (read_size == -1)
		return;

	size_t pos = 0;
	while (pos < read_size) {
		struct inotify_event *pevent;
		pevent = (struct inotify_event *)buf;
		size_t event_size = offsetof(struct inotify_event, name) + pevent->len;
		filesrc_inotify_event_parse(filesrc, log_list, pevent);
		pos += event_size;
	}
	
	return;
}

static void filesrc_init(const char *json, void **context)
{
	if (!json)
		return;

	filesrc_t *filesrc = (filesrc_t *) malloc(sizeof(filesrc_t));
	if (!filesrc)
		return;

	memset(filesrc, 0, sizeof(filesrc));

	filesrc->fd = inotify_init();
	if (filesrc->fd < 0)
		return;
	
	struct json_object *plugin_obj = json_tokener_parse(json);
	if (!plugin_obj)
		return;

	/* get log file path key-value */
	struct json_object *filepath_obj = json_object_object_get(plugin_obj, "file_path");
	const char *filepath = json_object_get_string(filepath_obj);
	memcpy(filesrc->filepath, filepath, strlen(filepath) + 1);

	/* add file path to inotify watch dir */
	filesrc->wd = inotify_add_watch(filesrc->fd, filepath, IN_MODIFY | IN_CREATE | IN_DELETE);
	if (filesrc->wd < 0)
		return;

	*context = (void *)filesrc;

	json_object_put(plugin_obj);

	return;
}

static void filesrc_exit(filesrc_t *filesrc)
{
	if (filesrc)
		free(filesrc);
}

void logagent_plugin_work(void *config, struct list_head *log_list)
{
	filesrc_t *filesrc = (filesrc_t *)config;

	filesrc_work(filesrc, log_list);

	return;
}

void logagent_plugin_init(void **context)
{
	char *json = (char *)context;

	filesrc_init(json, context);

	return;
}

void logagent_plugin_exit(void **context)
{
	filesrc_t *filesrc = (filesrc_t *)*context;

	filesrc_exit(filesrc);

	return;
}
