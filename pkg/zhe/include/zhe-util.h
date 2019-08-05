#ifndef ZHE_UTIL_H
#define ZHE_UTIL_H

#include "zhe.h"

struct zhe_platform *zhe(uint16_t port, const char *peers);
void zhe_dispatch(struct zhe_platform *platform);
void zhe_once(struct zhe_platform *platform, uint64_t delay);
void zhe_loop(struct zhe_platform *platform, uint64_t period);
zhe_paysize_t getrandomid(unsigned char *ownid, size_t ownidsize);
zhe_paysize_t getidfromarg(unsigned char *ownid, size_t ownidsize, const char *in);
void cfg_handle_addrs(struct zhe_config *cfg, struct zhe_platform *platform, const char *scoutaddrstr, const char *mcgroups_join_str, const char *mconduit_dstaddrs_str);

#endif
