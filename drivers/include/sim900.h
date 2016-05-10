#ifndef SIM900_H
#define SIM900_H

#include "mutex.h"
#include "thread.h"
#include "xtimer.h"

#include "net/gnrc.h"
#include "net/gnrc/ppp/ppp.h"
#include "net/hdlc/fcs.h"
#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SIM900_MAX_RESP_SIZE (100U)
#define SIM900_MAX_CMD_SIZE (100U)
#define SIM900_APN_SIZE (40U)


/**
 * @brief	Enum of possible states of sim900 device
 */
typedef enum {
    AT_STATE_IDLE,
    AT_STATE_CMD,
    AT_STATE_RX,
} dev_state_t;

/**
 * @brief	Enum of possible states while receiving data
 */
typedef enum {
    PPP_RX_STARTED,
    PPP_RX_IDLE
} ppp_rx_state_t;


/**
 * @brief sim900 pppdev device
 * @extends pppdev_t
 */
typedef struct sim900_t {
    pppdev_t netdev;                            /**< extended pppdev structure */
    uart_t uart;                                /**< UART port of device */
    uint8_t rx_buf[SIM900_MAX_CMD_SIZE];        /**< rx buffer */
    uint16_t rx_count;                          /**< number of bytes received from device */
    uint16_t int_count;                         /**< same as rx_count, but as a temporal variable in interrupt context */
    dev_state_t state;                          /**< state of device */
    mutex_t out_mutex;                          /**< mutex for locking thread on write ops */
    kernel_pid_t mac_pid;                       /**< PID of thread */
    uint8_t _num_esc;                           /**< number of of escape strings in current AT command */
    uint32_t _stream;                           /**< stream containing last 4 received AT characters */
    uint8_t at_status;                          /**< flags of AT command content */
    xtimer_t xtimer;                            /**< timer for handling timeouts */
    void (*_cb)(struct sim900_t *dev);          /**< callback when AT command finishes */
    void (*_timer_cb)(struct sim900_t *dev);    /**< callback when there's a timeout */
    msg_t msg;                                  /**< msg structure for sending events*/
    ppp_rx_state_t ppp_rx_state;                /**< rx state of device in data mode*/
    uint8_t escape;                             /**< escape character (0x20 or 0x00)*/
    uint16_t int_fcs;                           /**< fcs holder */
    uint32_t tx_accm;                           /**< Async Control Character Map for transmission */
    uint32_t rx_accm;                           /**< Async Control Character Map for reception */
    uint8_t apn[SIM900_APN_SIZE];               /**< stores APN name */
    uint8_t apn_len;                            /**< stores APN name */
} sim900_t;

/**
 * @brief	Struct containing the needed peripheral configuration
 */
typedef struct sim900_params_t {
    uart_t uart; /**< UART port of device */
} sim900_params_t;

/**
 * @brief   Ready the device for initialization through it's pppdev interface
 *
 * @param[in] dev           device descriptor
 * @param[in] params        peripheral configuration to use
 */
void sim900_setup(sim900_t *dev, const sim900_params_t *params);


#ifdef __cplusplus
}
#endif


#endif
