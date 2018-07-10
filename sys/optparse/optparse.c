/*
 *      optparse.c
 *
 *      Copyright 2010 Juan I Carrano <juan@carrano.com.ar>
 *		Copyright 2018 Freie Universität Berlin
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "../optparse.h"

#define TERM '\0'
#define ERROR 0
#define OPT '-'

#define P_ERR(...) fprintf(stderr, __VA_ARGS__)
#define STR_ERR(s) fputs(s, stderr)

#ifndef DEBUG

#define P_DEBUG(...)
#define LOAD_VAR(var, value) do { if ((var) != NULL) { *(var) = value; } } while (0)

#else /* DEBUG */

#define P_DEBUG P_ERR
#define LOAD_VAR(var, value) do { if ((var) != NULL) { *(var) = value; } \
                                  else { P_DEBUG("Warning! NULL pointer:" #var "%c\n", ' '); } } while (0)

#endif /*DEBUG */

#define HELP_STREAM stdout /* TODO: remove this */

#define SAFE_FPUTS(str, stream) (((str) != NULL) ? fputs(str, stream) : 0)
#define SAFE_FPUTC(c, stream) ((c != TERM) ? fputc(c, stream) : 0)

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
static char *strip_dash(char stri[])
{
    char *ret = NULL;

    if (stri[0] == OPT) {
        ret = &stri[1];
    }
    return ret;
}

/**
 * Return true if the string is not null and not empty.
 */
static bool str_notempty(char *str)
{
    return str != NULL && str[0] != TERM;
}

/* TODO: make help stream into an argument */
static void do_help(const struct opt_conf *config)
{
    int rule_i;

    SAFE_FPUTS(config->helpstr, HELP_STREAM);
    SAFE_FPUTC('\n', HELP_STREAM);
    for (rule_i = 0; rule_i < config->n_rules; rule_i++) {
        SAFE_FPUTC('-', HELP_STREAM);
        SAFE_FPUTC(config->rules[rule_i].short_id, HELP_STREAM);
        SAFE_FPUTS("\t--", HELP_STREAM);
        SAFE_FPUTS(config->rules[rule_i].long_id, HELP_STREAM);
        SAFE_FPUTC('\t', HELP_STREAM);
        SAFE_FPUTS(config->rules[rule_i].desc, HELP_STREAM);
        SAFE_FPUTC('\n', HELP_STREAM);
    }
}

/**
 * Execute the action associated with an argument.
 *
 * key is only used for custom commands.
 * value is only used for commands that need it.
 * This assumes key and value are not null if they should not be.
 *
 * @return	An exit code from OPTPARSE_RESULT.
 */
static int do_action(struct opt_rule *rule, char *key, char *value)
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
            LOAD_VAR(rule->data.d_str, value);
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
static struct opt_rule *find_rule(struct opt_conf *config, char *long_id,
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

int optparse_cmd(struct opt_conf *config, int argc, char *argv[])
{
    int error = 0, i, no_more_options = 0;
    /* Used for handling combined switches like -axf (equivalent to -a -x -f)
     * Instead of advancing argv, we keep reading from the string*/
    char *pending_opt = NULL;

    i = (config->tune & OPTPARSE_IGNORE_ARGV0) ? 1 : 0;

    while (error >= OPTPARSE_OK && i < argc) {
        char *key, *value;

        if (!no_more_options
            && ((pending_opt != NULL)
                || str_notempty(key = strip_dash(argv[i])))) {

            bool is_long;
            struct opt_rule *curr_rule;

            if (pending_opt == NULL) {
                char *tmp_key;
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

/* SETTERS : Should not be fed null arguments*/

void opt_conf_init(struct opt_conf *conf,
                   struct opt_rule *rules, size_t n_rules, char *helpstr,
                   optparse_tune tune, int (*arg_parser)(int, char *, void *),
                   void *arg_parser_data)
{
    conf->helpstr = helpstr;
    conf->n_rules = n_rules;
    conf->rules = rules;
    conf->tune = tune;
    conf->arg_parser = arg_parser;
    conf->arg_parser_data = arg_parser_data;
}

void set_parse_meta(struct opt_rule *rule, char short_id, const char *long_id,
                    const char *desc)
{
    rule->short_id = short_id;
    rule->long_id = long_id;
    rule->desc = desc;
}

#define SET_ACTION(rule, action_) (rule)->action = (action_)
#define MK_SETTER1(name, action, type, var) \
    void name(struct opt_rule *rule, type *data) { \
        SET_ACTION(rule, action); \
        rule->data.var = data; \
    }

#define MK_SETTER0(name, action) \
    void name(struct opt_rule *rule) { \
        SET_ACTION(rule, action); \
    }

void set_parse_custom(struct opt_rule *rule,
                      int (*callback)(char *, char *, void *, const char **), void *data)
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
MK_SETTER1(set_parse_str_nocopy, OPTPARSE_STR_NOCOPY, char *, d_str)
