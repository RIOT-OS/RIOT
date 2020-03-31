/**
 * @defgroup pkg_cryptoauthlib Microchip CryptoAuthentication Library
 * @ingroup  pkg drivers_misc sys_crypto
 * @brief    Provides the library for Microchip CryptoAuth devices
 * @see      https://github.com/MicrochipTech/cryptoauthlib
 *
 * # Introduction
 *
 * This package provides support for the official library for Microchip CryptoAuth devices.
 *
 *
 * ## Warning
 *
 * Some functions can only be used, when the data, config and otp zones of the
 * device are locked. Locking is permanent and cannot be undone. Be careful if
 * you're not sure you've configured everything correctly.
 * For more information we refer to the data sheet of the device.
 *
 *
 * ## Usage
 *
 * Add
 * USEPKG += cryptoauthlib
 * to your Makefile.
 *
 *
 * ### Shell
 *
 * To facilitate the device configuration the RIOT shell provides some
 * helper commands. Currently there are shell commands to read the device's
 * config zone, to lock the config zone (this will lock the config zone permanently
 * and cannot be undone) and to check whether config and data zone are locked.
 * The shell handler is enabled, if cryptoauthlib is included as a package in the
 * Makefile of an application that also includes the shell (e.g. examples/default).
 *
 *
 * ### No poll mode
 *
 * After sending a command to the device, responses are usually polled to enable
 * quicker response times.
 * Alternatively ATCA_NO_POLL can be enabled through CFLAGS to simply
 * wait for the max execution time of a command before reading the response.
 *
 *
 * ## Implementation status
 *
 * This implementation was partly tested with ATECC508A and ATECC608A devices.
 * We haven't tested the functions that require locking the device, yet. There's
 * a wrapper in the cryptoauthlib/contrib folder, which implements most of the
 * HAL functions.
 * Currently the functions hal_i2c_discover_devices and hal_i2c_discover_buses
 * are unimplemented, as well as hal_i2c_post_init.
 *
 * ### Wake function
 *
 * The wake function only works when a 0x00 byte is sent on an i2c interface that
 * runs with with 133kHz or less.
 * Currently RIOT sets the baudrate to the default value of 100 kHz and there's
 * no interface to change that. If the default speed ever changes to a value
 * higher than 133 kHz the wake function needs to be adapted.
 * For more information on how to send a proper wake condition we refer to the
 * data sheet of the device.
 *
 * ## Tests
 *
 * The library provides unittests for the library functions. There
 * is a directory called "pkg_cryptoauthlib_internal_tests" in the RIOT testfolder
 * which runs part of the unittests.
 * Some of the provided tests can only be run when the config, data and/or otp zones
 * of the device are locked. Some tests (but not all) will automatically lock zones
 * as needed. We omit those tests at the moment, because zones can only be locked
 * permanently. Unlocking is not possible!
 * Also there is a test for comparing the runtime of the RIOT software implementation
 * and the CryptoAuth hardware implementation for calculating a SHA-256 hash value.
 */
