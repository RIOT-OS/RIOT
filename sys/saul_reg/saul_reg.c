/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_saul_reg
 * @{
 *
 * @file
 * @brief       SAUL registry implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "saul_reg.h"

/**
 * @brief   Keep the head of the device list as global variable
 */
saul_reg_t *saul_reg = NULL;


int saul_reg_add(saul_reg_t *dev)
{
    saul_reg_t *tmp = saul_reg;

    if (dev == NULL) {
        return -ENODEV;
    }

    /* prepare new entry */
    dev->next = NULL;
    /* add to registry */
    if (saul_reg == NULL) {
        saul_reg = dev;
    }
    else {
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = dev;
    }
    return 0;
}

int saul_reg_rm(saul_reg_t *dev)
{
    saul_reg_t *tmp = saul_reg;

    if (saul_reg == NULL || dev == NULL) {
        return -ENODEV;
    }
    if (saul_reg == dev) {
        saul_reg = dev->next;
        return 0;
    }
    while (tmp->next && (tmp->next != dev)) {
        tmp = tmp->next;
    }
    if (tmp->next == dev) {
        tmp->next = dev->next;
    }
    else {
        return -ENODEV;
    }
    return 0;
}

saul_reg_t *saul_reg_find_nth(saul_ctxt_ptr_t *ctxt_ptr, int pos)
{
    /* initialize ctxt_ptr */
    ctxt_ptr->reg = saul_reg;
    ctxt_ptr->ctxt = 0;
    ctxt_ptr->num = 0;

    int i = 0;

    while (ctxt_ptr->reg) {
        /* if ctxtlist is empty, ignore it */
        if (ctxt_ptr->reg->ctxtlist == 0) {
            if (i == pos) {
                return ctxt_ptr->reg;
            }
            ctxt_ptr->reg = ctxt_ptr->reg->next;
            i++;
            continue;
        }

        /* if ctxtlist is not empty, examine it bitwise */
        ctxt_ptr->num = 0;
        for (unsigned j = 0; j < 8 * sizeof(ctxt_ptr->reg->ctxtlist); j++) {
            /* check to see if this context bit is enabled */
            if ((ctxt_ptr->reg->ctxtlist >> j) & 0x1) {
                if (i == pos) {
                    ctxt_ptr->ctxt = j;
                    return ctxt_ptr->reg;
                }

                /* increment num of enabled item in ctxtlist */
                ctxt_ptr->num++;

                /* increment list counter, as if context is real list item */
                i++;
            }
        }

        ctxt_ptr->reg = ctxt_ptr->reg->next;
    }

    return ctxt_ptr->reg;
}

saul_reg_t *saul_reg_find_type(uint8_t type)
{
    saul_reg_t *tmp = saul_reg;

    while (tmp) {
        if (tmp->driver->type == type) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

saul_reg_t *saul_reg_find_name(const char *name)
{
    saul_reg_t *tmp = saul_reg;

    while (tmp) {
        if (strcmp(tmp->name, name) == 0) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

int saul_reg_read(saul_reg_t *dev, uint8_t ctxt, phydat_t *res)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    return dev->driver->read(dev->dev, ctxt, res);
}

int saul_reg_write(saul_reg_t *dev, uint8_t ctxt, phydat_t *data)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    return dev->driver->write(dev->dev, ctxt, data);
}

void saul_reg_iter(saul_reg_iter_t func, void *arg)
{
    saul_ctxt_ptr_t ctxt_ptr = { .reg = saul_reg };

    unsigned i = 0;

    while (ctxt_ptr.reg) {
        ctxt_ptr.ctxt = 0;
        ctxt_ptr.num = 0;

        /* if ctxtlist is empty, ignore it */
        if (ctxt_ptr.reg->ctxtlist == 0) {
            func(i, ctxt_ptr, arg);

            ctxt_ptr.reg = ctxt_ptr.reg->next;
            i++;
            continue;
        }

        for (; ctxt_ptr.ctxt < 8 * sizeof(ctxt_ptr.reg->ctxtlist);
             ctxt_ptr.ctxt++) {

            /* check to see if this context bit is enabled */
            if ((ctxt_ptr.reg->ctxtlist >> ctxt_ptr.ctxt) & 0x1) {
                func(i, ctxt_ptr, arg);

                /* increment num of enabled item in ctxtlist */
                ctxt_ptr.num++;

                /* increment list counter, as if context is real list item */
                i++;
            }
        }

        ctxt_ptr.reg = ctxt_ptr.reg->next;
    }
}
