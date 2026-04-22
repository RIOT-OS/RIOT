/*
 * SPDX-FileCopyrightText: 2023 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_iec62056_21 IEC 62056-21 - Direct Local Data Exchange
 * @ingroup     sys_iec62056
 * @brief       IEC 62056-21 direct local data exchange protocol
 *
 * IEC 62056-21 (formerly IEC 61107) defines a protocol for direct local data
 * exchange between utility meters and handheld units (HHU). It supports
 * communication over optical interfaces or wired connections using ASCII-based
 * telegrams.
 *
 * This module provides functions for:
 * - Parsing IEC 62056-21 mode C/D telegrams (e.g., DSMR P1 smart meter data)
 * - Extracting OBIS-addressed data objects from telegram data lines
 * - Parsing register values (numeric values with optional units)
 * - Parsing timestamp values
 *
 * @note This implementation focuses on telegram parsing for modes A-D.
 *       Mode E (HDLC-based) is not supported.
 *
 * @see https://en.wikipedia.org/wiki/IEC_62056#IEC_62056-21
 *
 * @{
 *
 * @file
 * @brief       IEC 62056-21 direct local data exchange API
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include <sys/types.h>
#include <time.h>

#include "iec62056/obis.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Data object representing an address and value combination from a
 *        telegram
 *
 * The @ref value_index indicates the index of the value when an address has
 * multiple values associated.
 */
typedef struct {
    iec62056_obis_t obis;   /**< Address of the dataset */
    const char *value;      /**< Pointer to the raw value inside the telegram,
                                 not null terminated */
    size_t value_length;    /**< Length in number of character of the value */
    unsigned value_index;   /**< Index of the value for the dataset */
} iec62056_21_dataset_t;

/**
 * @brief Check if a dataset contains a value
 *
 * @param[in] dataset       The dataset to check
 *
 * @retval true if the dataset contains a value
 * @retval false if the dataset only contains an address
 */
static inline bool iec62056_21_dataset_is_without_value(const iec62056_21_dataset_t *dataset)
{
    return dataset->value == NULL;
}

/**
 * @brief Checks if the value of the dataset matches the register representation
 *
 * @note Simple integer values also match this format.
 *
 * @param[in] dataset       The dataset to check
 *
 * @retval true if the dataset value matches the register format
 * @retval false if the dataset value does not match the register format
 */
bool iec62056_21_dataset_is_register(const iec62056_21_dataset_t *dataset);

/**
 * @brief Checks if the value of the dataset matches a timestamp
 *
 * @param[in] dataset       The dataset to check
 *
 * @retval true if the dataset value matches the time format
 * @retval false if the dataset value does not match the time format
 */
bool iec62056_21_dataset_is_timestamp(const iec62056_21_dataset_t *dataset);

/**
 * @brief Parse the numerical part of a register value to a decimal fraction.
 *
 * Value is parsed to @p mantissa * 10 ^ @p scaling
 *
 * This function does not use floating point operations
 *
 * @param[in]  dataset      The dataset to parse
 * @param[out] mantissa     Resulting mantissa
 * @param[out] scaling      Resulting scaling factor
 *
 * @retval 0 on success
 * @retval <0 when parsing failed
 */
int iec62056_21_dataset_parse_register_to_decfrac(const iec62056_21_dataset_t *dataset,
                                                  int64_t *mantissa, int64_t *scaling);

/**
 * @brief Parse the numerical part of a register value to a floating point.
 *
 * @param[in]  dataset      The dataset to parse
 * @param[out] value        Parsed value
 *
 * @retval 0 on success
 * @retval <0 when parsing failed
 */
int iec62056_21_dataset_parse_register_to_float(const iec62056_21_dataset_t *dataset,
                                                float *value);

/**
 * @brief Parse the numerical part of a register value to a floating point double.
 *
 * @param[in]  dataset      The dataset to parse
 * @param[out] value        Parsed value
 *
 * @retval 0 on success
 * @retval <0 when parsing failed
 */
int iec62056_21_dataset_parse_register_to_double(const iec62056_21_dataset_t *dataset,
                                                 double *value);

/**
 * @brief Retrieve the unit of a register value and copy it into the provided
 *        buffer. Includes a null terminator in the output buffer.
 *
 * @param[in]  dataset                  The dataset to parse the unit from
 * @param[out] register_unit            Char buffer to copy the register unit in to
 * @param[in]  register_unit_max_length Size of the buffer to copy into
 *
 * @retval >0 number of copied bytes including the null terminator
 * @retval 0 if there is no unit included in the register
 * @retval <0 when parsing failed
 */
int iec62056_21_dataset_parse_register_unit(const iec62056_21_dataset_t *dataset,
                                            char *register_unit, size_t register_unit_max_length);

/**
 * @brief Retrieve a timestamp type value from a dataset
 *
 * @param[in]  dataset      The dataset to parse the timestamp from
 * @param[out] time         The time struct to write the time into
 *
 * @retval 0 on success
 * @retval <0 when parsing failed
 */
int iec62056_21_dataset_parse_timestamp(const iec62056_21_dataset_t *dataset, struct tm *time);

/**
 * @brief Callback used during parsing of the telegram, called for every
 *        address and value of the address
 *
 * For every address/value combination, the callback is first called with only
 * the address and the value set to @c NULL. This can be used to filter
 * specific address values, returning a negative value will let the parser skip
 * the values associated with the address and move to the next dataline.
 *
 * @param[in,out] arg       Context argument supplied to @ref iec62056_21_parser_init
 * @param[in]     dataset   Dataset parsed by @ref iec62056_21_parse_telegram
 *
 * @retval >=0 to continue parsing normally
 * @retval negative to skip the values of the dataline
 */
typedef int (*iec62056_21_callback_t)(void *arg, const iec62056_21_dataset_t *dataset);

/**
 * @brief IEC62056-21 telegram parser
 */
typedef struct {
    const char *telegram;               /**< Pointer to the telegram message */
    size_t telegram_length;             /**< Length of the telegram */
    iec62056_21_callback_t callback;    /**< Callback for reporting parsed telegram objects */
    void *callback_arg;                 /**< Extra argument for the callback */
} iec62056_21_parser_t;

/**
 * @brief Initialize an iec62056-21 parser object
 *
 * @param[out] parser       The parser context to initialize
 * @param[in]  callback     The callback to call for every parsed dataset
 * @param[in]  callback_arg The extra argument to pass to every call to @p callback
 */
static inline void iec62056_21_parser_init(iec62056_21_parser_t *parser,
                                           iec62056_21_callback_t callback, void *callback_arg)
{
    memset(parser, 0, sizeof(iec62056_21_parser_t));
    parser->callback = callback;
    parser->callback_arg = callback_arg;
}

/**
 * @brief Set the telegram to be parsed.
 *
 * The telegram does not have to be null terminated
 *
 * @param[in,out] parser            The parser to set the telegram for
 * @param[in]     telegram          The telegram to parse
 * @param[in]     telegram_length   The length in bytes of the telegram
 */
static inline void iec62056_21_parser_set_telegram(iec62056_21_parser_t *parser,
                                                   const char *telegram, size_t telegram_length)
{
    parser->telegram = telegram;
    parser->telegram_length = telegram_length;
}

/**
 * @brief Get the identifier header from the telegram. The identifier starts
 *        after the forward slash marking the beginning of the telegram
 *
 * @param[in]  parser   The parser with configured telegram to get the header from
 * @param[out] header   Returned pointer to the start of the header
 *
 * @retval The length of the header
 * @retval <0 when parsing the telegram failed
 */
ssize_t iec62056_21_get_header(const iec62056_21_parser_t *parser, const char **header);

/**
 * @brief Parse the datalines contained in the telegram and call the configured
 *        callback for every address and dataset in the telegram
 *
 * @param[in] parser    The parser with configured telegram to parse
 *
 * @retval 0 on success
 * @retval <0 when parsing the telegram failed at some point.
 */
int iec62056_21_parse_telegram(const iec62056_21_parser_t *parser);

#ifdef __cplusplus
}
#endif
/** @} */
