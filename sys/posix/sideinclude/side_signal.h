/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  posix
 * @{
 *
 * @file        side_signal.h
 * @brief       Auxilary header to extend <signal.h>
 *
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/signal.h.html">
 *              The Open Group Base Specifications Issue 7, <signal.h>
 *          </a>
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __SIDE_SIGNAL_H_
#define __SIDE_SIGNAL_H_

#include <signal.h>

/* TODO */
int kill(pid_t, int);

#endif /* __SIDE_SIGNAL_H_ */
/** @} */
