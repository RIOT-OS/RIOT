/*
 * Copyright (C) 2016 Freie Universität Berlin
 * Copyright (C) 2016 Lennart Dührsen <lennart.duehrsen@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup sys_senml SenML
 * @ingroup  sys
 * @{
 *
 * @file
 * @brief Reference implementation of the Media Types for Sensor Measurement Lists (SenML)
 *
 * SenML is a specification that defines media types for representing simple sensor measurements
 * and device parameters in the Sensor Measurement Lists (SenML).  Representations are defined in
 * JavaScript Object Notation (JSON), Concise Binary Object Representation (CBOR), eXtensible
 * Markup Language (XML), and Efficient XML Interchange (EXI), which share the common SenML data
 * model. A simple sensor, such as a temperature sensor, could use this media type in protocols
 * such as HTTP or CoAP to transport the measurements of the sensor or to be configured.
 *
 * @see [Media Types for Sensor Measurement Lists (SenML)](https://tools.ietf.org/html/draft-ietf-core-senml-04)
 *
 * @author      Lennart Dührsen <lennart.duehrsen@fu-berlin.de>
 */

#ifndef SENML_H
#define SENML_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define SENML_SUPPORTED_VERSION (5)   /**< The highest SenML version this implementation supports */

#define SJ_VERSION       "bver" /**< Key for the version attribute in JSON documents       */
#define SJ_BASE_NAME     "bn"   /**< Key for the base name attribute in JSON documents     */
#define SJ_BASE_TIME     "bt"   /**< Key for the base time attribute in JSON documents     */
#define SJ_BASE_UNIT     "bu"   /**< Key for the base unit attribute in JSON documents     */
#define SJ_BASE_VALUE    "bv"   /**< Key for the base value attribute in JSON documents    */
#define SJ_BASE_SUM      "bs"   /**< Key for the base sum attribute in JSON documents      */
#define SJ_NAME          "n"    /**< Key for the name attribute in JSON documents          */
#define SJ_UNIT          "u"    /**< Key for the unit attribute in JSON documents          */
#define SJ_VALUE         "v"    /**< Key for the value attribute in JSON documents         */
#define SJ_STRING_VALUE  "vs"   /**< Key for the string value attribute in JSON documents  */
#define SJ_BOOL_VALUE    "vb"   /**< Key for the boolean value attribute in JSON documents */
#define SJ_VALUE_SUM     "s"    /**< Key for the value sum attribute in JSON documents     */
#define SJ_TIME          "t"    /**< Key for the time attribute in JSON documents          */
#define SJ_UPDATE_TIME   "ut"   /**< Key for the update time attribute in JSON documents   */
#define SJ_DATA_VALUE    "vd"   /**< Key for the data value attribute in JSON documents    */
#define SJ_LINK          "l"    /**< Key for the link attribute in JSON documents          */

#define SC_VERSION       (-1)   /**< Key for the version attribute in CBOR documents       */
#define SC_BASE_NAME     (-2)   /**< Key for the base name attribute in CBOR documents     */
#define SC_BASE_TIME     (-3)   /**< Key for the base time attribute in CBOR documents     */
#define SC_BASE_UNIT     (-4)   /**< Key for the base unit attribute in CBOR documents     */
#define SC_BASE_VALUE    (-5)   /**< Key for the base value attribute in CBOR documents    */
#define SC_BASE_SUM      (-6)   /**< Key for the base sum attribute in CBOR documents      */
#define SC_NAME          (0)    /**< Key for the name attribute in CBOR documents          */
#define SC_UNIT          (1)    /**< Key for the unit attribute in CBOR documents          */
#define SC_VALUE         (2)    /**< Key for the value attribute in CBOR documents         */
#define SC_STRING_VALUE  (3)    /**< Key for the string value attribute in CBOR documents  */
#define SC_BOOL_VALUE    (4)    /**< Key for the boolean value attribute in CBOR documents */
#define SC_VALUE_SUM     (5)    /**< Key for the value sum attribute in CBOR documents     */
#define SC_TIME          (6)    /**< Key for the time attribute in CBOR documents          */
#define SC_UPDATE_TIME   (7)    /**< Key for the update time attribute in CBOR documents   */
#define SC_DATA_VALUE    (8)    /**< Key for the data value attribute in CBOR documents    */
#define SC_LINK          (9)    /**< Key for the link attribute in CBOR documents          */

/**
 * @brief The different data types SenML supports
 */
typedef enum {
    SENML_TYPE_UNDEF  = 0,  /**< Placeholder for when no value is provided */
    SENML_TYPE_FLOAT  = 1,  /**< Indicates a float value                   */
    SENML_TYPE_STRING = 2,  /**< Indicates a string value                  */
    SENML_TYPE_BOOL   = 3,  /**< Indicates a boolean value                 */
    SENML_TYPE_BINARY = 4   /**< Indicates a binary (data) value           */
} senml_value_type_t;

/**
 * @brief struct that contains base information which applies to all subsequent entries
 */
typedef struct {
    uint8_t             version;         /**< SenML version of this pack                            */
    char               *base_name;       /**< Concatenated with name attribute gives sensor's UID   */
    double              base_time;       /**< Update time values are relative to this time          */
    char               *base_unit;       /**< When all values have the same unit, set this field    */
    double              base_value;      /**< If not zero, add this to (float) value in each record */
    double              base_sum;        /**< If not zero, add this to sum in each record           */
} senml_base_info_t;

/**
 * @brief struct that contains the values of a SenML record
 */
typedef struct {
    char              *name;        /**< Sensor's name, will be appended to base name if provided */
    char              *unit;        /**< Unit of the measurement                                  */
    char              *link;        /**< Link to additional information (must be escaped!)        */
    double             time;        /**< Time (seconds since unix epoch) of the measurement       */
    double             update_time; /**< Time before sensor provides an updated measurement       */
    double             value_sum;   /**< Integrated sum of the values over time                   */
    senml_value_type_t value_type;  /**< Indicates which type the value is of                     */
    union {
        double            f;  /**< A float value                              */
        char             *s;  /**< A string value                             */
        bool              b;  /**< A boolean value                            */
        char             *d;  /**< A data value (binary data, base64 encoded) */
    } value;
} senml_record_t;

/**
 * @brief struct that holds a SenML pack (optional base info and 1..n records)
 */
typedef struct {
    senml_base_info_t  *base_info;  /**< Pointer to the base info, may be NULL           */
    senml_record_t     *records;    /**< Pointer to the beginning of an array of records */
    size_t              num;        /**< Number of records in the array                  */
} senml_pack_t;

/**
 * @brief Decodes a SenML document in JSON format
 *
 * @param[in]  input The JSON document containing the SenML pack
 * @param[out] pack  The @ref senml_pack_t structure in which the
 *                   records and base info will be stored.
 *
 * @return 0 on success, -1 if @p input is not a valid SenML document, or a
 *         positive number indicating by how much the number of records in
 *         @p input exceeds `pack->num`
 */
int senml_decode_json_s(char *input, senml_pack_t *pack);

/**
 * @brief Creates a SenML document in JSON format from a SenML pack
 *
 * @param[in]  pack   The pack that contains the optional base info and the records
 * @param[out] output The resulting JSON document
 * @param[in]  len    The length of @p output in bytes
 *
 * @return 0 on success, or -1 when @p output is not big enough
 */
int senml_encode_json_s(const senml_pack_t *pack, char *output, size_t len);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* SENML_H */

/** @} */
