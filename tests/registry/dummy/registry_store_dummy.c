#include <string.h>
#include "include/registry_store_dummy.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

dummy_store_storage_t dummy_store[DUMMY_STORE_CAPACITY];

static int registry_dummy_load(registry_store_t *store, load_cb_t cb,
                               void *cb_arg);
static int registry_dummy_save(registry_store_t *store, const char *name,
                               const char *value);


static registry_store_itf_t interface = {
    .load = registry_dummy_load,
    .save = registry_dummy_save
};

int registry_dummy_src(registry_dummy_t *dummy)
{
    dummy->store.itf = &interface;
    registry_src_register(&dummy->store);
    return 0;
}

int registry_dummy_dst(registry_dummy_t *dummy)
{
    dummy->store.itf = &interface;
    registry_dst_register(&dummy->store);
    return 0;
}

static int registry_dummy_load(registry_store_t *store, load_cb_t cb,
                               void *cb_arg)
{
    (void)store;
    char name[REGISTRY_MAX_NAME_LEN];
    char val[REGISTRY_MAX_NAME_LEN];

    DEBUG("[registry_store_dummy] Loading...\n");
    for (int i = 0; i < DUMMY_STORE_CAPACITY; i++) {
        if (strlen(dummy_store[i].name)) {
            strcpy(name, dummy_store[i].name);
            strcpy(val, dummy_store[i].val);
            cb(name, val, cb_arg);
        }
    }
    return 0;
}

static int registry_dummy_save(registry_store_t *store, const char *name,
                               const char *value)
{
    int free_slot = -1;
    (void)store;

    DEBUG("[registry_store_dummy] Saving: %s = %s\n", name, value);
    for (int i = 0; i < DUMMY_STORE_CAPACITY; i++) {
        if (strlen(dummy_store[i].name)) {
            DEBUG("[registr_store_dummy]: Checking slot with name %s\n",dummy_store[i].name);
            if (!strcmp(name, dummy_store[i].name)) {
                DEBUG("[registry_store_dummy] Saved in slot: %d\n", i);
                strcpy(dummy_store[i].val, value);
                return 0;
            }
        }
        else {
            if (free_slot == -1) {
                DEBUG("[registry_store_dummy]: Free slot in: %d\n", i);
                free_slot = i;
            }
        }
    }

    if (free_slot == -1) {
        return -1;
    }

    strcpy(dummy_store[free_slot].name, name);
    strcpy(dummy_store[free_slot].val, value);
    return 0;
}
