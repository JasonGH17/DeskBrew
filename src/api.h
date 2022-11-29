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