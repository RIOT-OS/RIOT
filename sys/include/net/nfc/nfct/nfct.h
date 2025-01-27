/*
 * Copyright (C) 2024 Technische Universität Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys/net/nfc
 * @{
 *
 * @file
 * @brief       Typedefs and function definitions for the NFCT high level API.
 *
 * Typedefs and function definitions for the NFCT high level API.
 *
 * @author      Max Stolze <max_karl.stolze@mailbox.tu-dresden.de>
 * @author      Nico Behrens <nifrabe@outlook.de>
 *
 */

#ifndef NFCT_H
#define NFCT_H

#include "net/nfc/ndef/ndef.h"
#include "net/nfc/t2t/t2t.h"
#include "modules.h"

#if IS_USED(MODULE_NFCT_CUSTOM_EMULATOR)
/* use your own emulator, otherwise a fitting emulator header is included */
#elif IS_USED(MODULE_NRFX_NFCT)
#  include "nrfx_nfct_t2t_emulator.h"
#endif

#if IS_USED(MODULE_NFC_T2T)
#endif
#include "nfct_t2t_emulator.h"
/**
 * @brief Starts emulation of the given type 2 tag using the emulator
 * 
 * @param[in] dev Device that uses the generic @ref nfct_t2t_emulator_t interface 
 * @param[in] tag Tag to emulate
 */
void nfct_start_type_2_tag(nfct_t2t_emulator_t *dev, nfc_t2t_t *tag);

/**
 * @brief Adds the given NDEF message to the given type 2 tag and emulates
 * the tag using the emulator
 *
 * @param[in] dev       Type 2 tag emulator
 * @param[in] tag       Type 2 tag
 * @param[in] message   NDEF message
 * @param[in] tag_type  NFC tag type
 */
void nfct_start_type_2_tag_with_ndef(nfct_t2t_emulator_t *dev, nfc_t2t_t *tag,
                                     const ndef_t *message);

/* convenience functions */
/**
 * @brief Initializes the type 2 tag struct, adds the given text content
 * and emulates it.
 *
 * @param[in] dev           Type 2 tag emulator
 * @param[in] tag           Uninitialized type 2 tag struct
 * @param[in] tag_mem       Empty tag memory
 * @param[in] tag_mem_size  Size of tag_mem
 * @param[in] content       Text String to put in tag
 * @param[in] lang_code     Language code for Text (defaults to "en" if NULL)
 * @param[in] encoding      Text Encoding
 */
void nfct_start_type_2_tag_with_text(nfct_t2t_emulator_t *dev, nfc_t2t_t *tag,
                                     uint8_t *tag_mem, size_t tag_mem_size, const char *content, char *lang_code,
                                     ndef_text_encoding_t encoding);

/**
 * @brief Initializes the type 2 tag struct, adds the given URI and emulates it.
 *
 * @param[in] dev               Type 2 tag emulator
 * @param[in] tag               Uninitialized type 2 tag struct
 * @param[in] tag_mem           Empty tag memory
 * @param[in] tag_mem_size      Size of tag_mem
 * @param[in] identifier_code   NDEF URI identifier
 * @param[in] uri               URI
 */
void nfct_start_type_2_tag_with_uri(nfct_t2t_emulator_t *dev, nfc_t2t_t *tag,
                                    uint8_t *tag_mem, size_t tag_mem_size,
                                    ndef_uri_identifier_code_t identifier_code,
                                    const char *uri);

/**
 * @brief Stops the emulation
 * 
 * @param[in] dev Type 2 Tag emulator to stop 
 */
void nfct_stop_type_2_tag(nfct_t2t_emulator_t *dev);

#endif
