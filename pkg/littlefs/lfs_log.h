#include "log.h"

#define _LFS_LOG(level, fmt, ...)\
    LOG(level, "lfs: " fmt "%s\n", __VA_ARGS__)

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
