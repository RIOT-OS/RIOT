#ifndef PING_TEST_H

#define PING_TEST_H

#include <stdint.h>
#include <cc1100-interface.h>

/**
 * Maximum and minimum radio address
 */
#define MIN_ADDR MIN_UID
#define MAX_ADDR MAX_UID

/**
 * Maximum and minimum channel address
 */
#define MIN_CHAN MIN_CHANNR
#define MAX_CHAN MAX_CHANNR

/**
 * Receive- and send-buffer size
 */
#define SND_BUFFER_SIZE     (100)
#define RCV_BUFFER_SIZE     (64)

/**
 * @brief Shows a helping text.
 * @param command the command name for which the help should be showed.
 */
void help(char * commandname);

/**
 * @brief Initializes a node for radio communication
 *
 * Initializes this node with a given radio address and on a given channel or
 * on a default channel, if no channel is given. A radio address must be given
 * in any case.
 *
 * @param arg
 */
void init(char * arg);

/**
 * @brief Pings another node.
 *
 * Sends out pings from this node to another node in a continuous manner, until
 * the stop command is used.
 */
void ping(char * arg);

/**
 * @brief Returns the radio address of this device.
 *
 * @return The radio-address of the device. Undefined if radio-address has not
 *         been set properly before.
 */
uint16_t get_radio_address(void);

/**
 * @brief Sets the radio address of this device. It is undefined what happens
 *        if radio-address is not in valid range.
 *
 * @param addr The radio-address to set.
 */
void set_radio_address(uint8_t addr);

/**
 * @brief Sets the radio channel used for the pinging or broadcasting of nodes.
 *        It is undefined what happens if radio-address is not in valid range.
 *
 * @param chan The channel number to set.
 */
void set_radio_channel(uint8_t chan);

/**
 * @brief Stops this node from pinging.
 *
 * Stops any current pinging-action by this node and prints a summary of how
 * many pings got returned.
 * If the node was not pinging at the time of the method-call, this method does
 * nothing.
 *
 * @param arg unused
 */
void stop_pings(char * arg);

/**
 * @brief The thread for the processing of received radio-packets.
 */
void radio(void);

#endif /* PING_TEST_H */
