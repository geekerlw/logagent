#ifndef __LOGAGENT_H__
#define __LOGAGENT_H__

#include <stdbool.h>

#include "logagent-list.h"

#include "logagent-plugin.h"
#include "logagent-pipeline.h"
#include "logagent-config.h"

#define MAX_PIPELINE_NUMS	10
#define MAX_JSON_BUF	(MAX_PIPELINE_NUMS * PIPELINE_JSON_SIZE)

bool logagent_need_exit;

#endif
