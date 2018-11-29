#include <string.h>
#include <errno.h>
#include "kernel_defines.h"
#include "clist.h"
#include "registry/registry.h"
#include "assert.h"
#define ENABLE_DEBUG (0)
#include "debug.h"

registry_store_t *save_dst;
clist_node_t load_srcs;

static void _registry_load_cb(char *name, char *val, void *cb_arg)
{
    (void)cb_arg;
    DEBUG("[registry_store] Setting %s to %s\n", name, val);
    registry_set_value(name, val);
}

void registry_store_init(void)
{
    load_srcs.next = NULL;
}

void registry_src_register(registry_store_t *src)
{
    assert(src != NULL);
    clist_rpush(&load_srcs, &(src->node));
}

void registry_dst_register(registry_store_t *dst)
{
    assert(dst != NULL);
    save_dst = dst;
}

int registry_load(void)
{
    clist_node_t *node = load_srcs.next;

    if (!node) {
        return -ENOENT;
    }

    do  {
        registry_store_t *src;
        src = container_of(node, registry_store_t, node);
        src->itf->load(src, _registry_load_cb, NULL);
    } while (node != load_srcs.next);
    return 0;
}

static void _registry_dup_check_cb(char *name, char *val, void *cb_arg)
{
    assert(cb_arg != NULL);
    registry_dup_check_arg_t *dup_arg = (registry_dup_check_arg_t *)cb_arg;
    if (strcmp(name, dup_arg->name)) {
        return;
    }
    if (!val) {
        if (!dup_arg->val || dup_arg->val[0] == '\0') {
            dup_arg->is_dup = 1;
        }
    }
    else {
        if (dup_arg->val && !strcmp(val, dup_arg->val)) {
            dup_arg->is_dup = 1;
        }
    }
}

int registry_save_one(const char *name, char *value)
{
    registry_store_t *dst = save_dst;
    registry_dup_check_arg_t dup;
    DEBUG("[registry_store] Saving: %s = %s\n",name, value);

    if (!dst) {
        return -ENOENT;
    }

    dup.name = (char *)name;
    dup.val = value;
    dup.is_dup = 0;

    save_dst->itf->load(save_dst, _registry_dup_check_cb, &dup);

    if (dup.is_dup == 1) {
        return -EEXIST;
    }

    return dst->itf->save(dst, name, value);
}

int registry_save(void)
{
    registry_handler_t *hndlr;
    clist_node_t *node = registry_handlers.next;
    int res = 0;
    int res2;

    if (!node) {
        return -1;
    }

    if (save_dst->itf->save_start) {
        save_dst->itf->save_start(save_dst);
    }

    do  {
        hndlr = container_of(node, registry_handler_t, node);
        if (hndlr->hndlr_export) {
            res2 = hndlr->hndlr_export(registry_save_one, 0, NULL,
                                       hndlr->context);
            if (res == 0) {
                res = res2;
            }
        }
    } while (node != registry_handlers.next);

    if (save_dst->itf->save_end) {
        save_dst->itf->save_end(save_dst);
    }

    return res;
}
