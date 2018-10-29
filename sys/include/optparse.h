/*
 * optparse.h
 *
 * Copyright 2010 Juan I Carrano <juan@carrano.com.ar>
 * Copyright 2018 Freie Universität Berlin
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
 * - Built-in parsers for most common options.
 * - Automatically formats and prints a help string.
 * - Supports custom parsers via callbacks.
 *
 * # Usage
 *
 * Initialize the required ::opt_conf_t and ::opt_rule_t structures and call
 * optparse_cmd().
 *
 * # Terminology
 *
 * option: Optional value. Consists of a key an possibly a value. Each type of
 *         options has a predefined number of arguments it takes, and that may
 *         be either 0 or 1. Example:
 *              -v --number 5 -k hello
 *         Options (key, value): (v, none), (number, 5), (k, hello)
 *
 * switch: An option taking 0 values.
 *
 * argument: A value without a key. For example, in "-c 3 abcd", abcd is an
 *          argument.
 *
 * short-option: Starts with a single dash and consists of a single character.
 *
 * long-option: Starts with two dashes and consists of one or more characters
 *          (i.e. a string).
 *
 *
 * # Tips for defining rules:
 *
 * 1. Make an enum with the name of the parameters:
 *    enum {HEIGHT, WIDTH, VERBOSE, ETC, .... , N_PARAMETERS }
 * 2. Declare `struct opt_rule rules[N_PARAMETERS];`
 * 3. Initialize. This can't be done in the initalization because it would be
 *    necessary to know the values of pointers beforehand.
 *    The standard way would be:
 *      ```
 *      rules[HEIGHT].short_id = 'h';
 *      rules[HEIGHT].long_id = "height";
 *      ... etc ...
 *      ```
 *    Alternatively, if the compiler supports compound literals and tags the
 *    following syntax can be used:
 *      ```
 *      rules[HEIGHT] = (struct optparse){ .short_id = 'a', .long_id= ....etc ....};
 *      ```
 * 4. If none of those options is adequate, initializer functions are provided.
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
    OPTPARSE_IGNORE_SWITCH, /**< Ignore a switch. Takes 0 arguments */
    OPTPARSE_IGNORE,        /**< Ignore a key and its value. Takes 1 argument. */

    OPTPARSE_SET_BOOL,      /**< Set *opt_rule.data.d_bool to true. Takes 0 arguments.*/
    OPTPARSE_UNSET_BOOL,    /**< Set *opt_rule.data.d_bool to false. Takes 0 arguments.*/
    OPTPARSE_COUNT,         /**< Count the number of occurences. Increments data::d_int
                             *  each time the option is found. */

    OPTPARSE_CUSTOM_ACTION, /**< Delegate the action to a callback
                             * int callback(char *key, char *value, void *data)
                             * The callback and the data are placed in
                             * opt_rule::data::d_custom
                             */
    OPTPARSE_INT,           /**< Parse the value as an int and store it in *opt_rule.data.d_int */
    OPTPARSE_FLOAT,         /**< Parse the value as float and store it in *opt_rule.data.d_float */

    OPTPARSE_STR,           /**< Make a copy of the value string with strdup() and place it
                             *  in *opt_rule.data.d_str. The string must be deallocated with free().
                             * It is suggested that the default value for this option is
                             * NULL, so that it is easier to know afterwards if the pointer
                             * should be freed.
                             */

    OPTPARSE_STR_NOCOPY,    /*! Place a pointer to the value string in data::d_cstr.
                             * In contrast with OPTPARSE_STR, this does NOT make a
                             * copy of the string. It should not be used if the
                             * strings will be modified.
                             */

    OPTPARSE_DO_HELP        /**< Print the following string, except if they are NULL:
                             * - opt_conf::helpstr
                             * - For each element of opt_conf::opt_rule:
                             * short_id (tab) long_id (tab) desc
                             *
                             * In addition it causes the parser to exit with code
                             * OPTPARSE_REQHELP.
                             */
};

/**
 * Configuration rules for a single command line option.
 */
typedef struct opt_rule {
    enum OPTPARSE_ACTIONS action;  /**< Parsing type/action. See OPTPARSE_ACTIONS. */
    char short_id;          /**< Short style option name ("-w"). Can be OPTPARSE_NO_SHORT.*/
    const char *long_id;    /**< Long style option name ("--width") . Can be NULL */
    const char *desc;       /**< Help description. Can be NULL */
    union {
        int *d_int;
        bool *d_bool;
        float *d_float;
        char **d_str;              /**< Pointer to a user-defined pointer */
        const char **d_cstr;       /**< Pointer to a user-defined pointer, constant variant */
        /**
         * Callback for user defined actions.
         *
         * Prototype:
         *  int callback(const char *key, const char *value,
         *               void *data, const char **msg)
         *
         * The callback must return a non-negative value to indicate success.
         * The callback can place a message into msg and it will get printed to
         * the error stream (without causing an error).
         */
        struct {
            void *data;
            int (*callback)(const char *, const char *, void *, const char **);
        } d_custom;
    } data; /**< The parse results will be placed in this union depending on action */
} opt_rule_t;

/**
 * Options that control the behavior of the option parser.
 *
 * The bitmasks (without the _b suffix are probably more useful.
 */
enum OPTPARSE_TUNABLES {
    OPTPARSE_IGNORE_ARGV0_b,
    OPTPARSE_NULL_ARGPARSER_b
};

/** Should argv[0] be skipped? */
#define OPTPARSE_IGNORE_ARGV0 (1 << OPTPARSE_IGNORE_ARGV0_b)
/** Should arguments be ignored if arg_parser is NULL? */
#define OPTPARSE_NULL_ARGPARSER (1 << OPTPARSE_NULL_ARGPARSER_b)

typedef uint16_t optparse_tune; /**< Option bitfield */

/**
 * Configuration for the command line parser.
 */
typedef struct opt_conf {
    char *helpstr;          /**< Program's description and general help string. */
    opt_rule_t *rules;      /**< Array of options. */
    int n_rules;            /**< Number of elements in rules. */
    optparse_tune tune;     /**< Option bitfield. */

    /**
     * int arg_parser(int current_index, const char *value, void *userdata)
     *
     * This function will be called each time an value without a key (i.e.
     * an argument) is found. The first parameter (current_index) indicates the
     * position if the argument and is useful for identifying argv[0].
     * arg_parser can be NULL. In that case, finding an argument will generate
     * an error, unless OPTPARSE_NULL_ARGPARSER is set.
     */
    int (*arg_parser)(int, const char *, void *);
    void *arg_parser_data; /**< Callback data for arg_parser. */
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
 * switches and all the remaining command line arguments are sent to
 * arg_parser.
 *
 * A single dash is interpreted as an argument and sent to arg_parser.
 *
 * @return  non-negative on sucess
 * @return  A negative error code from OPTPARSE_RESULT on error.
 */
int optparse_cmd(struct opt_conf *config, int argc, const char * const argv[]);

/**
 * @defgroup sys_optparse_initializers  Optparse initializers
 * @{
 *
 * @brief   Initialize optparse structures.
 *
 * These initializers can be used to safely construct configurations, without
 * the risk of missing a parameter.
 *
 */

/** Set general parser options */
void opt_conf_init(struct opt_conf *conf,
                   struct opt_rule *rules, size_t n_rules,
                   char *helpstr, optparse_tune tune,
                   int (*arg_parser)(int, const char *, void *),
                   void *arg_parser_data);

/** Set common options for a rule */
void set_parse_meta(struct opt_rule *rule, char short_id,
                    const char *long_id, const char *desc);


/** @{ */
void set_parse_ignore(struct opt_rule *rule);
void set_parse_ignore_sw(struct opt_rule *rule);
void set_parse_help(struct opt_rule *rule);

void set_parse_custom(struct opt_rule *rule,
                      int (*callback)(const char *, const char *, void *, const char **),
                      void *data);

void set_parse_int(struct opt_rule *rule, int *data);
void set_parse_bool(struct opt_rule *rule, bool *data);
void set_parse_bool_unset(struct opt_rule *rule, bool *data);
void set_parse_count(struct opt_rule *rule, int *data);
void set_parse_float(struct opt_rule *rule, float *data);
void set_parse_str(struct opt_rule *rule, char **data);
void set_parse_str_nocopy(struct opt_rule *rule, const char **data);
/** @} */

/** @} */

#endif /* OPTPARSE_H */

/** @} */
