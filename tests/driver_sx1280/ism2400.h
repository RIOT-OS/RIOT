/*
 * Copyright (C) 2020-2022 Université Grenoble Alpes
 */

/**
 * @file
 * @brief       LoRa 2.4 GHz modulation parameter.
 *
 * @author      Nicolas Albarel
 * @author      Didier Donsez
 * @author      Olivier Alphand
 *
 */

#ifndef ISM2400_H_
#define ISM2400_H_

// https://www.electronics-notes.com/articles/connectivity/wifi-ieee-802-11/channels-frequencies-bands-bandwidth.php
// Often referred to as the 2.4 GHz band, this spectrum is the most widely used of the bands available for Wi-Fi. Used by 802.11b, g, & n. It can carry a maximum of three non-overlapping channels. This band is widely used by many other non-licensed items including microwave ovens, Bluetooth, etc.

// See https://lora-developers.semtech.com/documentation/tech-papers-and-guides/physical-layer-proposal-2.4ghz/

#define SX1280_MAX_TXPOWER                  13

#define ISM2400_LOWER_FREQUENCY             2400000000
#define ISM2400_HIGHER_FREQUENCY            2500000000

#define LORAWAN_ISM2400_CHANNEL_1           2403000000
#define LORAWAN_ISM2400_CHANNEL_2           2479000000
#define LORAWAN_ISM2400_CHANNEL_3           2425000000
#define LORAWAN_ISM2400_CHANNEL_BEACON  	2422000000

#define LORAWAN_ISM2400_BANDWITH            RAL_LORA_BW_800_KHZ
#define LORAWAN_ISM2400_SF                  12
#define LORAWAN_ISM2400_CR                  RAL_LORA_CR_LI_4_5
#define LORAWAN_ISM2400_SYNCWORD_PUBLIC     (0x21)
// #define LORAWAN_ISM2400_SYNCWORD_PRIVATE		(0x34)

// https://www.multitech.net/developer/software/lora/mtac-lora-2g4-3/
// http://www.multitech.net/developer/wp-content/uploads/2020/06/global_conf.json_.2g4.txt
#define MULTITECH_ISM2400_CHANNEL_1         2422000000
#define MULTITECH_ISM2400_CHANNEL_2         2425000000
#define MULTITECH_ISM2400_CHANNEL_3         2479000000

#define MULTITECH_ISM2400_BANDWITH          RAL_LORA_BW_800_KHZ
#define MULTITECH_ISM2400_SF                12
#define MULTITECH_ISM2400_CR                RAL_LORA_CR_LI_4_8

// For DR0-DR5
#define MULTITECH_ISM2400_LEN_IN_SYMB       (8)
// For DR6-DR7
//#define MULTITECH_ISM2400_LEN_IN_SYMB       (12)

//#if MULTITECH_CONCENTRATOR_MCU_VERSION==V00_02_16
//#define MULTITECH_ISM2400_SYNCWORD_PUBLIC   (0x12)
//#else
// Since Concentrator MCU version is V01.00.01, LoRa sync_word 0x21 (public)
#define MULTITECH_ISM2400_SYNCWORD_PUBLIC   LORAWAN_ISM2400_SYNCWORD_PUBLIC
//#endif


#endif /* ISM2400_H_ */
