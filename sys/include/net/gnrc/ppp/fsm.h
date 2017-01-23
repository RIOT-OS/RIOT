/*
 * Copyright (C) 2016 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ppp Option Negotiation Automaton FSM header definitions
 * @ingroup     net_gnrc_ppp
 * @{
 *
 * @file
 * @brief  Option Negotiation Automaton FSM definitions
 *
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */
#ifndef GNRC_PPP_FSM_H
#define GNRC_PPP_FSM_H

#include "net/gnrc/ppp/opt.h"
#include "net/gnrc/ppp/prot.h"

#ifdef __cplusplus
extern "C" {
#endif


/*Function flags*/
#define PPP_F_TLU (1U << 0)
#define PPP_F_TLD (1U << 1)
#define PPP_F_TLS (1U << 2)
#define PPP_F_TLF (1U << 3)
#define PPP_F_IRC (1U << 4)
#define PPP_F_ZRC (1U << 5)
#define PPP_F_SCR (1U << 6)
#define PPP_F_SCA (1U << 7)
#define PPP_F_SCN (1U << 8)
#define PPP_F_STR (1U << 9)
#define PPP_F_STA (1U << 10)
#define PPP_F_SCJ (1U << 11)
#define PPP_F_SER (1U << 12)

/* flags for setting FSM codes */
#define FLAG_CONF_REQ (1 << 0)
#define FLAG_CONF_ACK (1 << 1)
#define FLAG_CONF_NAK (1 << 2)
#define FLAG_CONF_REJ (1 << 3)
#define FLAG_TERM_REQ (1 << 4)
#define FLAG_TERM_ACK (1 << 5)
#define FLAG_CODE_REJ (1 << 6)
#define FLAG_PROT_REJ (1 << 7)
#define FLAG_ECHO_REQ (1 << 8)
#define FLAG_ECHO_REP (1 << 9)
#define FLAG_DISC_REQ (1 << 10)
#define FLAG_IDENT (1 << 11)
#define FLAG_TIME_REM (1 << 12)


#define GNRC_PPP_MAX_TERMINATE (3)       /**< Maximum number of Terminate Request retransmission*/
#define GNRC_PPP_MAX_CONFIG (10)         /**< Maximum number of Configure Request retransmission */

#define GNRC_PPP_OPT_PAYLOAD_BUF_SIZE (100)  /**< Size of PPP packet payload (only for Auth, LCP, and NCP packets) */

#define GNRC_PPP_OPT_ENABLED (1)             /**< FSM option enabled */
#define GNRC_PPP_OPT_REQUIRED (2)            /**< FSM option is required */
/**
 * @brief Option Negotiation Automaton FSM events
 *
 * @see https://tools.ietf.org/html/rfc1661#section-4.3
 */
typedef enum {
    PPP_E_UP,
    PPP_E_DOWN,
    PPP_E_OPEN,
    PPP_E_CLOSE,
    PPP_E_TOp,
    PPP_E_TOm,
    PPP_E_RCRp,
    PPP_E_RCRm,
    PPP_E_RCA,
    PPP_E_RCN,
    PPP_E_RTR,
    PPP_E_RTA,
    PPP_E_RUC,
    PPP_E_RXJp,
    PPP_E_RXJm,
    PPP_E_RXR,
    PPP_NUM_EVENTS
} gnrc_ppp_fsm_event_t;


/**
 * @brief Option Negotiation Automaton FSM states
 *
 * @see https://tools.ietf.org/html/rfc1661#section-4.2
 */
typedef enum {
    PPP_S_UNDEF = -1,
    PPP_S_INITIAL = 0,
    PPP_S_STARTING,
    PPP_S_CLOSED,
    PPP_S_STOPPED,
    PPP_S_CLOSING,
    PPP_S_STOPPING,
    PPP_S_REQ_SENT,
    PPP_S_ACK_RCVD,
    PPP_S_ACK_SENT,
    PPP_S_OPENED,
    PPP_NUM_STATES
} gnrc_ppp_fsm_state_t;


/**
 * @brief Option Negotiation Automaton FSM transitions
 *
 * @see https://tools.ietf.org/html/rfc1661#section-4.1
 */
static const int8_t state_trans[PPP_NUM_EVENTS][PPP_NUM_STATES] = {
    { PPP_S_CLOSED, PPP_S_REQ_SENT, PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_UNDEF, 
        PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_UNDEF },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_INITIAL, PPP_S_STARTING, PPP_S_INITIAL,
        PPP_S_STARTING, PPP_S_STARTING, PPP_S_STARTING, PPP_S_STARTING, PPP_S_STARTING },
    { PPP_S_STARTING, PPP_S_STARTING, PPP_S_REQ_SENT, PPP_S_STOPPED, PPP_S_STOPPING,
        PPP_S_STOPPING, PPP_S_REQ_SENT, PPP_S_ACK_RCVD, PPP_S_ACK_SENT, PPP_S_OPENED },
    { PPP_S_INITIAL, PPP_S_INITIAL, PPP_S_CLOSED, PPP_S_CLOSED, PPP_S_CLOSING, 
        PPP_S_CLOSING, PPP_S_CLOSING, PPP_S_CLOSING, PPP_S_CLOSING, PPP_S_CLOSING },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSING, 
        PPP_S_STOPPING, PPP_S_REQ_SENT, PPP_S_REQ_SENT, PPP_S_ACK_SENT, PPP_S_UNDEF },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSED, 
        PPP_S_STOPPED, PPP_S_STOPPED, PPP_S_STOPPED, PPP_S_STOPPED, PPP_S_UNDEF },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSED, PPP_S_ACK_SENT, PPP_S_CLOSING,
        PPP_S_STOPPING, PPP_S_ACK_SENT, PPP_S_OPENED, PPP_S_ACK_SENT, PPP_S_ACK_SENT },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSED, PPP_S_REQ_SENT, PPP_S_CLOSING, 
        PPP_S_STOPPING, PPP_S_REQ_SENT, PPP_S_ACK_RCVD, PPP_S_REQ_SENT, PPP_S_REQ_SENT },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSED, PPP_S_STOPPED, PPP_S_CLOSING, 
        PPP_S_STOPPING, PPP_S_ACK_RCVD, PPP_S_UNDEF, PPP_S_OPENED, PPP_S_UNDEF },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSED, PPP_S_STOPPED, PPP_S_CLOSING, 
        PPP_S_STOPPING, PPP_S_REQ_SENT, PPP_S_UNDEF, PPP_S_ACK_SENT, PPP_S_UNDEF },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSED, PPP_S_STOPPED, PPP_S_CLOSING, 
        PPP_S_STOPPING, PPP_S_REQ_SENT, PPP_S_REQ_SENT, PPP_S_REQ_SENT, PPP_S_STOPPING },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSED, PPP_S_STOPPED, PPP_S_CLOSED, 
        PPP_S_STOPPED, PPP_S_REQ_SENT, PPP_S_REQ_SENT, PPP_S_ACK_SENT, PPP_S_REQ_SENT },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSED, PPP_S_STOPPED, PPP_S_CLOSING, 
        PPP_S_STOPPING, PPP_S_REQ_SENT, PPP_S_ACK_RCVD, PPP_S_ACK_SENT, PPP_S_OPENED },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSED, PPP_S_STOPPED, PPP_S_CLOSING, 
        PPP_S_STOPPING, PPP_S_REQ_SENT, PPP_S_REQ_SENT, PPP_S_ACK_SENT, PPP_S_OPENED },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSED, PPP_S_STOPPED, PPP_S_CLOSED, 
        PPP_S_STOPPED, PPP_S_STOPPED, PPP_S_STOPPED, PPP_S_STOPPED, PPP_S_STOPPING },
    { PPP_S_UNDEF, PPP_S_UNDEF, PPP_S_CLOSED, PPP_S_STOPPED, PPP_S_CLOSING, 
        PPP_S_STOPPING, PPP_S_REQ_SENT, PPP_S_ACK_RCVD, PPP_S_ACK_SENT, PPP_S_OPENED }
};



typedef struct gnrc_ppp_fsm gnrc_ppp_fsm_t;
typedef struct gnrc_ppp_fsm_conf gnrc_ppp_fsm_conf_t;


/**
 * @brief Option Negotiation Automaton FSM actions
 *
 * @see https://tools.ietf.org/html/rfc1661#section-4.4
 */
static const uint16_t actions[PPP_NUM_EVENTS][PPP_NUM_STATES] = {
    { 0, PPP_F_IRC | PPP_F_SCR, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, PPP_F_TLS, 0, 0, 0, 0, 0, PPP_F_TLD },
    { PPP_F_TLS, 0, PPP_F_IRC | PPP_F_SCR, 0, 0, 0, 0, 0, 0, 0 },
    { 0, PPP_F_TLF, 0, 0, 0, 0, PPP_F_IRC | PPP_F_STR, PPP_F_IRC |
        PPP_F_STR, PPP_F_IRC | PPP_F_STR, PPP_F_TLD | PPP_F_IRC | PPP_F_STR },
    { 0, 0, 0, 0, PPP_F_STR, PPP_F_STR, PPP_F_SCR, PPP_F_SCR, PPP_F_SCR, 0 },
    { 0, 0, 0, 0, PPP_F_TLF, PPP_F_TLF, PPP_F_TLF, PPP_F_TLF, PPP_F_TLF, 0 },
    { 0, 0, PPP_F_STA, PPP_F_IRC | PPP_F_SCR | PPP_F_SCA, 0, 0, PPP_F_SCA,
        PPP_F_SCA | PPP_F_TLU, PPP_F_SCA, PPP_F_TLD | PPP_F_SCR | PPP_F_SCA },
    { 0, 0, PPP_F_STA, PPP_F_IRC | PPP_F_SCR | PPP_F_SCN, 0, 0, PPP_F_SCN,
        PPP_F_SCN, PPP_F_SCN, PPP_F_TLD | PPP_F_SCR | PPP_F_SCN },
    { 0, 0, PPP_F_STA, PPP_F_STA, 0, 0, PPP_F_IRC, PPP_F_SCR, PPP_F_IRC |
        PPP_F_TLU, PPP_F_TLD | PPP_F_SCR },
    { 0, 0, PPP_F_STA, PPP_F_STA, 0, 0, PPP_F_IRC | PPP_F_SCR, PPP_F_SCR,
        PPP_F_IRC | PPP_F_SCR, PPP_F_TLD | PPP_F_SCR },
    { 0, 0, PPP_F_STA, PPP_F_STA, PPP_F_STA, PPP_F_STA, PPP_F_STA, PPP_F_STA,
        PPP_F_STA, PPP_F_TLD | PPP_F_ZRC | PPP_F_STA },
    { 0, 0, 0, 0, PPP_F_TLF, PPP_F_TLF, 0, 0, 0, PPP_F_TLD | PPP_F_SCR },
    { 0, 0, PPP_F_SCJ, PPP_F_SCJ, PPP_F_SCJ, PPP_F_SCJ, PPP_F_SCJ, PPP_F_SCJ,
        PPP_F_SCJ, PPP_F_SCJ },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, PPP_F_TLF, PPP_F_TLF, PPP_F_TLF, PPP_F_TLF, PPP_F_TLF, PPP_F_TLF,
        PPP_F_TLF, PPP_F_TLD | PPP_F_IRC | PPP_F_STR },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, PPP_F_SER }
};

/**
 * @brief Representation of the Option Negotiation Automaton FSM
 *
 * @extends ppp_protocol_t
 *
 */
typedef struct gnrc_ppp_fsm {
    gnrc_ppp_protocol_t prot;                                            /**< base class of FSM */
    gnrc_nettype_t prottype;                                        /**< GNRC NETTYPE of FSM */
    uint16_t supported_codes;                                       /**< supported codes of current FSM */
    uint8_t state;                                                  /**< current state of FSM */
    uint8_t restart_counter;                                        /**< restart counter value of FSM */
    uint32_t restart_timer;                                         /**< restart time value of FSM */
    xtimer_t xtimer;                                                /**< xtimer structure for timeout handling */
    uint8_t cr_sent_identifier;                                     /**< last configure request id sent */
    uint8_t cr_sent_opts[GNRC_PPP_OPT_PAYLOAD_BUF_SIZE];                     /**< string of last options sent */
    uint16_t cr_sent_size;                                          /**< size of last options sent */
    uint8_t tr_sent_identifier;                                     /**< last terminate request id sent */
    gnrc_ppp_fsm_conf_t * (*get_conf_by_code)(gnrc_ppp_fsm_t * cp, uint8_t code); /**< pointer to fsm's get_conf_by_code function. */
    gnrc_ppp_fsm_conf_t *conf;                                               /**< configuration array of current FSM */
    void (*on_layer_up)(gnrc_ppp_fsm_t *cp);                             /**< Optional callback when current FSM is ready. */
    void (*on_layer_down)(gnrc_ppp_fsm_t *cp);                           /**< Optional callback when current FSM is down */
} gnrc_ppp_fsm_t;

/**
 * @brief Data type or representing a PPP option
 */
typedef struct gnrc_ppp_fsm_conf {
    uint8_t type;                   /**< Option type */
    network_uint32_t value;         /**< Current option value */
    network_uint32_t default_value; /**< Default value of option (for resetting purposes). */
    size_t size;                    /**< Number of bytes of current option */
                                    /**
                                     * @brief flags of the current options.
                                     *
                                     * Set OPT_ENABLED flag for enabling or disabling the option. Set OPT_REQUIRED flag if option is mandatory
                                     */
    uint8_t flags;
                                    /**
                                     * @brief hook for is_valid function for a received option in a Configure Request
                                     *
                                     * @details This hook should return true if the option is valid. False otherwise
                                     */
    uint8_t (*is_valid)(gnrc_ppp_option_t *opt);
                                    /**
                                     * @brief hook for build_nak_options function for a NAK'd option in a Configure Request
                                     *
                                     * @details This hook should write the suggested options in buf. If buf is NULL, returns the size of the options.
                                     * Always returns the size of written buf.
                                     */
    uint8_t (*build_nak_opts)(uint8_t *buf);
                                    /**
                                     * @brief hook for set function for a ACK'd option in a Configure Request
                                     *
                                     * @details This hook should do whatever is necessary after ACK'd an option.
                                     */
    void (*set)(gnrc_ppp_fsm_t *t, gnrc_ppp_option_t *opt, uint8_t peer);
    gnrc_ppp_fsm_conf_t *next;        /**< pointer to next configuration */
} gnrc_ppp_fsm_conf_t;

/**
 * @brief init the Option Negotiation Automaton FSM
 *
 * @param[in] ppp_dev pointer to GNRC pppdev object
 * param[in] cp Pointer to the Option Negotiation Automaton FSM
 *
 * @return 0 for now.
 */
int fsm_init(gnrc_netdev2_t *ppp_dev, gnrc_ppp_fsm_t *cp);
/**
 * @brief triggers an event in the FSM
 *
 * @param cp Pointer to FSM
 * @param event event of Option Negotiation Automaton
 * @param pkt Pointer to a pkt. NULL if the event is not packet related
 *
 * @return -EBADMSG if there was an error in the packet
 * @return 0
 */
int trigger_fsm_event(gnrc_ppp_fsm_t *cp, int event, gnrc_pktsnip_t *pkt);
/**
 * @brief Protocol handler for an FSM.
 *
 * @param protocol pointer to FSM (base class)
 * @param ppp_event event to be handled
 * @param args args for handler
 *
 * @return 0
 * @return Error code if something went wrong
 */
int fsm_handle_ppp_msg(gnrc_ppp_protocol_t *protocol, uint8_t ppp_event, void *args);

#ifdef __cplusplus
}
#endif
#endif /*GNRC_PPP_FSM_H*/
/**
 * @}
 */
