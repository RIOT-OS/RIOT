/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup     sys_stdio_nimble STDIO over NimBLE
 * @ingroup      sys_stdio
 *
 * @experimental This feature is experimental as some use-cases, such as examples/twr_aloha, show
 *               unexpected behaviour.
 *
 * @brief        Standard input/output backend using NimBLE.
 *
 * @note         'stdio_read' blocks until at least one character was read.
 *
 * @note         'stdio_write' is considered non-blocking even though it uses a mutex to protect the
 *               write buffer since only 'stdio_write' uses this mutex. Characters will be written
 *               in FIFO mode. Characters that do not fit in the buffer will be dropped.
 *
 * This module uses NimBLE for stdio. The bluetooth characteristic for
 * stdin is writable and the characteristic for stdout uses the indicate
 * mechanism to publish the system's output to a connected device. Data will be
 * sent out asynchronously via callout functions.
 *
 * To use this module, add
 * ```
 * USEMODULE += stdio_nimble
 * ```
 * to your makefile.
 *
 * You can change the default buffer sizes by adding
 * ```
 * CFLAGS += -DCONFIG_STDIO_NIMBLE_STDIN_BUFSIZE=1024
 * CFLAGS += -DCONFIG_STDIO_NIMBLE_STDOUT_BUFSIZE=2048
 * ```
 * to your makefile.
 *
 * **NOTE:** These values must be a power of two!
 *
 * By default, stdin and stdout buffers are cleared on a connect event. To keep the
 * content add the following to your makefile:
 * ```
 * CFLAGS += -DCONFIG_STDIO_NIMBLE_CLEAR_BUFFER_ON_CONNECT=0
 * ```
 *
 * For automatic bluetooth advertising a module is provided: *nimble_autoadv*.
 * It will take care of enabling advertising on disconnect events and
 * disabling advertising on connect events. It can be enabled by adding
 * ```
 * USEMODULE += nimble_autoadv
 * ```
 * to your makefile.
 *
 * The advertised device name can then optionally be configured with
 * ```
 * CFLAGS += -DCONFIG_NIMBLE_AUTOADV_DEVICE_NAME='"Riot OS device"'
 * ```
 * Otherwise the device will appear as "*RIOT OS device*".
 *
 * ## Instructions to connect to the bluetooth shell via ble-serial
 *
 * ### Configure and compile shell app
 * This example is for the `nrf52840dongle` target in `tests/sys/shell`, other
 * targets that are supported by NimBLE can be used similarly.
 *
 * Add following to Makefile:
 * ```
 * BOARD = nrf52840dongle
 * USEMODULE += nimble_autoadv
 * USEMODULE += stdio_nimble
 * ```
 *
 * **NOTE:** You can also have a look at `tests/sys/shell_ble`, which is already
 * set up for NimBLE.
 *
 * ### Flash
 *
 * ```
 * $ make -C tests/sys/shell -j clean all flash
 * ```
 *
 * ### Install the ble-serial tool
 *
 * ```
 * $ pip install ble-serial
 * ```
 *
 * ### Scan for your device
 * To connect to the device, you have to find out the BLE address. For this
 * example, the device name is `Riot OS device`. For the `tests/sys/shell_ble`
 * test, the device name is `tests/sys/shell_ble`.
 * When you get an `ble-scan: command not found` error you can also run
 * `python -m ble_serial.scan` instead.
 *
 * **NOTE:** Windows and Linux use MAC addresses instead of the UUIDs.
 *
 * ```
 * $ ble-scan
 * Started BLE scan
 *
 * 6BE8174C-A0F8-4479-AFA6-9828372CAFE9 (RSSI=-40): Riot OS device
 * A2862DCB-D382-4C0B-95BF-FA9A961F8D88 (RSSI=-48): Unknown
 * F2C75C08-7DD7-4F43-BEF0-151C92068FE5 (RSSI=-66): Unknown
 * 69400683-FBE5-4B45-8CFE-98594076E5F4 (RSSI=-89): Unknown
 * ```
 *
 * ### Discover characteristics (check the one advertised by the gatt server stdin/stdout)
 * ```
 * $ ble-scan -d 6BE8174C-A0F8-4479-AFA6-9828372CAFE9
 * Started deep scan of 6BE8174C-A0F8-4479-AFA6-9828372CAFE9
 *
 * SERVICE e6d54866-0292-4779-b8f8-c52bbec91e71 (Handle: 10): Unknown
 *      CHARACTERISTIC 35f28386-3070-4f3b-ba38-27507e991762 (Handle: 11): Unknown ['indicate']
 *          DESCRIPTOR 00002902-0000-1000-8000-00805f9b34fb (Handle: 13): Client Characteristic
 * Configuration
 *      CHARACTERISTIC ccdd113f-40d5-4d68-86ac-a728dd82f4aa (Handle: 14): Unknown ['write']
 *
 * Completed deep scan of 6BE8174C-A0F8-4479-AFA6-9828372CAFE9
 * ```
 *
 * ### Create a virtual port and mount it on /tmp/dev_riot_ble
 *
 * **NOTE:** for older versions (`<=2.8.0`) of `ble-serial`, the command line option
 * `--write-with-response` is unsupported and has to be removed!
 *
 * ```
 * $ ble-serial -d 6BE8174C-A0F8-4479-AFA6-9828372CAFE9 -p /tmp/dev_riot_ble
 * --write-uuid ccdd113f-40d5-4d68-86ac-a728dd82f4aa
 * --read-uuid 35f28386-3070-4f3b-ba38-27507e991762 --write-with-response
 *
 * 17:44:18.765 | INFO | linux_pty.py: Slave created on /tmp/dev_riot_ble -> /dev/ttys006
 * 17:44:18.766 | INFO | ble_interface.py: Receiver set up
 * 17:44:18.766 | INFO | ble_interface.py: Trying to connect with 6BE8174C-A0F8-4479-AFA6-
 * 9828372CAFE9
 * 17:44:19.861 | INFO | ble_interface.py: Device 6BE8174C-A0F8-4479-AFA6-9828372CAFE9 connected
 * 17:44:19.862 | INFO | ble_interface.py: Found write characteristic ccdd113f-40d5-4d68-86ac-
 * a728dd82f4aa (H. 14)
 * 17:44:19.862 | INFO | ble_interface.py: Found notify characteristic 35f28386-3070-4f3b-ba38-
 * 27507e991762 (H. 11)
 * 17:44:19.883 | INFO | main.py: Running main loop!
 * ```
 *
 * ### Open the virtual com port (the port name is from the logs in previous steps)
 * ```
 * $ picocom -q -b 115200 --imap lfcrlf /tmp/dev_riot_ble
 * > ps
 *     pid | state    Q | pri
 *       1 | running  Q |   7
 *       2 | bl anyfl _ |   5
 *       3 | bl anyfl _ |   0
 * >
 * ```
 *
 * @{
 * @file
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#ifndef STDIO_NIMBLE_H
#define STDIO_NIMBLE_H

#include "stdio_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Whether to clear the buffers when establishing a new connection or
 *        not. Defaults to true.
 */
#ifndef CONFIG_STDIO_NIMBLE_CLEAR_BUFFER_ON_CONNECT
#define CONFIG_STDIO_NIMBLE_CLEAR_BUFFER_ON_CONNECT 1
#endif

/**
 * @brief Size of buffer for stdin in bytes
 *
 * @note Must be a power of two!
 */
#ifndef CONFIG_STDIO_NIMBLE_STDIN_BUFSIZE
#define CONFIG_STDIO_NIMBLE_STDIN_BUFSIZE 1024
#endif

/**
 * @brief Size of buffer for stdout in bytes
 *
 * @note Must be a power of two!
 */
#ifndef CONFIG_STDIO_NIMBLE_STDOUT_BUFSIZE
#define CONFIG_STDIO_NIMBLE_STDOUT_BUFSIZE 2048
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* STDIO_NIMBLE_H */
