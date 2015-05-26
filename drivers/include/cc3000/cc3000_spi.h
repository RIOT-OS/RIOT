/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_cc3000 CC3000
 * @ingroup     drivers
 * @{
 *
 * @file
 * @brief       CC3000 RIOT interface
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef CC3000_SPI_H_
#define CC3000_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CC3000_WRITE_CMD            (0x01)
#define CC3000_READ_CMD             (0x03)

#define CC3000_FIRST_WRITE_SIZE     (0x04)
#define CC3000_SPI_HEADER_SIZE      (0x05)
#define CC3000_MIN_DATA_LENGTH      (0x05)

#define CC3000_FIRST_WRITE_WAIT     HWTIMER_TICKS(50)

typedef void (*gcSpiHandleRx)(void *p);

/**
 * @brief   Possible CC3000 states
 */
typedef enum cc3000_states_t {
    CC3000_STATE_POWERUP,
    CC3000_STATE_INITIALIZED,
    CC3000_STATE_IDLE,
    CC3000_STATE_FIRST_WRITE,
    CC3000_STATE_WRITE,
    CC3000_STATE_READ_IRQ
} cc3000_states_t;

typedef struct cc3000_spi_information_t {
    gcSpiHandleRx SPIRxHandler;
    char *rx_buffer;
    enum cc3000_states_t curr_state;
} cc3000_spi_information_t;

extern unsigned char wlan_tx_buffer[];

void cc3000_init(void);

extern void SpiOpen(gcSpiHandleRx pfRxHandler);
extern void SpiClose(void);
extern long SpiWrite(unsigned char *pUserBuffer, unsigned short usLength);
extern void SpiResumeSpi(void);

#ifdef __cplusplus
}
#endif

#endif /* CC3000_SPI_H_ */
/** @} */
