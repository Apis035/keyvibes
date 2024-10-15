#pragma once

#include "config.h"

#ifndef RELEASE

#define LOGBUFSIZE 128
#define LOGLEVEL   LOG_DEBUG

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_FATAL,
} LogLevel;

#define __debug(fmt, ...) Log(LOG_DEBUG, TEXT(fmt), ##__VA_ARGS__)
#define __info(fmt, ...)  Log(LOG_INFO,  TEXT(fmt), ##__VA_ARGS__)
#define __warn(fmt, ...)  Log(LOG_WARN,  TEXT(fmt), ##__VA_ARGS__)
#define __fatal(fmt, ...) Log(LOG_FATAL, TEXT(fmt), ##__VA_ARGS__)

void LogInit();
void Log(LogLevel level, PCTSTR fmt, ...);

#else /* RELEASE */

#define __debug(...)
#define __info(...)
#define __warn(...)
#define __fatal(...)

#define LogInit()

#endif /* RELEASE */
