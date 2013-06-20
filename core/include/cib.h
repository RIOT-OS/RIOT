#ifndef __CIB_H
#define __CIB_H

typedef struct cib_t {
    unsigned int read_count;
    unsigned int write_count;
    unsigned int complement;
} cib_t;

void cib_init(cib_t *cib, unsigned int size);
int cib_get(cib_t *cib);
int cib_put(cib_t *cib);
int cib_avail(cib_t *cib);

#endif /* __CIB_H */
