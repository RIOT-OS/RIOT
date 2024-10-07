#include "log.h"

#ifdef LFS_YES_TRACE
  #define ENABLE_DEBUG 1
#endif
#include "debug.h"

#define _LFS_TRACE(fmt, ...) \
    DEBUG("%s:%d: " fmt "%s\n", __FILE__, __LINE__, __VA_ARGS__)

#define _LFS_LOG(level, fmt, ...)\
    LOG(level, "lfs: " fmt "%s\n", __VA_ARGS__)

#ifdef LFS_YES_TRACE
  #define LFS_TRACE(...) _LFS_TRACE(__VA_ARGS__, "")
#else
  #define LFS_TRACE(...)
#endif

#ifndef LFS_NO_DEBUG
  #define LFS_DEBUG(...) _LFS_LOG(LOG_DEBUG, __VA_ARGS__, "")
#else
  #define LFS_DEBUG(...)
#endif

#ifndef LFS_NO_WARN
  #define LFS_WARN(...) _LFS_LOG(LOG_WARNING, __VA_ARGS__, "")
#else
  #define LFS_WARN(...)
#endif

#ifndef LFS_NO_ERROR
  #define LFS_ERROR(...) _LFS_LOG(LOG_ERROR, __VA_ARGS__, "")
#else
  #define LFS_ERROR(...)
#endif
