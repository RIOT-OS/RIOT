/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       PSK and RPK keys for the dtls-sock example.
 *
 * @author      Raul Fuentes <raul.fuentes-samaniego@inria.fr>
 *
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Default keys examples for tinyDTLS (for RIOT, Linux and Contiki)
 */
#define PSK_DEFAULT_IDENTITY "Client_identity"
#define PSK_DEFAULT_KEY "secretPSK"
#define PSK_OPTIONS "i:k:"
#define PSK_ID_MAXLEN 32
#define PSK_MAXLEN 32

#ifdef __cplusplus
}
#endif
