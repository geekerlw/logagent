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

#ifndef __LOGAGENT_H__
#define __LOGAGENT_H__

#include <stdbool.h>

#define MAX_PIPELINE_NUMS	10
#define MAX_JSON_BUF	(MAX_PIPELINE_NUMS * PIPELINE_JSON_SIZE)

bool logagent_need_exit;

#endif
