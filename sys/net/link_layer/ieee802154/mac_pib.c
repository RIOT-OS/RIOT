/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author Moritz Holzer <moritz.holzer@haw-hamburg.de>
 */

#include "random.h"
#include "assert.h"
#include "net/ieee802154/mac.h"
#include "mac_pib.h"

#define IEEE802154_PIB_OFF(field)   ((uint16_t)offsetof(ieee802154_pib_t, field))
#define IEEE802154_PIB_SIZE(field)  ((uint16_t)sizeof(((ieee802154_pib_t *)0)->field))

#define ENABLE_DEBUG 0
#include "debug.h"

static const ieee802154_pib_attr_entry_t ieee802154_pib_attr[IEEE802154_PIB_ATTR_COUNT] = {
    [IEEE802154_PIB_AOA_ENABLE] = {
        .type = IEEE802154_PIB_TYPE_BOOL, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(AOA_ENABLE), .size = IEEE802154_PIB_SIZE(AOA_ENABLE),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true  }
    },

    [IEEE802154_PIB_AUTO_REQUEST] = {
        .type = IEEE802154_PIB_TYPE_BOOL, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(AUTO_REQUEST), .size = IEEE802154_PIB_SIZE(AUTO_REQUEST),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true  }
    },

    [IEEE802154_PIB_BATT_LIFE_EXT] = {
        .type = IEEE802154_PIB_TYPE_BOOL, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(BATT_LIFE_EXT), .size = IEEE802154_PIB_SIZE(BATT_LIFE_EXT),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true  }
    },

    [IEEE802154_PIB_BATT_LIFE_EXT_PERIODS] = {
        .type = IEEE802154_PIB_TYPE_U16, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(BATT_LIFE_EXT_PERIODS),
        .size = IEEE802154_PIB_SIZE(BATT_LIFE_EXT_PERIODS),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 6 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 41 }
    },

    [IEEE802154_PIB_BEACON_ORDER] = {
        .type = IEEE802154_PIB_TYPE_U8, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(BEACON_ORDER), .size = IEEE802154_PIB_SIZE(BEACON_ORDER),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 15 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 15 }
    },

    [IEEE802154_PIB_BEACON_PAYLOAD] = {
        .type = IEEE802154_PIB_TYPE_BYTES, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(BEACON_PAYLOAD), .size = IEEE802154_PIB_SIZE(BEACON_PAYLOAD),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BYTES,
                                         .v.bytes = { .ptr = NULL, .len = 0 } },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BYTES,
                                         .v.bytes = { .ptr = NULL, .len = 0 } },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BYTES,
                                         .v.bytes = { .ptr = NULL, .len = 0 } }
    },

    [IEEE802154_PIB_BSN] = {
        .type = IEEE802154_PIB_TYPE_U8, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(BSN), .size = IEEE802154_PIB_SIZE(BSN),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 0 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 0xFFU }
    },

    [IEEE802154_PIB_COORD_EXTENDED_ADDRESS] = {
        .type = IEEE802154_PIB_TYPE_EUI64, .access = IEEE802154_PIB_ACC_RO,
        .offset = IEEE802154_PIB_OFF(COORD_EXTENDED_ADDRESS),
        .size = IEEE802154_PIB_SIZE(COORD_EXTENDED_ADDRESS),
        .def = { .type = IEEE802154_PIB_TYPE_EUI64,
                 .v.ext_addr = { .uint8 = { 0, 0, 0, 0, 0, 0, 0, 0 } } },
        .min = { .type = IEEE802154_PIB_TYPE_EUI64,
                 .v.ext_addr = { .uint8 = { 0, 0, 0, 0, 0, 0, 0, 0 } } },
        .max = { .type = IEEE802154_PIB_TYPE_EUI64,
                 .v.ext_addr = { .uint8 = { 0, 0, 0, 0, 0, 0, 0, 0 } } }

    },

    [IEEE802154_PIB_COORD_SHORT_ADDRESS] = {
        .type = IEEE802154_PIB_TYPE_NUI16, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(COORD_SHORT_ADDRESS),
        .size = IEEE802154_PIB_SIZE(COORD_SHORT_ADDRESS),
        .def = { .type = IEEE802154_PIB_TYPE_NUI16,
                 .v.short_addr = { .u8 = { 0xFF, 0xFFU } } },
        .min = { .type = IEEE802154_PIB_TYPE_NUI16,
                 .v.short_addr = { .u8 = { 0x00, 0x00 } } },
        .max = { .type = IEEE802154_PIB_TYPE_NUI16,
                 .v.short_addr = { .u8 = { 0xFF, 0xFFU } } }
    },

    [IEEE802154_PIB_DSN] = {
        .type = IEEE802154_PIB_TYPE_U8, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(DSN), .size = IEEE802154_PIB_SIZE(DSN),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 0 }, // set random in init
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 0xFFU }
    },

    [IEEE802154_PIB_EXTENDED_ADDRESS] = {
        .type = IEEE802154_PIB_TYPE_EUI64, .access = IEEE802154_PIB_ACC_RO,
        .offset = IEEE802154_PIB_OFF(EXTENDED_ADDRESS),
        .size = IEEE802154_PIB_SIZE(EXTENDED_ADDRESS),
        .def = { .type = IEEE802154_PIB_TYPE_EUI64,
                 .v.ext_addr = { .uint8 = { 0, 0, 0, 0, 0, 0, 0, 0 } } },
        .min = { .type = IEEE802154_PIB_TYPE_EUI64,
                 .v.ext_addr = { .uint8 = { 0, 0, 0, 0, 0, 0, 0, 0 } } },
        .max = { .type = IEEE802154_PIB_TYPE_EUI64,
                 .v.ext_addr = { .uint8 = { 0, 0, 0, 0, 0, 0, 0, 0 } } }
    },

    [IEEE802154_PIB_FCS_TYPE] = {
        .type = IEEE802154_PIB_TYPE_U8, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(FCS_TYPE), .size = IEEE802154_PIB_SIZE(FCS_TYPE),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 0 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 1 }
    },

    [IEEE802154_PIB_GROUP_RX_MODE] = {
        .type = IEEE802154_PIB_TYPE_BOOL, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(GROUP_RX_MODE), .size = IEEE802154_PIB_SIZE(GROUP_RX_MODE),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true  }

    },

    [IEEE802154_PIB_GTS_PERMIT] = {
        .type = IEEE802154_PIB_TYPE_BOOL, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(GTS_PERMIT), .size = IEEE802154_PIB_SIZE(GTS_PERMIT),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true  }
    },

    [IEEE802154_PIB_IMPLICIT_BROADCAST] = {
        .type = IEEE802154_PIB_TYPE_BOOL, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(IMPLICIT_BROADCAST),
        .size = IEEE802154_PIB_SIZE(IMPLICIT_BROADCAST),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true  }
    },

    [IEEE802154_PIB_LIFS_PERIOD] = {
        .type = IEEE802154_PIB_TYPE_U16, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(LIFS_PERIOD), .size = IEEE802154_PIB_SIZE(LIFS_PERIOD),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0x0 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0xFFFFU }
    },

    [IEEE802154_PIB_MAX_BE] = {
        .type = IEEE802154_PIB_TYPE_U8, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(MAX_BE), .size = IEEE802154_PIB_SIZE(MAX_BE),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 5 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 3 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 8 }
    },

    [IEEE802154_PIB_MAX_CSMA_BACKOFFS] = {
        .type = IEEE802154_PIB_TYPE_U8, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(MAX_CSMA_BACKOFFS),
        .size = IEEE802154_PIB_SIZE(MAX_CSMA_BACKOFFS),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 4 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 5 }
    },

    [IEEE802154_PIB_NOTIFY_ALL_BEACONS] = {
        .type = IEEE802154_PIB_TYPE_BOOL, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(NOTIFY_ALL_BEACONS),
        .size = IEEE802154_PIB_SIZE(NOTIFY_ALL_BEACONS),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true  }
    },

    [IEEE802154_PIB_MIN_BE] = {
        .type = IEEE802154_PIB_TYPE_U8, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(MIN_BE), .size = IEEE802154_PIB_SIZE(MIN_BE),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 3 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 8 }
    },

    [IEEE802154_PIB_PAN_ID] = {
        .type = IEEE802154_PIB_TYPE_U16, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(PAN_ID), .size = IEEE802154_PIB_SIZE(PAN_ID),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0xFFFFU },//CONFIG_IEEE802154_DEFAULT_PANID },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0xFFFFU }
    },

    [IEEE802154_PIB_RESPONSE_WAIT_TIME] = {
        .type = IEEE802154_PIB_TYPE_U8, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(RESPONSE_WAIT_TIME),
        .size = IEEE802154_PIB_SIZE(RESPONSE_WAIT_TIME),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 32 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 2 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U8, .v.u8 = 64 }
    },

    [IEEE802154_PIB_RX_ON_WHEN_IDLE] = {
        .type = IEEE802154_PIB_TYPE_BOOL, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(RX_ON_WHEN_IDLE), .size = IEEE802154_PIB_SIZE(RX_ON_WHEN_IDLE),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true  }
    },

    [IEEE802154_PIB_SECURITY_ENABLED] = {
        .type = IEEE802154_PIB_TYPE_BOOL, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(SECURITY_ENABLED),
        .size = IEEE802154_PIB_SIZE(SECURITY_ENABLED),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true  }
    },

    [IEEE802154_PIB_SHORT_ADDR] = {
        .type = IEEE802154_PIB_TYPE_NUI16, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(SHORT_ADDR), .size = IEEE802154_PIB_SIZE(SHORT_ADDR),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_NUI16,
                                         .v.short_addr = { .u8 = { 0xFFU, 0xFFU } } },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_NUI16,
                                         .v.short_addr = { .u8 = { 0x00, 0x00 } } },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_NUI16,
                                         .v.short_addr = { .u8 = { 0xFFU, 0xFFU } } }
    },

    [IEEE802154_PIB_SIFS_PERIOD] = {
        .type = IEEE802154_PIB_TYPE_U16, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(SIFS_PERIOD), .size = IEEE802154_PIB_SIZE(SIFS_PERIOD),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0x0 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0xFFFFU }
    },

    [IEEE802154_PIB_SYNC_SYMBOL_OFFSET] = {
        .type = IEEE802154_PIB_TYPE_U16, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(SYNC_SYMBOL_OFFSET),
        .size = IEEE802154_PIB_SIZE(SYNC_SYMBOL_OFFSET),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0x0 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0xFFFFU }
    },

    [IEEE802154_PIB_TIMESTAMP_SUPPORTED] = {
        .type = IEEE802154_PIB_TYPE_BOOL, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(TIMESTAMP_SUPPORTED),
        .size = IEEE802154_PIB_SIZE(TIMESTAMP_SUPPORTED),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = false },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_BOOL, .v.b = true  }
    },

    [IEEE802154_PIB_TRANSACTION_PERSISTENCE_TIME] = {
        .type = IEEE802154_PIB_TYPE_U16, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(TRANSACTION_PERSISTENCE_TIME),
        .size = IEEE802154_PIB_SIZE(TRANSACTION_PERSISTENCE_TIME),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0x01F4U },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0xFFFFU }
    },

    [IEEE802154_PIB_UNIT_BACKOFF_PERIOD] = {
        .type = IEEE802154_PIB_TYPE_U16, .access = IEEE802154_PIB_ACC_RW,
        .offset = IEEE802154_PIB_OFF(UNIT_BACKOFF_PERIOD),
        .size = IEEE802154_PIB_SIZE(UNIT_BACKOFF_PERIOD),
        .def = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0x0 },
        .min = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0 },
        .max = (ieee802154_pib_value_t){ .type = IEEE802154_PIB_TYPE_U16, .v.u16 = 0xFFFFU }
    },
};

#undef IEEE802154_PIB_OFF
#undef IEEE802154_PIB_SIZE

static inline uint8_t rand_u8(void)
{
    return (uint8_t)random_uint32_range(0x00U, 0xFFU);
}

static inline uint8_t *ieee802154_pib_ptr_from_mac(ieee802154_mac_t *mac,
                                                   const ieee802154_pib_attr_entry_t *e)
{
    ieee802154_pib_t *p = &mac->pib;

    return (uint8_t *)((uint8_t *)p + (ptrdiff_t)e->offset);
}

static inline const uint8_t *ieee802154_pib_ptr_const_from_mac(const ieee802154_mac_t *mac,
                                                               const ieee802154_pib_attr_entry_t *e)
{
    const ieee802154_pib_t *p = &mac->pib;

    return (const uint8_t *)((const uint8_t *)p + (ptrdiff_t)e->offset);
}

static inline bool _pib_can_write(ieee802154_pib_access_t a)
{
    return (a == IEEE802154_PIB_ACC_RW);
}

void ieee802154_mac_mlme_set(ieee802154_mac_t *mac,
                             ieee802154_pib_attr_t attr,
                             const ieee802154_pib_value_t *in)
{

    mutex_lock(&mac->pib_lock);
    const ieee802154_pib_attr_entry_t *e = &ieee802154_pib_attr[attr];

    uint8_t *dst = ieee802154_pib_ptr_from_mac(mac, e);

    switch (e->type) {
    case IEEE802154_PIB_TYPE_BOOL:
        assert(e->size == sizeof(bool));
        memcpy(dst, &in->v.b, sizeof(in->v.b));
        break;

    case IEEE802154_PIB_TYPE_U8:
        assert(e->size == sizeof(uint8_t));
        memcpy(dst, &in->v.u8, sizeof(in->v.u8));
        break;

    case IEEE802154_PIB_TYPE_U16:
        assert(e->size == sizeof(uint16_t));
        memcpy(dst, &in->v.u16, sizeof(in->v.u16));
        break;

    case IEEE802154_PIB_TYPE_EUI64:
        assert(e->size == sizeof(eui64_t));
        memcpy(dst, &in->v.ext_addr, sizeof(in->v.ext_addr));
        break;

    case IEEE802154_PIB_TYPE_NUI16:
        assert(e->size == 2);
        memcpy(dst, &in->v.short_addr, sizeof(in->v.short_addr));
        break;

    case IEEE802154_PIB_TYPE_BYTES:
        assert(e->size == sizeof(ieee802154_octets_t));
        assert(e->size == sizeof(ieee802154_octets_t));
        memcpy(dst, &in->v.bytes, sizeof(in->v.bytes));
        break;
    }

    mutex_unlock(&mac->pib_lock);
}

// TODO: return value out of bound
void ieee802154_mac_mlme_get(ieee802154_mac_t *mac,
                             ieee802154_pib_attr_t attr,
                             ieee802154_pib_value_t *out)
{

    mutex_lock(&mac->pib_lock);
    const ieee802154_pib_attr_entry_t *e = &ieee802154_pib_attr[attr];
    const uint8_t *src = ieee802154_pib_ptr_const_from_mac(mac, e);
    out->type = e->type;

    switch (e->type) {
    case IEEE802154_PIB_TYPE_BOOL:
        assert(e->size == sizeof(bool));
        memcpy(&out->v.b, src, sizeof(out->v.b));
        break;

    case IEEE802154_PIB_TYPE_U8:
        assert(e->size == sizeof(uint8_t));
        memcpy(&out->v.u8, src, sizeof(out->v.u8));
        break;

    case IEEE802154_PIB_TYPE_U16:
        assert(e->size == sizeof(uint16_t));
        memcpy(&out->v.u16, src, sizeof(out->v.u16));
        break;

    case IEEE802154_PIB_TYPE_EUI64:
        assert(e->size == sizeof(eui64_t));
        memcpy(&out->v.ext_addr, src, sizeof(out->v.ext_addr));
        break;

    case IEEE802154_PIB_TYPE_NUI16:
        assert(e->size == 2);
        memcpy(&out->v.short_addr, src, sizeof(out->v.short_addr));
        break;

    case IEEE802154_PIB_TYPE_BYTES:
        assert(e->size == sizeof(ieee802154_octets_t));
        memcpy(&out->v.bytes, src, sizeof(out->v.bytes));
        break;
    }
    mutex_unlock(&mac->pib_lock);
}

void ieee802154_mac_pib_init(ieee802154_mac_t *mac)
{
    DEBUG("Internal MAC PIB init\n");
    mutex_init(&mac->pib_lock);
    for (unsigned i = 0; i < (uint8_t)IEEE802154_PIB_ATTR_COUNT; i++) {
        const ieee802154_pib_attr_entry_t *attr_entry = &ieee802154_pib_attr[i];

        if (attr_entry->def.type != attr_entry->type) {
            continue;
        }

        ieee802154_mac_mlme_set(mac, i, &attr_entry->def);
    }

    ieee802154_pib_value_t pib_value;

    pib_value.type = IEEE802154_PIB_TYPE_U8;
    pib_value.v.u8 = rand_u8();
    ieee802154_mac_mlme_set(mac, IEEE802154_PIB_BSN, &pib_value);

    pib_value.v.u8 = rand_u8();
    ieee802154_mac_mlme_set(mac, IEEE802154_PIB_DSN, &pib_value);
}

/** @} */
