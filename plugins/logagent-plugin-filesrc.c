/*
 * Copyright (c) 2018 Steven Lee <geekerlw@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 */

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
	char buf[FILESRC_BUF_SIZE] = { 0 };
	char offset_file[FILESRC_BUF_SIZE] = { 0 };

	sprintf(offset_file, "%s/.%s.logagent", filepath, filename);

	FILE *fd = fopen(offset_file, "r");
	if (!fd)
		return -1;

	if (fgets(buf, sizeof(buf), fd) != NULL) {
		offset = atol(buf);
	}

	fclose(fd);

	return offset;
}

static void filesrc_file_offset_save(const char *filepath, const char *filename, long offset)
{
	char offset_file[FILESRC_BUF_SIZE] = { 0 };

	sprintf(offset_file, "%s/.%s.logagent", filepath, filename);

	FILE *fd = fopen(offset_file, "w");
	if (!fd)
		return;

	fprintf(fd, "%ld", offset);

	fclose(fd);

	return;
}

static void filesrc_file_read(const char *filepath, const char *filename, struct list_head *log_list)
{
	char log_file[FILESRC_BUF_SIZE] = { 0 };
	char buf[MAX_LOG_SIZE] = { 0 };

	long old_offset = filesrc_file_offset_load(filepath, filename);

	sprintf(log_file, "%s/%s", filepath, filename);

	FILE *fd = fopen(log_file, "r");
	if (!fd)
		return;

	if (old_offset <= 0) {	
		fseek(fd, 0L, SEEK_END);
	} else {
		fseek(fd, old_offset, SEEK_SET);
	}

	while (!feof(fd)) {
		if (fgets(buf, MAX_LOG_SIZE, fd)) {
			logagent_log_add(log_list, buf);
		}
	}

	long new_offset = ftell(fd);
	filesrc_file_offset_save(filepath, filename, new_offset);

	fclose(fd);

	return;
}

static bool filesrc_file_ismatch(const char *filename)
{
	char logname[FILESRC_BUF_SIZE] = { 0 };
	char timestr[FILESRC_BUF_SIZE] = { 0 };
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
		if (filesrc_file_ismatch(event->name)) {
			filesrc_file_read(filesrc->filepath, event->name, log_list);
		}
	}

	return;
}

static int filesrc_work(filesrc_t *filesrc, struct list_head *log_list)
{
	char buf[FILESRC_INOTIFY_BUF_SIZE] = { 0 };
	int len = 0;
	int ret = ioctl(filesrc->fd, FIONREAD, &len);
	if (ret  == -1)
		return -1;

	if (len > 0) {
		int read_size = read(filesrc->fd, buf, sizeof(buf));
		if (read_size == -1)
			return -2;

		size_t pos = 0;
		while (pos < read_size) {
			struct inotify_event *pevent;
			pevent = (struct inotify_event *)buf;
			size_t event_size = offsetof(struct inotify_event, name) + pevent->len;
			filesrc_inotify_event_parse(filesrc, log_list, pevent);
			pos += event_size;
		}
	}
	
	return 0;
}

static int filesrc_init(const char *json, void **context)
{
	filesrc_t *filesrc = (filesrc_t *) malloc(sizeof(filesrc_t));
	if (!filesrc)
		return -1;

	memset(filesrc, 0, sizeof(filesrc));

	filesrc->fd = inotify_init();
	if (filesrc->fd < 0)
		return -2;
	
	struct json_object *plugin_obj = json_tokener_parse(json);
	if (!plugin_obj)
		return -3;

	/* get log file path key-value */
	struct json_object *filepath_obj;
	json_bool ret = json_object_object_get_ex(plugin_obj, "file_path", &filepath_obj);
	if (ret == false) {
		json_object_put(plugin_obj);
		return -4;
	}
	const char *filepath = json_object_get_string(filepath_obj);
	memcpy(filesrc->filepath, filepath, strlen(filepath) + 1);

	json_object_put(plugin_obj);

	/* add file path to inotify watch dir */
	filesrc->wd = inotify_add_watch(filesrc->fd, filesrc->filepath, IN_MODIFY);
	if (filesrc->wd < 0)
		return -5;

	*context = (void *)filesrc;

	return 0;
}

static int filesrc_exit(filesrc_t *filesrc)
{
	if (filesrc->wd > 0 || filesrc->fd > 0) {
		inotify_rm_watch(filesrc->wd, filesrc->fd);
		close(filesrc->fd);
	}
		
	if (filesrc)
		free(filesrc);

	return 0;
}

int logagent_plugin_work(void *config, struct list_head *log_list)
{
	filesrc_t *filesrc = (filesrc_t *)config;

	return filesrc_work(filesrc, log_list);
}

int logagent_plugin_init(void **context)
{
	char *json = (char *)context;

	return filesrc_init(json, context);
}

int logagent_plugin_exit(void **context)
{
	filesrc_t *filesrc = (filesrc_t *)*context;
	
	return filesrc_exit(filesrc);
}
