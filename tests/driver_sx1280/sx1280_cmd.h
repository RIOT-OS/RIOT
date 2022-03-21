/*
 * Copyright (C) 2020-2022 Université Grenoble Alpes
 */

/**
 * @file
 * @brief       shell commands for using the SX1280 as a LoRa 2.4 GHz modem.
 *
 * @author      Nicolas Albarel
 * @author      Didier Donsez
 * @author      Olivier Alphand
 */

#ifndef SX1280_CMD_H
#define SX1280_CMD_H

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "ral_defs.h"

#include "ism2400.h"

#ifndef DELAY_BETWEEN_TX
#define DELAY_BETWEEN_TX                    (2000000U)
#endif

#ifndef SECONDS_IF_INIT_FAILED
#define SECONDS_IF_INIT_FAILED              (5U)
#endif

/*
 * @brief The LoRa modulation parameters for Rx and Tx
 */
extern ral_params_lora_t params;

/*
 * @brief Get the frequency in Hz from the driver lora definition
 */
uint32_t getBW(ral_lora_bw_t lora_bw);

/*
 * @brief Initialize the SX1280 driver
 */
int sx1280_init(void);

/*
 * @brief Initialize the SX1280 driver and reboot on initialization failure
 */
void sx1280_init_and_reboot_on_failure(void);

/*
 * @brief Get/Set the central frequencies of the LoRa channel
 */
int sx1280_channel_cmd(int argc, char **argv);

/*
 * @brief Get/Set the TX power of the next TX LoRa communications
 */
int sx1280_txpower_cmd(int argc, char **argv);

/*
 * @brief Get/Set the sync word of the next LoRa communications
 */
int sx1280_syncword_cmd(int argc, char **argv);

/*
 * @brief Get/Set the preamble length of the next LoRa communications
 */
int sx1280_preamble_cmd(int argc, char **argv);

/*
 * @brief Get/Set the payload size of the next LoRa communications
 */
int sx1280_payload_cmd(int argc, char **argv);

/*
 * @brief Get/Set the IQ of the next LoRa communications
 */
int sx1280_invertiq_cmd(int argc, char **argv);

/*
 * @brief Get/Set the CRC of the next LoRa communications
 */
int sx1280_crc_cmd(int argc, char **argv);

/*
 * @brief Setup the bandwidth, the spreading factor and the code rate of the next LoRa communications
 */
int sx1280_setup_cmd(int argc, char **argv);

/*
 * @brief Send packets to the SX1280 radio
 */
int sx1280_send_cmd(int argc, char **argv);

/*
 * @brief Listen the SX1280 radio for count packets during timeout_in_sec seconds for each packet
 *
 * @param timeout_in_sec
 * @param count
 */
int sx1280_listen_cmd(int argc, char **argv);


#endif
