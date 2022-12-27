#pragma once

#include "api.h"

#include <stdio.h>
#include <cstdarg>
#include <mutex>

enum DBSystems
{
    DBWindow,
    DBVulkan,
    DBOpenGL,
    DBWin32,
    DBX11,
    DBMain,
    DBEvent,
    DBUtility,
    DBSYSTEMS
};

enum LSeverity
{
    Fatal,
    Error,
    Warn,
    Info,
    Debug,
    Trace,
    LSEVERITIES
};

void platformLog(LSeverity l, const char *msg);
int formatBufferLen(const char *msg, va_list args);
const char *formatMsg(const char *msg, ...);

void DBFatal(DBSystems system, const char *msg, ...);
void DBError(DBSystems system, const char *msg, ...);
void DBWarn(DBSystems system, const char *msg, ...);
void DBInfo(DBSystems system, const char *msg, ...);
void DBDebug(DBSystems system, const char *msg, ...);
void DBTrace(DBSystems system, const char *msg, ...);