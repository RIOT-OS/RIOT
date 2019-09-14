/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     nhdp
 * @{
 *
 * @file
 * @brief       Interface Information Base implementation for NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "mutex.h"
#include "timex.h"
#include "xtimer.h"
#include "utlist.h"
#include "kernel_types.h"

#include "rfc5444/rfc5444.h"
#include "rfc5444/rfc5444_iana.h"
#include "rfc5444/rfc5444_writer.h"

#include "iib_table.h"
#include "nhdp_address.h"
#include "nhdp_metric.h"
#include "nhdp_writer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Internal variables */
static mutex_t mtx_iib_access = MUTEX_INIT;
static iib_base_entry_t *iib_base_entry_head = NULL;

#if (NHDP_METRIC == NHDP_LMT_DAT)
static const double const_dat = (((double)DAT_CONSTANT) / DAT_MAXIMUM_LOSS);
#endif

/* Internal function prototypes */
static void rem_link_set_entry(iib_base_entry_t *base_entry, iib_link_set_entry_t *ls_entry);
static void cleanup_link_sets(void);
static iib_link_set_entry_t *add_default_link_set_entry(iib_base_entry_t *base_entry, timex_t *now,
                                                        uint64_t val_time);
static void reset_link_set_entry(iib_link_set_entry_t *ls_entry, timex_t *now, uint64_t val_time);
static iib_link_set_entry_t *update_link_set(iib_base_entry_t *base_entry, nib_entry_t *nb_elt,
                                             timex_t *now, uint64_t val_time,
                                             uint8_t sym, uint8_t lost);
static void release_link_tuple_addresses(iib_link_set_entry_t *ls_entry);

static int update_two_hop_set(iib_base_entry_t *base_entry, iib_link_set_entry_t *ls_entry,
                              timex_t *now, uint64_t val_time);
static int add_two_hop_entry(iib_base_entry_t *base_entry, iib_link_set_entry_t *ls_entry,
                             nhdp_addr_t *th_addr, timex_t *now, uint64_t val_time);
static void rem_two_hop_entry(iib_base_entry_t *base_entry, iib_two_hop_set_entry_t *th_entry);

static void wr_update_ls_status(iib_base_entry_t *base_entry,
                                iib_link_set_entry_t *ls_elt, timex_t *now);
static void update_nb_tuple_symmetry(iib_base_entry_t *base_entry,
                                     iib_link_set_entry_t *ls_entry, timex_t *now);
static void rem_not_heard_nb_tuple(iib_link_set_entry_t *ls_entry, timex_t *now);

static inline timex_t get_max_timex(timex_t time_one, timex_t time_two);
static iib_link_tuple_status_t get_tuple_status(iib_link_set_entry_t *ls_entry, timex_t *now);

#if (NHDP_METRIC == NHDP_LMT_DAT)
static void queue_rem(uint8_t *queue);
static uint16_t queue_sum(uint8_t *queue);
static void dat_metric_refresh(void);
#endif

/*---------------------------------------------------------------------------*
 *                       Interface Information Base API                      *
 *---------------------------------------------------------------------------*/

int iib_register_if(kernel_pid_t pid)
{
    iib_base_entry_t *new_entry = (iib_base_entry_t *) malloc(sizeof(iib_base_entry_t));

    if (!new_entry) {
        /* Insufficient memory */
        return -1;
    }

    new_entry->if_pid = pid;
    new_entry->link_set_head = NULL;
    new_entry->two_hop_set_head = NULL;
    LL_PREPEND(iib_base_entry_head, new_entry);

    return 0;
}

iib_link_set_entry_t *iib_process_hello(kernel_pid_t if_pid, nib_entry_t *nb_elt,
                                        uint64_t validity_time, uint8_t is_sym_nb,
                                        uint8_t is_lost)
{
    iib_base_entry_t *base_elt = NULL;
    iib_link_set_entry_t *ls_entry = NULL;
    timex_t now;

    mutex_lock(&mtx_iib_access);

    /* Remove link tuple addresses that are included in the Removed Addr List */
    cleanup_link_sets();

    LL_FOREACH(iib_base_entry_head, base_elt) {
        /* Find the link set and two hop set for the interface */
        if (base_elt->if_pid == if_pid) {
            break;
        }
    }

    if (base_elt) {
        xtimer_now_timex(&now);

        /* Create a new link tuple for the neighbor that originated the hello */
        ls_entry = update_link_set(base_elt, nb_elt, &now, validity_time, is_sym_nb, is_lost);

        /* Create new two hop tuples for signaled symmetric neighbors */
        if (ls_entry) {
            update_two_hop_set(base_elt, ls_entry, &now, validity_time);
        }
    }

    mutex_unlock(&mtx_iib_access);

    return ls_entry;
}

void iib_fill_wr_addresses(kernel_pid_t if_pid, struct rfc5444_writer *wr)
{
    iib_base_entry_t *base_elt  = NULL;
    iib_link_set_entry_t *ls_elt  = NULL;
    nhdp_addr_entry_t *addr_elt = NULL;
    timex_t now;

    mutex_lock(&mtx_iib_access);

    xtimer_now_timex(&now);

    /* Before adding addresses first update the status of all link tuples */
    iib_update_lt_status(&now);

    /* Add all addresses of Link Tuples of the given interface's Link Set to the current HELLO */
    LL_FOREACH(iib_base_entry_head, base_elt) {
        if (base_elt->if_pid == if_pid) {
            LL_FOREACH(base_elt->link_set_head, ls_elt) {
                if (ls_elt->last_status != IIB_LT_STATUS_PENDING) {
                    /* Exclude addresses from tuples with L_STATUS = PENDING */
                    LL_FOREACH(ls_elt->address_list_head, addr_elt) {
                        if (!NHDP_ADDR_TMP_IN_ANY(addr_elt->address)) {
                            /* Add address to the writers next packet */
                            switch (ls_elt->last_status) {
                                case IIB_LT_STATUS_SYM:
                                    nhdp_writer_add_addr(wr, addr_elt->address,
                                                         RFC5444_ADDRTLV_LINK_STATUS,
                                                         RFC5444_LINKSTATUS_SYMMETRIC,
                                                         rfc5444_metric_encode(ls_elt->metric_in),
                                                         rfc5444_metric_encode(ls_elt->metric_out));
                                    addr_elt->address->in_tmp_table = NHDP_ADDR_TMP_SYM;
                                    break;

                                case IIB_LT_STATUS_HEARD:
                                    nhdp_writer_add_addr(wr, addr_elt->address,
                                                         RFC5444_ADDRTLV_LINK_STATUS,
                                                         RFC5444_LINKSTATUS_HEARD,
                                                         rfc5444_metric_encode(ls_elt->metric_in),
                                                         rfc5444_metric_encode(ls_elt->metric_out));
                                    addr_elt->address->in_tmp_table = NHDP_ADDR_TMP_ANY;
                                    break;

                                case IIB_LT_STATUS_UNKNOWN:
                                    /* Fall through */

                                case IIB_LT_STATUS_LOST:
                                    nhdp_writer_add_addr(wr, addr_elt->address,
                                                         RFC5444_ADDRTLV_LINK_STATUS,
                                                         RFC5444_LINKSTATUS_LOST,
                                                         rfc5444_metric_encode(ls_elt->metric_in),
                                                         rfc5444_metric_encode(ls_elt->metric_out));
                                    addr_elt->address->in_tmp_table = NHDP_ADDR_TMP_ANY;
                                    break;

                                case IIB_LT_STATUS_PENDING:
                                    /* Pending link tuples are not included */
                                    break;

                                default:
                                    /* Should not happen */
                                    DEBUG("%s:%d in %s: [WARNING] Unknown link tuple status\n",
                                          RIOT_FILE_RELATIVE, __LINE__, DEBUG_FUNC);
                                    break;
                            }
                        }
                    }
                }
            }
            /* IF's link set found */
            break;
        }
    }

    mutex_unlock(&mtx_iib_access);
}

void iib_update_lt_status(timex_t *now)
{
    iib_base_entry_t *base_elt = NULL;
    iib_link_set_entry_t *ls_elt = NULL, *ls_tmp = NULL;

    LL_FOREACH(iib_base_entry_head, base_elt) {
        LL_FOREACH_SAFE(base_elt->link_set_head, ls_elt, ls_tmp) {
            wr_update_ls_status(base_elt, ls_elt, now);
        }
    }
}

void iib_propagate_nb_entry_change(nib_entry_t *old_entry, nib_entry_t *new_entry)
{
    iib_base_entry_t *base_elt = NULL;
    iib_link_set_entry_t *ls_elt = NULL;
    LL_FOREACH(iib_base_entry_head, base_elt) {
        LL_FOREACH(base_elt->link_set_head, ls_elt) {
            if (ls_elt->nb_elt == old_entry) {
                ls_elt->nb_elt = new_entry;
            }
        }
    }
}

void iib_process_metric_msg(iib_link_set_entry_t *ls_entry, uint64_t int_time)
{
#if (NHDP_METRIC == NHDP_LMT_HOP_COUNT)
    /* Hop metric value for an existing direct link is always 1 */
    (void)int_time;
    ls_entry->metric_in = 1;
    ls_entry->metric_out = 1;
    if (ls_entry->nb_elt) {
        ls_entry->nb_elt->metric_in = 1;
        ls_entry->nb_elt->metric_out = 1;
    }
#elif (NHDP_METRIC == NHDP_LMT_DAT)
    /* Process required DAT metric steps */
    ls_entry->hello_interval = rfc5444_timetlv_encode(int_time);
    if (ls_entry->last_seq_no == 0) {
        timex_t now, i_time;
        xtimer_now_timex(&now);
        i_time = timex_from_uint64(int_time * US_PER_MS * DAT_HELLO_TIMEOUT_FACTOR);
        ls_entry->dat_received[0]++;
        ls_entry->dat_total[0]++;
        ls_entry->dat_time = timex_add(now, i_time);
    }
#else
    /* NHDP_METRIC is not set properly */
    (void)ls_entry;
    (void)int_time;
    DEBUG("%s:%d in %s: [WARNING] Unknown NHDP_METRIC setting\n",
          RIOT_FILE_RELATIVE, __LINE__, DEBUG_FUNC);
#endif
}

void iib_process_metric_pckt(iib_link_set_entry_t *ls_entry, uint32_t metric_out, uint16_t seq_no)
{
#if (NHDP_METRIC == NHDP_LMT_HOP_COUNT)
    /* Nothing to do here */
    (void)ls_entry;
    (void)metric_out;
    (void)seq_no;
#elif (NHDP_METRIC == NHDP_LMT_DAT)
    /* Metric packet processing */
    if (ls_entry->last_seq_no == 0) {
        ls_entry->dat_received[0] = 1;
        ls_entry->dat_total[0] = 1;
    }
    /* Don't add values to the queue for duplicate packets */
    else if (seq_no != ls_entry->last_seq_no) {
        uint16_t seq_diff;
        if (seq_no < ls_entry->last_seq_no) {
            seq_diff = (uint16_t) ((((uint32_t) seq_no) + 0xFFFF) - ls_entry->last_seq_no);
        }
        else {
            seq_diff = seq_no - ls_entry->last_seq_no;
        }
        ls_entry->dat_total[0] += (seq_diff > NHDP_SEQNO_RESTART_DETECT) ? 1 : seq_diff;
        ls_entry->dat_received[0]++;
    }

    ls_entry->last_seq_no = seq_no;
    ls_entry->lost_hellos = 0;

    if (ls_entry->hello_interval != 0) {
        timex_t now, i_time;
        xtimer_now_timex(&now);
        i_time = timex_from_uint64(rfc5444_timetlv_decode(ls_entry->hello_interval)
                * US_PER_MS * DAT_HELLO_TIMEOUT_FACTOR);
        ls_entry->dat_time = timex_add(now, i_time);
    }

    /* Refresh metric value for link tuple and corresponding neighbor tuple */
    if (ls_entry->nb_elt) {
        if ((metric_out <= ls_entry->nb_elt->metric_out) ||
                (ls_entry->nb_elt->metric_out == NHDP_METRIC_UNKNOWN)) {
            /* Better value, use it also for your neighbor */
            ls_entry->nb_elt->metric_out = metric_out;
        }
        else if (ls_entry->metric_out == ls_entry->nb_elt->metric_out){
            /* The corresponding neighbor tuples metric needs to be updated */
            iib_base_entry_t *base_elt = NULL;
            iib_link_set_entry_t *ls_elt;
            ls_entry->nb_elt->metric_out = metric_out;
            LL_FOREACH(iib_base_entry_head, base_elt) {
                LL_FOREACH(base_elt->link_set_head, ls_elt) {
                    if ((ls_elt->nb_elt == ls_entry->nb_elt) && (ls_elt != ls_entry)) {
                        if (ls_elt->metric_out < ls_entry->nb_elt->metric_out) {
                            /* Smaller DAT value is better */
                            ls_entry->nb_elt->metric_out = ls_elt->metric_out;
                        }
                        break;
                    }
                }
            }
        }
    }
    ls_entry->metric_out = metric_out;
#else
    /* NHDP_METRIC is not set properly */
    (void)ls_entry;
    (void)metric_out;
    (void)seq_no;
    DEBUG("%s:%d in %s: [WARNING] Unknown NHDP_METRIC setting\n",
          RIOT_FILE_RELATIVE, __LINE__, DEBUG_FUNC);
#endif
}

void iib_process_metric_refresh(void)
{
#if (NHDP_METRIC == NHDP_LMT_HOP_COUNT)
    /* Nothing to do here */
#elif (NHDP_METRIC == NHDP_LMT_DAT)
    dat_metric_refresh();
#else
    /* NHDP_METRIC is not set properly */
    DEBUG("%s:%d in %s: [WARNING] Unknown NHDP_METRIC setting\n",
          RIOT_FILE_RELATIVE, __LINE__, DEBUG_FUNC);
#endif
}


/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

/**
 * Remove addresses included in the Removed Address List from all existing Link Tuples
 */
static void cleanup_link_sets(void)
{
    iib_base_entry_t *base_elt = NULL;

    /* Loop through all link sets */
    LL_FOREACH(iib_base_entry_head, base_elt) {
        /* Loop through all link tuples of the link set */
        iib_link_set_entry_t *ls_elt = NULL, *ls_tmp = NULL;
        LL_FOREACH_SAFE(base_elt->link_set_head, ls_elt, ls_tmp) {
            /* Loop through all addresses of the link tuples */
            nhdp_addr_entry_t *lt_elt = NULL, *lt_tmp = NULL;
            LL_FOREACH_SAFE(ls_elt->address_list_head, lt_elt, lt_tmp) {
                if (NHDP_ADDR_TMP_IN_REM_LIST(lt_elt->address)) {
                    /* Remove link tuple address if included in the Removed Addr List */
                    LL_DELETE(ls_elt->address_list_head, lt_elt);
                    nhdp_free_addr_entry(lt_elt);
                }
            }

            /* Remove link tuples with empty address list */
            if (!ls_elt->address_list_head) {
                if (ls_elt->last_status == IIB_LT_STATUS_SYM) {
                    /* Remove all two hop entries for the corresponding link tuple */
                    iib_two_hop_set_entry_t *th_elt = NULL, *th_tmp = NULL;
                    LL_FOREACH_SAFE(base_elt->two_hop_set_head, th_elt, th_tmp) {
                        if (th_elt->ls_elt == ls_elt) {
                            rem_two_hop_entry(base_elt, th_elt);
                        }
                    }
                }

                rem_link_set_entry(base_elt, ls_elt);
            }
        }
    }
}

/**
 * Update the Link Set for the receiving interface during HELLO message processing
 */
static iib_link_set_entry_t *update_link_set(iib_base_entry_t *base_entry, nib_entry_t *nb_elt,
                                             timex_t *now, uint64_t val_time,
                                             uint8_t sym, uint8_t lost)
{
    iib_link_set_entry_t *ls_elt = NULL, *ls_tmp = NULL;
    iib_link_set_entry_t *matching_lt = NULL;
    nhdp_addr_entry_t *lt_elt = NULL;
    timex_t v_time, l_hold;
    uint8_t matches = 0;

    /* Loop through every link tuple of the interface to update the link set */
    LL_FOREACH_SAFE(base_entry->link_set_head, ls_elt, ls_tmp) {
        /* Loop through all addresses of the link tuple */
        LL_FOREACH(ls_elt->address_list_head, lt_elt) {
            if (NHDP_ADDR_TMP_IN_SEND_LIST(lt_elt->address)) {
                /* If link tuple address matches a sending addr we found a fitting tuple */
                matches++;

                if (matches > 1) {
                    /* Multiple matching link tuples, delete the previous one */
                    if (matching_lt->last_status == IIB_LT_STATUS_SYM) {
                        update_nb_tuple_symmetry(base_entry, matching_lt, now);
                    }

                    rem_link_set_entry(base_entry, matching_lt);
                }

                matching_lt = ls_elt;
                break;
            }
        }
    }

    if (matches > 1) {
        /* Multiple matching link tuples, reset the last one for reuse */
        if (matching_lt->last_status == IIB_LT_STATUS_SYM) {
            update_nb_tuple_symmetry(base_entry, matching_lt, now);
        }

        reset_link_set_entry(matching_lt, now, val_time);
    }
    else if (matches == 1) {
        /* A single matching link tuple, only release the address list */
        release_link_tuple_addresses(matching_lt);
    }
    else {
        /* No single matching link tuple existent, create a new one */
        matching_lt = add_default_link_set_entry(base_entry, now, val_time);

        if (!matching_lt) {
            /* Insufficient memory */
            return NULL;
        }
    }

    v_time = timex_from_uint64(val_time * US_PER_MS);
    l_hold = timex_from_uint64(((uint64_t)NHDP_L_HOLD_TIME_MS) * US_PER_MS);

    /* Set Sending Address List as this tuples address list */
    matching_lt->address_list_head = nhdp_generate_addr_list_from_tmp(NHDP_ADDR_TMP_SEND_LIST);

    if (!matching_lt->address_list_head) {
        /* Insufficient memory */
        rem_link_set_entry(base_entry, matching_lt);
        return NULL;
    }

    matching_lt->nb_elt = nb_elt;

    /* Set values dependent on link status */
    if (sym) {
        if (matching_lt->last_status != IIB_LT_STATUS_SYM) {
            /* Set corresponding neighbor tuple to symmetric (Section 13.1 of RFC 6130) */
            if (matching_lt->nb_elt) {
                nib_set_nb_entry_sym(matching_lt->nb_elt);
            }
        }

        matching_lt->sym_time = timex_add(*now, v_time);
        matching_lt->last_status = IIB_LT_STATUS_SYM;
    }
    else if (lost) {
        matching_lt->sym_time.microseconds = 0;
        matching_lt->sym_time.seconds = 0;

        if (matching_lt->last_status == IIB_LT_STATUS_SYM) {
            update_nb_tuple_symmetry(base_entry, matching_lt, now);
        }

        if (get_tuple_status(matching_lt, now) == IIB_LT_STATUS_HEARD) {
            matching_lt->last_status = IIB_LT_STATUS_HEARD;
            matching_lt->exp_time = timex_add(*now, l_hold);
        }
        else {
            matching_lt->last_status = IIB_LT_STATUS_UNKNOWN;
        }
    }

    /* Set time values */
    matching_lt->heard_time = get_max_timex(timex_add(*now, v_time), matching_lt->sym_time);

    if (matching_lt->pending) {
        /* L_status is PENDING */
        matching_lt->exp_time = get_max_timex(matching_lt->exp_time, matching_lt->heard_time);
    }
    else if (!matching_lt->lost) {
        if ((timex_cmp(matching_lt->sym_time, *now) == 1)
            || (timex_cmp(matching_lt->heard_time, *now) == 1)) {
            /* L_status is HEARD or SYMMETRIC */
            matching_lt->exp_time = get_max_timex(matching_lt->exp_time,
                                                  timex_add(matching_lt->heard_time, l_hold));
        }
    }

    return matching_lt;
}

/**
 * Update the status of a link tuple and process necessary changes and execute
 * necessary changes in the 2-Hop Set and in the Neighbor Information Base
 * Implements logic of Section 13 of RFC 6130
 */
static void wr_update_ls_status(iib_base_entry_t *base_entry,
                                iib_link_set_entry_t *ls_elt, timex_t *now)
{
    if (timex_cmp(ls_elt->exp_time, *now) != 1) {
        /* Entry expired and has to be removed */
        if (ls_elt->last_status == IIB_LT_STATUS_SYM) {
            update_nb_tuple_symmetry(base_entry, ls_elt, now);
        }

        rem_not_heard_nb_tuple(ls_elt, now);
        rem_link_set_entry(base_entry, ls_elt);
    }
    else if ((ls_elt->last_status == IIB_LT_STATUS_SYM)
             && (timex_cmp(ls_elt->sym_time, *now) != 1)) {
        /* Status changed from SYMMETRIC to HEARD */
        update_nb_tuple_symmetry(base_entry, ls_elt, now);
        ls_elt->last_status = IIB_LT_STATUS_HEARD;

        if (timex_cmp(ls_elt->heard_time, *now) != 1) {
            /* New status is LOST (equals IIB_LT_STATUS_UNKNOWN) */
            rem_not_heard_nb_tuple(ls_elt, now);
            ls_elt->nb_elt = NULL;
            ls_elt->last_status = IIB_LT_STATUS_UNKNOWN;
        }
    }
    else if ((ls_elt->last_status == IIB_LT_STATUS_HEARD)
             && (timex_cmp(ls_elt->heard_time, *now) != 1)) {
        /* Status changed from HEARD to LOST (equals IIB_LT_STATUS_UNKNOWN) */
        rem_not_heard_nb_tuple(ls_elt, now);
        ls_elt->nb_elt = NULL;
        ls_elt->last_status = IIB_LT_STATUS_UNKNOWN;
    }
}

/**
 * Add a new Link Tuple with default values to the given Link Set
 */
static iib_link_set_entry_t *add_default_link_set_entry(iib_base_entry_t *base_entry, timex_t *now,
                                                        uint64_t val_time)
{
    iib_link_set_entry_t *new_entry;

    new_entry = (iib_link_set_entry_t *) malloc(sizeof(iib_link_set_entry_t));

    if (!new_entry) {
        /* Insufficient memory */
        return NULL;
    }

    new_entry->address_list_head = NULL;
    reset_link_set_entry(new_entry, now, val_time);
    LL_PREPEND(base_entry->link_set_head, new_entry);

    return new_entry;
}

/**
 * Reset a given Link Tuple for reusage
 */
static void reset_link_set_entry(iib_link_set_entry_t *ls_entry, timex_t *now, uint64_t val_time)
{
    timex_t v_time = timex_from_uint64(val_time * US_PER_MS);

    release_link_tuple_addresses(ls_entry);
    ls_entry->sym_time.microseconds = 0;
    ls_entry->sym_time.seconds = 0;
    ls_entry->heard_time.microseconds = 0;
    ls_entry->heard_time.seconds = 0;
    ls_entry->pending = NHDP_INITIAL_PENDING;
    ls_entry->lost = 0;
    ls_entry->exp_time = timex_add(*now, v_time);
    ls_entry->nb_elt = NULL;
    ls_entry->last_status = IIB_LT_STATUS_UNKNOWN;
    ls_entry->metric_in = NHDP_METRIC_UNKNOWN;
    ls_entry->metric_out = NHDP_METRIC_UNKNOWN;
#if (NHDP_METRIC == NHDP_LMT_DAT)
    memset(ls_entry->dat_received, 0, NHDP_Q_MEM_LENGTH);
    memset(ls_entry->dat_total, 0, NHDP_Q_MEM_LENGTH);
    ls_entry->dat_time.microseconds = 0;
    ls_entry->dat_time.seconds = 0;
    ls_entry->hello_interval = 0;
    ls_entry->lost_hellos = 0;
    ls_entry->rx_bitrate = 100000;
    ls_entry->last_seq_no = 0;
#endif
}

/**
 * Remove a given Link Tuple
 */
static void rem_link_set_entry(iib_base_entry_t *base_entry, iib_link_set_entry_t *ls_entry)
{
    LL_DELETE(base_entry->link_set_head, ls_entry);
    release_link_tuple_addresses(ls_entry);
    free(ls_entry);
}

/**
 * Free all address entries of a link tuple
 */
static void release_link_tuple_addresses(iib_link_set_entry_t *ls_entry)
{
    nhdp_free_addr_list(ls_entry->address_list_head);
    ls_entry->address_list_head = NULL;
}

/**
 * Update the 2-Hop Set during HELLO message processing
 */
static int update_two_hop_set(iib_base_entry_t *base_entry, iib_link_set_entry_t *ls_entry,
                              timex_t *now, uint64_t val_time)
{
    /* Check whether a corresponding link tuple was created */
    if (ls_entry == NULL) {
        return -1;
    }

    /* If the link to the neighbor is still symmetric */
    if (get_tuple_status(ls_entry, now) == IIB_LT_STATUS_SYM) {
        iib_two_hop_set_entry_t *ths_elt = NULL, *ths_tmp = NULL;
        nhdp_addr_t *addr_elt = NULL;

        /* Loop through all the two hop tuples of the two hop set */
        LL_FOREACH_SAFE(base_entry->two_hop_set_head, ths_elt, ths_tmp) {
            if (timex_cmp(ths_elt->exp_time, *now) != 1) {
                /* Entry is expired, remove it */
                rem_two_hop_entry(base_entry, ths_elt);
            }
            else if (ths_elt->ls_elt == ls_entry) {
                if (ths_elt->th_nb_addr->in_tmp_table &
                    (NHDP_ADDR_TMP_TH_REM_LIST | NHDP_ADDR_TMP_TH_SYM_LIST)) {
                    rem_two_hop_entry(base_entry, ths_elt);
                }
            }
        }

        /* Add a new entry for every signaled symmetric neighbor address */
        LL_FOREACH(nhdp_get_addr_db_head(), addr_elt) {
            if (NHDP_ADDR_TMP_IN_TH_SYM_LIST(addr_elt)) {
                if (add_two_hop_entry(base_entry, ls_entry, addr_elt, now, val_time)) {
                    /* No more memory available, return error */
                    return -1;
                }
            }
        }
    }

    return 0;
}

/**
 * Add a 2-Hop Tuple for a given address
 */
static int add_two_hop_entry(iib_base_entry_t *base_entry, iib_link_set_entry_t *ls_entry,
                             nhdp_addr_t *th_addr, timex_t *now, uint64_t val_time)
{
    iib_two_hop_set_entry_t *new_entry;
    timex_t v_time = timex_from_uint64(val_time * US_PER_MS);

    new_entry = (iib_two_hop_set_entry_t *) malloc(sizeof(iib_two_hop_set_entry_t));

    if (!new_entry) {
        /* Insufficient memory */
        return -1;
    }

    /* Increment usage counter of address in central NHDP address storage */
    th_addr->usg_count++;
    new_entry->th_nb_addr = th_addr;
    new_entry->ls_elt = ls_entry;
    new_entry->exp_time = timex_add(*now, v_time);
    if (th_addr->tmp_metric_val != NHDP_METRIC_UNKNOWN) {
        new_entry->metric_in = rfc5444_metric_decode(th_addr->tmp_metric_val);
        new_entry->metric_out = rfc5444_metric_decode(th_addr->tmp_metric_val);
    }
    else {
        new_entry->metric_in = NHDP_METRIC_UNKNOWN;
        new_entry->metric_out = NHDP_METRIC_UNKNOWN;
    }

    LL_PREPEND(base_entry->two_hop_set_head, new_entry);

    return 0;
}

/**
 * Remove a given 2-Hop Tuple
 */
static void rem_two_hop_entry(iib_base_entry_t *base_entry, iib_two_hop_set_entry_t *th_entry)
{
    LL_DELETE(base_entry->two_hop_set_head, th_entry);
    nhdp_decrement_addr_usage(th_entry->th_nb_addr);
    free(th_entry);
}

/**
 * Remove all corresponding two hop entries for a given link tuple that lost symmetry status.
 * Additionally reset the neighbor tuple's symmmetry flag (for the neighbor tuple this link
 * tuple is represented in), if no more corresponding symmetric link tuples are left.
 * Implements section 13.2 of RFC 6130
 */
static void update_nb_tuple_symmetry(iib_base_entry_t *base_entry,
                                     iib_link_set_entry_t *ls_entry, timex_t *now)
{
    iib_two_hop_set_entry_t *th_elt = NULL, *th_tmp = NULL;

    /* First remove all two hop entries for the corresponding link tuple */
    LL_FOREACH_SAFE(base_entry->two_hop_set_head, th_elt, th_tmp) {
        if (th_elt->ls_elt == ls_entry) {
            rem_two_hop_entry(base_entry, th_elt);
        }
    }

    /* Afterwards check the neighbor tuple containing the link tuple's addresses */
    if ((ls_entry->nb_elt != NULL) && (ls_entry->nb_elt->symmetric == 1)) {
        iib_base_entry_t *base_tmp = NULL;
        LL_FOREACH(iib_base_entry_head, base_tmp) {
            iib_link_set_entry_t *ls_tmp = NULL;
            LL_FOREACH(base_tmp->link_set_head, ls_tmp) {
                if ((ls_entry->nb_elt == ls_tmp->nb_elt) && (ls_entry != ls_tmp)) {
                    if (timex_cmp(ls_tmp->sym_time, *now) == 1) {
                        return;
                    }
                }
            }
        }

        /* No remaining symmetric link tuple for the neighbor tuple */
        nib_reset_nb_entry_sym(ls_entry->nb_elt, now);
    }
}

/**
 * Remove a neighbor tuple if no more corresponding heard link tuples are left
 * Implements section 13.3 of RFC 6130
 */
static void rem_not_heard_nb_tuple(iib_link_set_entry_t *ls_entry, timex_t *now)
{
    /* Check whether the corresponding neighbor tuple still exists */
    if (ls_entry->nb_elt) {
        iib_base_entry_t *base_tmp = NULL;
        LL_FOREACH(iib_base_entry_head, base_tmp) {
            iib_link_set_entry_t *ls_tmp = NULL;
            LL_FOREACH(base_tmp->link_set_head, ls_tmp) {
                if ((ls_entry->nb_elt == ls_tmp->nb_elt) && (ls_entry != ls_tmp)) {
                    if (timex_cmp(ls_tmp->heard_time, *now) == 1) {
                        return;
                    }

                    ls_tmp->nb_elt = NULL;
                }
            }
        }

        /* No remaining heard link tuple for the neighbor tuple */
        nib_rem_nb_entry(ls_entry->nb_elt);
    }
}

/**
 * Get the L_STATUS value of a given link tuple
 */
static iib_link_tuple_status_t get_tuple_status(iib_link_set_entry_t *ls_entry, timex_t *now)
{
    if (ls_entry->pending) {
        return IIB_LT_STATUS_PENDING;
    }
    else if (ls_entry->lost) {
        return IIB_LT_STATUS_LOST;
    }
    else if (timex_cmp(ls_entry->sym_time, *now) == 1) {
        return IIB_LT_STATUS_SYM;
    }
    else if (timex_cmp(ls_entry->heard_time, *now) == 1) {
        return IIB_LT_STATUS_HEARD;
    }

    return IIB_LT_STATUS_UNKNOWN;
}

/**
 * Get the later one of two timex representation
 */
static inline timex_t get_max_timex(timex_t time_one, timex_t time_two)
{
    if (timex_cmp(time_one, time_two) != -1) {
        return time_one;
    }

    return time_two;
}

#if (NHDP_METRIC == NHDP_LMT_DAT)
/**
 * Sum all elements in the queue
 */
static uint16_t queue_sum(uint8_t *queue)
{
    uint16_t sum = 0;

    for (int i = 0; i < NHDP_Q_MEM_LENGTH; i++) {
        sum += queue[i];
    }

    return sum;
}

/**
 * Remove the oldest element in the queue
 */
static void queue_rem(uint8_t *queue)
{
    uint8_t temp;
    uint8_t prev_value = queue[0];

    /* Clear spot for a new element */
    queue[0] = 0;

    /* Shift elements */
    for (int i = 1; i < NHDP_Q_MEM_LENGTH; i++) {
        temp = queue[i];
        queue[i] = prev_value;
        prev_value = temp;
    }
}

/**
 * Update DAT metric values for all Link Tuples
 */
static void dat_metric_refresh(void)
{
    iib_base_entry_t *base_elt = NULL;
    iib_link_set_entry_t *ls_elt;
    uint32_t metric_temp;
    double sum_total, sum_rcvd, loss;

    LL_FOREACH(iib_base_entry_head, base_elt) {
        LL_FOREACH(base_elt->link_set_head, ls_elt) {
            sum_rcvd = queue_sum(ls_elt->dat_received);
            sum_total = queue_sum(ls_elt->dat_total);
            metric_temp = ls_elt->metric_in;

            if ((ls_elt->hello_interval != 0) && (ls_elt->lost_hellos > 0)) {
                /* Compute lost time proportion */
                loss = (((double)ls_elt->hello_interval) * ((double)ls_elt->lost_hellos))
                        / DAT_MEMORY_LENGTH;
                if (loss >= 1.0) {
                    sum_rcvd = 0.0;
                }
                else {
                    sum_rcvd *= (1.0 - loss);
                }
            }

            if (sum_rcvd < 1.0) {
                ls_elt->metric_in = NHDP_METRIC_MAXIMUM;
            }
            else {
                loss = sum_total / sum_rcvd;
                if (loss > DAT_MAXIMUM_LOSS) {
                    loss = DAT_MAXIMUM_LOSS;
                }
                ls_elt->metric_in = (const_dat * loss) / (ls_elt->rx_bitrate / DAT_MINIMUM_BITRATE);
                if (ls_elt->metric_in > NHDP_METRIC_MAXIMUM) {
                    ls_elt->metric_in = NHDP_METRIC_MAXIMUM;
                }
            }

            if (ls_elt->nb_elt) {
                if (ls_elt->metric_in <= ls_elt->nb_elt->metric_in ||
                        (ls_elt->nb_elt->metric_in == NHDP_METRIC_UNKNOWN)) {
                    /* Better value, use it also for your neighbor */
                    ls_elt->nb_elt->metric_in = ls_elt->metric_in;
                }
                else if (metric_temp == ls_elt->nb_elt->metric_in){
                    /* The corresponding neighbor tuples metric needs to be updated */
                    iib_base_entry_t *base_entry;
                    iib_link_set_entry_t *ls_entry;
                    ls_elt->nb_elt->metric_in = ls_elt->metric_in;
                    LL_FOREACH(iib_base_entry_head, base_entry) {
                        LL_FOREACH(base_entry->link_set_head, ls_entry) {
                            if ((ls_elt->nb_elt == ls_entry->nb_elt) && (ls_elt != ls_entry)) {
                                if (ls_entry->metric_in < ls_elt->nb_elt->metric_in) {
                                    /* Smaller DAT value is better */
                                    ls_elt->nb_elt->metric_in = ls_entry->metric_in;
                                }
                                break;
                            }
                        }
                    }
                }
            }

            queue_rem(ls_elt->dat_received);
            queue_rem(ls_elt->dat_total);
        }
    }
}
#endif
