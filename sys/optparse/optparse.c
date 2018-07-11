/*
 * optparse.c
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
 * @ingroup  sys_optparse
 * @{
 * @file
 *
 * @brief   Command line parser implementation.
 * @author  Juan Carrano <j.carrano@fu-berlin.de>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "optparse.h"

#define TERM '\0'   /**< String terminator character */
#define OPT '-'     /**< The character that marks an option */

#define P_ERR(...) fprintf(stderr, __VA_ARGS__)
#define STR_ERR(s) fputs(s, stderr)

#ifndef DEVELHELP

#define P_DEBUG(...)
#define LOAD_VAR(var, value) do { if ((var) != NULL) { *(var) = value; } } while (0)

#else /* DEVELHELP */

#define P_DEBUG P_ERR
#define LOAD_VAR(var, value) do { if ((var) != NULL) { *(var) = value; } \
                                  else { P_DEBUG("Warning! NULL pointer:" #var "%c\n", ' '); } } while (0)

#endif /*DEVELHELP */

#define HELP_STREAM stdout /* TODO: remove this */

/**
 * Like fputs, but the string can be null.
 */
static int safe_fputs(const char *s, FILE *stream)
{
    return (s != NULL) ? fputs(s, stream) : 0;
}

/**
 * Like fputc, but nothing is printed if the character is the terminator.
 */
static int safe_fputc(int c, FILE *stream)
{
    return (c != TERM) ? fputc(c, stream) : 0;
}

#if (_XOPEN_SOURCE >= 500) || (_POSIX_C_SOURCE > 200809L)
#define my_strdup strdup
#else

char *my_strdup(const char *s)
{
    char *dup;
    int len = strlen(s) + 1;

    if ((dup = malloc(len)) != NULL) {
        memcpy(dup, s, len);
    }

    return dup;
}

#endif /* no posix source */

#define _MSK(x) (1 << (x))
const uint16_t need_value_mask = _MSK(OPTPARSE_IGNORE)
                                 | _MSK(OPTPARSE_CUSTOM_ACTION) | _MSK(OPTPARSE_INT)
                                 | _MSK(OPTPARSE_DOUBLE) | _MSK(OPTPARSE_FLOAT)
                                 | _MSK(OPTPARSE_STR) | _MSK(OPTPARSE_STR_NOCOPY);

#define NEEDS_VALUE(rule) (!!(_MSK((rule)->action) & need_value_mask))

/**
 * If the string stri starts with a dash, remove it and return a string to
 * the part after the dash.
 *
 * If it does not start with a dash, returns null.
 */
static const char *strip_dash(const char stri[])
{
    const char *ret = NULL;

    if (stri[0] == OPT) {
        ret = &stri[1];
    }
    return ret;
}

/**
 * Return true if the string is not null and not empty.
 */
static bool str_notempty(const char *str)
{
    return str != NULL && str[0] != TERM;
}

/* TODO: make help stream into an argument */
static void do_help(const opt_conf_t *config)
{
    int rule_i;

    safe_fputs(config->helpstr, HELP_STREAM);
    safe_fputc('\n', HELP_STREAM);

    for (rule_i = 0; rule_i < config->n_rules; rule_i++) {
        safe_fputc('-', HELP_STREAM);
        safe_fputc(config->rules[rule_i].short_id, HELP_STREAM);
        safe_fputs("\t--", HELP_STREAM);
        safe_fputs(config->rules[rule_i].long_id, HELP_STREAM);
        safe_fputc('\t', HELP_STREAM);
        safe_fputs(config->rules[rule_i].desc, HELP_STREAM);
        safe_fputc('\n', HELP_STREAM);
    }
}

/**
 * Execute the action associated with an argument.
 *
 * key is only used for custom commands.
 * value is only used for commands that need it.
 * This assumes key and value are not null if they should not be.
 *
 * @return  An exit code from OPTPARSE_RESULT.
 */
static int do_action(opt_rule_t *rule, const char *key, const char *value)
{
    int ret = OPTPARSE_OK;
    char *copied_str = NULL;
    const char *custom_message = NULL;

    switch (rule->action) {
        case OPTPARSE_IGNORE: case OPTPARSE_IGNORE_SWITCH:
            break;
        case OPTPARSE_INT:
            LOAD_VAR(rule->data.d_int, strtol(value, NULL, 0));
            break;
        case OPTPARSE_DOUBLE:
            LOAD_VAR(rule->data.d_double, strtod(value, NULL));
            break;
        case OPTPARSE_FLOAT:
            LOAD_VAR(rule->data.d_float, strtof(value, NULL));
            break;
        case OPTPARSE_STR_NOCOPY:
            LOAD_VAR(rule->data.d_cstr, value);
            break;
        case OPTPARSE_SET_BOOL:
            LOAD_VAR(rule->data.d_bool, true);
            break;
        case OPTPARSE_UNSET_BOOL:
            LOAD_VAR(rule->data.d_bool, false);
            break;
        case OPTPARSE_COUNT:
            LOAD_VAR(rule->data.d_int, *rule->data.d_int + 1);
            break;
        case OPTPARSE_STR:
            if (rule->data.d_str != NULL && (copied_str = my_strdup(value)) != NULL) {
                *rule->data.d_str = copied_str;
            }
            else {
                P_DEBUG("OPTPARSE_STR failed: d_str: %p,", (void *)rule->data.d_str);
                P_DEBUG(" copied_str %p\n", copied_str);
                if (rule->data.d_str == NULL) {
                    ret = -OPTPARSE_BADCONFIG;
                }
                else {
                    ret = -OPTPARSE_NOMEM;
                }
            }
            break;
        case OPTPARSE_DO_HELP:
            P_DEBUG("do_action found OPTPARSE_DO_HELP\n");
            break; /*this is a meta-action and has to be implemented in generic_parser*/
        case OPTPARSE_CUSTOM_ACTION:
            ret = rule->data.d_custom.callback(key, value,
                                               rule->data.d_custom.data, &custom_message);
            if (custom_message != NULL) {
                STR_ERR(custom_message);
            }
            break;
        default:
            P_DEBUG("Unknown action: %d\n", rule->action);
            ret = -OPTPARSE_BADCONFIG;
            break;
    }

    return ret;
}

/**
 * Find a rule with the given short id or long id.
 *
 * A short id of 0 never matches. A NULL long id never matches.
 */
static opt_rule_t *find_rule(opt_conf_t *config, const char *long_id,
                             char short_id)
{
    int rule_i;

    for (rule_i = 0; rule_i < config->n_rules; rule_i++) {
        if ((short_id
             && config->rules[rule_i].short_id == short_id)
            || ((long_id != NULL) && (config->rules[rule_i].long_id != NULL)
                && strcmp(long_id, config->rules[rule_i].long_id) == 0)) {
            return config->rules + rule_i;
        }
    }

    return NULL;
}

int optparse_cmd(opt_conf_t *config, int argc, const char * const argv[])
{
    int error = 0, i, no_more_options = 0;
    /* Used for handling combined switches like -axf (equivalent to -a -x -f)
     * Instead of advancing argv, we keep reading from the string*/
    const char *pending_opt = NULL;

    i = (config->tune & OPTPARSE_IGNORE_ARGV0) ? 1 : 0;

    while (error >= OPTPARSE_OK && i < argc) {
        const char *key, *value;

        if (!no_more_options
            && ((pending_opt != NULL)
                || str_notempty(key = strip_dash(argv[i])))) {

            bool is_long;
            opt_rule_t *curr_rule;

            if (pending_opt == NULL) {
                const char *tmp_key;
                is_long = (tmp_key = strip_dash(key)) != NULL;

                if (is_long) {
                    key = tmp_key;
                }

                if (is_long && tmp_key[0] == TERM) { /* we read a "--" */
                    no_more_options = 1;
                    goto parse_loop_end;
                }
            }
            else { /* pending_opt != NULL, we are reading combined switches */
                is_long = false;
                key = pending_opt;
                pending_opt = NULL; /* We reset this because we need to check again
                                       if the current option is a switch*/
            }

            curr_rule = find_rule(config, is_long ? key : NULL,
                                  (!is_long) ? key[0] : 0);

            if (curr_rule != NULL) {
                if (curr_rule->action == OPTPARSE_DO_HELP) {
                    do_help(config);
                    error = -OPTPARSE_REQHELP; /* BYE! <===========> */
                }
                else if (NEEDS_VALUE(curr_rule)) {
                    if (!is_long && key[1] != TERM) {
                        /* This allows one to write the option value like -d12.6 */
                        value = &key[1];
                    }
                    else if (i < (argc - 1)) {
                        /* "i" is only incremented here and at the end of the loop */
                        value = argv[++i];
                    }
                    else {
                        (is_long) ?
                        P_ERR("Option is missing value: %s\n", key)
                        : P_ERR("Option is missing value: %c\n", key[0]);
                        error = -OPTPARSE_BADSYNTAX; /* BYE! <===========> */
                    }
                }
                else {     /* Handle switches (no arguments) */
                    value = NULL;
                    if (!is_long && key[1] != TERM) {
                        pending_opt = &key[1];
                    }
                }

                if (error >= OPTPARSE_OK) {
                    error = do_action(curr_rule, key, value); /* BYE???? <==> */
                }
            }
            else {
                P_ERR("Unknown option: %s\n", key);
                error = -OPTPARSE_BADSYNTAX;
            }
        }
        else if (config->arg_parser != NULL) {
            /* BYE???? <===========> */
            error = config->arg_parser(i, argv[i], config->arg_parser_data);
        }
        else {
            P_ERR("Argument %s missing key\n", argv[i] );
            if (config->tune & OPTPARSE_NULL_ARGPARSER) {
                error = -OPTPARSE_BADSYNTAX; /* BYE! <===========> */
            }
        }
parse_loop_end:
        if (pending_opt == NULL) {
            i++; /* "i" is only incremented here and above */
        }
    }

    return error;
}

void opt_conf_init(opt_conf_t *conf,
                   opt_rule_t *rules, size_t n_rules, char *helpstr,
                   optparse_tune tune, int (*arg_parser)(int, const char *, void *),
                   void *arg_parser_data)
{
    conf->helpstr = helpstr;
    conf->n_rules = n_rules;
    conf->rules = rules;
    conf->tune = tune;
    conf->arg_parser = arg_parser;
    conf->arg_parser_data = arg_parser_data;
}

void set_parse_meta(opt_rule_t *rule, char short_id, const char *long_id,
                    const char *desc)
{
    rule->short_id = short_id;
    rule->long_id = long_id;
    rule->desc = desc;
}

#define SET_ACTION(rule, action_) (rule)->action = (action_)
#define MK_SETTER1(name, action, type, var) \
    void name(opt_rule_t * rule, type * data) { \
        SET_ACTION(rule, action); \
        rule->data.var = data; \
    }

#define MK_SETTER0(name, action) \
    void name(opt_rule_t * rule) { \
        SET_ACTION(rule, action); \
    }

void set_parse_custom(opt_rule_t *rule,
                      int (*callback)(const char *, const char *, void *, const char **), void *data)
{
    SET_ACTION(rule, OPTPARSE_CUSTOM_ACTION);
    rule->data.d_custom.callback = callback;
    rule->data.d_custom.data = data;
}

MK_SETTER0(set_parse_ignore, OPTPARSE_IGNORE)
MK_SETTER0(set_parse_ignore_sw, OPTPARSE_IGNORE_SWITCH)
MK_SETTER0(set_parse_help, OPTPARSE_DO_HELP)

MK_SETTER1(set_parse_int, OPTPARSE_INT, int, d_int)
MK_SETTER1(set_parse_double, OPTPARSE_DOUBLE, double, d_double)
MK_SETTER1(set_parse_float, OPTPARSE_FLOAT, float, d_float)
MK_SETTER1(set_parse_bool, OPTPARSE_SET_BOOL, bool, d_bool)
MK_SETTER1(set_parse_bool_unset, OPTPARSE_UNSET_BOOL, bool, d_bool)
MK_SETTER1(set_parse_count, OPTPARSE_COUNT, int, d_int)
MK_SETTER1(set_parse_str, OPTPARSE_STR, char *, d_str)
MK_SETTER1(set_parse_str_nocopy, OPTPARSE_STR_NOCOPY, const char *, d_cstr)

/** @} */
