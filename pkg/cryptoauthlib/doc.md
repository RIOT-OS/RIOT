@defgroup pkg_cryptoauthlib Microchip CryptoAuthentication Library
@ingroup  pkg drivers_misc
@brief    Provides the library for Microchip CryptoAuth devices
@see      https://github.com/MicrochipTech/cryptoauthlib

# Introduction

This package provides support for the official library for Microchip CryptoAuth devices.

@warning Some functions can only be used, when the data, config and otp zones
         of the device are locked. Locking is permanent and cannot be undone.
         Be careful if you're not sure you've configured everything correctly.
         For more information please refer to the data sheet of the device.

## Usage

Add
@code
USEPKG += cryptoauthlib
@endcode
to your Makefile.

### Shell

To facilitate the device configuration the RIOT shell provides some
helper commands. Currently there are shell commands to read the device's
config zone, to lock the config zone (this will lock the config zone permanently
and cannot be undone) and to check whether config and data zone are locked.
The shell handler is enabled, if cryptoauthlib is included as a package in the
Makefile of an application that also includes the shell (e.g. examples/basic/default).

### No poll mode

After sending a command to the device, responses are usually polled to enable
quicker response times.
Alternatively ATCA_NO_POLL can be enabled through CFLAGS to simply
wait for the max execution time of a command before reading the response.

## Implementation status

This implementation was partly tested with ATECC508A and ATECC608A devices.
Currently the functions `hal_i2c_release`, `hal_i2c_discover_devices` and
`hal_i2c_discover_buses` are unimplemented, as well as `hal_i2c_post_init`.

## Tests

The library provides unittests for the library functions. There
is a directory called "pkg_cryptoauthlib_internal_tests" in the RIOT testfolder
which runs part of the unittests.
Some of the provided tests can only be run when the config, data and/or otp zones
of the device are locked. Some tests (but not all) will automatically lock zones
as needed. We omit those tests at the moment, because zones can only be locked
permanently. Unlocking is not possible!
Also there is a test for comparing the runtime of the RIOT software implementation
and the CryptoAuth hardware implementation for calculating a SHA-256 hash value.

## Using Multiple ATECCX08A Devices {#multi-ateccx08a}

When using more than one device, you can either connect devices with different
I2C addresses to one bus or devices with the same address to separate buses.
The ATECCX08A devices provide a way to change the I2C address during device
configuration (for more details, read the datasheet or the API documentation).

ATECCX08A device parameters are configured in
`RIOT/pkg/cryptoauthlib/include/atca_params.h`.
There you can specify your device's address, the I2C bus to use and more by
defining `ATCA_PARAMS`. Per default one device is defined in RIOT (example shown below).

@code
#define ATCA_PARAM_I2C       (I2C_DEV(0))
#define ATCA_PARAM_ADDR      (ATCA_I2C_ADDR)
#define ATCA_RX_RETRIES      (20)
#define ATCA_DEVTYPE         (ATECC608A)

#define ATCA_PARAMS                  {   .iface_type = ATCA_I2C_IFACE, \
                                         .devtype = ATCA_DEVTYPE, \
                                         .atcai2c.address = ATCA_PARAM_ADDR, \
                                         .atcai2c.bus = ATCA_PARAM_I2C, \
                                         .atcai2c.baud = -1, \
                                         .wake_delay = 1500, \
                                         .rx_retries = ATCA_RX_RETRIES }
@endcode

If you want to use more than one device, the best way is to create a file called
`custom_atca_params.h` in your application folder (you can see an example of this in
`examples/advanced/psa_crypto`).

In your custom file you can now add a second device to `ATCA_PARAMS`:
@code
#define ATCA_PARAM_I2C_DEV0       (I2C_DEV(0))
#define ATCA_PARAM_ADDR_DEV0      (ATCA_I2C_ADDR_DEV0)
#define ATCA_RX_RETRIES_DEV0      (20)
#define ATCA_DEVTYPE_DEV0         (ATECC608A)

#define ATCA_PARAM_I2C_DEV1       (I2C_DEV(0))
#define ATCA_PARAM_ADDR_DEV1      (ATCA_I2C_ADDR_DEV1)
#define ATCA_RX_RETRIES_DEV1      (20)
#define ATCA_DEVTYPE_DEV1         (ATECC608A)

#define ATCA_PARAMS                  {   .iface_type = ATCA_I2C_IFACE, \
                                         .devtype = ATCA_DEVTYPE_DEV0, \
                                         .atcai2c.address = ATCA_PARAM_ADDR_DEV0, \
                                         .atcai2c.bus = ATCA_PARAM_I2C_DEV0, \
                                         .atcai2c.baud = -1, \
                                         .wake_delay = 1500, \
                                         .rx_retries = ATCA_RX_RETRIES }, \
                                     {   .iface_type = ATCA_I2C_IFACE, \
                                         .devtype = ATCA_DEVTYPE_DEV1, \
                                         .atcai2c.address = ATCA_PARAM_ADDR_DEV1, \
                                         .atcai2c.bus = ATCA_PARAM_I2C_DEV1, \
                                         .atcai2c.baud = -1, \
                                         .wake_delay = 1500, \
                                         .rx_retries = ATCA_RX_RETRIES }
@endcode

Now you just need to add the following to your Makefile:
@code
CFLAGS += -DCUSTOM_ATCA_PARAMS
INCLUDES += -I$(APPDIR)
@endcode

This way your custom params file is included in the build process and both your
devices will be initialized by the `auto_init` module.

To use them you need to utilize the `calib`-API of the cryptoauth driver, which
allows you to pass a device handle. Pointers to all initialized devices are stored
in the `atca_devs_ptr` array, which is included in `atca_params.h`.
Include `atca_params.h` in your source file and pass the device handle as shown below.

@code {.c}
ATCADevice dev = atca_devs_ptr[0];
calib_sha_start(dev);
@endcode

## Using Cryptoauthlib as a backend for PSA Crypto {#psa-cryptoauthlib}

To use cryptoauthlib as a backend for [PSA Crypto](#sys_psa_crypto), it is best to
overwrite the `atca_params.h` file with your own `custom_atca_params.h` file, similar
to the one described in [Using Multiple ATECCX08A Devices](#multi-ateccx08a).

When using PSA, the `ATCA_PARAMS` define contains an additional value: A location
value of the type @ref psa_key_location_t. Each secure element you use needs its own
location value. The primary device can get the value
@ref PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT. All others must be within the range of
@ref PSA_KEY_LOCATION_SE_MIN and @ref PSA_KEY_LOCATION_SE_MAX.

Your structure should now look like this:
@code
#define PSA_ATCA_LOCATION  (PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT)
#define ATCA_PARAM_I2C     (I2C_DEV(0))
#define ATCA_PARAM_ADDR    (ATCA_I2C_ADDR)
#define ATCA_DEVTYPE       (ATECC608A)
#define ATCA_RX_RETRIES    (20)

#define ATCA_PARAMS                     {   .atca_loc = PSA_ATCA_LOCATION,\
                                            .cfg = {\
                                                .iface_type = ATCA_I2C_IFACE, \
                                                .devtype = ATCA_DEVTYPE, \
                                                .atcai2c.address = ATCA_PARAM_ADDR, \
                                                .atcai2c.bus = ATCA_PARAM_I2C, \
                                                .atcai2c.baud = -1, \
                                                .wake_delay = 1500, \
                                                .rx_retries = ATCA_RX_RETRIES } \
                                        }
@endcode

When using multiple SEs, just add more device parameters as shown in section
[Using Multiple ATECCX08A Devices](#multi-ateccx08a).

## Slot Configurations
The ATECCX08A devices have their own key management, which can be configured by
setting flags in the device's configuration and data zone. There is a large variety
of possible configurations, which can not entirely be represented by PSA Crypto.

For now we assume that users are familiar with their device's datasheet and have configured
it in a way that it is usable. PSA can not yet work with keys that are already stored on the
device, which means all key slots must be writable after locking the configuration and data zone.

For ECC operations this means that key slot configurations need to allow at least the
use of the `gen_key` command, for AES and HMAC key slots it means that clear write operations
must be allowed.

@warning     In case of AES and HMAC keys this may lead to security issues,
             as it allows for manipulating key material.

Users need to make their configurations known to PSA Crypto.
For this you need to initialize a list of key slot configuration structures, with a structure
for each slot.

For these devices the structure looks like this:
@code {.c}
typedef struct {
    psa_key_type_t key_type_allowed; // Declare the key type allowed in this slot
    uint8_t key_persistent;          // Ignore for now, PSA does not yet support persistent keys
    uint8_t slot_occupied;           // Set to 0, PSA will set this to one after writing a key
} psa_atca_slot_config_t;
@endcode

To make your configurations known to PSA, simply add the following to your `custom_atca_params.h`
file (these values are only an example, of course you need to modify them according to
your needs).

@code {.c}
#define ATCA_SLOTS_DEV0  {   { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                             { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                             { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                             { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                             { PSA_KEY_TYPE_AES, 0, 0 }, \
                             { PSA_KEY_TYPE_HMAC, 0, 0 }, \
                             { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                             { 0, 1, 1 }, \
                             { 0, 0, 0 }, \
                             { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                             { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                             { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                             { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                             { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                             { 0, 0, 0 }, \
                             { 0, 0, 0 }}

#define ATCA_CONFIG_LIST     { ATCA_SLOTS_DEV0 }
@endcode

To use multiple devices, define `ATCA_SLOTS_DEV0` - `ATCA_SLOTS_DEVX` and add them to
`ATCA_CONFIG_LIST` like so:

@code {.c}
#define ATCA_CONFIG_LIST     { ATCA_SLOTS_DEV0 }, \
                             { ATCA_SLOTS_DEV1 }, \
                                     ...          \
                             { ATCA_SLOTS_DEVX }
@endcode

A usage example for this can be found in `examples/advanced/psa_crypto`.

## Troubleshooting

### Device Initialization fails

Make sure the I2C bus speed is set to `I2C_SPEED_NORMAL`.
