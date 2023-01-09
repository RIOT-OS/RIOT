/*
 * Copyright (C) 2013 Freie Universität Berlin
 *               2015 INRIA
 *               2018 RWTH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers Drivers
 * @brief       Drivers for external devices as radios, sensors, memory etc.
 *
 * The module contains all kind of drivers for specific devices.
 */

/**
 * @defgroup    drivers_multimedia Multimedia Device Drivers
 * @ingroup     drivers
 * @brief       Drivers for external multimedia devices.
 */

/**
 * @defgroup    drivers_netdev Network Device Drivers
 * @ingroup     drivers
 * @brief       Drivers for external network devices.
 */

/**
 * @defgroup    drivers_sensors Sensor Device Drivers
 * @ingroup     drivers
 * @brief       Drivers for external sensors.
 *
 * Most of these drivers will be connected over some bus or serial connection
 * to the MCU.
 */

/**
 * @defgroup    drivers_actuators Actuator Device Drivers
 * @ingroup     drivers
 * @brief       Drivers for actuating devices
 *
 * The group of actuators includes all kind of devices that can actively
 * interact with the physical world, as e.g. motors, lights, sound devices, etc.
 */

/**
 * @defgroup    drivers_storage Storage Device Drivers
 * @ingroup     drivers
 * @brief       Drivers for storage devices
 */

/**
 * @defgroup    drivers_display Display Device Drivers
 * @ingroup     drivers
 * @brief       Drivers for display devices
 */

/**
 * @defgroup    drivers_soft_periph Soft Peripheral Driver Interface
 * @ingroup     drivers
 * @brief       Software emulated @ref drivers_periph for UART, SPI, etc
 */

/**
 * @defgroup    drivers_power Power Supply Drivers
 * @ingroup     drivers
 * @brief       Drivers for power supply devices
 *
 * The group of power supply drivers indludes all kinds of AC/DC, DC/DC supply devices
 * or charger ICs which can be controlled from the host e.g by config pin settings or by bus.
 */

/**
 * @defgroup    drivers_misc Miscellaneous Drivers
 * @ingroup     drivers
 * @brief       Drivers for different kinds of devices that do not match any
 *              other category
 */

/**
 * @defgroup    config_drivers_netdev Network Device Configurations
 * @ingroup     config
 * @brief       Compile time configurations for external network devices
 */

/**
 * @defgroup    config_drivers_sensors Sensor Device Configurations
 * @ingroup     config
 * @brief       Compile time configurations for Sensors
 */

/**
 * @defgroup    config_drivers_actuators Actuator Device Configurations
 * @ingroup     config
 * @brief       Compile time configurations for actuating devices
 */

/**
 * @defgroup    config_drivers_storage Storage Device Configurations
 * @ingroup     config
 * @brief       Compile time configurations for storage devices
 */

 /**
 * @defgroup    config_drivers_display Display Device Configurations
 * @ingroup     config
 * @brief       Compile time configurations for display devices
 */

/**
 * @defgroup    config_drivers_soft_periph Soft Peripheral Configurations
 * @ingroup     config
 * @brief       Compile time configurations for Software emulated
 *              @ref drivers_periph for UART, SPI, etc
 */

/**
 * @defgroup    config_drivers_misc Miscellaneous Device Configurations
 * @ingroup     config
 * @brief       Compile time configurations for different kinds of
 *              devices that do not match any other category
 */