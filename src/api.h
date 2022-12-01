#ifdef _WIN32
#ifndef _WIN64
#error "Windows build only supports 64-bit machines..."
#endif
#define DB_PLAT_WIN64 1
#endif
#if defined(__linux__) || defined(__linux_gnu__)
#define DB_PLAT_LINUX 1
#endif

#ifdef DB_EXPORT
#ifdef _MSC_VER
#define DBAPI __declspec(dllexport)
#else
#define DBAPI __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define DBAPI __declspec(dllimport)
#else
#define DBAPI
#endif
#endif