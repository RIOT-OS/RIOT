Writing a Device Driver in RIOT                                 {#driver-guide}
===============================

This document describes the requirement, design objectives, and some
non-function details when writing device drivers in/for RIOT. The term device
driver in this context includes all 'CPU-external' devices connected to the CPU
typically via peripherals like SPI, I2C, UART, GPIO, and similar. CPU
peripherals itself are in RIOT not considered to be device drivers, but
peripheral or low-level drivers. Typical devices are network devices like
radios, Ethernet adapters, sensors, and actuators.

[TOC]

# General Design Objectives                   {#driver-guide-design-objectives}

Device drivers should be as easy to use as possible. This implies an
'initialize->ready' paradigm, meaning, that device drivers should be ready to use
and in operation right after they have been initialized. On top, devices should
work with physical values wherever possible. So e.g. sensors should return
already converted values in some physical unit instead of RAW data, so that
users can work directly with data from different devices directly without having
to care about device specific conversion.

However, please avoid the use of `float` or `double`. Instead, multiply to the
next SI (or appropriate) unit. E.g. if an ADC would return values like `1.23 V`,
chose to return `1230 mV` instead.

Additionally towards ease of use, all device drivers in RIOT should provide a
similar 'look and feel'. They should behave similar concerning things like their
state after initialization, like their used data representation and so on.

Secondly, all device drivers should be optimized for minimal RAM/ROM usage, as
RIOT targets (very) constrained devices. This implies, that instead of exposing
all thinkable functionality, the drivers should focus on exporting and
implementing a device's core functionality, thus covering ~95% of the use cases.
Furthermore great care should be put into ...(?)

Third, it should always be possible, to handle more than a single device of one
kind. Drivers and their interfaces are thus designed to keep their state
information in a parameterized location instead of driver defined global
variables.

Fourth, RIOT defines high-level interfaces for certain groups of devices (i.e.
netdev for network devices, SAUL for sensors and actuators), which enable users
to work with a wide variety of devices without having to know anything about the
actual device that is mapped.

Fifth, during initialization we make sure that we can communicate with a device.
Other functions should check the dev pointer is not void, and should also handle
error return values from the lower layer peripheral driver implementations,
where there are some.

Sixth, device drivers SHOULD be implemented independent of any CPU/board code.
To achieve this, the driver implementations should strictly be based on
platform independent interfaces as the peripheral drivers, xtimer, etc.

# General                                               {#driver-guide-general}

## Documentation                                            {#driver-guide-doc}

Document what your driver does! Most devices come with a very large number of
features, while the corresponding device driver only supports a sub-set of them.
This should be clearly stated in the device driver's documentation, so that
anyone wanting to use the driver can find out the supported features without
having to scan through the code.

## Device descriptor and parameter configuration          {#driver-guide-types}

Each device MUST supply a data structure, holding the devices state and
configuration, using the naming scheme of `DEVNAME_t` (e.g. `dht_t`, or
`at86rf2xx_t`). In the context of RIOT, we call this data structure the device
descriptor.

This device descriptor MUST contain all the state data of a device. By this, we
are not limited on the number of instances of the driver we can run
concurrently. The descriptor is hereby used for identifying the device we want
to interact with, and SHOULD always be the first parameter for all device driver
related functions.

Typical things found in these descriptors are e.g. used peripherals (e.g. SPI or
I2C bus used, interfacing GPIO pins), data buffers (e.g. RX/TX buffers where
needed), or state machine information.

On top of the device descriptor, each device driver MUST also define a data
structure holding the needed configuration data. The naming scheme for this type
is `DEVNAME_params_t`. In contrary to the device descriptor, this data structure
should only contain static information, that is needed for the device
initialization as it is preferably allocated in ROM.

A simple I2C temperature sensors's device descriptor could look like this:

@code{.c}
typedef struct {
    tmpabc_params_t p;  /**< device configuration parameter like I2C bus and bus addr */
    int scale;          /**< some custom scaling factor for converting the results */
} tmpabc_t;

/* with params being */
typedef struct {
    i2c_t bus;          /**< I2C bus the device is connected to */
    uint8_t addr;       /**< the device's address on the bus */
} tmpabc_params_t;
@endcode

**NOTE:** In many cases it makes sense, to copy the `xxx_params` data into the
device descriptor during initialization. In some cases, it is however better to
just link the `params` data via pointer and only copy selected data. This way,
configuration data that is only used once can be read directly from ROM, while
often used fields (e.g. used peripherals) are stored directly in the device
descriptor and one saves hereby one de-referencing step when accessing them.

## Default device configuration                          {#driver-guide-config}

Each device driver in RIOT MUST supply a default configuration file, named
`DEVNAME_params.h`. This file should be located in the `RIOT/drivers/...`. The
idea is, that this file can be overridden by an application or a board, by
simply putting a file with the same name in the application's or the board's
include folders, while RIOT's build system takes care of preferring those files
instead of the default params file.

A default parameter header file for the example temperature sensor above would
look like this (`tmpabc_params.h`):

@code{.c}
/* ... */

#include "board.h"  /* THIS INCLUDE IS MANDATORY */
#include "tmpabc.h"

/* ... */

/**
 * @brief   Default configuration parameters for TMPABC sensors
 * @{
 */
#ifndef TMPABC_PARAM_I2C
#define TMPABC_PARAM_I2C        (I2C_DEV(0))
#endif
#ifndef TMPABC_PARAM_ADDR
#define TMPABC_PARAM_ADDR       (0xab)
#endif

#ifndef TMPABC_PARAMS
#define TMPABC_PARAMS           { .i2c  = TMPABC_PARAM_I2C \
                                  .addr = TMPABC_PARAM_ADDR }
#endif
/** @} */

/**
 * @brief   Allocation of TMPABC configuration
 */
static const tmpabc_params_t tmpabc_params[] = {
    TMPABC_PARAMS
}
/* ... */
@endcode

Now to influence the default configuration parameters, we have these options:

First, we can override one or more of the parameter from the makesystem, e.g.

@code{.sh}
CFLAGS="-DTMPABC_PARAM_ADDR=0x23" make all
@endcode

Second, we can override selected parameters from the board configuration
(`board.h`):

@code.{c}
/* ... */
/**
 * @brief   TMPABC sensor configuration
 * @{
 */
#define TMPABC_PARAM_I2C        (I2C_DEV(1))
#define TMPABC_PARAM_ADDR       (0x17)
/** @} */
/* ... */
@endcode

Third, we can define more than a single device in the board configuration
(`board.h`):

@code{.c}
/* ... */
/**
 * @brief   Configure the on-board temperature sensors
 * @{
 */
#define TMPABC_PARAMS           { \
                                  .i2c  = I2C_DEV(1), \
                                  .addr = 0x31 \
                                }, \
                                { \
                                    .i2c  = I2C_DEV(1), \
                                    .addr = 0x21 \
                                }
/** @} */
/* ... */
@endcode

And finally, we can simply override the `tmpabc_params.h` file as described
above.

## Initialization                                {#driver-guide-initialization}

In general, the initialization functions should to the following:

- initialize the device descriptor
- initialize non-shared peripherals they use, e.g. GPIO pins
- test for device connectivity, e.g. does a SPI/I2C slave react
- reset the device to a well defined state, e.g. use external reset lines or do
  a software rest
- do the actual device initialization

For testing a device's connectivity, it is recommended to read certain
configuration data with a defined value from the device. Some devices offer
`WHO_AM_I` or `DEVICE_ID` registers for this purpose. Writing and reading back
some data to the device is another valid option for testing it's responsiveness.

For more detailed information on how the signature of the init functions should
look like, please refer below to the specific requirements for network devices
and sensors.

## Return values                                  {#driver-guide-return-values}

As stated above, we check communication of a device during initialization, and
handle error return values from the lower layers, where they exist. To prevent
subsequent misuse by passing NULL pointers and similar to the subsequent
functions, the recommended way is to check parameter using `assert`, e.g.:

@code{.c}
int16_t tmpabc_read(const tmpabc_t *dev)
{
    assert(dev);
    /* ... */
    return value;
}
@endcode

Whenever status/error return values are implemented by you in your driver, they
should be named, meaning that the driver MUST define an enum assigning names to
the actual used value, e.g.

@code{.c}
enum {
    TMPABC_OK    = 0,       /**< all went as expected */
    TMPABC_NOI2C = -1,      /**< error using the I2C bus */
    TMPABC_NODEV = -2       /**< no device with the configured address found on the bus */
};
@endcode

## General Device Driver Checklist            {#driver-guide-general-checklist}

- *MUST*: the supported feature set and any custom behavior is clearly
  documented
- *MUST*: device descriptor is defined: `devab_t`
- *MUST*: device parameter `struct` is defined: `devab_params_t`
- *MUST*: a default parameter configuration file is present, e.g.
  `RIOT/drivers/devab/include/devab_params.h`
- *MUST*: all error and status return codes are named, e.g.
  `DEVAB_OK, DEVAB_NOSPI, DEVAB_OVERFLOW, ...`
- *MUST*: use `const devab_t *dev` when the device descriptor can be access
  read-only

# Sensors                                               {#driver-guide-sensors}

## SAUL                                                    {#driver-guide-saul}

All sensor drivers SHOULD implement the SAUL interface. It is however
recommended, that the drivers are written in a way, that the drivers do not
solely export the SAUL interface, but map the SAUL interface upon a driver
specific one.

For example, a temperature driver provides the following function (`tmpabc.c`):

@code{.c}
int16_t tmpabc_read(tmpabc_t *dev);
@endcode

which then can easily be mapped to SAUL (`tmpabc_saul.c`):

@code{.c}
int saul_read(saul_t *dev, phydat_t *data)
{
    memset(data, 0, sizeof(phydat_t));
    data->x = tmpabc_read((tmpabc_t *)dev);
    data->unit = UNIT_TEMP_C;
    data->scale = -2;
    return 1;
}
@endcode

This ensures the versatility of the device driver, having in mind that one might
want to use the driver without SAUL or maybe in a context without RIOT.

## Initialization                         {#driver-guide-sensor-initialization}

Sensor device drivers are expected to implement a single initialization
function, `DEVNAME_init`, taking the device descriptor and the device's
parameter struct as argument:

@code{.c}
int tmpabc_init(tmpabc_t *dev, const tmpabc_params_t *params);
@endcode

After this function is called, the device MUST be running and usable.

## Value handling                         {#driver-guide-sensor-value-handling}

### Value semantics                      {#driver-guide-sensor-value-semantics}

All sensors in RIOT MUST return their reading results as real physical values.
When working with sensor data, these are the values of interest, and the
overhead of the conversion is normally neglectable.

### Typing                                         {#driver-guide-sensor-types}

All values SHOULD be returned using integer types, with `int16_t` being the
preferred type where applicable.

In many situations, the physical values cannot be mapped directly to integer
values. For example, we do not want to map temperature values to integers
directly while using their fraction. The recommended way to solve this is by
scaling the result value using decimal fixed point arithmetic, in other words
just return centi-degree instead of degree (e.g. 2372c°C instead of 23.72°C).

## Additional Sensor Driver Checklist          {#driver-guide-sensor-checklist}

- *MUST*: mandatory device parameters are configurable through this file, e.g.
  sampling rate, resolution, sensitivity
- *MUST*: an init function in the style of
  `int devab_init(devab_t *dev, const devab_params_t *params);` exists
- *MUST*: after initialization, the device must be operational
- *MUST*: all error and return values are named, e.g.
  `DEVAB_OK, DEVAB_NODEV, ...`
- *MUST*: all 'read' functions return values in their physical representation,
  e.g. `centi-degree, Pa, lux, mili-G`
- *MUST*: all 'read' functions return integer values, preferably `int16_t`
- *SHOULD*: if multiple dimensions are read, they SHOULD be combined into a
  data structure
- *SHOULD*: the driver implements the SAUL interface
- *SHOULD*: the driver exports functions for putting it to sleep and waking up
  the device

# Network devices                                        {#driver-guide-netdev}

## Initialization                                   {#driver-guide-netdev-init}

The initialization process MUST be split into 2 steps: first initialize the
device descriptor and if applicable the used peripherals, and secondly do the
actual device initialization. The reason for this is, that before a device is
actually activated and can start to process data, the network stack for the
device needs to be initialized. By supplying a second init function, that does
the actual initialization, we can hand the control over when this is done to the
actual network stacks.

The initialization functions SHOULD follow this naming scheme:

@code{.c}
void netabc_setup(netabc_t *dev, const netabc_params_t *params);
int netabs_init(netabc_t *dev);
@endcode

## netdev                                      {#driver-guide-netdev-interface}

Device driver for network device SHOULD implement the `netdev` interface. It is
up to the implementer, if the device driver also offers a device specific
interface which is then mapped to the `netdev` interface, or if the device
driver can be purely interfaced using `netdev`. While the second option is
recommended for efficiency reasons, this is not mandatory.

## Additional Network Device Driver Checklist  {#driver-guide-netdev-checklist}

- *MUST*: a setup function in the style of
  `int devab_setup(devab_t *dev, const devab_params_t *params);` exists
- *MUST*: an init function in the style of `int devnet_init(devnet_t *dev)`
  exists
- *SHOULD*: the driver implements 'netdev' [if applicable]

# TODO                                                     {#driver-guide-todo}

Add some information about how to handle multiple threads, when to use mutexes,
and how to deal with interrupts?  And especially patterns for being nice from
other threads and power consumption point of view...
