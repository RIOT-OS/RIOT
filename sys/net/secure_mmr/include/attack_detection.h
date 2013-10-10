/*
 * attack_detection.h - Looks at incoming packets to discover an attack
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        attack_detection.h
 * @brief       Looks at incoming packets to discover if an attack has taken
 *              place
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        16.09.2013 11:35:11
 */


#ifndef __ATTACK_DETECTION_H
#define __ATTACK_DETECTION_H

/**
 * @brief   Processes security status message to decide if an attack has
 *          happened
 *
 * @param   sender_id  Address of the sender
 * @param   msg        The security status message
 *
 * @return Result of operation as NetworkSecurity_Result
 */
int attack_detection_security_status_arrived(uint16_t sender_id,
                                             security_status_msg_t *msg);
#endif
