#ifndef REGISTRY_STORE_DUMMY_H
#define REGISTRY_STORE_DUMMY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "registry/registry.h"

#define DUMMY_MAX_NAME_LEN      64
#define DUMMY_MAX_VAL_LEN       64
#define DUMMY_STORE_CAPACITY    64

typedef struct {
    char name[DUMMY_MAX_NAME_LEN];
    char val[DUMMY_MAX_VAL_LEN];
} dummy_store_storage_t;

typedef struct {
    registry_store_t store;
} registry_dummy_t;

/* Dummy Registry Storage */
int registry_dummy_src(registry_dummy_t *dummy);

int registry_dummy_dst(registry_dummy_t *dummy);

#ifdef __cplusplus
}
#endif

#endif /* REGISTRY_STORE_DUMMY_H */
