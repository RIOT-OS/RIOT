/*
 * Copyright (C) 2016 Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sdcard_spi
 * @{
 *
 * @file
 * @brief       Interface for issuing commands on sd-cards via SPI.
 * @details     For details of the sd card standard and the spi mode refer to
 *              "SD Specifications Part 1 Physical Layer Simplified Specification".
 *              References to the sd specs in this file apply to Version 5.00
 *              from August 10, 2016. For further details see
 *              https://www.sdcard.org/downloads/pls/pdf/part1_500.pdf.
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifndef SDCARD_SPI_INTERNAL_H
#define SDCARD_SPI_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/spi.h"
#include "periph/gpio.h"
#include "stdbool.h"
#include "sdcard_spi.h"


/* number of clocks that should be applied to the card on init
   before taking furter actions (see sd spec. 6.4.1.1 Power Up Time of Card) */
#define SD_POWERSEQUENCE_CLOCK_COUNT 74

#define SD_CARD_PREINIT_CLOCK_PERIOD_US 10 /* used to generate 100 kHz clock in init phase*/
#define SD_CARD_WAIT_AFTER_POWER_UP_US  1000

/* R1 response bits (see sd spec. 7.3.2.1 Format R1) */
#define SD_R1_RESPONSE_PARAM_ERROR       (1<<6)
#define SD_R1_RESPONSE_ADDR_ERROR        (1<<5)
#define SD_R1_RESPONSE_ERASE_SEQ_ERROR   (1<<4)
#define SD_R1_RESPONSE_CMD_CRC_ERROR     (1<<3)
#define SD_R1_RESPONSE_ILLEGAL_CMD_ERROR (1<<2)
#define SD_R1_RESPONSE_ERASE_RESET       (1<<1)
#define SD_R1_RESPONSE_IN_IDLE_STATE     (0x01)
#define SD_INVALID_R1_RESPONSE           (1<<7)

#define R1_VALID(X) (((X) >> 7) == 0)
#define R1_PARAM_ERR(X)   ((((X) &SD_R1_RESPONSE_PARAM_ERROR) != 0))
#define R1_ADDR_ERR(X)    ((((X) &SD_R1_RESPONSE_ADDR_ERROR) != 0))
#define R1_ERASE_ERR(X)   ((((X) &SD_R1_RESPONSE_ERASE_SEQ_ERROR) != 0))
#define R1_CMD_CRC_ERR(X) ((((X) &SD_R1_RESPONSE_CMD_CRC_ERROR) != 0))
#define R1_ILL_CMD_ERR(X) ((((X) &SD_R1_RESPONSE_ILLEGAL_CMD_ERROR) != 0))
#define R1_IDLE_BIT_SET(X) (((X) &SD_R1_RESPONSE_IN_IDLE_STATE) != 0)
#define R1_ERROR(X) (R1_PARAM_ERR(X) || R1_ADDR_ERR(X) || R1_ERASE_ERR(X) || \
                     R1_CMD_CRC_ERR(X) || R1_ILL_CMD_ERR(X))

/* see sd spec. 7.3.3.1 Data Response Token */
#define DATA_RESPONSE_IS_VALID(X)  (((X) & 0x11) == 0x01)
#define DATA_RESPONSE_ACCEPTED(X)  (((X) & 0x0E) == (1<<2))
#define DATA_RESPONSE_CRC_ERR(X)   (((X) & 0x0E) == 0x0A)
#define DATA_RESPONSE_WRITE_ERR(X) (((X) & 0x0E) == 0x0C)

/* see sd spec. 5.1 OCR register */
#define OCR_VOLTAGE_3_2_TO_3_3 (1L << 20)
#define OCR_VOLTAGE_3_3_TO_3_4 (1L << 21)

/* card capacity status (CCS=0: the card is SDSD; CCS=1: card is SDHC or SDXC) */
#define OCR_CCS (1L << 30)

/* This bit is set to low if the card has not finished power up routine */
#define OCR_POWER_UP_STATUS (1L << 31)

/* to ensure the voltage range check on init is done properly you need to
   define this according to your actual interface/wiring with the sd-card */
#define SYSTEM_VOLTAGE (OCR_VOLTAGE_3_2_TO_3_3 | OCR_VOLTAGE_3_2_TO_3_3)


/* see sd spec. 7.3.1.3 Detailed Command Description */
#define SD_CMD_PREFIX_MASK (1<<6)

#define SD_CMD_0 0   /* Resets the SD Memory Card */
#define SD_CMD_1 1   /* Sends host capacity support info and starts the cards init process */
#define SD_CMD_8 8   /* Sends SD Card interface condition incl. host supply voltage info */
#define SD_CMD_9 9   /* Asks the selected card to send its card-specific data (CSD) */
#define SD_CMD_10 10 /* Asks the selected card to send its card identification (CID) */
#define SD_CMD_12 12 /* Forces the card to stop transmission in Multiple Block Read Operation */
#define SD_CMD_13 13 /* Sent as ACMD13 asks the card to send it's SD status */

#define SD_CMD_16 16 /* In case of SDSC Card, block length is set by this command */
#define SD_CMD_17 17 /* Reads a block of the size selected by the SET_BLOCKLEN command */
#define SD_CMD_18 18 /* Continuously transfers data blocks from card to host
                        until interrupted by a STOP_TRANSMISSION command */
#define SD_CMD_24 24 /* Writes a block of the size selected by the SET_BLOCKLEN command */
#define SD_CMD_25 25 /* Continuously writes blocks of data until 'Stop Tran'token is sent */
#define SD_CMD_41 41 /* Reserved (used for ACMD41) */
#define SD_CMD_55 55 /* Defines to the card that the next commmand is an application specific
                        command rather than a standard command */
#define SD_CMD_58 58 /* Reads the OCR register of a card */
#define SD_CMD_59 59 /* Turns the CRC option on or off. Argument: 1:on; 0:off */

#define SD_CMD_8_VHS_2_7_V_TO_3_6_V 0x01
#define SD_CMD_8_CHECK_PATTERN      0xB5
#define SD_CMD_NO_ARG     0x00000000
#define SD_ACMD_41_ARG_HC 0x40000000
#define SD_CMD_59_ARG_EN  0x00000001
#define SD_CMD_59_ARG_DIS 0x00000000

/* see sd spec. 7.3.3 Control Tokens */
#define SD_DATA_TOKEN_CMD_17_18_24 0xFE
#define SD_DATA_TOKEN_CMD_25       0xFC
#define SD_DATA_TOKEN_CMD_25_STOP  0xFD

#define SD_SIZE_OF_CID_AND_CSD_REG 16
#define SD_SIZE_OF_SD_STATUS 64
#define SD_BLOCKS_FOR_REG_READ 1
#define SD_GET_CSD_STRUCTURE(CSD_RAW_DATA) ((CSD_RAW_DATA)[0] >> 6)
#define SD_CSD_V1 0
#define SD_CSD_V2 1
#define SD_CSD_VUNSUPPORTED -1

/* the retry counters below are used as timeouts for specific actions.
   The values may need some adjustments to either give the card more time to respond
   to commands or to achieve a lower delay / avoid infinite blocking. */
#define R1_POLLING_RETRY_CNT       1000000
#define SD_DATA_TOKEN_RETRY_CNT    1000000
#define INIT_CMD_RETRY_CNT         1000000
#define INIT_CMD0_RETRY_CNT        3
#define SD_WAIT_FOR_NOT_BUSY_CNT   1000000 /* use -1 for full blocking till the card isn't busy */
#define SD_BLOCK_READ_CMD_RETRIES  10     /* only affects sending of cmd not whole transaction! */
#define SD_BLOCK_WRITE_CMD_RETRIES 10    /* only affects sending of cmd not whole transaction! */

/* memory capacity in bytes = (C_SIZE+1) * SD_CSD_V2_C_SIZE_BLOCK_MULT * BLOCK_LEN */
#define SD_CSD_V2_C_SIZE_BLOCK_MULT 1024

#define SD_CARD_SPI_MODE SPI_MODE_0

/* this speed setting is only used while the init procedure is performed */
#define SD_CARD_SPI_SPEED_PREINIT SPI_CLK_400KHZ

/* after init procedure is finished the driver auto sets the card to this speed */
#define SD_CARD_SPI_SPEED_POSTINIT SPI_CLK_10MHZ

#define SD_CARD_DUMMY_BYTE 0xFF

#define SDCARD_SPI_IEC_KIBI (1024L)
#define SDCARD_SPI_SI_KILO  (1000L)

typedef enum {
    SD_INIT_START,
    SD_INIT_SPI_POWER_SEQ,
    SD_INIT_SEND_CMD0,
    SD_INIT_SEND_CMD8,
    SD_INIT_CARD_UNKNOWN,
    SD_INIT_SEND_ACMD41_HCS,
    SD_INIT_SEND_ACMD41,
    SD_INIT_SEND_CMD1,
    SD_INIT_SEND_CMD58,
    SD_INIT_SEND_CMD16,
    SD_INIT_ENABLE_CRC,
    SD_INIT_READ_CID,
    SD_INIT_READ_CSD,
    SD_INIT_SET_MAX_SPI_SPEED,
    SD_INIT_FINISH
} sd_init_fsm_state_t;

/**
 * @brief                 Sends a cmd to the sd card.
 *
 * @param[in] card        Initialized sd-card struct
 * @param[in] sd_cmd_idx  A supported sd-card command index for SPI-mode like defined in
 *                        "7.3.1.3 Detailed Command Description" of sd spec.
 *                        (for CMDX this parameter is simply the integer value X).
 * @param[in] argument    The argument for the given cmd. As described by "7.3.1.1 Command Format".
 *                        This argument is transmitted byte wise with most significant byte first.
 * @param[in] max_retry   Specifies how often the command should be retried if an error occures.
 *                        Use 0 to try only once, -1 to try forever, or n to retry n times.
 *
 * @return                R1 response of the command if no (low-level) communication error occured
 * @return                SD_INVALID_R1_RESPONSE if either waiting for the card to enter
 *                        not-busy-state timed out or spi communication failed
 */
char sdcard_spi_send_cmd(sdcard_spi_t *card, char sd_cmd_idx, uint32_t argument, int32_t max_retry);

/**
 * @brief                 Sends an acmd to the sd card. ACMD<n> consists of sending CMD55 + CMD<n>
 *
 * @param[in] card        Initialized sd-card struct
 * @param[in] sd_cmd_idx  A supported sd-card command index for SPI-mode like defined in
 *                        "7.3.1.3 Detailed Command Description" of sd spec.
 *                        (for ACMDX this parameter is simply the integer value X).
 * @param[in] argument    The argument for the given cmd. As described by "7.3.1.1 Command Format".
 *                        This argument is transmitted byte wise with most significant byte first.
 * @param[in] max_retry   Specifies how often the command should be retried if an error occures.
 *                        Use 0 to try only once, -1 to try forever, or n to retry n times.
 *
 * @return                R1 response of the command if no (low-level) communication error occured
 * @return                SD_INVALID_R1_RESPONSE if either waiting for the card to enter
 *                        not-busy-state timed out or spi communication failed
 */
char sdcard_spi_send_acmd(sdcard_spi_t *card, char sd_cmd_idx, uint32_t argument, int32_t max_retry);

/**
 * @brief                 Gets the sector count of the card.
 *
 * @param[in] card        Initialized sd-card struct
 *
 * @return                number of available sectors
 */
uint32_t sdcard_spi_get_sector_count(sdcard_spi_t *card);

/**
 * @brief                 Gets the allocation unit size of the card.
 *
 * @param[in] card        Initialized sd-card struct
 *
 * @return                size of AU in bytes
 */
uint32_t sdcard_spi_get_au_size(sdcard_spi_t *card);

/**
 * @brief                 Gets the SD status of the card.
 *
 * @param[in]  card       Initialized sd-card struct
 * @param[out] sd_status  memory location where status struct is stored
 *
 * @return                sd_status_t struct that contains all SD status information
 */
sd_rw_response_t sdcard_spi_read_sds(sdcard_spi_t *card, sd_status_t *sd_status);


#ifdef __cplusplus
}
#endif

#endif /* SDCARD_SPI_INTERNAL_H */
/** @} */
