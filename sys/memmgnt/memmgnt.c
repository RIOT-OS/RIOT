/**
 * malloc implementation from
 * "The C Programming Language, 2nd Edition by Ritchie and Kernighan"
 *
 * Needed on platforms without malloc in libc, e.g. msb430
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sys
 * @{
 * @file
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @}
 */

#include <stdio.h>

#include "memmgnt.h"

union memmgnt_header { /* block header */
    struct {
        union memmgnt_header *next; /* next block if on free list */
        unsigned size; /* size of this block */
    } s;
    long x; /* force alignment of blocks */
};
typedef union memmgnt_header memmgnt_t;

extern void *sbrk(int incr);
static memmgnt_t *morecore(unsigned nu);

static memmgnt_t base; /* empty list to get started */
static memmgnt_t *freep = NULL; /* start of free list */

/* malloc: general-purpose storage allocator */
void *memmgnt_malloc(unsigned nbytes)
{
    memmgnt_t *p, *prevp;
    unsigned nunits;
    nunits = (nbytes + sizeof(memmgnt_t) - 1) / sizeof(memmgnt_t) + 1;
    if ((prevp = freep) == NULL) {
        /* no free list yet */
        base.s.next = freep = prevp = &base;
        base.s.size = 0;
    }
    for (p = prevp->s.next;; prevp = p, p = p->s.next) {
        if (p->s.size >= nunits) { /* big enough */
            if (p->s.size == nunits) /* exactly */
                prevp->s.next = p->s.next;
            else {
                /* allocate tail end */
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *) (p + 1);
        }
        if (p == freep) /* wrapped around free list */
            if ((p = morecore(nunits)) == NULL)
                return NULL;
        /* none left */
    }
    return NULL;
}

#define NALLOC 1024 /* minimum #units to request */

/* morecore: ask system for more memory */
static memmgnt_t *morecore(unsigned nu)
{
    char *cp;
    memmgnt_t *up;
    if (nu < NALLOC)
        nu = NALLOC;
    cp = sbrk(nu * sizeof(memmgnt_t));
    if (cp == (char *) -1)
        /* no space at all */
        return NULL;
    up = (memmgnt_t *) cp;
    up->s.size = nu;
    memmgnt_free((void *) (up + 1));
    return freep;
}

/* free: put block ap in free list */
void memmgnt_free(void *ap)
{
    memmgnt_t *bp, *p;
    bp = (memmgnt_t *) ap - 1;
    /* point to block header */
    for (p = freep; !(bp > p && bp < p->s.next); p = p->s.next)
        if (p >= p->s.next && (bp > p || bp < p->s.next))
            break; /* freed block at start or end of arena */

    if (bp + bp->s.size == p->s.next) {
        /* join to upper nbr */
        bp->s.size += p->s.next->s.size;
        bp->s.next = p->s.next->s.next;
    } else
        bp->s.next = p->s.next;
    if (p + p->s.size == bp) {
        /* join to lower nbr */
        p->s.size += bp->s.size;
        p->s.next = bp->s.next;
    } else
        p->s.next = bp;
    freep = p;
}
