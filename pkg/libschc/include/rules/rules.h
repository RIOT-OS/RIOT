/*
 * Copyright (C) 2018 imec IDLab
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @internal
 * @author  boortmans <bart.moons@gmail.com>
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */
#ifndef RULES_RULES_H
#define RULES_RULES_H

#include "kernel_defines.h"

#include "schc.h"
#ifdef USE_COAP
#include "net/coap.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if USE_IP6
static const struct schc_ipv6_rule_t ipv6_rule1 = {
    .up = 10, .down = 10, .length = 11,
    {
        /* field,       ML, len, pos, dir,  val,                MO,             CDA         */
        { IP6_V,         0,   4,   1, BI,   {6},                &mo_equal,      NOTSENT     },
        { IP6_TC,        0,   8,   1, BI,   {0},                &mo_ignore,     NOTSENT     },
        { IP6_FL,        0,  20,   1, BI,   {0, 0, 0},          &mo_ignore,     NOTSENT     },
        { IP6_LEN,       0,  16,   1, BI,   {0, 0},             &mo_ignore,     COMPLENGTH  },
        { IP6_NH,        0,   8,   1, BI,   {17},               &mo_equal,      NOTSENT     },
        { IP6_HL,        0,   8,   1, UP,   {64},               &mo_equal,      NOTSENT     },
        { IP6_HL,        0,   8,   1, DOWN, {0},                &mo_ignore,     VALUESENT   },
        { IP6_DEVPRE,    0,  64,   1, BI,   {
                0x20, 0x01, 0x0d, 0xb8, 0x00, 0x01, 0x00, 0x00
            },                                                  &mo_equal,      NOTSENT     },
        { IP6_DEVIID,    0,  64,   1, BI,   {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
            },                                                  &mo_equal,      NOTSENT },
        { IP6_APPPRE,    4,  64,   1, BI,   {
                /* you can store as many IPs as (MAX_FIELD_LENGTH / 8) */
                0x20, 0x01, 0x0d, 0xb8, 0x00, 0x01, 0x00, 0x00,
                0x20, 0x01, 0x0d, 0xb8, 0x00, 0x02, 0x00, 0x00,
                0x20, 0x01, 0x0d, 0xb8, 0x00, 0x03, 0x00, 0x00,
                0x20, 0x01, 0x0d, 0xb8, 0x00, 0x04, 0x00, 0x00
            },                                                  &mo_matchmap,   MAPPINGSENT },
        { IP6_APPIID,    0,  64,   1, BI,   {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02
            },                                                  &mo_equal,      NOTSENT },
    }
};

/* link local test rule */
static const struct schc_ipv6_rule_t ipv6_rule2 = {
    .up = 10, .down = 10, .length = 10,
    {
        /* field,       ML, len, pos, dir,  val,                MO,             CDA         */
        { IP6_V,         0,   4,   1, BI,   {6},                &mo_equal,      NOTSENT     },
        { IP6_TC,        0,   8,   1, BI,   {0},                &mo_ignore,     NOTSENT     },
        { IP6_FL,        0,  20,   1, BI,   {0, 0, 0},          &mo_ignore,     NOTSENT     },
        { IP6_LEN,       0,  16,   1, BI,   {0, 0},             &mo_ignore,     COMPLENGTH  },
        { IP6_NH,        2,   8,   1, BI,   {17, 58},           &mo_matchmap,   MAPPINGSENT },
        { IP6_HL,        2,   8,   1, BI,   {64, 255},          &mo_matchmap,   NOTSENT     },
        { IP6_DEVPRE,    0,  64,   1, BI,   {
                0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            },                                                  &mo_equal,      NOTSENT     },
        { IP6_DEVIID,   62,  64,   1, BI,   {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
            },                                                  &mo_MSB,        LSB      },
        { IP6_APPPRE,    0,  64,   1, BI,   {
                0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            },                                                  &mo_equal,      NOTSENT     },
        { IP6_APPIID,   62,  64,   1, BI,   {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
            },                                                  &mo_MSB,        LSB      },
    }
};
#endif

#if USE_UDP
static const struct schc_udp_rule_t udp_rule1 = {
    .up = 4, .down = 4, .length = 4,
    {
        /* field,       ML, len, pos, dir,  val,                MO,             CDA         */
        /* set field length to 16 to indicate 16 bit values
         *  MO param length to 2 to indicate 2 indices */
        { UDP_DEV,       2,  16,   1,  BI,  {
                0x33, 0x16, /* 5683 or */
                0x33, 0x17  /* 5684 */
            },                                                  &mo_matchmap,   MAPPINGSENT },
        { UDP_APP,       2,  16,   1,  BI,  {
                0x33, 0x16, /* 5683 or */
                0x33, 0x17  /* 5684 */
            },                                                  &mo_matchmap,   MAPPINGSENT },
        { UDP_LEN,       0,  16,   1,  BI,  {0, 0},             &mo_ignore,     COMPLENGTH  },
        { UDP_CHK,       0,  16,   1,  BI,  {0, 0},             &mo_ignore,     COMPCHK     },
    }
};

static const struct schc_udp_rule_t udp_rule2 = {
    .up = 4, .down = 4, .length = 4,
    {
        /* field,       ML, len, pos, dir,  val,                MO,             CDA         */
        { UDP_DEV,      12,  16,   1,  BI,  {0x1F, 0x40},       &mo_MSB,        LSB         },
        { UDP_APP,      12,  16,   1,  BI,  {0x1F, 0x40},       &mo_MSB,        LSB         },
        { UDP_LEN,       0,  16,   1,  BI,  {0, 0},             &mo_ignore,     COMPLENGTH  },
        { UDP_CHK,       0,  16,   1,  BI,  {0, 0},             &mo_ignore,     COMPCHK     },
    }
};

static const struct schc_udp_rule_t udp_rule3 = {
    .up = 4, .down = 4, .length = 4,
    {
        /* field,       ML, len, pos, dir,  val,                MO,             CDA         */
        { UDP_DEV,       0,  16,   1,  BI,  {0x13, 0x89},       &mo_equal,      NOTSENT     },
        { UDP_APP,       0,  16,   1,  BI,  {0x13, 0x88},       &mo_equal,      NOTSENT     },
        { UDP_LEN,       0,  16,   1,  BI,  {0, 0},             &mo_ignore,     COMPLENGTH  },
        { UDP_CHK,       0,  16,   1,  BI,  {0, 0},             &mo_ignore,     COMPCHK     },
    }
};
#endif

#if USE_COAP
/* It is important to use strings, identical to the ones defined in coap.h for the options. */

/* GET /usage */
static const struct schc_coap_rule_t coap_rule1 = {
    .up = 9, .down = 7, .length = 9,
    {
        /* field,       ML, len, pos, dir,  val,                MO,             CDA         */
        { COAP_V,        0,   2,   1,  BI,  {COAP_V1},          &mo_equal,      NOTSENT     },
        /* the MO_param_length (ML) is used to indicate the true length of the list */
        { COAP_T,        4,   2,   1,  BI,  {
                COAP_TYPE_CON, COAP_TYPE_NON, COAP_TYPE_ACK, COAP_TYPE_RST
            },                                                  &mo_matchmap,   MAPPINGSENT },
        { COAP_TKL,      0,   4,   1,  BI,  {4},                &mo_equal,      NOTSENT     },
        { COAP_C,        0,   8,   1,  BI,  {COAP_METHOD_PUT},  &mo_equal,      NOTSENT     },
        { COAP_MID,      0,  16,   1,  BI,  {0x23, 0xBB},       &mo_equal,      NOTSENT     },
        { COAP_TKN,     24,  32,   1,  BI,  {
                0x21, 0xFA, 0x01, 0x00
            },                                                  &mo_MSB,        LSB         },
        { COAP_URIPATH,  0,  40,   1,  BI,  "usage",            &mo_equal,      NOTSENT     },
        { COAP_NORESP,   0,   8,   1,  BI,  {0x1A},             &mo_equal,      NOTSENT     },
        { COAP_PAYLOAD,  0,   8,   1,  BI,  {0xFF},             &mo_equal,      NOTSENT     }
    }
};

/* POST temperature value */
static const struct schc_coap_rule_t coap_rule2 = {
    .up = 7, .down = 7, .length = 10,
    {
        /* field,       ML, len, pos, dir,  val,                MO,             CDA         */
        { COAP_V,        0,   2,   1, BI,   {COAP_V1},          &mo_equal,      NOTSENT     },
        { COAP_T,        0,   2,   1, BI,   {0},                &mo_ignore,     VALUESENT },
        { COAP_TKL,      0,   4,   1, BI,   {4},                &mo_equal,      NOTSENT     },
        { COAP_C,        0,   8,   1, UP,   {COAP_CODE_CONTENT}, &mo_equal,     NOTSENT     },
        { COAP_C,        0,   8,   1, DOWN, {COAP_METHOD_GET},  &mo_equal,      NOTSENT     },
        /* match the first 12 bits */
        { COAP_MID,     12,  16,   1, UP,   {0x23, 0xBB},       &mo_MSB,        LSB         },
        { COAP_MID,      0,  16,   1, DOWN, {0, 0},             &mo_ignore,     VALUESENT   },
        { COAP_TKN,      0,  32,   1, BI,   {0, 0, 0, 0},       &mo_ignore,     VALUESENT   },
        { COAP_URIPATH,  0,  32,   1, DOWN, "temp",             &mo_equal,      NOTSENT },
        /* respond with CONTENT */
        { COAP_PAYLOAD,  0,   8,   1, UP,   {0xFF},             &mo_equal,      NOTSENT     }
    }
};

static const struct schc_coap_rule_t coap_rule3 = {
    .up = 1, .down = 1, .length = 1,
    {
        /* field,       ML, len, pos, dir,  val,                MO,             CDA         */
        { COAP_V,        0,   2,   1,  BI,  {COAP_V1},          &mo_equal,      NOTSENT     },
    }
};

static const struct schc_coap_rule_t coap_rule4 = {
    .up = 12, .down = 12, .length = 12,
    {
        /* field,       ML, len, pos, dir,  val,                MO,             CDA         */
        { COAP_V,        0,   2,   1, BI,   {COAP_V1},          &mo_equal,      NOTSENT     },
        { COAP_T,        0,   2,   1, BI,   {COAP_TYPE_CON},    &mo_equal,      NOTSENT     },
        { COAP_TKL,      0,   4,   1, BI,   {8},                &mo_equal,      NOTSENT     },
        { COAP_C,        0,   8,   1, BI,   {COAP_METHOD_POST}, &mo_equal,      NOTSENT     },
        { COAP_MID,      0,  16,   1, BI,   {0x23, 0xBB},       &mo_ignore,     VALUESENT   },
        /* match the 24 first bits, send the last 8 */
        { COAP_TKN,     24,  32,   1, BI,   {
                0x21, 0xFA, 0x01, 0x00
        },                                                      &mo_MSB,        LSB         },
        { COAP_URIPATH,  0,  16,   1, BI,   "rd",               &mo_equal,      NOTSENT     },
        { COAP_CONTENTF, 0,   8,   1, BI,   {0x28},             &mo_equal,      NOTSENT     },
        { COAP_URIQUERY, 0,  72,   1, BI,   "lwm2m=1.0",        &mo_equal,      NOTSENT     },
        { COAP_URIQUERY, 0,  88,   1, BI,   "ep=magician",      &mo_equal,      NOTSENT     },
        { COAP_URIQUERY, 0,  48,   1, BI,   "lt=121",           &mo_equal,      NOTSENT     },
        /* respond with CONTENT */
        { COAP_PAYLOAD,  0,   8,   1, BI,   {0xff},             &mo_equal,      NOTSENT     }
    }
};
#endif

static const struct schc_compression_rule_t comp_rule_1 = {
    .rule_id = 0x01,
    .rule_id_size_bits = 8U,
#if USE_IP6
    &ipv6_rule1,
#endif
#if USE_UDP
    &udp_rule1,
#endif
#if USE_COAP
    &coap_rule1,
#endif
};

static const struct schc_compression_rule_t comp_rule_2 = {
    .rule_id = 0x02,
    .rule_id_size_bits = 8U,
#if USE_IP6
    &ipv6_rule1,
#endif
#if USE_UDP
    &udp_rule3,
#endif
#if USE_COAP
    &coap_rule2,
#endif
};

static const struct schc_compression_rule_t comp_rule_3 = {
    .rule_id = 0x03,
    .rule_id_size_bits = 8U,
#if USE_IP6
    &ipv6_rule2,
#endif
#if USE_UDP
    &udp_rule2,
#endif
#if USE_COAP
    &coap_rule3,
#endif
};

static const struct schc_compression_rule_t comp_rule_4 = {
    .rule_id = 0x04,
    .rule_id_size_bits = 8U,
#if USE_IP6
    &ipv6_rule2,
#endif
#if USE_UDP
    &udp_rule2,
#endif
#if USE_COAP
    &coap_rule4,
#endif
};

static const struct schc_fragmentation_rule_t frag_rule_21 = {
    .rule_id = 21,
    .rule_id_size_bits = 8,
    .mode = NO_ACK,
    .dir = BI,
    .FCN_SIZE = 1,      /* FCN size */
    .MAX_WND_FCN = 0,   /* Maximum fragments per window */
    .WINDOW_SIZE = 0,   /* Window size */
    .DTAG_SIZE = 0,     /* DTAG size */
};

static const struct schc_fragmentation_rule_t frag_rule_22 = {
    .rule_id = 22,
    .rule_id_size_bits = 8,
    .mode = ACK_ON_ERROR,
    .dir = BI,
    .FCN_SIZE = 3,      /* FCN size */
    .MAX_WND_FCN = 6,   /* Maximum fragments per window */
    .WINDOW_SIZE = 1,   /* Window size */
    .DTAG_SIZE = 0,     /* DTAG size */
};

static const struct schc_fragmentation_rule_t frag_rule_23 = {
    .rule_id = 23,
    .rule_id_size_bits = 8,
    .mode = ACK_ALWAYS,
    .dir = BI,
    .FCN_SIZE = 3,      /* FCN size */
    .MAX_WND_FCN = 6,   /* Maximum fragments per window */
    .WINDOW_SIZE = 1,   /* Window size */
    .DTAG_SIZE = 0,     /* DTAG size */
};

/* save compression rules in flash */
static const struct schc_compression_rule_t* node1_compression_rules[] = {
    &comp_rule_1, &comp_rule_2, &comp_rule_3, &comp_rule_4
};

/* save fragmentation rules in flash */
static const struct schc_fragmentation_rule_t* node1_fragmentation_rules[] = {
    &frag_rule_21, &frag_rule_22, &frag_rule_23,
};

/* rules for a particular device */
static const struct schc_device node1 = {
    .device_id = 1,
    .uncomp_rule_id = 0,
    .uncomp_rule_id_size_bits = 8,
    .compression_rule_count = ARRAY_SIZE(node1_compression_rules),
    .compression_context = &node1_compression_rules,
    .fragmentation_rule_count = ARRAY_SIZE(node1_fragmentation_rules),
    .fragmentation_context = &node1_fragmentation_rules
};
static const struct schc_device node2 = {
    .device_id = 2,
    .uncomp_rule_id = 0,
    .uncomp_rule_id_size_bits = 8,
    .compression_rule_count = ARRAY_SIZE(node1_compression_rules),
    .compression_context = &node1_compression_rules,
    .fragmentation_rule_count = ARRAY_SIZE(node1_fragmentation_rules),
    .fragmentation_context = &node1_fragmentation_rules
};

/* server keeps track of multiple devices: add devices to device list */
static const struct schc_device* devices[] = { &node1, &node2 };

#define DEVICE_COUNT    ((int)ARRAY_SIZE(devices))

#ifdef __cplusplus
}
#endif

#endif /* RULES_RULES_H */
