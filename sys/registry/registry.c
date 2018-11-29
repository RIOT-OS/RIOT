#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "clist.h"
#include "registry/registry.h"
#include "kernel_defines.h"
#include "assert.h"
#define ENABLE_DEBUG (0)
#include "debug.h"

static int _registry_cmp_name(clist_node_t *current, void *name);
static int _registry_call_commit(clist_node_t *current, void *name);
static void _parse_name(char *name, int *name_argc, char *name_argv[]);
static registry_handler_t *_handler_parse_and_lookup(char *name, int *name_argc,
                                                     char *name_argv[]);

clist_node_t registry_handlers;

static int _registry_cmp_name(clist_node_t *current, void *name)
{
    assert(current != NULL);
    registry_handler_t *hndlr = container_of(current, registry_handler_t, node);
    return !strcmp(hndlr->name, (char *)name);
}

static registry_handler_t *_handler_lookup(char *name)
{
    clist_node_t *node;
    registry_handler_t *hndlr = NULL;
    node = clist_foreach(&registry_handlers, _registry_cmp_name, name);

    if (node != NULL) {
        hndlr = container_of(node, registry_handler_t, node);
    }

    return hndlr;
}

static void _parse_name(char *name, int *name_argc, char *name_argv[])
{
    int i = 0;
    char *_name_p = &name[0];

    while (_name_p) {
        name_argv[i++] = _name_p;
        while(1) {
            if (*_name_p == '\0') {
                _name_p = NULL;
                break;
            }

            if (*_name_p == REGISTRY_NAME_SEPARATOR) {
                *_name_p = '\0';
                _name_p++;
                break;
            }
            _name_p++;
        }
    }

    *name_argc = i;
}

void registry_init(void)
{
    registry_handlers.next = NULL;
    registry_store_init();
}

void registry_register(registry_handler_t *handler)
{
    assert(handler != NULL);
    clist_rpush(&registry_handlers, &(handler->node));
}

static registry_handler_t *_handler_parse_and_lookup(char *name, int *name_argc,
                                                     char *name_argv[])
{
    _parse_name(name, name_argc, name_argv);
    return _handler_lookup(name_argv[0]);
}

int registry_set_value(char *name, char *val_str)
{
    int name_argc;
    char *name_argv[REGISTRY_MAX_DIR_DEPTH];
    registry_handler_t *hndlr;

    hndlr = _handler_parse_and_lookup(name, &name_argc, name_argv);

    if (!hndlr) {
        return -EINVAL;
    }

    return hndlr->hndlr_set(name_argc - 1, &name_argv[1], val_str,
                            hndlr->context);
}

char *registry_get_value(char *name, char *buf, int buf_len)
{
    int name_argc;
    char *name_argv[REGISTRY_MAX_DIR_DEPTH];
    registry_handler_t *hndlr;

    hndlr = _handler_parse_and_lookup(name, &name_argc, name_argv);

    if (!hndlr) {
        return NULL;
    }

    if (!hndlr->hndlr_get) {
        return NULL;
    }

    return hndlr->hndlr_get(name_argc - 1, &name_argv[1], buf, buf_len,
                            hndlr->context);
}

static int _registry_call_commit(clist_node_t *current, void *res)
{
    assert(current != NULL);
    int _res = *(int *)res;
    registry_handler_t *hndlr = container_of(current, registry_handler_t, node);
    if (hndlr->hndlr_commit) {
        _res = hndlr->hndlr_commit(hndlr->context);
        if (!*(int *)res) {
            *(int *)res = _res;
        }
    }
    return 0;
}

int registry_commit(char *name)
{
    int name_argc;
    int rc = 0;

    if (name) {
        registry_handler_t *hndlr;
        char *name_argv[REGISTRY_MAX_DIR_DEPTH];

        hndlr = _handler_parse_and_lookup(name, &name_argc, name_argv);
        if (!hndlr) {
            return -EINVAL;
        }
        if (hndlr->hndlr_commit) {
            return hndlr->hndlr_commit(hndlr->context);
        }
        else {
            return 0;
        }
    }
    else {
        clist_foreach(&registry_handlers, _registry_call_commit,
                      (void *)(&rc));
        return rc;
    }
}

int registry_export(int (*export_func)(const char *name, char *val), char *name)
{
    assert(export_func != NULL);
    int name_argc;
    char *name_argv[REGISTRY_MAX_DIR_DEPTH];
    registry_handler_t *hndlr;

    if (name) {
        DEBUG("[registry export] exporting %s\n", name);
        hndlr = _handler_parse_and_lookup(name, &name_argc, name_argv);
        if (!hndlr) {
            return -EINVAL;
        }
        if (hndlr->hndlr_export) {
            return hndlr->hndlr_export(export_func, name_argc - 1,
                                       &name_argv[1], hndlr->context);
        }
        else {
            return 0;
        }
    }
    else {
        DEBUG("[registry export] exporting all\n");
        clist_node_t *node = registry_handlers.next;

        if (!node) {
            return -1;
        }

        do  {
            node = node->next;
            hndlr = container_of(node, registry_handler_t, node);
            if (hndlr->hndlr_export) {
                hndlr->hndlr_export(export_func, 0, NULL, hndlr->context);
            }
        } while (node != registry_handlers.next);
        return 0;
    }
}
