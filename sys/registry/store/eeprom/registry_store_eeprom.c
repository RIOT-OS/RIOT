
#include <string.h>
#include <errno.h>
#include "registry/store/registry_store_eeprom.h"
#include "periph/eeprom.h"
#include "eepreg.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int registry_eeprom_load(registry_store_t *store, load_cb_t cb,
                               void *cb_arg);
static int registry_eeprom_save(registry_store_t *store, const char *name,
                               const char *value);


static registry_store_itf_t interface = {
    .load = registry_eeprom_load,
    .save = registry_eeprom_save
};

int registry_eeprom_src(registry_eeprom_t *eeprom)
{
    eeprom->store.itf = &interface;
    registry_src_register(&eeprom->store);
    return 0;
}

int registry_eeprom_dst(registry_eeprom_t *eeprom)
{
    eeprom->store.itf = &interface;
    registry_dst_register(&eeprom->store);
    return 0;
}

typedef struct {
    load_cb_t cb;
    void *arg;
} load_cb_arg_t;

int _load_cb(char *name, void *arg)
{
    load_cb_arg_t *cb_arg = (load_cb_arg_t *)arg;
    uint32_t pos;
    int i = 0;
    char val[REGISTRY_MAX_VAL_LEN];

    DEBUG("[registry_store_eeprom]: Load CB name = %s\n", name);

    eepreg_read(&pos, name);

    do {
        eeprom_read(pos + i, (uint8_t *)&val[i], 1);
        i++;
    } while(val[i] != '\0' && i < REGISTRY_MAX_VAL_LEN);

    /* Return error if could not find '\0' */
    cb_arg->cb(name, val, cb_arg->arg);
    return 0;
}

static int registry_eeprom_load(registry_store_t *store, load_cb_t cb,
                               void *cb_arg)
{
    load_cb_arg_t load_cb_arg = {
        .cb = cb,
        .arg = cb_arg
    };
    (void)store;

    DEBUG("[registry_store_eeprom] Loading...\n");

    eepreg_iter(_load_cb, &load_cb_arg);
    return 0;
}

static int registry_eeprom_save(registry_store_t *store, const char *name,
                               const char *value)
{
    uint32_t pos;
    const uint8_t ending = '\0';
    size_t len = strlen(value);
    (void)store;

    DEBUG("[registry_store_eeprom] Saving: %s = %s\n", name, value);

    if (!eepreg_add(&pos, name, (uint32_t)(len + 1))) {
        eeprom_write(pos, (const uint8_t *)value, len);
        eeprom_write(pos + len, &ending, 1);
        return 0;
    }
    else {
        return -ENOMEM;
    }
}
