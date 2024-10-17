/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_wakaama
 * @defgroup    lwm2m_platform Platform adaption for Wakaama package
 * @brief Adaption of Wakaama LwM2M package to RIOT
 * @{
 * @file
 * @brief Definitions and public API for Wakaama adaption layer
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */
#ifndef LWM2M_PLATFORM_H
#define LWM2M_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup lwm2m_platform_conf Wakaama LwM2M platform adaption
 * @ingroup config
 * @brief Compile-time configuration options for the Wakaama LwM2M platform
 *        adaption layer.
 * @{
 */
/** @brief Size of allocation buffer in bytes */
#ifndef CONFIG_LWM2M_TLSF_BUFFER
#define CONFIG_LWM2M_TLSF_BUFFER     5120
#endif
/** @} */

/**
 * @brief Initializes the platform adaption for Wakaama LwM2M
 */
void lwm2m_platform_init(void);

/**
 * @brief Prints the status of TLSF allocation buffer, for development use.
 */
void lwm2m_tlsf_status(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LWM2M_PLATFORM_H */
