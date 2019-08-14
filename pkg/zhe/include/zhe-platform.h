#ifndef ZHE_PLATFORM_H
#define ZHE_PLATFORM_H

#include <stddef.h>
#include <stdbool.h>
#include "zhe-config-int.h"

#ifdef __cplusplus
extern "C" {
#endif

/* modes for transport should not include 0 */
#define TRANSPORT_PACKET 1
#define TRANSPORT_STREAM 2

#define SENDRECV_HANGUP (-1)
#define SENDRECV_ERROR  (-2)

struct zhe_address;
struct zhe_platform;

/* Return 0 if a and b are different addresses, 1 if they are the same */
int zhe_platform_addr_eq(const struct zhe_address *a, const struct zhe_address *b);

/* Convert the address in ADDR to a standard string representation, and write the result into str.
 No more than size bytes shall be written to str, and str shall be null-terminated. Size must
 consequently be > 0. Number of characters written into str shall be returned. Failure is not an
 option. */
size_t zhe_platform_addr2string(const struct zhe_platform *pf, char *str, size_t size, const struct zhe_address *addr);

/* Sends the packet in buf of size bytes to address dst. Returns the number of bytes written (which
 must be sz for now), or 0 to indicate nothing was written, or SENDRECV_HANGUP to indicate the
 other side hung up on us (i.e., when using TCP), or SENDRECV_ERROR for unspecified, and therefore
 fatal errors. Should be non-blocking. */
int zhe_platform_send(struct zhe_platform *pf, const void *buf, size_t size, const struct zhe_address *dst);

/* Return true if the source address of outgoing packets has changed since last call, false if not. Returning true when nothing changed will cause unnecessary keepalives to be sent, returning false when in fact it did change causes trouble. Called periodically (SCOUT_INTERVAL) so it should be cheap. */
bool zhe_platform_needs_keepalive(struct zhe_platform *pf);

/* Called by zhe_housekeeping to do whatever the platform the requires in terms of background activity */
void zhe_platform_housekeeping(struct zhe_platform *pf, zhe_time_t tnow);

/* Called whenever the zhe core code drops a session */
void zhe_platform_close_session(struct zhe_platform *pf, const struct zhe_address *addr);

void zhe_platform_trace(struct zhe_platform *pf, const char *fmt, ...)
#ifdef __GNUC__
__attribute__((format (printf, 2, 3)))
#endif
;

#ifdef __cplusplus
}
#endif

#endif
