/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef L3GXXXX_H
#define L3GXXXX_H

/**
 * @defgroup    drivers_l3gxxxx L3Gxxxx 3-axis gyroscope sensor family
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device Driver for ST L3Gxxxx 3-axis gyroscope sensor family
 *
 * \section l3gxxxx Driver for ST L3Gxxxx 3-axis gyroscope sensor family
 *
 * ## Table of contents {#l3gxxxx_toc}
 *
 * 1. [Overview](#l3gxxxx_overview)
 *     1. [About the sensor](#l3gxxxx_about)
 *     2. [Supported features](#l3gxxxx_supported)
 * 2. [Measurement Process](#l3gxxxx_measurement_process)
 *     1. [Sensor modes](#l3gxxxx_sensor_modes)
 *     2. [Output Data Rates and Filters](#l3gxxxx_odr_filters)
 * 3. [Using the driver (basic functionality)](#l3gxxxx_using_driver)
 *     1. [Initialization](#l3gxxxx_initialization)
 *     2. [Output data format](#l3gxxxx_output_data)
 *     3. [Fetching data](#l3gxxxx_fetching_data)
 * 4. [Using the FIFO](#l3gxxxx_fifo)
 *     1. [Configuration of the FIFO](#l3gxxxx_fifo_config)
 *     2. [Reading data from the FIFO](#l3gxxxx_fifo_read_data)
 * 5. [Using Interrupts](#l3gxxxx_interrupts)
 *     1. [Data interrupts (data ready and FIFO status) on signal `INT2/DRDY`]
 *        (#l3gxxxx_data_interrupt)
 *     2. [Event interrupts (Axes movement and wake-up) on signal `INT1`]
 *        (#l3gxxxx_event_interrupt)
 *     3. [Interrupt context problem](#l3gxxxx_interrupt_context)
 *     4. [Interrupt signal properties](#l3gxxxx_interrupt_signal)
 * 6. [Power Saving](#l3gxxxx_power_saving)
 * 7. [Low level functions](#l3gxxxx_low_level)
 * 8. [Default configuration](#l3gxxxx_default_configuration)
 *
 * # Overview {#l3gxxxx_overview}
 *
 * ## About the sensor {#l3gxxxx_about}
 *
 * ST L3Gxxxx sensors are low-power **3-axis angular rate sensors** connected
 * to **I2C** or **SPI** with a full scale of up to **2000 dps**. It supports
 * different measuring rates with a user selectable bandwidth.
 *
 * **Main features** of the sensor are:
 * - 3 selectable full scales of ±245, ±500, and ±2000 dps
 * - 7 measuring rates from 12.5 Hz to 800 Hz with 4 bandwidths
 * - 16 bit angular rate value data output
 * - 2 dedicated interrupt signals for data and event interrupts
 * - integrated high-pass filters with 3 modes and 10 different cutoff
 *   frequencies
 * - embedded temperature sensor with 8 bit data output
 * - embedded 32 levels of 16 bit data output FIFO
 * - I2C and SPI digital interface
 * - embedded power-down and sleep mode with fast power-on and wake-up
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * ## Supported Features {#l3gxxxx_supported}
 *
 * The driver supports the following sensors of the L3Gxxxx 3-axis gyro sensor
 * family. Used sensor variant has to be specified by using the respective
 * pseudomodule.
 *
 * <center>
 * | Sensor Variant   | Pseudomodule  | Vendor Status                   |
 * |:-----------------|:--------------|:--------------------------------|
 * | L3GD20H          | `l3gd20h`     | Not recommended for new designs |
 * | L3GD20           | `l3gd20`      | Obsolete                        |
 * | L3G4200D         | `l3g4200_ng`  | Obsolete                        |
 * | A3G4250D         | `a3g4250d`    | Active                          |
 * | I3G4250D         | `i3g4250d`    | Active                          |
 * </center><br>
 *
 * The driver is modular and supports different levels of functionality, which
 * can be enabled using pseudomodules according to the requirements of the
 * application. This ensures that the driver only uses as much ROM
 * and RAM as really needed.
 *
 * As basic functionality the driver supports
 * - a static configuration of the sensor by a default configuration parameter
 *   set of type #l3gxxxx_params_t as defined in the file l3gxxxx_params.h
 * - the polling of raw output data or angular rates in millidegrees per
 *   second (mdps)
 * - the power-down and power-up of the sensor
 * - the use of the I2C or SPI interface
 *
 * The following pseudomodules are used to enable additional functionalities:
 * <center>
 * | Pseudomodule        | Functionality                                           |
 * |:--------------------|:--------------------------------------------------------|
 * | `l3gxxxx_i2c`       | I2C interface enabled                                   |
 * | `l3gxxxx_spi`       | SPI interface enabled                                   |
 * | `l3gxxxx_low_odr`   | Low output data rates enabled (L3GD20H only)            |
 * | `l3gxxxx_fifo`      | 32 level FIFO enabled                                   |
 * | `l3gxxxx_irq_data`  | Data interrupt (`INT2/DRDY`) handling enabled           |
 * | `l3gxxxx_irq_event` | Event interrupt (`INT1`) handling enabled               |
 * | `l3gxxxx_sleep`     | Sleep and wake-up functions enabled                      |
 * | `l3gxxxx_config`    | Functions for changing configurations at runtime nabled |
 * </center><br>
 *
 * The following table shows the mapping of which modules have to be used
 * to enable which functions of the L3Gxxxx.
 *
 * <center>
 * | Feature                                                       | Module              |
 * |:------------------------------------------------------------- |:--------------------|
 * | 16 bit angular rate data output (raw and angular rate)        | `l3gxxxx`           |
 * | Full scales of ±245, ±500, and ±2000 dps                      | `l3gxxxx`           |
 * | Using high-pass filter (HPF) and low-pass filter (LPF1/LPF2)  | `l3gxxxx`           |
 * | Output data rates (ODR) from 100 Hz to 800 Hz                 | `l3gxxxx`           |
 * | Output data rates (ODR) from 12.5 Hz to 50 Hz (L3GD20H only)  | `l3gxxxx_low_odr`   |
 * | Polling data                                                  | `l3gxxxx`           |
 * | SAUL sensor interface                                         | `l3gxxxx`           |
 * | Power-down and power-up functionality                         | `l3gxxxx`           |
 * | Sleep and wake-up functionality                               | `l3gxxxx_sleep`     |
 * | 32 level FIFO handling                                        | `l3gxxxx_fifo`      |
 * | Data interrupt (`INT2/DRDY`) handling (data ready and FIFO)   | `l3gxxxx_irq_data`  |
 * | Event interrupt (`INT1`) handling (Axes movement and wake-up) | `l3gxxxx_irq_event` |
 * | Configuration of all sensor functions at runtime              | `l3gxxxx_config`    |
 * | I2C interface                                                 | `l3gxxxx_i2c`       |
 * | SPI interface (SPI mode 3)                                    | `l3gxxxx_spi`       |
 * </center><br>
 *
 * @note
 * - Multiple L3Gxxxx sensors of same type with both SPI and I2C interfaces
 *   can be used simultaneously. If neither the I2C nor the SPI interface
 *   are enabled by using the modules `l3gxxxx_i2c` or `l3gxxxx_spi`,
 *   the I2C interface is used by default.<br>
 *   <br>
 * - In default configuration, the sensor is configured with
 *   an output data rate (ODR) of 100 Hz with a LPF2 cutoff frequency
 *   of 25 Hz (#L3GXXXX_ODR_100_25) and a full scale of 245 dps
 *   (#L3GXXXX_SCALE_245_DPS). The data are filtered with HPF and LPF2,
 *   where the HPF is used in normal mode with reset (#L3GXXXX_HPF_NORMAL)
 *   and a cutoff frequency of 8 Hz (see #l3gxxxx_config_hpf).<br>
 *   <br>
 *   This configuration can either be changed by overriding default
 *   parameters #CONFIG_L3GXXXX_ODR, #CONFIG_L3GXXXX_SCALE,
 *   #CONFIG_L3GXXXX_FILTER_SEL, #CONFIG_L3GXXXX_HPF_MODE and
 *   #CONFIG_L3GXXXX_HPF_CUTOFF or by using functions #l3gxxxx_set_mode,
 *   #l3gxxxx_set_scale, #l3gxxxx_select_output_filter, #l3gxxxx_config_hpf
 *   if module `l3gxxxx_config` is enabled.<br>
 *   <br>
 * - If module 'l3gxxxx_fifo' is used, the FIFO is enabled in mode
 *   #L3GXXXX_FIFO with a watermark level (threshold) of 23, i.e. the
 *   interrupt #L3GXXXX_INT_FIFO_WATERMARK is triggered (if enabled) when
 *   the 24th sample is stored in the FIFO.<br>
 *   <br>
 *   This configuration can be changed either by overriding the default
 *   configuration parameters #CONFIG_L3GXXXX_FIFO_MODE and
 *   #CONFIG_L3GXXXX_FIFO_WATERMARK or by function #l3gxxxx_set_fifo_mode
 *   if module `l3gxxxx_config` is used.<br>
 *   <br>
 * - If the handling of data interrupts on signal `INT2/DRDY` is
 *   enabled by module `l3gxxxx_irq_data`, it depends on whether module
 *   `l3gxxxx_fifo` is used, which data interrupts are enabled
 *   by default. If `l3gxxxx_fifo` is used, #L3GXXXX_INT_FIFO_WATERMARK and
 *   #L3GXXXX_INT_FIFO_OVERRUN interrupts are enabled by default. Otherwise
 *   only #L3GXXXX_INT_DATA_READY is enabled by default.<br>
 *   <br>
 *   This configuration can be changed using function #l3gxxxx_enable_int.<br>
 *   <br>
 * - If the handling of event interrupts on signal `INT1` is enabled
 *   by module `l3gxxxx_irq_event`, the high event interrupt is enabled by
 *   default for all axes. This means that for each individual axis an
 *   interrupt is generated when the absolute value of its angular rate
 *   exceeds a threshold value of ~30 dps (high event).<br>
 *   <br>
 *   The axis data are filtered with HPF and LPF2. The interrupt signal
 *   `INT1` is triggered when the data for one axis becomes greater than
 *   the specified threshold (OR condition). The interrupt is latched
 *   by default.<br>
 *   <br>
 *   This configuration can be changed either by overriding the default
 *   configuration parameters #CONFIG_L3GXXXX_INT1_X_THRESH,
 *   #CONFIG_L3GXXXX_INT1_Y_THRESH, #CONFIG_L3GXXXX_INT1_Z_THRESH,
 *   #CONFIG_L3GXXXX_INT1_X_LT_THRESH, #CONFIG_L3GXXXX_INT1_X_GT_THRESH,
 *   #CONFIG_L3GXXXX_INT1_Y_LT_THRESH, #CONFIG_L3GXXXX_INT1_Y_GT_THRESH,
 *   #CONFIG_L3GXXXX_INT1_Z_LT_THRESH, #CONFIG_L3GXXXX_INT1_Z_GT_THRESH,
 *   #CONFIG_L3GXXXX_INT1_FILTER, #CONFIG_L3GXXXX_INT1_AND and
 *   #CONFIG_L3GXXXX_INT1_LATCH or at runtime using function
 *   #l3gxxxx_set_int_event_cfg function.<br>
 *   <br>
 *   The `INT1` signal is a HIGH active push/pull output.
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * # Measurement Process {#l3gxxxx_measurement_process}
 *
 * ## Sensor modes {#l3gxxxx_sensor_modes}
 *
 * L3Gxxxx sensors provide different operating modes.
 *
 * - **Power-down mode** is configured automatically after power up boot
 *   sequence. In this mode, all gyros are switched off. Therefore, it takes
 *   up to 100 ms to switch to another mode. The power consumption in this mode
 *   is about 1 uA.
 *
 * - **Normal mode** is the normal measurement mode. All gyros are switched on
 *   and at least one axis is enabled for measurements. Measurements are
 *   performed at a defined output data rate (**ODR**). The power consumption
 *   in this mode is about 5 mA.
 *
 * - **Sleep mode** is the normal mode when no axes era enabled for
 *   measurement. In this modes, all gyros are kept switched on. Therefore,
 *   it only takes 1/ODR to switch to normal mode if low pass filtering is
 *   disabled or 6/ODR if low pass filtering is enabled. The power consumption
 *   in this mode is about 2.5 mA.
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * ## Output Data Rates and Filters {#l3gxxxx_odr_filters}
 *
 * In normal mode, measurements are performed at a defined output rate (ODR)
 * with a user selectable bandwidth.
 *
 * ### Used filter selection
 *
 * L3Gxxxx sensors integrate a combination
 * of two low pass filters (LPF) and one high pass filter (HPF).
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                                  +--------------->   L3GXXXX_NO_FILTER
 *                                  |    +----- +
 *                     +------------+--->|      |--->   L3GXXXX_LPF2_ONLY
 *                     |                 | LPF2 |
 * +-----+   +------+  |   +-----+  +--->|      |--->   L3GXXXX_HPF_AND_LPF2
 * |     |   |      |  |   |     |  |    +------+
 * | ADC |-->| LPF1 |--+-->| HPF |--+--------------->   L3GXXXX_HPF_ONLY
 * |     |   |      |      |     |
 * +-----+   +------+      +-----+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * First, raw sensor data are always filtered by LPF1 with a cutoff frequency
 * that is fixed for the selected output data rate (ODR), see #l3gxxxx_odr_t.
 * Resulting data can then optionally be filtered by HPF and/or LPF2. Both
 * filters can be used or bypassed.
 *
 * The figure above shows possible **filter selections** and the driver symbols
 * defined by #l3gxxxx_filter_sel_t. These can be used to set the filter
 * combination separately for the output data and the data for event interrupt
 * generation.
 *
 * <center>
 * | Driver symbol         | High pass filter (HPF) used | Low pass filter 2 (LPF2) used |
 * |:----------------------|:---------------------------:|:------------------------------:
 * | #L3GXXXX_NO_FILTER    | -                           | -                             |
 * | #L3GXXXX_HPF_ONLY     | x                           | -                             |
 * | #L3GXXXX_LPF2_ONLY    | -                           | x                             |
 * | #L3GXXXX_HPF_AND_LPF2 | x                           | x                             |
 * </center><br>
 *
 * The default filter selection for the output data is #L3GXXXX_HPF_AND_LPF2
 * and is defined by the default configuration parameter
 * #CONFIG_L3GXXXX_FILTER_SEL. If the module `l3gxxxx_config` is used, it can
 * be changed at runtime using function #l3gxxxx_select_output_filter.
 *
 * The default filter selection for event interrupt generation is
 * #L3GXXXX_HPF_AND_LPF2 and is defined by default configuration parameter
 * #CONFIG_L3GXXXX_INT1_FILTER. It can be changed at runtime with function
 * #l3gxxxx_set_int_event_cfg.
 *
 * @note Since same filters are used for the output data as well as the
 * data used for event interrupt generation (selective axes movement / wake-up),
 * the configuration of the filters always affects both data. If the HPF is
 * enabled for filtering the output data, it is also active for filtering the
 * sensor data used for interrupt generation if the LPF2 is enabled for
 * interrupt generation. The other way around, the HPF is also active for
 * filtering the output data when it is enabled for interrupt generation and
 * when the LPF2 is enabled for the output data.
 *
 * ### High pass filter mode
 *
 * The high pass filter (HPF) can be used in different modes.
 *
 * <center>
 * | Driver symbol          | HPF mode                |
 * |:-----------------------|:------------------------|
 * | #L3GXXXX_HPF_NORMAL    | Normal mode             |
 * | #L3GXXXX_HPF_REFERENCE | Reference mode          |
 * | #L3GXXXX_HPF_AUTORESET | Auto-reset on interrupt |
 * </center><br>
 *
 * In normal mode, the HPF can be reset by reading the REFERENCE
 * register, which instantly deletes the DC component
 * of the angular rate. In reference mode, output data are the difference
 * of raw sensor data and the contents of the REFERENCE register. In autoreset
 * mode, HPF is automatically reset when a configured event interrupt occurs.
 *
 * The default HPF mode is #L3GXXXX_HPF_NORMAL and is defined by the
 * default configuration parameter #CONFIG_L3GXXXX_HPF_MODE. If module
 * `l3gxxxx_config` is used, it can be changed at runtime using
 * function #l3gxxxx_config_hpf.
 *
 * ### Output data rates and filter cutoff frequencies
 *
 * The cutoff frequencies of LPF1 and LPF2 are determined by used output
 * data rate #l3gxxxx_odr_t. The following **output data rates (ODR)**
 * and the LPF1/LPF2 cutoff frequencies are defined
 * (Reference: Application Note AN4506):
 *
 * <center>
 * | Mode    | ODR [Hz] | LPF1 cutoff [Hz] | LPF2 cutoff [Hz] | Driver symbol        |
 * |:------- |:--------:|:----------------:|:----------------:|:---------------------|
 * | Normal  | 100      | 32               | 12.5             | #L3GXXXX_ODR_100_12  |
 * | Normal  | 100      | 32               | 25               | #L3GXXXX_ODR_100_25  |
 * | Normal  | 200      | 63.3             | 12.5             | #L3GXXXX_ODR_200_12  |
 * | Normal  | 200      | 63.3             | 25               | #L3GXXXX_ODR_200_25  |
 * | Normal  | 200      | 63.3             | 50               | #L3GXXXX_ODR_200_50  |
 * | Normal  | 200      | 63.3             | 70               | #L3GXXXX_ODR_200_70  |
 * | Normal  | 400      | 128              | 20               | #L3GXXXX_ODR_400_20  |
 * | Normal  | 400      | 128              | 25               | #L3GXXXX_ODR_400_25  |
 * | Normal  | 400      | 128              | 50               | #L3GXXXX_ODR_400_50  |
 * | Normal  | 400      | 128              | 110              | #L3GXXXX_ODR_400_110 |
 * | Normal  | 800      | 211              | 30               | #L3GXXXX_ODR_800_30  |
 * | Normal  | 800      | 211              | 35               | #L3GXXXX_ODR_800_35  |
 * | Normal  | 800      | 211              | 30               | #L3GXXXX_ODR_800_30  |
 * | Normal  | 800      | 211              | 100              | #L3GXXXX_ODR_800_100 |
 * | Low ODR | 12.5     | 3.9              | -                | #L3GXXXX_ODR_12      |
 * | Low ODR | 25       | 7.8              | -                | #L3GXXXX_ODR_25      |
 * | Low ODR | 50       | 16               | 16.6             | #L3GXXXX_ODR_50      |
 * </center><br>
 *
 * @note Low ODRs are only available on L3GD20H and if module `l3gxxxx_low_odr`
 * is used.
 *
 * The default output data rate (ODR) is #L3GXXXX_ODR_100_12 and defined by
 * the default configuration parameter #CONFIG_L3GXXXX_ODR. If module
 * `l3gxxxx_config` is used, it can be changed at runtime using
 * function #l3gxxxx_set_mode, for example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_set_mode(&dev, L3GXXXX_ODR_400_20, true, true, false);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The **cutoff frequencies** of the HPF depend on the selected output data
 * rate (ODR) and are specified by an index from 0 to 9, as shown in the
 * following table. All frequencies are given in Hz.
 *
 * <center>
 * | ODR [Hz] | 12.5  | 25    | 50    | 100  | 200  | 400  | 800 |
 * |---------:|:-----:|:-----:|:-----:|:----:|:----:|:----:|:---:|
 * | 0        | 1     | 2     | 4     | 8    | 15   | 30   | 56  |
 * | 1        | 0.5   | 1     | 2     | 4    | 8    | 15   | 30  |
 * | 2        | 0.2   | 0.5   | 1     | 2    | 4    | 8    | 15  |
 * | 3        | 0.1   | 0.2   | 0.5   | 1    | 2    | 4    | 8   |
 * | 4        | 0.05  | 0.1   | 0.2   | 0.5  | 1    | 2    | 4   |
 * | 5        | 0.02  | 0.05  | 0.1   | 0.2  | 0.5  | 1    | 2   |
 * | 6        | 0.01  | 0.02  | 0.05  | 0.1  | 0.2  | 0.5  | 1   |
 * | 7        | 0.005 | 0.01  | 0.02  | 0.05 | 0.1  | 0.2  | 0.5 |
 * | 8        | 0.002 | 0.005 | 0.01  | 0.02 | 0.05 | 0.1  | 0.2 |
 * | 9        | 0.001 | 0.002 | 0.005 | 0.01 | 0.02 | 0.05 | 0.1 |
 * </center><br>
 *
 * The default cutoff frequency of HPF is 8 Hz (index 0) and set by the
 * default configuration parameter #CONFIG_L3GXXXX_HPF_CUTOFF. If module
 * `l3gxxxx_config` is used, it can be changed at runtime using function
 * #l3gxxxx_config_hpf,
 * for example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_config_hpf(&dev, L3GXXXX_HPF_NORMAL, 0);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * # Using the driver {#l3gxxxx_using_driver}
 *
 * ## Initialization {#l3gxxxx_initialization}
 *
 * The **easiest way to use the driver** is simply to initialize the sensor
 * with function #l3gxxxx_init using the default configuration parameter set
 * #l3gxxxx_params as defined in file l3gxxxx_params.h.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * static l3gxxxx_t dev;
 *
 * if (l3gxxxx_init(&dev, &l3gxxxx_params[0]) != L3DG20H_OK) {
 *     ... // error handling
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * After this initialization, the sensor is fully operational and data can
 * be fetched either by polling or interrupt driven.
 *
 * @note Function #l3gxxxx_init resets the sensor completely. All registers
 * are reset to default values and the embedded FIFO is cleared.
 *
 * The default configuration parameter set defines
 * - the communication interface,
 * - the output data rate (ODR) including LPF1/LPF2 cutoff frequencies,
 * - the filter combination and HPF cutoff frequency,
 * - the sensitivity level selected by full scale,
 * - the FIFO parameters if module `l3gxxxx_fifo` is used,
 * - the data interrupt (`INT2/DRDY`) pin if module `l3gxxxx_irq_data` is used, and
 * - the event interrupt (`INT1`) configuration if module `l3gxxxx_irq_event` is used.
 *
 * Most of these configuration parameters can also be changed at runtime
 * by respective functions if the module `l3gxxxx_config` is used or by
 * overriding default configuration parameters. Detailed information about
 * the default configuration can be found in section
 * [Configuration](#l3gxxxx_default_configuration).
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * ## Output data format {#l3gxxxx_output_data}
 *
 * In normal mode, the sensor determines periodically the angular rate for all
 * axes that are enabled for measurement and produces raw output data
 * with the selected output data rate (ODR).
 *
 * These **raw output data** are 16-bit signed integer values in two’s
 * complement representation. Their range and their resolution depend on the
 * sensitivity of the sensor which is selected by the **full scale** value.
 * L3Gxxxx sensors allow to select the following full scales:
 *
 * <center>
 * | Full Scale  | Sensitivity | Driver symbol           | Remark                    |
 * | -----------:|------------:|:------------------------|:--------------------------|
 * | ±245 dps    | 8.75 mdps   | #L3GXXXX_SCALE_245_DPS  |                           |
 * | ±500 dps    | 17.50 mdps  | #L3GXXXX_SCALE_500_DPS  | not available on A3G4250D |
 * | ±2000 dps   | 70.00 mdps  | #L3GXXXX_SCALE_2000_DPS | not available on A3G4250D |
 * </center><br>
 *
 * @note On the A34250D, only 245 dps (#L3GXXXX_SCALE_245_DPS) is available
 * as full scale value.
 *
 * The default full scale value is ±245 dps which is defined by the default
 * configuration parameter #CONFIG_L3GXXXX_SCALE. If module `l3gxxxx_config`
 * is used, it can be changed at runtime using function #l3gxxxx_set_scale,
 * for example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_set_scale(&dev, L3GXXXX_SCALE_500_DPS);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * ## Fetching data {#l3gxxxx_fetching_data}
 *
 * To get the information whether new data are available, the user task can
 * either use
 *
 * - the function #l3gxxxx_data_ready to check periodically whether new
 *   output data are available, or
 * - the data ready interrupt (`INT2/DRDY`) which is triggered as soon as new
 *   output data are available (see below).
 *
 * Last measurement results can then be fetched either
 *
 * - as 16 bit raw output data of type #l3gxxxx_raw_data_t using function
 *   #l3gxxxx_read_raw or
 * - as 32 bit integer angular rates type #l3gxxxx_data_t in
 *   millidegrees per second (mdps) using function #l3gxxxx_read.
 *
 * It is recommended to use function #l3gxxxx_read since the driver already
 * converts raw output data to angular rates according to the selected
 * full scale value.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * while (1)
 * {
 *     l3gxxxx_data_t data;
 *
 *     // execute task every 20 ms
 *     xtimer_usleep(20 * US_PER_MS);
 *     ...
 *     // test for new data and fetch them if available
 *     if ((l3gxxxx_data_ready(&dev) > 0) &&
 *         (l3gxxxx_read(&dev, &data) == L3GXXXX_OK)) {
 *         // do something with data
 *         ...
 *     }
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note
 * The functions #l3gxxxx_read and #l3gxxxx_read_raw always return the last
 * available results. If these functions are called more often than
 * measurements are performed, some measurement results are retrieved
 * multiple times. If these functions are called too rarely, some measurement
 * results will be lost.
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * # Using the FIFO {#l3gxxxx_fifo}
 *
 * In order to limit the rate at which the host processor has to fetch the
 * data, L3Gxxxx sensors embed a FIFO buffer. This is in particular helpful
 * at high output data rates. The FIFO buffer can work in seven different
 * modes and is able to store up to 32 data samples. Please refer the
 * [datasheet](http://www.st.com/resource/en/datasheet/l3gd20.pdf) or
 * [application note](http://www.st.com/resource/en/application_note/dm00119036.pdf)
 * for more details.
 *
 * <center>
 * | Driver symbol             | FIFO mode             | Remark                  |
 * |---------------------------|-----------------------|:------------------------|
 * | #L3GXXXX_BYPASS           | Bypass mode           | FIFO is not used        |
 * | #L3GXXXX_FIFO             | FIFO mode             |                         |
 * | #L3GXXXX_STREAM           | Stream mode           |                         |
 * | #L3GXXXX_STREAM_TO_FIFO   | Stream-to-FIFO mode   | L3GD20H and L3GD20 only |
 * | #L3GXXXX_BYPASS_TO_STREAM | Bypass-to-Stream mode | L3GD20H and L3GD20 only |
 * | #L3GXXXX_DYNAMIC_STREAM   | Dynamic Stream mode   | L3GD20H only            |
 * | #L3GXXXX_BYPASS_TO_FIFO   | Bypass to FIFO mode   | L3GD20H only            |
 * </center><br>
 *
 * A watermark level (threshold) can be set for the FIFO. If the number of
 * data samples in the FIFO exceeds this value, the watermark flag is set
 * and the interrupt #L3GXXXX_INT_FIFO_WATERMARK is triggered, if enabled.
 * This interrupt can be used to gather a minimum number of samples of raw
 * output data before the data are fetched as a single read operation from
 * the sensor.
 *
 * ## Configuration of the FIFO {#l3gxxxx_fifo_config}
 *
 * The default FIFO mode is defined by the default configuration parameter
 * #CONFIG_L3GXXXX_FIFO_MODE. The default watermark level (threshold) of
 * the FIFO is defined by the default configuration parameter
 * #CONFIG_L3GXXXX_FIFO_WATERMARK.
 *
 * If module `l3gxxxx_config` is used, both configuration parameters can be
 * changed at runtime with function #l3gxxxx_set_fifo_mode. This function
 * takes two parameters, the FIFO mode and the watermark level.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * ...
 * // set the FIFO mode with a watermark level (threshold) of 10, i.e. the
 * // watermark flag is set or the interrupt is triggered for the 11th sample
 * l3gxxxx_set_fifo_mode(&dev, L3GXXXX_STREAM, 10);
 * ...
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note To clear the FIFO at any time, set the FIFO mode to #L3GXXXX_BYPASS
 * and back to the desired FIFO mode.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_set_fifo_mode(&dev, L3GXXXX_BYPASS, 0);
 * l3gxxxx_set_fifo_mode(&dev, L3GXXXX_STREAM, 10);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ## Reading data from FIFO {#l3gxxxx_fifo_read_data}
 *
 * To read data from the FIFO, just use either
 *
 * - function #l3gxxxx_read_raw_fifo to get all raw output data stored
 *   in the FIFO or
 * - function #l3gxxxx_read_fifo to get all data stored in the FIFO
 *   and converted to angular rates in mdps (millidegrees per second).
 *
 * Both functions clear the FIFO and return the number of samples read
 * from the FIFO.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_data_fifo_t  data;
 *
 * while (1)
 * {
 *     // execute task every 500 ms
 *     xtimer_usleep(500 * US_PER_MS);
 *     ...
 *     // test for new data
 *     if (l3gxxxx_data_ready(&dev) > 0) {
 *
 *         // fetch data from fifo
 *         int num = l3gxxxx_read_fifo(dev, data);
 *
 *         for (int i = 0; i < num; i++) {
 *            // do something with data[i] ...
 *         }
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * # Using Interrupts {#l3gxxxx_interrupts}
 *
 * L3Gxxxx sensors allow to activate interrupts on two different signals:
 *
 * - for data (data ready and FIFO status) on signal `INT2/DRDY`, and
 * - for events (axis movement and wake-up) on signal `INT1`.
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * ## Data interrupts (data ready and FIFO status) on signal INT2/DRDY {#l3gxxxx_data_interrupt}
 *
 * Interrupts on signal `INT2/DRDY` can be generated by the following sources:
 *
 * <center>
 * | Interrupt source                         | Driver symbol               |
 * |:-----------------------------------------|:----------------------------|
 * | Output data are ready to be read         | #L3GXXXX_INT_DATA_READY     |
 * | FIFO content exceeds the watermark level | #L3GXXXX_INT_FIFO_WATERMARK |
 * | FIFO is completely filled                | #L3GXXXX_INT_FIFO_OVERRUN   |
 * | FIFO becomes empty                       | #L3GXXXX_INT_FIFO_EMPTY     |
 * </center><br>
 *
 * #L3GXXXX_INT_DATA is the bitwise OR combination of these symbols.
 *
 * @note Using data interrupts requires to enable module `l3gxxxx_irq_data`.
 *
 * Each interrupt source can be enabled or disabled separately with
 * function #l3gxxxx_enable_int.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_enable_int(&dev, L3GXXXX_INT_DATA_READY, true);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * If `l3gxxxx_fifo` is used, #L3GXXXX_INT_FIFO_WATERMARK and
 * #L3GXXXX_INT_FIFO_OVERRUN interrupts are enabled by default. Otherwise
 * only #L3GXXXX_INT_DATA_READY is enabled by default.
 *
 * The MCU GPIO pin used for the `INT2/DRDY` interrupt signal has to be defined
 * by the hardware configuration parameter #L3GXXXX_INT2_PIN.
 *
 * Once a data interrupt is enabled, function #l3gxxxx_wait_int can be
 * used to wait for an interrupt on signal `INT2/DRDY`. This function
 * returns a structure with the interrupt sources of type #l3gxxxx_int_src_t
 * which contains a flag for each possible data interrupt source in
 * member #l3gxxxx_int_src_t::data that can be tested for true.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_int_src_t int_src = l3gxxxx_wait_int(&dev);
 *
 * if (int_src.data.data_ready) {
 *     l3gxxxx_read(&dev, &data)
 *     ...
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * If module `l3gxxxx_fifo` is used, the corresponding interrupt sources can
 * be tested.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_int_src_t int_src = l3gxxxx_wait_int(&dev);
 *
 * ...
 * if (int_src.data.fifo_threshold) {
 *     l3gxxxx_read(&dev, &data);
 *     ...
 * }
 * if (int_src.data.fifo_overrun) {
 *     printf("FIFO overrun");
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * ## Event interrupts (axes movement and wake-up) on signal INT1 {#l3gxxxx_event_interrupt}
 *
 * This interrupt signal allows to recognize independent rotations of
 * the X, Y and Z axes. For this purpose, a separate threshold can be
 * defined for each axis. If activated, the angular rate of each axis
 * is compared with its threshold to check whether it is below or above
 * the threshold. The results of all activated comparisons are combined
 * OR or AND to generate the interrupt signal.
 *
 * The configuration of the thresholds, the activated comparisons and
 * selected AND/OR combination allows to recognize special situations
 * like selective axis movement (SA) or axes movement wake-up (WU).
 *
 * - **Selective axis movement recognition (SA)** means that only one
 *   axis is rotating. This is the case if the angular rate of selected
 *   axis is above its threshold AND angular rates of all other axes are
 *   below their thresholds.
 *
 * - **Axis movement wake-up (WU)** means that the angular rate of any
 *   axis is above its threshold (OR).
 *
 * @note Using event interrupts requires to enable module `l3gxxxx_irq_event`.
 *
 * The MCU GPIO pin used for the `INT1` interrupt signal is defined by the
 * hardware configuration parameter #L3GXXXX_INT1_PIN.
 *
 * The default configuration for event interrupts is defined by
 * #L3GXXXX_INT1_PARAMS. This configuration can be changed either by
 * overriding default configuration parameters
 * #CONFIG_L3GXXXX_INT1_X_THRESH, #CONFIG_L3GXXXX_INT1_Y_THRESH,
 * #CONFIG_L3GXXXX_INT1_Z_THRESH,
 * #CONFIG_L3GXXXX_INT1_X_LT_THRESH, #CONFIG_L3GXXXX_INT1_X_GT_THRESH,
 * #CONFIG_L3GXXXX_INT1_Y_LT_THRESH, #CONFIG_L3GXXXX_INT1_Y_GT_THRESH,
 * #CONFIG_L3GXXXX_INT1_Z_LT_THRESH, #CONFIG_L3GXXXX_INT1_Z_GT_THRESH,
 * #CONFIG_L3GXXXX_INT1_FILTER, #CONFIG_L3GXXXX_INT1_AND and
 * #CONFIG_L3GXXXX_INT1_LATCH or at runtime using function
 * #l3gxxxx_set_int_event_cfg with a set of parameters
 * of type #l3gxxxx_int_event_cfg_t that contains the configuration.
 * For example, selective axis movement recognition (SA) for the z-axis
 * could be configured as following. With this configuration, the event
 * interrupt is only triggered if all conditions are met.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_int_event_cfg_t int_cfg;
 *
 * // thresholds
 * int_cfg.x_threshold = 100;
 * int_cfg.y_threshold = 100;
 * int_cfg.z_threshold = 1000;
 *
 * // X axis below threshold enabled
 * int_cfg.x_low_enabled  = true;
 * int_cfg.x_high_enabled = false;
 *
 * // Y axis below threshold enabled
 * int_cfg.y_low_enabled  = true;
 * int_cfg.y_high_enabled = false;
 *
 * // Z axis below threshold enabled
 * int_cfg.z_low_enabled  = false;
 * int_cfg.z_high_enabled = true;
 *
 * // AND combination of all conditions
 * int_cfg.and_or = true;
 *
 * // further parameters
 * int_cfg.filter = L3GXXXX_HPF_ONLY;
 * int_cfg.latch = true;
 * int_cfg.duration = 0;
 * int_cfg.wait = false;
 *
 * // set the configuration and enable the interrupt
 * l3gxxxx_set_int_event_cfg(&dev, &int_cfg);
 * l3gxxxx_enable_int(&dev, L3GXXXX_INT_EVENT, true);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The data structure #l3gxxxx_int_event_cfg_t defines
 * - whether the interrupt signal should be latched until the interrupt
 *   source is read by function #l3gxxxx_wait_int,
 * - which filters are applied to the data used for interrupt generation,
 * - which time measured in 1/ODR an interrupt condition has to be given before
 *   the interrupt is generated, and
 * - whether this time is also used when interrupt condition is no
 *   longer given before interrupt signal is reset.
 *
 * @note
 * - If the interrupt is configured to be latched, the interrupt signal is
 *   active until the interrupt source is read by function #l3gxxxx_wait_int
 *   AND next raw output data are available. Otherwise the interrupt signal is
 *   active as long as the interrupt condition is satisfied.
 * - The driver function #l3gxxxx_wait_int uses the leading flank of the
 *   interrupt signal to detect an interrupt and read the interrupt source.
 *
 * Function #l3gxxxx_enable_int is used to enable or disable the
 * event interrupt generation (#L3GXXXX_INT_EVENT).
 *
 * As with data interrupts function #l3gxxxx_wait_int can be used to
 * wait for an interrupt on signal `INT1` if event interrupts are enabled.
 * This function returns a structure with the interrupt sources of type
 * #l3gxxxx_int_src_t which contains a flag for each possible event interrupt
 * source in member #l3gxxxx_int_src_t::event that can be tested for true.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_int_src_t int_src = l3gxxxx_wait_int(&dev);
 *
 * if (int_src.event.x_low) {
 *     printf("x below ");
 * }
 * if (int_src.event.x_high) {
 *     printf("x above ");
 * }
 * ...
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Activating all threshold comparisons and using the OR combination is the
 * most flexible way to realize functions like selective axis movement by
 * combining the different interrupt sources as required by the application.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_int_event_cfg_t int_cfg;
 *
 * // thresholds
 * int_cfg.x_threshold = 100;
 * int_cfg.y_threshold = 100;
 * int_cfg.z_threshold = 100;
 *
 * // X axis below and above threshold enabled
 * int_cfg.x_low_enabled  = true;
 * int_cfg.x_high_enabled = true;
 *
 * // Y axis below and above threshold enabled
 * int_cfg.y_low_enabled  = true;
 * int_cfg.y_high_enabled = true;
 *
 * // Z axis below and above threshold enabled
 * int_cfg.z_low_enabled  = true;
 * int_cfg.z_high_enabled = true;
 *
 * // OR combination of all conditions
 * int_cfg.and_or = false;
 * ...
 * // set the configuration and enable the interrupt
 * l3gxxxx_set_int_event_cfg(&dev, &int_cfg);
 * l3gxxxx_enable_int(&dev, l3gxxxx_int_event, true);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Following example shows the selective axis movement recognition (SA)
 * for the Z-axis.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * l3gxxxx_int_src_t int_src = l3gxxxx_wait_int(&dev);
 *
 * if (int_src.event.y_low && int_src.event.y_low && int_src.event.z_high) {
 *     // selective axis movement of Z-axis
 *     ...
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ## Interrupt context problem {#l3gxxxx_interrupt_context}
 *
 * All functions of the driver require direct access to the sensor via
 * I2C or SPI which does not work in interrupt context.
 *
 * Therefore, the driver prevents the direct use of the interrupts and
 * application specific ISRs. The only way to use interrupts is to call
 * the function #l3gxxxx_wait_int which enables the interrupt signals
 * for the configured MCU GPIO pins and then blocks the calling thread
 * until an interrupt is triggered.
 *
 * Once an interrupt is triggered, the driver handles the interrupt with
 * an internal ISR and then returns from the #l3gxxxx_wait_int function.
 * The return value is a structure with the interrupt sources of type
 * #l3gxxxx_int_src_t, which contains a flag for each possible interrupt
 * source that can be tested for true.
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * ## Interrupt signal properties {#l3gxxxx_interrupt_signal}
 *
 * By default, interrupt signals are high active push/pull outputs.
 *
 * # Power Saving {#l3gxxxx_power_saving}
 *
 * L3Gxxxx sensors offer two modes for power saving:
 *
 * - **Power-down** mode
 * - **Sleep** mode
 *
 * While in power-down mode almost all internal blocks of the device including
 * the gyros are switched off, in sleep mode only the measuring functions for
 * all three axes are deactivated. Therefore, the time to continue measurements
 * after sleep mode is drastically shorter than after power down.
 *
 * <center>
 * | Starting mode | Target mode | Turn-on time typical |
 * | ------------- | ----------- | ---------------------|
 * | Power-down    | Normal      | 100 ms               |
 * | Power-down    | Sleep       | 100 ms               |
 * | Sleep         | Normal      | 1/ODR when LPF2 disabled <br> 6/ODR when LPF2 enabled |
 * </center><br>
 *
 * L3Gxxxx sensors can be powered down when no measurements are required
 * using the function #l3gxxxx_power_down. The power consumption is reduced
 * to some uA in power-down mode. To restart the L3Gxxxx in previous
 * measurement mode, the #l3gxxxx_power_up function can be used.
 *
 * If module `l3gxxxx_sleep` is enabled, the sleep mode can be activated with
 * function #l3gxxxx_sleep. The power consumption is then reduced from 4.8 mA
 * to 2.4 mA and thus to half. The #l3gxxxx_wake_up function continues the
 * measurement in previous operation mode.
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * # Low level functions {#l3gxxxx_low_level}
 *
 * L3Gxxxx sensors are complex and flexible sensors with a lot of
 * features. It can be used for a big number of different use cases.
 * Since it is impossible to implement a high level interface
 * which is generic enough to cover all the functionality of the sensor
 * for all different use cases, there are two low level interface
 * functions that allow direct read and write access to the registers
 * of the sensor.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * bool l3gxxxx_reg_read(l3gxxxx_sensor_t* dev, uint8_t reg, uint8_t *data, uint16_t len);
 * bool l3gxxxx_reg_write(l3gxxxx_sensor_t* dev, uint8_t reg, uint8_t *data, uint16_t len);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @warning
 * These functions should only be used to do something special that
 * is not covered by the high level interface AND if you exactly
 * know what you do and what it might affect. Please be aware that
 * it might affect the high level interface.
 *
 * [Back to Table of Contents](#l3gxxxx_toc)
 *
 * # Default configuration {#l3gxxxx_default_configuration}
 *
 * Default sensor hardware configurations are set in file `l3gxxxx_params.h`
 * using the following defines:
 *
 * <center>
 * | Hardware configuration | Driver name       | Default Value       |
 * |:-----------------------|:------------------|:--------------------|
 * | I2C device             | #L3GXXXX_I2C_DEV  | I2C_DEV(0)          |
 * | I2C address            | #L3GXXXX_I2C_ADDR | #L3GXXXX_I2C_ADDR_2 |
 * | SPI device             | #L3GXXXX_SPI_DEV  | SPI_DEV(0)          |
 * | SPI clock frequency    | #L3GXXXX_SPI_CLK  | SPI_CLK_1MHZ        |
 * | SPI CS signal          | #L3GXXXX_SPI_CS   | GPIO_PIN(0, 0)      |
 * | `INT1` MCU pin         | #L3GXXXX_INT1_PIN | GPIO_PIN(0, 1)      |
 * | `INT2/DRDY` MCU pin    | #L3GXXXX_INT2_PIN | GPIO_PIN(0, 2)      |
 * </center><br>
 *
 * These hardware configurations can be overridden either by the board
 * definition or by defining them in the `CFLAGS` variable in the make
 * command, for example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * DRIVER=l3gd20h USEMODULE='l3gxxxx_irq_data` \
 * CLFAGS='-DL3GXXXX_INT2_PIN=GPIO_PIN\(0, 5\)' \
 * BOARD=... make -C tests/drivers/l3gxxxx
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The default configuration of the sensor is defined in file
 * `l3gxxxx_params.h` using the following defines:
 *
 * <center>
 * | Configuration parameter              | Driver name                   | Default Value         |
 * |:-------------------------------------|:------------------------------|:----------------------|
 * | ODR and LPF2 cutoff frequency        | #CONFIG_L3GXXXX_ODR           | #L3GXXXX_ODR_100_25   |
 * | Full scale                           | #CONFIG_L3GXXXX_SCALE         | #L3GXXXX_SCALE_245_DPS|
 * | Filter selection used for output data| #CONFIG_L3GXXXX_FILTER_SEL    | #L3GXXXX_HPF_AND_LPF2 |
 * | HPF mode used for output data        | #CONFIG_L3GXXXX_HPF_MODE      | #L3GXXXX_HPF_NORMAL   |
 * | HPF cutoff frequency 8 Hz            | #CONFIG_L3GXXXX_HPF_CUTOFF    | 0                     |
 * | FIFO mode if FIFO is used            | #CONFIG_L3GXXXX_FIFO_MODE     | #L3GXXXX_FIFO         |
 * | FIFO threshold value if FIFO is used | #CONFIG_L3GXXXX_FIFO_WATERMARK| 23                    |
 * | INT1 threshold for X axis (~30 dps)  | #CONFIG_L3GXXXX_INT1_X_THRESH | 4012                  |
 * | INT1 threshold for Y axis (~30 dps)  | #CONFIG_L3GXXXX_INT1_Y_THRESH | 4012                  |
 * | INT1 threshold for Z axis (~30 dps)  | #CONFIG_L3GXXXX_INT1_Z_THRESH | 4012                  |
 * | INT1 interrupt enable for X > X threshold | #CONFIG_L3GXXXX_INT1_X_GT_THRESH | true          |
 * | INT1 interrupt enable for Y > Y threshold | #CONFIG_L3GXXXX_INT1_Y_GT_THRESH | true          |
 * | INT1 interrupt enable for Z > Z threshold | #CONFIG_L3GXXXX_INT1_Z_GT_THRESH | true          |
 * | INT1 interrupt enable for X < X threshold | #CONFIG_L3GXXXX_INT1_X_LT_THRESH | false         |
 * | INT1 interrupt enable for Y < Y threshold | #CONFIG_L3GXXXX_INT1_Y_LT_THRESH | false         |
 * | INT1 interrupt enable for Z < Z threshold | #CONFIG_L3GXXXX_INT1_Z_LT_THRESH | false         |
 * | INT1 filter selection                | #CONFIG_L3GXXXX_INT1_FILTER   | #L3GXXXX_HPF_AND_LPF2 |
 * | INT1 interrupt combination           | #CONFIG_L3GXXXX_INT1_AND      | false                 |
 * | INT1 interrupt latch enabled         | #CONFIG_L3GXXXX_INT1_LATCH    | true                  |
 * </center><br>
 *
 * Single or all parameters of the default configuration can be overridden
 * either by defining them in the variable `CFLAGS` in the make command
 * line or by placing a modified file `l3gxxxx_params.h` in the
 * application directory, for example:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * DRIVER=l3gd20h USEMODULE='l3gxxxx_low_odr l3gxxxx_irq_data` \
 * CLFAGS='-DCONFIG_L3GXXXX_ODR=L3GXXXX_ODR_12 -DL3GXXXX_INT2_PIN=GPIO_PIN\(0,5\)' \
 * BOARD=... make -C tests/drivers/l3gxxxx
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @{
 * @file
 * @brief       Device Driver for ST L3Gxxxx 3-axis gyroscope sensor family
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "mutex.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/spi.h"

#include "l3gxxxx_regs.h"

#if !IS_USED(MODULE_L3GD20H) && !IS_USED(MODULE_L3GD20) \
                             && !IS_USED(MODULE_L3G4200D_NG) \
                             && !IS_USED(MODULE_A3G4250D) \
                             && !IS_USED(MODULE_I3G4250D)
#error Please select your sensor variant by using the respective pseudomodule.
#endif

/**
 * @name L3Gxxxx addresses
 * @{
 */
#if IS_USED(MODULE_L3GD20H) || IS_USED(MODULE_L3GD20)
#define L3GXXXX_I2C_ADDR_1      (0x6a)  /**< SDO pin is low */
#define L3GXXXX_I2C_ADDR_2      (0x6b)  /**< SDO pin is high */
#else
#define L3GXXXX_I2C_ADDR_1      (0x68)  /**< SDO pin is low */
#define L3GXXXX_I2C_ADDR_2      (0x69)  /**< SDO pin is high */
#endif
/** @} */

/**
 * @name L3Gxxxx chip ids
 * @{
 */
#define L3GXXXX_CHIP_ID_L3GD20H  (0xd7)  /**< Chip ID for L3GD20H */
#define L3GXXXX_CHIP_ID_L3GD20   (0xd4)  /**< Chip ID for L3GD20 */
#define L3GXXXX_CHIP_ID_X3G42XXD (0xd3)  /**< Chip ID for L3G4200D, I3G4250D, A3G4250D */
/** @} */

/** Definition of error codes */
typedef enum {
    L3GXXXX_OK,                   /**< success */
    L3GXXXX_ERROR_I2C,            /**< I2C communication error */
    L3GXXXX_ERROR_SPI,            /**< SPI communication error */
    L3GXXXX_ERROR_WRONG_CHIP_ID,  /**< wrong chip id read from WHO_AM_I reg */
    L3GXXXX_ERROR_INV_DEV,        /**< invalid device type used */
    L3GXXXX_ERROR_INV_MODE,       /**< sensor mode is invalid or not available */
    L3GXXXX_ERROR_INV_FIFO_MODE,  /**< FIFO mode is invalid or not available */
    L3GXXXX_ERROR_INV_INT_TYPE,   /**< invalid interrupt type used */
    L3GXXXX_ERROR_NO_NEW_DATA,    /**< no new data are available */
    L3GXXXX_ERROR_RAW_DATA,       /**< reading raw output data failed */
    L3GXXXX_ERROR_RAW_DATA_FIFO,  /**< reading raw output data from FIFO failed */
    L3GXXXX_ERROR_NO_INT1_PIN,    /**< `INT1` signal pin not configured */
    L3GXXXX_ERROR_NO_INT2_PIN,    /**< `INT2/DRDY` signal pin not configured */
    L3GXXXX_ERROR_BYPASS_MODE,    /**< sensor is in bypass mode */
    L3GXXXX_ERROR_FIFO_MODE,      /**< sensor is in FIFO mode */
} l3gxxxx_error_codes_t;

/**
 * @brief   Sensor output data rates (ODR) and LPF2 cutoff frequencies
 *
 * The following output data rates (ODR) and the LPF1/LPF2 cutoff frequencies
 * are defined (Reference: Application Note AN4506):
 *
 * <center>
 * | Mode                   | ODR [Hz] | LPF1 cutoff [Hz] | LPF2 cutoff [Hz] |
 * |:-----------------------|:--------:|:----------------:|:----------------:|
 * | High ODR               |          |                  |                  |
 * | L3GXXXX_ODR_100_12     | 100      | 32               | 12.5             |
 * | L3GXXXX_ODR_100_25     | 100      | 32               | 25               |
 * | L3GXXXX_ODR_200_12     | 200      | 63.3             | 12.5             |
 * | L3GXXXX_ODR_200_25     | 200      | 63.3             | 25               |
 * | L3GXXXX_ODR_200_50     | 200      | 63.3             | 50               |
 * | L3GXXXX_ODR_200_70     | 200      | 63.3             | 70               |
 * | L3GXXXX_ODR_400_20     | 400      | 128              | 20               |
 * | L3GXXXX_ODR_400_25     | 400      | 128              | 25               |
 * | L3GXXXX_ODR_400_50     | 400      | 128              | 50               |
 * | L3GXXXX_ODR_400_110    | 400      | 128              | 110              |
 * | L3GXXXX_ODR_800_30     | 800      | 211              | 30               |
 * | L3GXXXX_ODR_800_35     | 800      | 211              | 35               |
 * | L3GXXXX_ODR_800_50     | 800      | 211              | 50               |
 * | L3GXXXX_ODR_800_100    | 800      | 211              | 100              |
 * |                        |          |                  |                  |
 * | Low ODR (L3GD20H only) |          |                  |                  |
 * | L3GXXXX_ODR_12         | 12.5     | 3.9              | -                |
 * | L3GXXXX_ODR_25         | 25       | 7.8              | -                |
 * | L3GXXXX_ODR_50         | 50       | 16               | 16.6             |
 * </center><br>
 *
 * Detailed information about the filter chain and possible
 * filter combinations can be found in the section
 * [Output data rates and filters](#l3gxxxx_odr_filters).
 *
 * While LPF1 is always used, LPF2 and HPF have to be explicitly enabled
 * by the configuration parameter l3gxxxx_params_t::filter_sel or the
 * #l3gxxxx_select_output_filter function if module `l3gxxxx_config` is
 * used. #L3GXXXX_ODR_100_25 is used by the default configuration.
 *
 * @note Low data rates 12.5 Hz, 25 Hz and 50 Hz are only supported by L3GXXXX.
 */
typedef enum {
    L3GXXXX_ODR_100_12  = 0x00, /**< High ODR 100 Hz, LPF1 cutoff 32 Hz,   LPF2 cutoff 12.5 Hz */
    L3GXXXX_ODR_100_25  = 0x01, /**< High ODR 100 Hz, LPF1 cutoff 32 Hz,   LPF2 cutoff 25 Hz */
    L3GXXXX_ODR_200_12  = 0x04, /**< High ODR 200 Hz, LPF1 cutoff 63.3 Hz, LPF2 cutoff 12.5 Hz */
    L3GXXXX_ODR_200_25  = 0x05, /**< High ODR 200 Hz, LPF1 cutoff 63.3 Hz, LPF2 cutoff 25 Hz */
    L3GXXXX_ODR_200_50  = 0x06, /**< High ODR 200 Hz, LPF1 cutoff 63.3 Hz, LPF2 cutoff 50 Hz */
    L3GXXXX_ODR_200_70  = 0x07, /**< High ODR 200 Hz, LPF1 cutoff 63.3 Hz, LPF2 cutoff 70 Hz */
    L3GXXXX_ODR_400_20  = 0x08, /**< High ODR 400 Hz, LPF1 cutoff 128 Hz,  LPF2 cutoff 20 Hz */
    L3GXXXX_ODR_400_25  = 0x09, /**< High ODR 400 Hz, LPF1 cutoff 128 Hz,  LPF2 cutoff 25 Hz */
    L3GXXXX_ODR_400_50  = 0x0a, /**< High ODR 400 Hz, LPF1 cutoff 128 Hz,  LPF2 cutoff 50 Hz */
    L3GXXXX_ODR_400_110 = 0x0b, /**< High ODR 400 Hz, LPF1 cutoff 128 Hz,  LPF2 cutoff 110 Hz */
    L3GXXXX_ODR_800_30  = 0x0c, /**< High ODR 400 Hz, LPF1 cutoff 211 Hz,  LPF2 cutoff 30 Hz */
    L3GXXXX_ODR_800_35  = 0x0d, /**< High ODR 400 Hz, LPF1 cutoff 211 Hz,  LPF2 cutoff 35 Hz */
    L3GXXXX_ODR_800_50  = 0x0e, /**< High ODR 400 Hz, LPF1 cutoff 211 Hz,  LPF2 cutoff 50 Hz */
    L3GXXXX_ODR_800_100 = 0x0f, /**< High ODR 400 Hz, LPF1 cutoff 211 Hz,  LPF2 cutoff 100 Hz */
#if IS_USED(MODULE_L3GD20H) || IS_USED(MODULE_L3GXXXX_LOW_ODR) || DOXYGEN
    L3GXXXX_ODR_12      = 0x10, /**< Low ODR 12.5 Hz, LPF1 cutoff 3.9 Hz,  LPF2 not used */
    L3GXXXX_ODR_25      = 0x14, /**< Low ODR 25 Hz,   LPF1 cutoff 7.8 Hz,  LPF2 not used */
    L3GXXXX_ODR_50      = 0x18, /**< Low ODR 50 Hz,   LPF1 cutoff 16 Hz,   LPF2 cutoff 16.6 Hz */
#endif
} l3gxxxx_odr_t;

/**
 * @brief   Full scale in degrees per second (dps)
 *
 * The full scale value determines the sensitivity of the sensor and thus
 * the range and resolution of the sensor's output data. The resolution
 * of the output data is about Full Scale/INT16_MAX.
 *
 * @note On the A34250D, only 245 dps (#L3GXXXX_SCALE_245_DPS) is
 * available as full scale value.
 */
typedef enum {
    L3GXXXX_SCALE_245_DPS  = 0,  /**< 245 dps (default) */
    L3GXXXX_SCALE_500_DPS  = 1,  /**< 500 dps */
    L3GXXXX_SCALE_2000_DPS = 2,  /**< 2000 dps */
} l3gxxxx_scale_t;

/**
 * @brief   FIFO mode
 *
 * The integrated FIFO with up to 32 data samples can be used in different
 * modes. The mode defines the behavior of FIFO when it becomes full.
 */
typedef enum {
    L3GXXXX_BYPASS           = 0, /**< FIFO is not used (default) */

    L3GXXXX_FIFO             = 1, /**< Data samples are stored in the FIFO until
                                       it is full */
    L3GXXXX_STREAM           = 2, /**< FIFO is used as ring buffer and newest
                                       data samples are stored continuously */
#if IS_USED(MODULE_L3GD20H) || IS_USED(MODULE_L3GD20) || DOXYGEN
    L3GXXXX_STREAM_TO_FIFO   = 3, /**< FIFO is used in Stream mode until an
                                       interrupt, switches then to FIFO mode
                                       (L3GD20H and L3GD20 only) */
    L3GXXXX_BYPASS_TO_STREAM = 4, /**< FIFO is not used until an interrupt,
                                       switches then to Stream mode
                                       (L3GD20H and L3GD20 only) */
#endif
#if IS_USED(MODULE_L3GD20H) || DOXYGEN
    L3GXXXX_DYNAMIC_STREAM   = 6, /**< like Stream mode, but differs in reading
                                       the first data sample after emptying
                                       (L3GD20H only) */
    L3GXXXX_BYPASS_TO_FIFO   = 7  /**< FIFO is not used until an interrupt,
                                       switches then to FIFO mode
                                       (L3GD20H only) */
#endif
} l3gxxxx_fifo_mode_t;

/**
 * @brief   High pass filter (HPF) and low pass filter 2 (LPF2) selection
 *
 * L3Gxxxx sensors integrate a combination of two low pass filters (LPF) and
 * one high pass filter (HPF).
 *
 * First, raw sensor data are always filtered by LPF1 with a cutoff frequency
 * that is fixed for the selected output data rate (ODR), see #l3gxxxx_odr_t.
 * Resulting data can then optionally be filtered by HPF and/or LPF2. Both
 * filters can be used or bypassed.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                                  +--------------->   L3GXXXX_NO_FILTER
 *                                  |    +----- +
 *                     +------------+--->|      |--->   L3GXXXX_LPF2_ONLY
 *                     |                 | LPF2 |
 * +-----+   +------+  |   +-----+  +--->|      |--->   L3GXXXX_HPF_AND_LPF2
 * |     |   |      |  |   |     |  |    +------+
 * | ADC |-->| LPF1 |--+-->| HPF |--+--------------->   L3GXXXX_HPF_ONLY
 * |     |   |      |      |     |
 * +-----+   +------+      +-----+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * #l3gxxxx_filter_sel_t defines the possible filter combinations that can
 * be used to select the filters for the output data and for the
 * interrupt generation separately.
 *
 * The default filter selection for the output data is #L3GXXXX_HPF_AND_LPF2
 * and is defined by the default configuration parameter
 * #CONFIG_L3GXXXX_FILTER_SEL. If the module `l3gxxxx_config` is used, it can
 * be changed at runtime using function #l3gxxxx_select_output_filter.
 *
 * The default filter selection for event interrupt generation is
 * #L3GXXXX_HPF_AND_LPF2 and is defined by default configuration parameter
 * #CONFIG_L3GXXXX_INT1_FILTER. It can be changed at runtime with function
 * #l3gxxxx_set_int_event_cfg.
 *
 * @note Since same filters are used for the output data as well as the
 * data used for event interrupt generation (selective axes movement / wake-up),
 * the configuration of the filters always affects both data. If the HPF is
 * enabled for filtering the output data, it is also active for filtering the
 * sensor data used for interrupt generation if the LPF2 is enabled for
 * interrupt generation. The other way around, the HPF is also active for
 * filtering the output data when it is enabled for interrupt generation and
 * when the LPF2 is enabled for the output data.
 *
 * The cutoff frequencies of LPF1 and LPF2 are determined by the used output
 * data rate #l3gxxxx_odr_t, see section [Output Data Rates and Filters]
 * (#l3gxxxx_odr_filters). The default cutoff frequency of HPF is 8 Hz and
 * set by the default configuration parameter #CONFIG_L3GXXXX_HPF_CUTOFF.
 * If module `l3gxxxx_config` is used, it can be changed at runtime using
 * function #l3gxxxx_config_hpf.
 */
typedef enum {
    L3GXXXX_NO_FILTER    = 0,   /**< HPF not used, LPF2 not used (default) */
    L3GXXXX_HPF_ONLY     = 1,   /**< HPF used, LPF2 not used */
    L3GXXXX_LPF2_ONLY    = 2,   /**< HPF not used, LPF2 used */
    L3GXXXX_HPF_AND_LPF2 = 3    /**< HPF used, LPF2 used */
} l3gxxxx_filter_sel_t;

/**
 * @brief   HPF (high pass filter) modes
 *
 * The high pass filter can be used in different modes.
 */
typedef enum {
    L3GXXXX_HPF_NORMAL    = 0,  /**< Normal mode, HPF is reset by reading
                                     the REFERENCE register */
    L3GXXXX_HPF_REFERENCE = 1,  /**< Reference mode, output data are the
                                     difference to the REFERENCE register */
    L3GXXXX_HPF_AUTORESET = 3   /**< Autoreset mode, HPF is automatically reset
                                     when a configured event interrupt occurs */
} l3gxxxx_hpf_mode_t;

/**
 * @brief   Interrupt types
 *
 * L3Gxxxx sensors support different types of interrupts. These are on the
 * one hand the various data interrupts on signal `INT2/DRDY` and on the
 * other hand event interrupts on signal `INT1`.
 *
 * The enumeration values correspond to the according bits in register
 * CTRL3 (#L3GXXXX_REG_CTRL3).
 *
 * #L3GXXXX_INT_DATA combines the various data interrupts to an ORed value.
 */
typedef enum {
    /**
     * Data interrupt on signal `INT2/DRDY`: Output data are ready to be read.
     */
    L3GXXXX_INT_DATA_READY = L3GXXXX_INT2_DRDY,
    /**
     * Data interrupt on signal `INT2/DRDY`: FIFO filling exceeds the
     * watermark level (threshold)
     */
    L3GXXXX_INT_FIFO_WATERMARK = L3GXXXX_INT2_WTM,
    /**
     * Data interrupt on signal `INT2/DRDY`: FIFO is completely filled
     */
    L3GXXXX_INT_FIFO_OVERRUN = L3GXXXX_INT2_ORUN,
    /**
     * Data interrupt on signal `INT2/DRDY`: FIFO becomes empty
     */
    L3GXXXX_INT_FIFO_EMPTY = L3GXXXX_INT2_EMPTY,
    /**
     * Event interrupt on signal `INT1`: Angular rate of one or more axes
     * is lower or higher than the configured threshold.
     */
    L3GXXXX_INT_EVENT = L3GXXXX_INT1_IG,
} l3gxxxx_int_types_t;

/**
 * @brief   Data interrupts (Data ready and FIFO status)
 *
 * This define combines the data interrupt types of #l3gxxxx_int_types_t
 * that use the `INT2/DRDY` signal to an ORed value.
 */
#define L3GXXXX_INT_DATA (L3GXXXX_INT_DATA_READY | \
                          L3GXXXX_INT_FIFO_WATERMARK | \
                          L3GXXXX_INT_FIFO_OVERRUN | \
                          L3GXXXX_INT_FIFO_EMPTY)

/**
 * @brief   Event interrupt generator configuration (axis movement and wake-up)
 *
 * memset to 0 to disable all interrupt conditions (default)
 */
typedef struct {
    uint16_t x_threshold;    /**< X threshold value in full scale / INT16_MAX */
    uint16_t y_threshold;    /**< Y threshold value in full scale / INT16_MAX */
    uint16_t z_threshold;    /**< Z threshold value in full scale / INT16_MAX */

    bool x_low_enabled;      /**< Interrupt enabled for |X| < X threshold (X low event) */
    bool x_high_enabled;     /**< Interrupt enabled for |X| > X threshold (X high event) */

    bool y_low_enabled;      /**< Interrupt enabled for |Y| < Y threshold (Y low event) */
    bool y_high_enabled;     /**< Interrupt enabled for |Y| > Y threshold (Y high event) */

    bool z_low_enabled;      /**< Interrupt enabled for |Z| < Z threshold (Z low event) */
    bool z_high_enabled;     /**< Interrupt enabled for |Z| > Z threshold (Y high event) */

    l3gxxxx_filter_sel_t filter; /**< HPF and LPF2 filter selection used
                                      for threshold comparison */

    bool and_or;       /**< Combination of interrupt events (true=AND, false=OR):<br>
                            AND - all enabled axes passed their thresholds<br>
                            OR - at least one axis passed its threshold */
    bool latch;        /**< Latch the interrupt when true until the interrupt
                            source has been read by function l3gxxxx_wait_int. */
    uint8_t duration;  /**< Duration in 1/ODR an interrupt condition has to be
                            given before the interrupt is generated. */
    bool wait;         /**< When true, duration is also used when interrupt
                            condition in no longer given before interrupt
                            signal is reset. */
    bool counter_mode; /**< DCRM is not documented and not used therefore. */
} l3gxxxx_int_event_cfg_t;

/**
 * @brief   Event interrupt sources (axis movement and wake-up)
 */
typedef union {
    struct {
        uint8_t x_low :1; /**< true on |X| < X threshold (X low event) */
        uint8_t x_high:1; /**< true on |X| > X threshold (X high event) */
        uint8_t y_low :1; /**< true on |Y| < Y threshold (Y low event) */
        uint8_t y_high:1; /**< true on |Y| > Y threshold (Y high event) */
        uint8_t z_low :1; /**< true on |Z| < Z threshold (Z low event) */
        uint8_t z_high:1; /**< true on |Z| > Z threshold (Z high event) */
        uint8_t active:1; /**< true when one or more events have been generated */
        uint8_t unused:1; /**< not used */
    };
    uint8_t val;          /**< event interrupt sources as value that
                               can be used for bitwise operations */
} l3gxxxx_int_event_src_t;

/**
 * @brief   Data interrupt sources (data ready and FIFO status)
 */
typedef union {
    struct {
        uint8_t fifo_empty    :1;  /**< true when FIFO is empty */
        uint8_t fifo_overrun  :1;  /**< true when FIFO is completely filled */
        uint8_t fifo_watermark:1;  /**< true when FIFO filling > watermark */
        uint8_t data_ready    :1;  /**< true when data are ready to read */
        uint8_t unused        :4;  /**< not used */
    };
    uint8_t val;                   /**< data interrupt sources as value that
                                        can be used for bitwise operations */
} l3gxxxx_int_data_src_t;

/**
 * @brief   Composite type for all possible interrupt sources
 *
 * This type combines the possible interrupt sources for event interrupts on
 * signal `INT1` (l3gxxxx_int_event_src_t) with those for data interrupts on
 * signal `INT2/DRDY` (l3gxxxx_int_data_src_t).
 */
typedef struct {
    l3gxxxx_int_event_src_t event;  /**< event interrupt sources */
    l3gxxxx_int_data_src_t  data;   /**< data interrupt sources */
} l3gxxxx_int_src_t;

/**
 * @brief   `INT1`, `INT2/DRDY` sensor signal activity level
 */
typedef enum {
    L3GXXXX_HIGH = 0,       /**< INT signals are High active (default) */
    L3GXXXX_LOW             /**< INT signals are Low active */
} l3gxxxx_int_pin_level_t;

/**
 * @brief   `INT1`, `INT2/DRDY` sensor signal type
 */
typedef enum {
    L3GXXXX_PUSH_PULL = 0,  /**< INT pins are push/pull outputs (default) */
    L3GXXXX_OPEN_DRAIN      /**< INT pins are open-drain */
} l3gxxxx_int_pin_type_t;

/**
 * @brief   Raw output data set as two's complements
 */
typedef struct {
    int16_t x; /**< X angular rate (roll) as 16 bit two's complements */
    int16_t y; /**< Y angular rate (pitch) as 16 bit two's complements */
    int16_t z; /**< Z angular rate (yaw) as 16 bit two's complements */
} l3gxxxx_raw_data_t;

/**
 * @brief   Angular rates in millidegrees per seconds (mdps)
 */
typedef struct {
    int32_t x; /**< X angular rate (roll) */
    int32_t y; /**< Y angular rate (pitch) */
    int32_t z; /**< Z angular rate (yaw) */
} l3gxxxx_data_t;

/**
 * @brief   Raw output data FIFO type
 */
typedef l3gxxxx_raw_data_t l3gxxxx_raw_data_fifo_t[32];

/**
 * @brief   Angular rates FIFO type
 */
typedef l3gxxxx_data_t l3gxxxx_data_fifo_t[32];

/**
 * @brief   L3Gxxxx interface types
 */
typedef enum {
#if IS_USED(MODULE_L3GXXXX_I2C) || DOXYGEN
    L3GXXXX_I2C,                   /**< I2C interface used */
#endif
#if IS_USED(MODULE_L3GXXXX_SPI) || DOXYGEN
    L3GXXXX_SPI,                   /**< SPI interface used */
#endif
} l3gxxxx_if_t;

#if IS_USED(MODULE_L3GXXXX_I2C) || DOXYGEN
/**
 * @brief   L3Gxxxx I2C interface parameters
 */
typedef struct {
    i2c_t   dev;                    /**< I2C device used */
    uint8_t addr;                   /**< I2C slave address */
} l3gxxxx_i2c_params_t;
#endif

#if IS_USED(MODULE_L3GXXXX_SPI) || DOXYGEN
/**
 * @brief   L3Gxxxx SPI interface parameters
 */
typedef struct {
    spi_t     dev;                  /**< SPI device used */
    spi_clk_t clk;                  /**< SPI clock speed */
    gpio_t    cs;                   /**< SPI chip Select pin */
} l3gxxxx_spi_params_t;
#endif

/**
 * @brief   L3Gxxxx Hardware interface parameters union
 */
typedef struct {
    l3gxxxx_if_t type;             /**< I2C/SPI interface type selector */
    union {
#if IS_USED(MODULE_L3GXXXX_I2C) || DOXYGEN
        l3gxxxx_i2c_params_t i2c;  /**< I2C interface parameters */
#endif
#if IS_USED(MODULE_L3GXXXX_SPI) || DOXYGEN
        l3gxxxx_spi_params_t spi;  /**< SPI interface parameters */
#endif
    };
} l3gxxxx_if_params_t;

/**
 * @brief   L3Gxxxx device initialization parameters
 */
typedef struct {
    l3gxxxx_if_params_t  if_params;      /**< Interface parameters (I2C/SPI) */

    l3gxxxx_odr_t        odr;            /**< ODR and LPF2 cutoff frequency */
    l3gxxxx_scale_t      scale;          /**< Full scale */

    l3gxxxx_filter_sel_t filter_sel;     /**< HPF/LPF2 filter selection */
    l3gxxxx_hpf_mode_t   hpf_mode;       /**< HPF mode */
    uint8_t              hpf_cutoff;     /**< HPF cutoff frequency 0..9, see
                                              l3gxxxx_config_hpf for details */
#if IS_USED(MODULE_L3GXXXX_FIFO) || DOXYGEN
    l3gxxxx_fifo_mode_t  fifo_mode;      /**< FIFO operation mode */
    uint8_t              fifo_watermark; /**< FIFO watermark setting 0..31 */
#endif

#if IS_USED(MODULE_L3GXXXX_IRQ_DATA) || DOXYGEN
    gpio_t               int2_pin;       /**< MCU GPIO pin for data interrupts
                                              on signal `INT2/DRDY` pin */
#endif

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT) || DOXYGEN
    l3gxxxx_int_event_cfg_t int1_cfg;    /**< event interrupt parameters */
    gpio_t                  int1_pin;    /**< MCU GPIO pin for event interrupts
                                              on signal `INT1` */
#endif

} l3gxxxx_params_t;

/**
 * @brief   L3Gxxxx sensor device data structure type
 */
typedef struct {
    l3gxxxx_params_t params;      /**< Device initialization parameters */
    l3gxxxx_int_types_t int_type; /**< Type of the last interrupt triggered */
    mutex_t int_lock;             /**< Used to lock the calling thread while
                                       waiting for an interrupt */
    enum {                        /**< Sensor detected at runtime */
        L3GD20H,                  /**< L3GD20H detected */
        L3GD20,                   /**< L3GD20 detected */
        X3G42XXD                  /**< L3G400D, I3G4250D or A3G4250D detected */
    } sensor;                     /**< recognized sensor type */
} l3gxxxx_t;

/**
 * @name    Sensor initialization and configuration
 * @{
 */

/**
 * @brief   Initialize the L3Gxxxx sensor device
 *
 * This function resets the sensor and initializes it according to the
 * given configuration parameter set. All registers are reset to their
 * default values. The FIFO is cleared.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor to be initialized
 * @param[in]   params  L3Gxxxx configuration parameters
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_init(l3gxxxx_t *dev, const l3gxxxx_params_t *params);

#if IS_USED(MODULE_L3GXXXX_CONFIG) || DOXYGEN
/**
 * @brief   Set sensor mode
 *
 * @note This function is available only if module `l3gxxxx_config` is used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[in]   odr     output data rate (ODR) and LPF2 cutoff frequency
 * @param[in]   x       enable X axis measurements if true
 * @param[in]   y       enable Y axis measurements if true
 * @param[in]   z       enable Z axis measurements if true
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_set_mode(l3gxxxx_t *dev,
                     l3gxxxx_odr_t odr, bool x, bool y, bool z);

/**
 * @brief   Set full scale
 *
 * @note This function is available only if module `l3gxxxx_config` is used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[in]   scale   fulle scale
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_set_scale(l3gxxxx_t *dev, l3gxxxx_scale_t scale);

#endif /* IS_USED(MODULE_L3GXXXX_CONFIG) || DOXYGEN */
/** @} */

/**
 * @name    Power saving functions
 * @{
 */
/**
 * @brief   Power down the sensor
 *
 * Changes the sensor operation mode to power-down mode. In this mode almost all
 * internal blocks including the gyros are switched off. I2C and SPI interfaces
 * are still active. The content of the configuration registers is preserved.
 *
 * @param[in]   dev     Device descriptor of L3Gxxxx device to read from
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_power_down (l3gxxxx_t *dev);

/**
 * @brief   Power up the sensor
 *
 * Swichtes the sensor back into the last active operation mode. It takes
 * up to 100 ms since the gyros have to be switched on.
 *
 * @param[in]   dev     Device descriptor of L3Gxxxx device to read from
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_power_up (l3gxxxx_t *dev);

#if IS_USED(MODULE_L3GXXXX_SLEEP) || DOXYGEN
/**
 * @brief   Sleep mode
 *
 * Activates the sleep mode of the sensor. In this mode, measurements for all
 * axes are disabled, but the gyroscopes remain on. To return from sleep mode,
 * function #l3gxxxx_wake_up is used.
 *
 * @note This function is available only if module `l3gxxxx_sleep` is used.
 *
 * @param[in]   dev     Device descriptor of L3Gxxxx device to read from
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_sleep (l3gxxxx_t *dev);

/**
 * @brief   Wake-up the sensor
 *
 * Swichtes the sensor back into the last active operation mode. It takes only
 * 1/ODR when LPF2 is disabled and 6/ODR when LPF2 is enabled to
 * continue measurements.
 *
 * @note This function is available only if module `l3gxxxx_sleep` is used.
 *
 * @param[in]   dev     Device descriptor of L3Gxxxx device to read from
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_wake_up (l3gxxxx_t *dev);

#endif /* IS_USED(MODULE_L3GXXXX_SLEEP) || DOXYGEN */
/** @} */

/**
 * @name    Basic sensor data handling
 * @{
 */

/**
 * @brief    Data ready status function
 *
 * This function returns the number of new data samples that are ready to be
 * read or 0 if no new data samples are available.
 *
 * If the FIFO is not used or used in bypass mode (#L3GXXXX_BYPASS), the
 * maximum number of available data samples is 1. If another FIFO mode is
 * used, the number of available data samples is equal to the number of
 * new data samples in the FIFO.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 *
 * @return  number of data samples available for read or negative error code,
 *          see #l3gxxxx_error_codes_t
 */
int l3gxxxx_data_ready(const l3gxxxx_t *dev);

/**
 * @brief   Read last sample of angular rates in millidegree per second (mpds)
 *
 * Raw output data are read from the sensor and converted to angular rates
 * in millidegrees per second (mdps). The resolution of the angular rates
 * depends on the configured full scale value as follows:
 *
 * <center>
 * | Full Scale  | Resolution       | Driver symbol           | Remark                    |
 * | -----------:|-----------------:|:------------------------|:--------------------------|
 * | ±245 dps    | 8.75 mdps / LSB  | #L3GXXXX_SCALE_245_DPS  |                           |
 * | ±500 dps    | 17.50 mdps / LSB | #L3GXXXX_SCALE_500_DPS  | not available on A3G4250D |
 * | ±2000 dps   | 70.00 mdps / LSB | #L3GXXXX_SCALE_2000_DPS | not available on A3G4250D |
 * </center><br>
 *
 * @note If the FIFO is enabled by module `l3gxxxxx_fifo`, the function
 * returns only the last sample. To read all samples from the FIFO, function
 * l3gxxxx_read_fifo has to be used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[out]  data    last sample of angular rates in mdps
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_read(const l3gxxxx_t *dev, l3gxxxx_data_t *data);

/**
 * @brief   Read last sample of raw output data as 16 bit two's complements
 *
 * @note If the FIFO is enabled by module `l3gxxxxx_fifo`, the function
 * returns only the last sample. To read all samples from the FIFO, function
 * l3gxxxx_read_raw_fifo has to be used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param       raw     last sample of raw output data as 16 bit two's
 *                      complements
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_read_raw(const l3gxxxx_t *dev, l3gxxxx_raw_data_t *raw);

/** @} */

#if IS_USED(MODULE_L3GXXXX_FIFO) || DOXYGEN

/**
 * @name    FIFO handling
 * @{
 */

#if IS_USED(MODULE_L3GXXXX_CONFIG) || DOXYGEN
/**
 * @brief   Set FIFO mode and watermark level (threshold)
 *
 * The FIFO buffer can work in seven different modes and is able to store
 * up to 32 data samples, see #l3gxxxx_fifo_mode_t. The use of the FIFO allows
 * to reduce the interaction events of the MCU with the sensor and thus to
 * save power.
 *
 * The watermark level can be used to define the number of raw output data
 * samples that have to be stored in the FIFO before the watermark flag is
 * set and the #L3GXXXX_INT_FIFO_WATERMARK is triggered, if enabled. The
 * watermark flag is set and the interrupt #L3GXXXX_INT_FIFO_WATERMARK is
 * triggered when the number of samples stored in the FIFO becomes greater
 * than this watermark level (threshold).
 *
 * @note This function is available only if modules `l3gxxxx_fifo`
 * and `l3gxxxx_config` are used.
 *
 * @param[in]   dev        device descriptor of the L3Gxxxx sensor
 * @param[in]   mode       FIFO mode
 * @param[in]   watermark  FIFO watermark (ignored in bypass mode)
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_set_fifo_mode(l3gxxxx_t *dev,
                          l3gxxxx_fifo_mode_t mode, uint8_t watermark);

#endif /* IS_USED(MODULE_L3GXXXX_CONFIG) || DOXYGEN */

/**
 * @brief   Get all samples of angular rates stored in the FIFO (unit mdps)
 *
 * This function reads all samples of raw output data from the FIFO and
 * converts them to angular rates in millidegrees per second (mdps) according
 * to the configured full scale. For details about the resolution of these
 * angular rates see l3gxxxx_read.
 *
 * In bypass mode (#L3GXXXX_BYPASS), it returns only the last sample.
 *
 * @note This function is available only if module `l3gxxxx_fifo` is used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[out]  data    array for up to 32 samples of angular rates in mdps
 *
 * @return  number of data samples read on success, or negative error code,
 *          see #l3gxxxx_error_codes_t
 */
int l3gxxxx_read_fifo(const l3gxxxx_t *dev,
                      l3gxxxx_data_fifo_t data);

/**
 * @brief   Get all samples of raw output data stored in the FIFO
 *
 * This function reads all samples of raw output data from the FIFO.
 * In bypass mode (#L3GXXXX_BYPASS), it returns only the last raw
 * output data sample.
 *
 * @note This function is available only if module `l3gxxxx_fifo` is used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[out]  raw     array for up to 32 raw output data as two's complement
 *
 * @return  number of data samples read on success, or negative error code,
 *          see #l3gxxxx_error_codes_t
 */
int l3gxxxx_read_raw_fifo(const l3gxxxx_t *dev,
                          l3gxxxx_raw_data_fifo_t raw);

#endif /* IS_USED(MODULE_L3GXXXX_FIFO) || DOXYGEN */
/** @} */

#if IS_USED(MODULE_L3GXXXX_CONFIG) || DOXYGEN
/**
 * @name    Filter configuration and handling
 * @{
 */

/**
 * @brief   Filter selection for raw output data
 *
 * L3Gxxxx supports a combination of a high pass filter (HPF) and a second
 * low pass filter (LPF2). This function selects the combination of the HPF
 * and the LPF2 applied to raw output data.
 *
 * @note
 * - This function is available only if module `l3gxxxx_config` is used.
 * - The filter selection for the output data also affects the filter
 *   selection for event interrupt generation. If the HPF is enabled for
 *   filtering the output data, it is also active for filtering the sensor
 *   data used for interrupt generation if the LPF2 is enabled for interrupt
 *   generation.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[in]   filter  selected filters for output values
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_select_output_filter(l3gxxxx_t *dev,
                                 l3gxxxx_filter_sel_t filter);

/**
 * @brief   Config HPF (high pass filter) for output data
 *
 * The cutoff frequency of the HPF depends on the selected output data rate
 * (ODR). The following table shows the possible values of parameter \p cutoff
 * and the cutoff frequencies for the according ODRs. All frequencies are
 * given in Hz.
 *
 * <center>
 * | cutoff / ODR | 12.5  | 25    | 50    | 100  | 200  | 400  | 800 |
 * |-------------:|:-----:|:-----:|:-----:|:----:|:----:|:----:|:---:|
 * | 0            | 1     | 2     | 4     | 8    | 15   | 30   | 56  |
 * | 1            | 0.5   | 1     | 2     | 4    | 8    | 15   | 30  |
 * | 2            | 0.2   | 0.5   | 1     | 2    | 4    | 8    | 15  |
 * | 3            | 0.1   | 0.2   | 0.5   | 1    | 2    | 4    | 8   |
 * | 4            | 0.05  | 0.1   | 0.2   | 0.5  | 1    | 2    | 4   |
 * | 5            | 0.02  | 0.05  | 0.1   | 0.2  | 0.5  | 1    | 2   |
 * | 6            | 0.01  | 0.02  | 0.05  | 0.1  | 0.2  | 0.5  | 1   |
 * | 7            | 0.005 | 0.01  | 0.02  | 0.05 | 0.1  | 0.2  | 0.5 |
 * | 8            | 0.002 | 0.005 | 0.01  | 0.02 | 0.05 | 0.1  | 0.2 |
 * | 9            | 0.001 | 0.002 | 0.005 | 0.01 | 0.02 | 0.05 | 0.1 |
 * </center><br>
 *
 * @note This function is available only if module `l3gxxxx_config` is used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[in]   mode    high pass filter mode, see #l3gxxxx_hpf_mode_t
 * @param[in]   cutoff  cutoff frequency (depends on ODR) [0 ... 9]
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_config_hpf(const l3gxxxx_t *dev,
                       l3gxxxx_hpf_mode_t mode, uint8_t cutoff);

/**
 * @brief   Set HPF (high pass filter) reference
 *
 * Used to set the reference for HPF in reference mode #L3GXXXX_HPF_REFERENCE
 * and to reset the HPF in autoreset mode #L3GXXXX_HPF_AUTORESET.
 * Reference is given as two's complement.
 *
 * @note This function is available only if module `l3gxxxx_config` is used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[in]   ref     reference in #L3GXXXX_HPF_REFERENCE mode, otherwise ignored
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_set_hpf_ref(const l3gxxxx_t *dev, int8_t ref);

/**
 * @brief   Get HPF (high pass filter) reference
 *
 * Returns the content of the REFERENCE register. In normal mode
 * #L3GXXXX_HPF_NORMAL, it is also used to reset the HPF.
 *
 * @note This function is available only if module `l3gxxxx_config` is used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[out]  ref     reference
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_get_hpf_ref(const l3gxxxx_t *dev, int8_t *ref);

#endif /* IS_USED(MODULE_L3GXXXX_CONFIG) || DOXYGEN */
/** @} */

#if IS_USED(MODULE_L3GXXXX_IRQ) || DOXYGEN
/**
 * @name    Interrupt configuration and handling
 * @{
 */

/**
 * @brief   Enable or disable event and/or data interrupts on signal `INT1` and `INT2/DRDY`
 *
 * This function is used to enable or disable interrupts. The parameter \p mask
 * is the ORed value of the interrupts that are enabled or disabled by the
 * function call.
 *
 * @pre MCU GPIO pins for the `INT1` signal respectively the `INT2/DRDY`
 * signal have to be defined for enabled interrupts
 * (l3gxxxx_params_t::int1_pin and l3gxxxx_params_t::int2_pin).
 *
 * @note This function is available only if module `l3gxxxx_irq_data` and/or
 * module `l3gxxxx_irq_event` are used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[in]   mask    interrupts to be enabled or disabled
 * @param[in]   enable  enable the interrupts if true, otherwise disable them
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_enable_int(const l3gxxxx_t *dev,
                       l3gxxxx_int_types_t mask, bool enable);

/**
 * @brief   Wait for event and/or data interrupts on signals `INT1` and `INT2/DRDY`
 *
 * The function waits for a configured interrupt and returns the sources of
 * triggered interrupts. Since data interrupts (data ready and FIFO status)
 * and event interrupts (axis movement and wake-up) use different signals,
 * both data and event interrupts can occur simultaneously. The return
 * value of type l3gxxxx_int_src_t contains all sources for which the interrupt
 * conditions are fulfilled at the same time.
 *
 * @pre MCU GPIO pins for the `INT1` signal respectively the `INT2/DRDY`
 * signal have to be defined for enabled interrupts
 * (l3gxxxx_params_t::int1_pin and l3gxxxx_params_t::int2_pin).
 *
 * @note This function is available only if module `l3gxxxx_irq_data` and/or
 * module `l3gxxxx_irq_event` are used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
l3gxxxx_int_src_t l3gxxxx_wait_int(l3gxxxx_t *dev);

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT) || DOXYGEN
/**
 * @brief   Set new configuration for event interrupt generation
 *
 * The event interrupt generator produces interrupts (axis movement and wake-up)
 * on signal `INT1` whenever the angular rate of one or more axes becomes higher
 * or lower than defined thresholds.
 *
 * This function can be used at runtime to change the configuration of
 * the interrupt conditions for event interrupt generation. This includes
 * thresholds for all axes, enabled threshold interrupts, filter selection
 * used for interrupt generation and other parameters, see
 * l3gxxxx_int_event_cfg_t for details.
 *
 * @note This function is available only if module `l3gxxxx_irq_event` is used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[in]   cfg     event interrupt generator configuration
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_set_int_event_cfg(const l3gxxxx_t *dev,
                              const l3gxxxx_int_event_cfg_t *cfg);

#if IS_USED(MODULE_L3GXXXX_CONFIG) || DOXYGEN
/**
 * @brief   Get current configuration of event interrupt generation
 *
 * This function can be used to retrieve the configuration of interrupt
 * conditions currently used to generate event interrupts. See
 * l3gxxxx_int_event_cfg_t for details.
 *
 * @note This function is available only if module `l3gxxxx_irq_event` and
 * module `l3gxxxx_irq_config` are used.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[out]  cfg     event interrupt generator configuration
 *
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_get_int_event_cfg(const l3gxxxx_t *dev,
                              l3gxxxx_int_event_cfg_t *cfg);

#endif /* IS_USED(MODULE_L3GXXXX_CONFIG) || DOXYGEN */
#endif /* IS_USED(MODULE_L3GXXXX_IRQ_EVENT) || DOXYGEN */

/** @} */

#endif /* IS_USED(MODULE_L3GXXXX_IRQ) || DOXYGEN */

/**
 * @name   Low level interface functions
 * @{
 */

/**
 * @brief   Direct write to register
 *
 * @note This function should only be used to do something special that
 * is not covered by the high level interface AND if you exactly know what you
 * do and what effects it might have. Please be aware that it might affect the
 * high level interface.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[in]   reg     address of the first register to be changed
 * @param[in]   data    pointer to the data to be written to the register
 * @param[in]   len     number of bytes to be written to the register
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_reg_write(const l3gxxxx_t *dev,
                      uint8_t reg, const uint8_t *data, uint8_t len);

/**
 * @brief   Direct read from register
 *
 * @note This function should only be used to do something special that
 * is not covered by the high level interface AND if you exactly know what you
 * do and what effects it might have. Please be aware that it might affect the
 * high level interface.
 *
 * @param[in]   dev     device descriptor of the L3Gxxxx sensor
 * @param[in]   reg     address of the first register to be read
 * @param[out]  data    pointer to the data to be read from the register
 * @param[in]   len     number of bytes to be read from the register
 * @retval  L3GXXXX_OK      on success
 * @retval  L3GXXXX_ERROR_* negative error code, see #l3gxxxx_error_codes_t
 */
int l3gxxxx_reg_read(const l3gxxxx_t *dev,
                     uint8_t reg, uint8_t *data, uint8_t len);
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* L3GXXXX_H */
