#ifndef READER_H_
#define READER_H_

#include "common/common_types.h"
#include "rfc5444/rfc5444_reader.h"

void reader_init(void);
int reader_handle_packet(void *buffer, size_t length, struct netaddr *src, uint8_t metric_in);
void reader_cleanup(void);

#endif /* READER_H_ */
