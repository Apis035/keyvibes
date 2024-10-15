#include "config.h"

#ifndef RELEASE

#include <tchar.h>
#include <windows.h>
#include <strsafe.h>

#include "heap.h"
#include "log.h"

HANDLE hConsoleOutput;

const struct {
    WORD color;
    PCTSTR text;
} logLevelDesc[4] = {
    [LOG_DEBUG] = {0xA, TEXT("DEBUG: ")},
    [LOG_INFO]  = {0x8, TEXT("INFO:  ")},
    [LOG_WARN]  = {0xE, TEXT("WARN:  ")},
    [LOG_FATAL] = {0xC, TEXT("FATAL: ")},
};

void LogInit()
{
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
}

void Log(LogLevel level, PCTSTR fmt, ...)
{
    if (level < LOGLEVEL) return;

    TCHAR buf[LOGBUFSIZE];
    va_list args;
    va_start(args, fmt);
    StringCchVPrintf(buf, LOGBUFSIZE, fmt, args);
    va_end(args);

    #define WriteCon(text) WriteConsole(hConsoleOutput, text, _tcslen(text), NULL, NULL)

    SetConsoleTextAttribute(hConsoleOutput, logLevelDesc[level].color);
    WriteCon(logLevelDesc[level].text);
    SetConsoleTextAttribute(hConsoleOutput, 0xf);
    WriteCon(buf);
    WriteCon(TEXT("\n"));
}

#endif /* RELEASE */
