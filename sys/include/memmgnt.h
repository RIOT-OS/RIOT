/**
 * @ingroup sys
 * @{
 * @file
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 */

#ifndef _MEMMGNT_H
#define _MEMMGNT_H

void *memmgnt_malloc(unsigned size);
void memmgnt_free(void *ptr);

/** @} */
#endif /* _MEMMGNT_H */
