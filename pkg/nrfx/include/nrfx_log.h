/*
 * Copyright (c) 2017 - 2024, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NRFX_LOG_H__
#define NRFX_LOG_H__

// THIS IS A TEMPLATE FILE.
// It should be copied to a suitable location within the host environment into
// which nrfx is integrated, and the following macros should be provided with
// appropriate implementations.
// And this comment should be removed from the customized file.

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"

/**
 * @defgroup nrfx_log nrfx_log.h
 * @{
 * @ingroup nrfx
 *
 * @brief This file contains macros that should be implemented according to
 *        the needs of the host environment into which @em nrfx is integrated.
 */

/**
 * @brief Macro for logging a message with the severity level ERROR.
 *
 * @param format printf-style format string, optionally followed by arguments
 *               to be formatted and inserted in the resulting string.
 */
#define NRFX_LOG_ERROR(format, ...) printf(format, ##__VA_ARGS__); puts("");

/**
 * @brief Macro for logging a message with the severity level WARNING.
 *
 * @param format printf-style format string, optionally followed by arguments
 *               to be formatted and inserted in the resulting string.
 */
#define NRFX_LOG_WARNING(format, ...) printf(format, ##__VA_ARGS__); puts("");

/**
 * @brief Macro for logging a message with the severity level INFO.
 *
 * @param format printf-style format string, optionally followed by arguments
 *               to be formatted and inserted in the resulting string.
 */
#define NRFX_LOG_INFO(format, ...) printf(format, ##__VA_ARGS__); puts("");

/**
 * @brief Macro for logging a message with the severity level DEBUG.
 *
 * @param format printf-style format string, optionally followed by arguments
 *               to be formatted and inserted in the resulting string.
 */
#define NRFX_LOG_DEBUG(format, ...) printf(format, ##__VA_ARGS__); puts("");

/**
 * @brief Macro for logging a memory dump with the severity level ERROR.
 *
 * @param[in] p_memory Pointer to the memory region to be dumped.
 * @param[in] length   Length of the memory region in bytes.
 */
#define NRFX_LOG_HEXDUMP_ERROR(p_memory, length)

/**
 * @brief Macro for logging a memory dump with the severity level WARNING.
 *
 * @param[in] p_memory Pointer to the memory region to be dumped.
 * @param[in] length   Length of the memory region in bytes.
 */
#define NRFX_LOG_HEXDUMP_WARNING(p_memory, length)

/**
 * @brief Macro for logging a memory dump with the severity level INFO.
 *
 * @param[in] p_memory Pointer to the memory region to be dumped.
 * @param[in] length   Length of the memory region in bytes.
 */
#define NRFX_LOG_HEXDUMP_INFO(p_memory, length)

/**
 * @brief Macro for logging a memory dump with the severity level DEBUG.
 *
 * @param[in] p_memory Pointer to the memory region to be dumped.
 * @param[in] length   Length of the memory region in bytes.
 */
#define NRFX_LOG_HEXDUMP_DEBUG(p_memory, length)


/**
 * @brief Macro for getting the textual representation of a given error code.
 *
 * @param[in] error_code Error code.
 *
 * @return String containing the textual representation of the error code.
 */
static char __error_string[64]; 

static char* error_code_to_string(unsigned int error_code) {
    sprintf(__error_string, "%d (0x%08X)", error_code, error_code);
    return __error_string;
}

#define NRFX_LOG_ERROR_STRING_GET(error_code) error_code_to_string(error_code) 

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_LOG_H__
