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
#include <assert.h>

#include "optparse.h"

#define DEVELHELP

#define TERM '\0'   /**< String terminator character */
#define OPT '-'     /**< The character that marks an option */

#define P_ERR(...) fprintf(stderr, __VA_ARGS__)
#define STR_ERR(s) fputs(s, stderr)

#ifndef DEVELHELP

#define P_DEBUG(...)

#else /* DEVELHELP */

#define P_DEBUG P_ERR

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
                                 | _MSK(OPTPARSE_CUSTOM_ACTION)
                                 | _MSK(OPTPARSE_INT)
                                 | _MSK(OPTPARSE_UINT)
                                 | _MSK(OPTPARSE_FLOAT)
                                 | _MSK(OPTPARSE_STR)
                                 | _MSK(OPTPARSE_STR_NOCOPY);

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

/**
 * Return true if the action indicates a positional argument.
 */
static bool _is_argument(enum OPTPARSE_ACTIONS action)
{
    return action == OPTPARSE_POSITIONAL || action == OPTPARSE_POSITIONAL_OPT;
}

/**
 * Return true if the action indicates an optional argument OR option.
 */
static bool _is_optional(enum OPTPARSE_ACTIONS action)
{
    return action != OPTPARSE_POSITIONAL;
}

static void _option_help(const opt_rule_t *rule)
{
    safe_fputc('-', HELP_STREAM);
    safe_fputc(rule->action_data.option.short_id, HELP_STREAM);
    safe_fputs("\t--", HELP_STREAM);
    safe_fputs(rule->action_data.option.long_id, HELP_STREAM);
    safe_fputc('\t', HELP_STREAM);
    safe_fputs(rule->desc, HELP_STREAM);
    safe_fputc('\n', HELP_STREAM);
}

static void _argument_help(const opt_rule_t *rule)
{
    if (_is_optional(rule->action)) {
        safe_fputc('[', HELP_STREAM);
    }

    safe_fputs(rule->action_data.argument.name, HELP_STREAM);

    if (_is_optional(rule->action)) {
        safe_fputc(']', HELP_STREAM);
    }

    safe_fputc('\t', HELP_STREAM);
    safe_fputs(rule->desc, HELP_STREAM);

    safe_fputc('\n', HELP_STREAM);
}

/* TODO: make help stream into an argument */
/* TODO: Expose this function in the public API */
static void do_help(const opt_conf_t *config)
{
    int rule_i;

    safe_fputs(config->helpstr, HELP_STREAM);
    safe_fputc('\n', HELP_STREAM);

    for (rule_i = 0; rule_i < config->n_rules; rule_i++) {
        const opt_rule_t *rule = config->rules + rule_i;

        if (_is_argument(rule->action)) {
            _argument_help(rule);
        }
        else {
            _option_help(rule);
        }
    }
}

#ifndef NDEBUG
/**
 * Check that there are no optional arguments before non optional ones.
 */
static bool sanity_check(const opt_conf_t *config)
{
   int rule_i;
   bool found_optional = false;

    for (rule_i = 0; rule_i < config->n_rules; rule_i++) {
        bool is_positional = _is_argument(config->rules[rule_i].action);
        bool is_optional = _is_optional(config->rules[rule_i].action);

        if (!is_positional)
            continue;

        if (found_optional && !is_optional)
            return false;

        found_optional = found_optional || !is_optional;
    }

    return true;
}
#endif

static int do_user_callback(const opt_rule_t *rule, opt_data_t *dest,
                            int positional_idx, const char *value)
{
    union opt_key key;
    struct opt_positionalkey pkey;
    int ret;

    if (_is_argument(rule->action)) {
        pkey.position = positional_idx;
        pkey.name = rule->action_data.argument.name;
        key.argument = &pkey;
    } else {
        key.option = &rule->action_data.option;
    }

    const char *custom_message = NULL;
    ret = rule->default_value._thin_callback(key, value,
                                             dest, &custom_message);
    if (custom_message != NULL) {
        STR_ERR(custom_message);
    }

    return ret;
}

/**
 * Execute the action associated with an argument.
 *
 * positional_idx is only used for custom commands in positional arguments.
 * value is only used for commands that need it.
 * This assumes key and value are not null if they should not be.
 *
 * @return  An exit code from OPTPARSE_RESULT.
 */
static int do_action(const opt_rule_t *rule,
                     opt_data_t *dest,
                     int positional_idx, const char *value)
{
    int ret = OPTPARSE_OK;
    char *end_of_conversion;
    enum OPTPARSE_ACTIONS action = _is_argument(rule->action)?
                                    rule->action_data.argument.pos_action
                                  : rule->action;

    switch (action) {
        case OPTPARSE_IGNORE: case OPTPARSE_IGNORE_SWITCH:
            break;
        case OPTPARSE_INT:
            dest->d_int = strtol(value, &end_of_conversion, 0);
            if (*end_of_conversion != '\0')
                ret = -OPTPARSE_BADSYNTAX;
            break;
        case OPTPARSE_UINT:
            dest->d_uint = strtoul(value, &end_of_conversion, 0);
            if (*end_of_conversion != '\0')
                ret = -OPTPARSE_BADSYNTAX;
            break;
        case OPTPARSE_FLOAT:
            dest->d_float = strtof(value, &end_of_conversion);
            if (*end_of_conversion != '\0')
                ret = -OPTPARSE_BADSYNTAX;
            break;
        case OPTPARSE_STR_NOCOPY:
            dest->d_cstr = value;
            break;
        case OPTPARSE_SET_BOOL:
            dest->d_bool = true;
            break;
        case OPTPARSE_UNSET_BOOL:
            dest->d_bool = false;
            break;
        case OPTPARSE_COUNT:
            dest->d_int++;
            break;
        case OPTPARSE_STR:
            {
                /* avoid memory leak if the option is given multiple times. */
                free(dest->d_str);
                dest->d_str = my_strdup(value);
                if (dest->d_str == NULL) {
                    P_DEBUG("OPTPARSE_STR failed: out of memory\n");
                    ret = -OPTPARSE_NOMEM;
                }
            }
            break;
        case OPTPARSE_DO_HELP:
            P_DEBUG("do_action found OPTPARSE_DO_HELP\n");
            assert(0);
            break; /*this is a meta-action and has to be implemented in generic_parser*/
        case OPTPARSE_CUSTOM_ACTION:
            ret = do_user_callback(rule, dest, positional_idx, value);
            break;
        default:
            P_DEBUG("Unknown action: %d\n", rule->action);
            ret = -OPTPARSE_BADCONFIG;
            break;
    }

    return ret;
}

static bool _match_optionkey(struct opt_optionkey opt_key,
                             const char *long_id, char short_id)
{
    return (short_id && opt_key.short_id == short_id)
           || ((long_id != NULL) && (opt_key.long_id != NULL)
               && strcmp(long_id, opt_key.long_id) == 0);
}

/**
 * Find a rule with the given short id or long id.
 *
 * A short id of 0 never matches. A NULL long id never matches.
 */
static const opt_rule_t *find_opt_rule(const opt_conf_t *config,
                                       const char *long_id,
                                       char short_id)
{
    int rule_i;

    for (rule_i = 0; rule_i < config->n_rules; rule_i++) {
        const opt_rule_t this_rule = config->rules[rule_i];

        if (!_is_argument(this_rule.action)
            && _match_optionkey(this_rule.action_data.option, long_id,
                                short_id)) {
            return config->rules + rule_i;
        }
    }

    return NULL;
}

/**
 * Find the positional argument handler for the arg_n-th position (arg_n counts
 * from zero).
 *
 * If repeat_last is true, if arg_n is greater than or equal to the number of
 * available handler, the last one (if any) is returned.
 *
 * @returns     pointer to the matching rule, or NULL if none is found.
 */
static const opt_rule_t *find_arg_rule(const opt_conf_t *config, int arg_n)
{
    int rule_i;
    int last_handler;
    int handlers_found = 0;
    bool repeat_last = !!(config->tune | OPTPARSE_COLLECT_LAST_POS);

    for (rule_i = 0; rule_i < config->n_rules; rule_i++) {
        const opt_rule_t this_rule = config->rules[rule_i];

        if (_is_argument(this_rule.action)) {
            handlers_found++;
            last_handler = rule_i;
            if (handlers_found > arg_n) {
                return config->rules + rule_i;
            }
        }
    }

    return (repeat_last && handlers_found) ? config->rules + last_handler
           : NULL;
}

/**
 * Get the memory location where the parse result should be stored.
 */
static opt_data_t *get_destination(const opt_conf_t *config,
                                   const opt_rule_t *rule,
                                   opt_data_t *result_base)
{
    return result_base + (rule - config->rules);
}

/**
 * Copy over the default values from the rules array to the result array.
 *
 * For custom actions, the callback will be called with value = NULL.
 *
 * This procedure also counts the number of required positional arguments and
 * returns it in n_required (which must NOT be null).
 *
 * On error, n_required may not be accurate.
 *
 * This procedure ensures guarantees that no result item will have a wild
 * pointer in d_str (i.e, it will point to an allocated block or be NULL.)
 */
static int assign_default(const opt_conf_t *config, opt_data_t *result,
                          int *n_required)
{
    int rule_i;
    int error = 0;
    int positional_idx = 0;
    int _required = 0;

    for (rule_i = 0; !error && rule_i < config->n_rules; rule_i++) {
        enum OPTPARSE_ACTIONS action;
        const opt_rule_t *this_rule = config->rules + rule_i;

        if (!_is_optional(this_rule->action)) {
            _required++;
        }

        if (_is_argument(this_rule->action)) {
            positional_idx++;
            action = this_rule->action_data.argument.pos_action;
        }
        else {
            action = this_rule->action;
        }

        if (config->rules[rule_i].action == OPTPARSE_STR
           && this_rule->default_value.d_str != NULL) {
            result[rule_i].d_str = my_strdup(this_rule->default_value.d_str);
            if (result[rule_i].d_str == NULL) {
                P_DEBUG("initialization failed: out of memory\n");
                error = -OPTPARSE_NOMEM;
            }
        }
        else if (action != OPTPARSE_CUSTOM_ACTION) {
            result[rule_i] = this_rule->default_value;
        }
        else {
            error = do_user_callback(this_rule, result + rule_i,
                                     positional_idx, NULL);
            if (error) {
                P_DEBUG("User cb at index %d failed in init with code %d.\n",
                        rule_i, error);
            }
        }
    }

    /* If we exited early, ensure that we do not leave any wild. We will have
     * to free them later. */
    for (; rule_i < config->n_rules; rule_i++) {
        if (config->rules[rule_i].action == OPTPARSE_STR) {
            result[rule_i].d_str = NULL;
        }
    }

    *n_required = _required;

    return error;
}


void optparse_free_strings(const opt_conf_t *config, opt_data_t *result)
{
    int rule_i;

    for (rule_i = 0; rule_i < config->n_rules; rule_i++) {
        if (config->rules[rule_i].action == OPTPARSE_STR) {
            free(result[rule_i].d_str);
            result[rule_i].d_str = NULL;
        }
    }
}

int optparse_cmd(const opt_conf_t *config,
                 opt_data_t *result,
                 int argc, const char * const argv[])
{
    int error = 0, i, no_more_options = 0;
    /* Index of the next positional argument */
    int positional_idx = 0;
    int n_required;
    /* Used for handling combined switches like -axf (equivalent to -a -x -f)
     * Instead of advancing argv, we keep reading from the string*/
    const char *pending_opt = NULL;

    assert(sanity_check(config));

    i = (config->tune & OPTPARSE_IGNORE_ARGV0) ? 1 : 0;

    error = assign_default(config, result, &n_required);
    if (error) {
        P_ERR("Error initializing default values.\n");
    }

    while (error >= OPTPARSE_OK && i < argc) {
        const char *key, *value;
        const opt_rule_t *curr_rule = NULL;

        if (!no_more_options
            && ((pending_opt != NULL)
                || str_notempty(key = strip_dash(argv[i])))) {

            bool is_long;

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

            curr_rule = find_opt_rule(config, is_long ? key : NULL,
                                      (!is_long) ? key[0] : 0);

            if (curr_rule != NULL) {
                if (curr_rule->action == OPTPARSE_DO_HELP) {
                    do_help(config);
                    error = -OPTPARSE_REQHELP; /* BYE! <===========> */
                }
                else if (NEEDS_VALUE(curr_rule)) {
                    if (!is_long && key[1] != TERM) {
                        /* This allows one to write the option value like -d12.6 */
                        value = key + 1;
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
                        pending_opt = key + 1;
                    }
                }
            }
            else {
                P_ERR("Unknown option: %s\n", key);
                error = -OPTPARSE_BADSYNTAX;
            }
        }
        else {
            curr_rule = find_arg_rule(config, positional_idx);
            value = argv[i];
            positional_idx++;

            if (!positional_idx) {
                P_ERR("Maximum number if arguments (%d) exceeded\n", UCHAR_MAX);
                error = -OPTPARSE_BADSYNTAX;
            }

            if (curr_rule == NULL) {
                P_ERR("Argument %s missing key\n", argv[i] );
                error = -OPTPARSE_BADSYNTAX; /* BYE! <===========> */
            }
        }

        if (error >= OPTPARSE_OK && curr_rule != NULL) {
            error = do_action(curr_rule,
                              get_destination(config, curr_rule, result),
                              positional_idx, value); /* BYE???? <==> */
        }
parse_loop_end:
        if (pending_opt == NULL) {
            i++; /* "i" is only incremented here and above */
        }
    }

    if (error >= OPTPARSE_OK && n_required > positional_idx) {
        P_ERR("%d argument required but only %d given\n", n_required,
              positional_idx);
        error = -OPTPARSE_BADSYNTAX;
    }

    if (error < OPTPARSE_OK) {
        optparse_free_strings(config, result);
    }

    return error >= OPTPARSE_OK? positional_idx : error;
}

/** @} */
