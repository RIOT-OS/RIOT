#ifndef ZHE_TRACING_H
#define ZHE_TRACING_H

#include "zhe-config-int.h"

#if ENABLE_TRACING
#define ZTCAT_ERROR      1u
#define ZTCAT_DEBUG      2u
#define ZTCAT_PEERDISC   4u
#define ZTCAT_TRANSPORT  8u
#define ZTCAT_RELIABLE  16u
#define ZTCAT_PUBSUB    32u

struct zhe_platform;
extern unsigned zhe_trace_cats;
extern struct zhe_platform *zhe_platform;

#define ZTT(catsimple_) (zhe_trace_cats & ZTCAT_##catsimple_)
#define ZT(catsimple_, ...) ((zhe_trace_cats & ZTCAT_##catsimple_) ? zhe_platform_trace(zhe_platform, __VA_ARGS__) : (void)0)

#else

#define ZTT(catsimple_) (0)
#define ZT(catsimple_, ...) ((void)0)

#endif

#endif
