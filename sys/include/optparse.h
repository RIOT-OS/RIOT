/*
 * optparse.h
 *
 * Copyright 2010 Juan I Carrano <juan@carrano.com.ar>
 * Copyright 2018-2019 Freie Universität Berlin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/**
 * @defgroup sys_optparse   Command line option parser.
 * @ingroup sys
 * @{
 *
 * This module provides a generic way of describing and parsing command line
 * arguments. Features:
 *
 * - Supports both short form ("-k") and long form ("--key") options.
 * - Supports both mandatory and optional positional arguments.
 * - Built-in parsers for most common options/argument types.
 * - Automatically formats and prints a help string.
 * - Supports custom parsers via callbacks.
 * - The parser specification is designed to be read-only so it can be
 *   placed in the .text section of a binary (and in an MCU, into the FLASH).
 *
 * Limitations:
 *
 * - There is no way to tell that an option was not specified: options have
 *   default value and it is generally not possible to tell this value apart
 *   from a user supplied value.
 * - When using custom actions, a character used as a short option key should
 *   NOT be used as a long option key in another rule or else the user callback
 *   will not be able to tell them apart (may or may not be an issue.)
 *
 * # Usage
 *
 * Initialize the required ::opt_conf_t and ::opt_rule_t structures and call
 * optparse_cmd().
 *
 * # Terminology
 *
 * option: Optional value. Consists of a key an possibly a value. Each type of
 *         options has a predefined number of arguments it takes that may
 *         be either 0 or 1. Example:
 *              -v --number 5 -k hello
 *         Options (key, value): (v, none), (number, 5), (k, hello)
 *         Options are identified by a string (or character) key.
 *
 * switch: An option taking 0 values.
 *
 * (positional) argument: A value without a key. For example, in "-c 3 abcd",
 *         abcd is an argument. Arguments can be either mandatory or optional
 *         and are identified by an integer index. Because of this, optional
 *         arguments must always come AFTER mandatory ones.
 *
 * short-option: Starts with a single dash and consists of a single character.
 *         For example "-k 78", "-c", "-f filename".
 *
 * long-option: Starts with two dashes and consists of one or more characters
 *          (i.e. a string). For example "--echo", "--factor 1.5".
 *
 *
 * # Tips for defining rules:
 *
 * 1. Make an enum with the name of the parameters (options and arguments) and
 *    with a last element marking its lebgth:
 *    enum {HEIGHT, WIDTH, VERBOSE, ETC, .... , N_PARAMETERS }
 * 2. Declare `const opt_rule_t rules[N_PARAMETERS];`
 * 3. Initialize it. If your compiler supports specifying subobject in array and
 *    struct/union initializers, they initializer macros can be used. These
 *    provide a fail-safe way to construct an array (i.e. they ensure that the
 *    correct tags and union elements are set:
 *      ```
 *      const struct opt_rule rules[N_PARAMETERS] = {
 *      [WIDTH] = OPTPARSE_O_INT('w', "width", 640, "Display height in px"),
 *      [HEIGHT] = OPTPARSE_O_INT('h', "height", 480, "Display height in px"),
 *      ... etc ...
 *      }
 *      ```
 * 4. Declare and initialize ::opt_conf_t.
 *
 * @{
 * @file
 * @brief  Command-line parser
 * @author Juan I Carrano <j.carrano@fu-berlin.de>
 * @}
 */

#ifndef OPTPARSE_H
#define OPTPARSE_H

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Used to indicate that an option has no short (i.e. single character) variant.
 */
#define OPTPARSE_NO_SHORT '\0'

/**
 * Error codes used by optparse_cmd.
 *
 * User callbacks should use the same error codes. On error, a negative code
 * is returned.
 */
enum OPTPARSE_RESULT {
    OPTPARSE_OK,            /**< Parsing suceeded */
    OPTPARSE_NOMEM,         /**< Not enough memory. Only OPTPARSE_STR or a
                                 custom parser may cause this error. */
    OPTPARSE_BADSYNTAX,     /**< Command line is wrongly formed */
    OPTPARSE_BADCONFIG,     /**< The parser configuration is invalid. */
    OPTPARSE_REQHELP        /**< The help option was requested. */
};

/**
 * Built-in actions for options.
 */
enum OPTPARSE_ACTIONS {
    OPTPARSE_IGNORE,        /**< Ignore a key and its value. Takes 1 argument. */

    OPTPARSE_CUSTOM_ACTION, /**< Delegate the action to a callback
                             * int _thin_callback(union opt_key key,
                                                  const char *value,
                                                  union opt_data *data,
                                                  const char **msg)
                             */

    // TODO: figure out if this is needed
    //OPTPARSE_CUSTOM_ACTION_FAT, /**< */
    OPTPARSE_UINT,          /**< Parse the value as an unsigned int and store it in opt_data::d_uint */
    OPTPARSE_INT,           /**< Parse the value as an int and store it in opt_data::d_int */
    OPTPARSE_FLOAT,         /**< Parse the value as float and store it in opt_data::d_float */

    OPTPARSE_STR,           /**< Make a copy of the value string with strdup() and place it
                             *  in data::d_str. The string must be deallocated with free().
                             */

    OPTPARSE_STR_NOCOPY,    /*! Place a pointer to the value string in data::d_cstr.
                             * In contrast with OPTPARSE_STR, this does NOT make a
                             * copy of the string. It should not be used if the
                             * strings will be modified.
                             */
    _OPTPARSE_MAX_NEEDS_VALUE_END, /**< Marker for the end of options that need a value.*/
    OPTPARSE_IGNORE_SWITCH=_OPTPARSE_MAX_NEEDS_VALUE_END,
                            /**< Ignore a switch. Takes 0 arguments */
    OPTPARSE_SET_BOOL,      /**< Set opt_data::d_bool to true. Takes 0 arguments.*/
    OPTPARSE_UNSET_BOOL,    /**< Set opt_data::d_bool to false. Takes 0 arguments.*/
    OPTPARSE_COUNT,         /**< Count the number of occurences. Increments data::d_int
                             *  each time the option is found. */

    OPTPARSE_DO_HELP,       /**< Print the following string, except if they are NULL:
                             * - opt_conf::helpstr
                             * - For each element of opt_conf::opt_rule:
                             * short_id (tab) long_id (tab) desc
                             *
                             * In addition it causes the parser to exit with code
                             * OPTPARSE_REQHELP.
                             */
    _OPTPARSE_POSITIONAL_START,
    OPTPARSE_POSITIONAL=_OPTPARSE_POSITIONAL_START,
                                /**< Indicates that the rule is a mandatory
                                 positional argument and not an option. */
    OPTPARSE_POSITIONAL_OPT /**< Indicates that the rule is an optional
                                 positional argument and not an option. */
};

/**
 * Built-in actions for positional arguments.
 *
 * These are a subset of the actions for options.
 */
enum OPTPARSE_POSITIONAL_ACTIONS {
    OPTPARSE_POS_IGNORE = OPTPARSE_IGNORE,
    OPTPARSE_POS_COUNT = OPTPARSE_COUNT,
    OPTPARSE_POS_CUSTOM_ACTION = OPTPARSE_CUSTOM_ACTION,
//    OPTPARSE_POS_CUSTOM_ACTION_FAT = OPTPARSE_CUSTOM_ACTION_FAT,
    OPTPARSE_POS_UINT = OPTPARSE_UINT,
    OPTPARSE_POS_INT = OPTPARSE_INT,
    OPTPARSE_POS_FLOAT = OPTPARSE_FLOAT,
    OPTPARSE_POS_STR = OPTPARSE_STR,
    OPTPARSE_POS_STR_NOCOPY = OPTPARSE_STR_NOCOPY,
};

/** Maximum number of positional arguments supported. */
#define OPTPARSE_MAX_POSITIONAL UCHAR_MAX

/**
 * Identify a positional argument.
 */
struct opt_positionalkey {
    unsigned char position; /**< Position (counting from zero) of this argument.
                                 Note that options do not contribute to this count
                                 Whether argv[0] is counted depends on the status
                                 of OPTPARSE_IGNORE_ARGV0.*/
    const char *name;       /**< User-supplied name of this arguments (from
                                 opt_rule_t::name .*/
};

struct opt_optionkey;

/**
 * Structure to inform user callbacks of the argument or option being
 * parsed.
 */
union opt_key {
    const struct opt_positionalkey *argument;
    const struct opt_optionkey *option;
};

/**
 * Value type for options and arguments.
 */
typedef union opt_data {
    int d_int;
    unsigned int d_uint;
    bool d_bool;
    float d_float;
    char *d_str;           /**< Pointer to a string */
    const char *d_cstr;    /**< Pointer to a string, constant variant */
    void *data;            /**< Generic data for custom parsers. */

    /**
     * Callback for user defined actions.
     *
     * Prototype:
     *  int _thin_callback(union opt_key key,
     *                     const char *value,
     *                     union opt_data *data, const char **msg)
     *
     * When parsing an option, key.option will point to a struct holding the
     * short and long option keys. When parsing a positional argument
     * key.option will point to a opt_positionalkey structure holding the index
     * and the name.
     *
     * The callback must return a non-negative value to indicate success.
     * It should place the result of the conversion in *data.
     *
     * During the initialization procedure, it will be called with a NULL
     * valued data to set the default value. After this, data will never be
     * NULL.
     *
     * The callback can place a message into msg and it will get printed to
     * the error stream (without causing an error if the result value is not
     * negative).
     */
    int (*_thin_callback)(union opt_key, const char *, union opt_data *,
                          const char **);
} opt_data_t;


/**
 * Configuration rules for a single command line option.
 */
typedef struct opt_rule {
    enum OPTPARSE_ACTIONS action;  /**< Parsing type/action. See OPTPARSE_ACTIONS. */

    union {
        /** If the action is OPTPARSE_POSITIONAL or OPTPARSE_POSITIONAL_OPT,
            this should hold the required action. */
        struct {
            enum OPTPARSE_POSITIONAL_ACTIONS pos_action;
            const char *name;
        } argument;

        /** If this rule is for an option (not a positional argument) this
            should hold the option keys.*/
        struct opt_optionkey {
            char short_id;          /**< Short style option name ("-w"). Can be OPTPARSE_NO_SHORT.*/
            const char *long_id;    /**< Long style option name ("--width") . Can be NULL */
        } option;
    } action_data;      /**< Keys for options, action type for positional args*/
    const char *desc;   /**< Help description. Can be NULL */

    opt_data_t default_value; /**< Value for when the option is not given. */
} opt_rule_t;

// /**< The parse results will be placed in this union depending on action */

/**
 * Options that control the behavior of the option parser.
 *
 * The bitmasks (without the _b suffix are probably more useful.
 */
enum OPTPARSE_TUNABLES {
    OPTPARSE_IGNORE_ARGV0_b,
    OPTPARSE_COLLECT_LAST_POS_b,
};

/** Should argv[0] be skipped? */
#define OPTPARSE_IGNORE_ARGV0 (1 << OPTPARSE_IGNORE_ARGV0_b)
/** If this flag is set, the last positional argument rule is applied to all
    extra positional arguments (i.e. it "collects" them all) */
#define OPTPARSE_COLLECT_LAST_POS (1 << OPTPARSE_COLLECT_LAST_POS_b)

typedef uint16_t optparse_tune; /**< Option bitfield */

/**
 * Configuration for the command line parser.
 */
typedef struct opt_conf {
    const char *helpstr;    /**< Program's description and general help string. */
    const opt_rule_t *rules;      /**< Array of options. */
    int n_rules;            /**< Number of elements in rules. */
    optparse_tune tune;     /**< Option bitfield. */
} opt_conf_t;

/**
 * Main interface to the option parser.
 *
 * Short options:
 *
 * A short option that takes a value can be immediately followed by the value
 * in the same argv string. For example "-upeter" would assign "peter" to the
 * "u" option.
 *
 * Short switches can be merged together like "-xj".
 *
 * Dash handling:
 *
 * A double dash (--) indicates the parser that there are no more  options/
 * switches and all the remaining command line arguments are to be interpreted
 * as positional arguments
 *
 * A single dash is interpreted as a positional argument argument.
 *
 * Optional positional arguments:
 *
 * Optional positional arguments MUST follow mandatory arguments. The n-th
 * optional argument cannot be set if the (n-1)th optional argument is not
 * set.
 *
 * @return  Number of positional arguments converted.
 * @return  A negative error code from OPTPARSE_RESULT on error.
 */
int optparse_cmd(const struct opt_conf *config,
                 union opt_data *result,
                 int argc, const char * const argv[]);

/**
 * Free all strings allocated by OPTPARSE_STR and OPTPARSE_POS_STR.
 *
 * Note that on a parsing error all strings are automatically deallocated.
 *
 * This procedure will set all d_str fields to NULL, so it is safe to call
 * it more than once.
 */
void optparse_free_strings(const opt_conf_t *config, opt_data_t *result);

/**
 * @defgroup sys_optparse_initializers  Optparse initializers
 * @{
 *
 * @brief   Initialize optparse structures.
 *
 * These initializers can be used to safely construct configurations, without
 * the risk of missing a parameter.
 */

#define _OPTPARSE_IGNORE_SWITCH_INIT    d_int
#define _OPTPARSE_IGNORE_INIT           d_int
#define _OPTPARSE_SET_BOOL_INIT         d_bool
#define _OPTPARSE_UNSET_BOOL_INIT       d_bool
#define _OPTPARSE_COUNT_INIT            d_int
#define _OPTPARSE_CUSTOM_ACTION_INIT    _thin_callback
#define _OPTPARSE_UINT_INIT             d_uint
#define _OPTPARSE_INT_INIT              d_int
#define _OPTPARSE_FLOAT_INIT            d_float
#define _OPTPARSE_STR_INIT              d_str
#define _OPTPARSE_STR_NOCOPY_INIT       d_cstr
#define _OPTPARSE_DO_HELP_INIT          d_int

/**
 * Declare an option.
 *
 * type must be a member of OPTPARSE_ACTIONS, with the "OPTPARSE_" prefix
 * removed. For example:
 *
 * OPTPARSE_O(SET_BOOL, 's', "set", "Set a flag 's'", false)
 */
#define OPTPARSE_O(type, short_, long_, description, default_value_) \
    {.action=OPTPARSE_##type, .action_data.option.short_id=(short_), \
     .action_data.option.long_id=(long_), .desc=(description), \
     .default_value._OPTPARSE_##type##_INIT = (default_value_)}

#define _OPTPARSE_P(type0, type, name_, description, default_value_) \
    {.action=(type0), .action_data.argument.pos_action = OPTPARSE_POS_##type, \
     .action_data.argument.name=(name_), .desc=(description), \
     .default_value._OPTPARSE_##type##_INIT = (default_value_)}

/**
 * Declare a mandatory positional argument.
 *
 * type must be a member of OPTPARSE_POS_ACTIONS, with the "OPTPARSE_POS" prefix
 * removed. For example:
 *
 * OPTPARSE_P(STR_NOCOPY, "filename", "Output file", "whatever.out")
 */
#define OPTPARSE_P(type, name, description, default_value) \
    _OPTPARSE_P(OPTPARSE_POSITIONAL, type, name, description, default_value)

/**
 * Declare an optional positional argument.
 *
 * Like OPTPARSE_P, but it is not an error if the argument is missing.
 * Remember that optional arguments MUST come after mandatory ones on the
 * opt_conf::rules array.
 */
#define OPTPARSE_P_OPT(type, name, description, default_value) \
    _OPTPARSE_P(OPTPARSE_POSITIONAL_OPT, type, name, description, default_value)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* OPTPARSE_H */

/** @} */


