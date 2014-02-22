#ifndef UTIL_H_
#define UTIL_H_

#include "common/netaddr.h"

struct netaddr_rc {
	struct netaddr super;
	uint8_t _refs;
};

const char* netaddr_to_str_s(struct netaddr_str* dst, const struct netaddr* src);

struct netaddr* netaddr_dup(struct netaddr* addr);
struct netaddr* netaddr_use(struct netaddr* addr);
struct netaddr* netaddr_reuse(struct netaddr* addr);
struct netaddr* netaddr_free(struct netaddr* addr);

time_t time_now(void);
void sleep_s(int secs);

#endif
