/*
 * SPDX-FileCopyrightText: 2025 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_gp8xxx
 * @{
 *
 * @file
 * @brief       Definitions of the Guestgood GP8xxx I2C DACs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "gp8xxx_info.h"

const gp8xxx_info_t gp8503_info = {
    .type = GP8503_INFO_TYPE,
    .channels = GP8503_INFO_CHANNELS,
    .range = GP8503_INFO_RANGE,
    .resolution = GP8503_INFO_RESOLUTION,
};

const gp8xxx_info_t gp8512_info = {
    .type = GP8512_INFO_TYPE,
    .channels = GP8512_INFO_CHANNELS,
    .range = GP8512_INFO_RANGE,
    .resolution = GP8512_INFO_RESOLUTION,
};

const gp8xxx_info_t gp8211s_info = {
    .type = GP8211S_INFO_TYPE,
    .channels = GP8211S_INFO_CHANNELS,
    .range = GP8211S_INFO_RANGE,
    .resolution = GP8211S_INFO_RESOLUTION,
};

const gp8xxx_info_t gp8403_info = {
    .type = GP8403_INFO_TYPE,
    .channels = GP8403_INFO_CHANNELS,
    .range = GP8403_INFO_RANGE,
    .resolution = GP8403_INFO_RESOLUTION,
};

const gp8xxx_info_t gp8413_info = {
    .type = GP8413_INFO_TYPE,
    .channels = GP8413_INFO_CHANNELS,
    .range = GP8413_INFO_RANGE,
    .resolution = GP8413_INFO_RESOLUTION,
};

const gp8xxx_info_t gp8302_info = {
    .type = GP8302_INFO_TYPE,
    .channels = GP8302_INFO_CHANNELS,
    .range = GP8302_INFO_RANGE,
    .resolution = GP8302_INFO_RESOLUTION,
};
