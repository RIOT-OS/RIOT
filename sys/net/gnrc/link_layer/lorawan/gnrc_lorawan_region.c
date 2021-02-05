/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */
#include "kernel_defines.h"
#include "bitarithm.h"
#include "random.h"
#include "net/gnrc/lorawan/region.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define GNRC_LORAWAN_DATARATES_NUMOF (6U)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static uint8_t dr_sf[GNRC_LORAWAN_DATARATES_NUMOF] =
{ LORA_SF12, LORA_SF11, LORA_SF10, LORA_SF9, LORA_SF8, LORA_SF7 };
static uint8_t dr_bw[GNRC_LORAWAN_DATARATES_NUMOF] =
{ LORA_BW_125_KHZ, LORA_BW_125_KHZ, LORA_BW_125_KHZ, LORA_BW_125_KHZ,
  LORA_BW_125_KHZ, LORA_BW_125_KHZ };

int gnrc_lorawan_set_dr(gnrc_lorawan_t *mac, uint8_t datarate)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);

    if (!gnrc_lorawan_validate_dr(datarate)) {
        return -EINVAL;
    }
    DEBUG("gnrc_lorawan_region: Data Rate: DR%u \n",datarate);
    uint8_t bw = dr_bw[datarate];
    uint8_t sf = dr_sf[datarate];

    dev->driver->set(dev, NETOPT_BANDWIDTH, &bw, sizeof(bw));
    dev->driver->set(dev, NETOPT_SPREADING_FACTOR, &sf, sizeof(sf));

    return 0;
}

#if (IS_ACTIVE(CONFIG_LORAMAC_REGION_EU_868))
uint8_t gnrc_lorawan_rx1_get_dr_offset(uint8_t dr_up, uint8_t dr_offset)
{
    DEBUG("gnrc_lorawan_region: RX1DRoffset: %u \n", dr_offset);
    return (dr_up > dr_offset) ? (dr_up - dr_offset) : 0;
}

#elif (IS_ACTIVE(CONFIG_LORAMAC_REGION_IN_865))
uint8_t gnrc_lorawan_rx1_get_dr_offset(uint8_t dr_up, uint8_t dr_offset)
{
    DEBUG("gnrc_lorawan_region: RX1DRoffset: %u \n", dr_offset);
    int dr_eff = dr_offset > 5 ? 5 - dr_offset : dr_offset;
    return MIN(5, MAX(0, dr_up - dr_eff));
}
#endif

int gnrc_lorawan_phy_set_channel_mask(gnrc_lorawan_t *mac, uint16_t channel_mask)
{
    if (!channel_mask) {
        return -EINVAL;
    }

    for (int n = channel_mask, i = 0; n; n = n >> 1, i++) {
        if ((n & 0x1) && !mac->channel[i]) {
           return -EINVAL;
        }
    }

    mac->channel_mask = channel_mask;
    return 0;
}

void gnrc_lorawan_channels_init(gnrc_lorawan_t *mac)
{
    /* We set the channel mask for the default channels and populate from the list */
    mac->channel_mask = UINT16_MAX >> (16 - GNRC_LORAWAN_DEFAULT_CHANNELS_NUMOF);

    for (unsigned i = 0; i < GNRC_LORAWAN_DEFAULT_CHANNELS_NUMOF; i++) {
        mac->channel[i] = gnrc_lorawan_default_channels[i];
        DEBUG("gnrc_lorawan_region: Mac -> Channel %u %" PRIu32 " \n", i, mac->channel[i]);
    }

    for (unsigned i = GNRC_LORAWAN_DEFAULT_CHANNELS_NUMOF;
         i < GNRC_LORAWAN_MAX_CHANNELS; i++) {
        mac->channel[i] = 0;
    }
}

uint32_t gnrc_lorawan_pick_channel(gnrc_lorawan_t *mac)
{
    uint8_t index = 0;

    uint8_t random_number = random_uint32_range(0, bitarithm_bits_set(mac->channel_mask));
    unsigned state = mac->channel_mask;

    for (int i = 0; i < random_number; i++) {
        state = bitarithm_test_and_clear(state, &index);
    }
    return mac->channel[index];
}

void gnrc_lorawan_process_cflist(gnrc_lorawan_t *mac, uint8_t *cflist)
{
    /* TODO: Check CFListType to 0 */
    for (unsigned i = GNRC_LORAWAN_DEFAULT_CHANNELS_NUMOF; i < 8; i++) {
        le_uint32_t cl;
        cl.u32 = 0;
        memcpy(&cl, cflist, GNRC_LORAWAN_CFLIST_ENTRY_SIZE);
        mac->channel[i] = byteorder_ltohl(cl) * 100;
        mac->channel_mask |= 1 << i;
        cflist += GNRC_LORAWAN_CFLIST_ENTRY_SIZE;
        DEBUG("gnrc_lorawan_region: Mac -> Channel %u %" PRIu32 " \n", i, mac->channel[i]);
    }
}

uint8_t gnrc_lorawan_region_mac_payload_max(uint8_t datarate)
{
    if (datarate < 3) {
        return GNRC_LORAWAN_MAX_PAYLOAD_1;
    }
    else if (datarate == 3) {
        return GNRC_LORAWAN_MAX_PAYLOAD_2;
    }
    else {
        return GNRC_LORAWAN_MAX_PAYLOAD_3;
    }
}

bool gnrc_lorawan_validate_dr(uint8_t dr)
{
    if (dr < GNRC_LORAWAN_DATARATES_NUMOF) {
        return true;
    }
    DEBUG("gnrc_lorawan_region: Invalid DR.\n");
    return false;
}

/** @} */
