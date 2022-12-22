#include "Logger.h"

#ifdef DB_PLAT_WIN64
#include <windows.h>
static int lvCodes[6] = {64, 4, 6, 2, 1, 8};
#endif
#ifdef DB_PLAT_LINUX
static const char *lvCodes[6] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
#endif

const char *systems[DBSYSTEMS] = {"Window", "Vulkan", "Win32", "X11", "Main", "Events", "Utility"};

const char *severity[LSEVERITIES] = {
    "[FATAL]",
    "[ERROR]",
    "[WARN]",
    "[INFO]",
    "[DEBUG]",
    "[TRACE]",
};

std::mutex lMutex;

void DBFatal(DBSystems system, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    const int len = formatBufferLen(msg, args);
    char *f1 = (char *)malloc((size_t)len);
    std::vsnprintf(f1, len, msg, args);
    va_end(args);
    const char *out = formatMsg("%s - Subsystem: %s - Message: %s\n", severity[Fatal], systems[system], f1);
    platformLog(Fatal, out);
    exit(1);
}

void DBError(DBSystems system, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    const int len = formatBufferLen(msg, args);
    char *f1 = (char *)malloc((size_t)len);
    std::vsnprintf(f1, len, msg, args);
    va_end(args);
    const char *out = formatMsg("%s - Subsystem: %s - Message: %s\n", severity[Error], systems[system], f1);
    platformLog(Error, out);
}

void DBWarn(DBSystems system, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    const int len = formatBufferLen(msg, args);
    char *f1 = (char *)malloc((size_t)len);
    std::vsnprintf(f1, len, msg, args);
    va_end(args);
    const char *out = formatMsg("%s - Subsystem: %s - Message: %s\n", severity[Warn], systems[system], f1);
    platformLog(Warn, out);
}

void DBInfo(DBSystems system, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    const int len = formatBufferLen(msg, args);
    char *f1 = (char *)malloc((size_t)len);
    std::vsnprintf(f1, len, msg, args);
    va_end(args);
    const char *out = formatMsg("%s - Subsystem: %s - Message: %s\n", severity[Info], systems[system], f1);
    platformLog(Info, out);
}

void DBDebug(DBSystems system, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    const int len = formatBufferLen(msg, args);
    char *f1 = (char *)malloc((size_t)len);
    std::vsnprintf(f1, len, msg, args);
    va_end(args);
    const char *out = formatMsg("%s - Subsystem: %s - Message: %s\n", severity[Debug], systems[system], f1);
    platformLog(Debug, out);
}

void DBTrace(DBSystems system, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    const int len = formatBufferLen(msg, args);
    char *f1 = (char *)malloc((size_t)len);
    std::vsnprintf(f1, len, msg, args);
    va_end(args);
    const char *out = formatMsg("%s - Subsystem: %s - Message: %s\n", severity[Trace], systems[system], f1);
    platformLog(Trace, out);
}

int formatBufferLen(const char *msg, va_list args)
{
    va_list copy;
    va_copy(copy, args);
    const int len = std::vsnprintf(NULL, 0, msg, copy);
    va_end(copy);
    return len + 1;
}

const char *formatMsg(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    const int len = formatBufferLen(msg, args);
    char *buffer = (char *)malloc((size_t)len);
    std::vsnprintf(buffer, len, msg, args);
    va_end(args);
    return buffer;
}

void platformLog(LSeverity l, const char *msg)
{
    std::scoped_lock lock(lMutex);
#ifdef DB_PLAT_WIN64
    HANDLE cHandle = GetStdHandle(l < 3 ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(cHandle, lvCodes[l]);
    LPDWORD written = 0;
    WriteConsoleA(cHandle, msg, strlen(msg), written, 0);
#endif
#ifdef DB_PLAT_LINUX
    fprintf(l < 3 ? stderr : stdout, "\033[%sm%s\033[0m", lvCodes[l], msg);
#endif
}