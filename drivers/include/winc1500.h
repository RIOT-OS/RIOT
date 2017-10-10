/*
 * Copyright (C) 2017 Bumsik Kim <k.bumsik@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_winc1500 WINC1500
 * @ingroup     drivers_netdev
 * @brief       Driver for the WINC1500 WiFi module
 * @details
 *
 * ## Introduction
 * WINC1500 (or ATWINC1500) is a Wi-Fi chipset from Atmel. This chipset is
 * designed to be used with microcontrollers(MCUs). This is offered as Wi-Fi
 * only modules to interface with a MCU through SPI and complete MCU modules
 * such as ATSAMW25, a heart of Arduino-mkr1000.
 *
 * ## Hardware/Firmware support
 * This driver supports the following modules but not limited to:
 *      * ATWINC1500-XPRO
 *      * Arduino-mkr1000
 *
 * Currently, the supported module firmware verison is `19.5.2`. Since this
 * firmware version is the lastest version, you may need to upgrade the firmware
 * to use this driver. You can use [Arduino IDE (for Arduino-mkr1000)][1] or
 * look at [Appendix D in the WINC1500 Software Design Guide][2] and
 * [Section 5.1 in the Getting Started Guide for ATWINC1500][3] in order
 * to update.
 *
 * [1]: https://www.arduino.cc/en/Tutorial/FirmwareUpdater
 * [2]: http://www.atmel.com/Images/Atmel-42420-WINC1500-Software-Design-Guide_UserGuide.pdf
 * [3]: http://ww1.microchip.com/downloads/en/devicedoc/atmel-42417-getting-started-guide-for-atwinc1500wifi-using-samd21-xplained-pro_userguide.pdf
 *
 * ## How to use the driver
 * Firstly, you need to include `USEMODULE += winc1500` in a Makefile. This
 * will add Wi-Fi connectivity to your board. You need to use GNRC network stack
 * for higher level networking capability.
 *
 *
 * @todo        Support for firmware version lower than 19.5.2.
 * @todo        RIOT Socket port for the module's socket API.
 * @todo        WPS-Enterprise support.
 * @todo        More Wi-Fi mode support, such as Wi-Fi AP mode.
 *
 * @{
 *
 * @file
 * @brief       Device driver definition for the WINC1500 WiFi module.
 *
 * @author      Bumsik Kim <k.bumsik@gmail.com>
 */
#ifndef WINC1500_H
#define WINC1500_H

#include "mutex.h"
#include "mbox.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#ifdef MODULE_GNRC_NETDEV
#include "net/netdev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name String length definitions from Wi-Fi standard specification.
 * @{
 */
#define WINC1500_MAX_SSID_LEN   (32) /**< Maximum length of SSID without a null charater */
#define WINC1500_MAX_PSK_LEN    (63) /**< Maximum length of PSK without a null charater */
#define WINC1500_MIN_PSK_LEN    (8)  /**< Minimum length of PSK without a null charater */
#define WINC1500_MAC_ADDRES_LEN (6)  /**< MAC address length */
/** @} */

/**
 * @brief   Flags of an access points' security options
 */
typedef enum {
    WINC1500_SEC_UNKNOWN =            (0),      /**< Error state */
    WINC1500_SEC_FLAGS_OPEN =         (1<<0),   /**< Open AP */
    WINC1500_SEC_FLAGS_WPA =          (1<<1),   /**< AP with WPA security */
    WINC1500_SEC_FLAGS_WPA2 =         (1<<2),   /**< AP with WPA2 security */
    WINC1500_SEC_FLAGS_WEP =          (1<<3),   /**< AP with WEP security */
    WINC1500_SEC_FLAGS_ENTERPRISE =   (1<<4),   /**< AP with WPA2-Enterprise security */
    WINC1500_SEC_FLAGS_WPS =          (1<<5)    /**< AP with WPS function enabled */
} winc1500_sec_flags_t;

/**
 * @brief   Status and error return codes
 */
enum {
    WINC1500_OK              = (0),      /**< everything was fine */
    WINC1500_ERR             = (-1),     /**< generic winc1500 module error */
    WINC1500_ERR_SPI         = (-2),     /**< error initializing the SPI bus */
    WINC1500_ERR_NODEV       = (-3),     /**< did not detect WINC1500 */
    WINC1500_ERR_FW_VER_MISMATCH = (-4), /**< did not detect WINC1500 */
} ;

/**
 * @brief   Struct containing the needed peripheral configuration
 *
 * These parameters are needed to configure the device at startup.
 */
typedef struct {
    char *ssid;                 /**< SSID string. The string must not longer
                                        than WINC1500_MAX_SSID_LEN */
    char *password;             /**< Passphrase of an AP. The string must not longer
                                        than WINC1500_MAX_PSK_LEN and not shorter
                                        than WINC1500_MIN_PSK_LEN */
    winc1500_sec_flags_t sec;   /**< Security options of an AP */
    int8_t rssi;                /**< Signal strength from an AP */
} winc1500_ap_t;

/**
 * @brief   Struct containing the needed peripheral configuration
 *
 * These parameters are needed to configure the device at startup.
 */
typedef struct {
    spi_t     spi;          /**< SPI device the module is connected to */
    spi_cs_t  cs_pin;       /**< SPI chip select pin */
    gpio_t    int_pin;      /**< SPI interrupt pin */
    gpio_t    reset_pin;    /**< Module RESET pin */
    gpio_t    en_pin;       /**< Module EN pin */
    gpio_t    wake_pin;     /**< Module WAKE pin */
    spi_clk_t spi_clk;      /**< SPI clock speed to use */
} winc1500_params_t;

/**
 * @brief   Device descriptor for the WINC1500 WiFi module
 */
typedef struct {
#ifdef MODULE_GNRC_NETDEV
    netdev_t netdev;      /**< extended netdev structure */
    mutex_t  mutex;       /**< Mutex used for locking concurrent sends */
    uint32_t rx_addr;     /**< The module's base addresss of the received buffer */
    uint16_t rx_offset;   /**< The module's addresss offset to the received buffer */
    int      rx_size;     /**< The module's size of the received buffer */
    uint16_t rx_rem_size; /**< The module's remaining size of the received buffer */
#endif
    winc1500_params_t params;   /**< Configuration parameters */
    mbox_t event_mbox;          /**< Message from the event handler */
    uint32_t ip_addr;           /**< Device's local IPv4 address */
    uint8_t  state;             /**< current state of the radio */
    kernel_pid_t pid;           /**< pid of the event handler thread */
} winc1500_t;

/**
 * @brief   Setup an WINC1500 based device state.
 *
 * This function sets SPI pins, initializes the device state structure.
 * It does not initialize the device itself. In case GNRC_NETDEV is not used,
 * this function doesn't need to be called to initialize the device.
 *
 * @param[out] dev     the handle of the device to initialize
 * @param[in]  params  parameters for device initialization
 */
void winc1500_setup(winc1500_t *dev, const winc1500_params_t *params);

/**
 * @brief   Initialize a WINC1500 device
 *
 * This function sets a SPI bus and GPIOs, and initializes the device. When
 * GNRC_NETDEV module is used this function won't have any effect, since the
 * device is automatically initialized on startup.
 *
 * @param[in]  params       parameters for device initialization
 *
 * @return                  BME280_OK on success
 * @return                  BME280_ERR_I2C
 * @return                  BME280_ERR_NODEV
 * @return                  BME280_ERR_NOCAL
 */
int winc1500_init(const winc1500_params_t *params);

/**
 * @brief   Get MAC address stored in a WINC1500 device's flash memory
 *
 * @param[out]  addr    An array to store MAC address. The input array must be
 *                      longer than 6 bytes.
 *
 * @return      0       on success
 * @return      -1      on error
 */
int winc1500_get_mac_addr(uint8_t addr[]);

/**
 * @brief Scan for the number of access points nearby
 *
 * @return >0 the number of APs scanned.
 * @return -1 on error
 */
int winc1500_scan(void);

/**
 * @brief   Retrieve information of an access point.
 *
 * @note    `winc1500_scan()` has to be called prior to this function.
 *
 * @param[out]  ap_result   Result of an AP's information.
 * @param[in]   ap_idx      An index of an AP. The maximum index number
 *                          is decided from `winc1500_scan()`.
 *
 * @return      0       on success
 * @return      -1      on error
 */
int winc1500_read_ap(winc1500_ap_t *ap_result, uint8_t ap_idx);

/**
 * @brief   Connects an access point.
 *
 * @param[in]   ap_info   SSID, password, and security level
 *                          of a connecting AP.
 *
 * @return      0       on success
 * @return      -1      on failure
 */
int winc1500_connect(const winc1500_ap_t *ap_info);

/**
 * @brief   Connects any access points from a list.
 *
 * @note    An AP coming first has more priority.
 *
 * @param[in]   ap_info   An array of SSID, password, and security level of
 *                          connecting APs.
 *
 * @return      0       on success
 * @return      -1      on failure
 */
int winc1500_connect_list(const winc1500_ap_t ap_info[]);

/**
 * @brief   Retrieve information of a connected access point.
 *
 * @note    It will return error when the Wi-Fi is not connected to an AP. So
 *          it also can be used to check whether the WINC1500 is connected or not.
 *
 * @param[out]   ap_result   Result of the connected AP's information.
 * @param[out]   mac_addr    The MAC address of the AP.
 *
 * @return      0       on success
 * @return      -1      on failure
 */
int winc1500_connection_info(winc1500_ap_t *ap_result, uint8_t *mac_addr);

/**
 * @brief   Disconnect an access point.
 *
 * @return      0       on success
 * @return      -1      on failure
 */
int winc1500_disconnect(void);

#ifdef __cplusplus
}
#endif

#endif /* WINC1500_H */
/** @} */
