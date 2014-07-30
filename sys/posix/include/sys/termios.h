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
 * @file        sys/termios.h
 * @brief       define values for termios (imported by newlib through <termios.h>)
 *
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/termios.h.html">
 *              The Open Group Base Specifications Issue 7, <termios.h>
 *          </a>
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __TERMIOS_H_
#define __TERMIOS_H_

#define NCCS    (12)

#define VEOF    (1)
#define VEOL    (2)
#define VERASE  (3)
#define VINTR   (4)
#define VKILL   (5)
#define VMIN    (6)
#define VQUIT   (7)
#define VSTART  (8)
#define VSTOP   (9)
#define VSUSP   (10)
#define VTIME   (11)

#define TCSANOW     (1)
#define TCSADRAIN   (2)
#define TCSAFLUSH   (3)

#define ECHO    (401)
#define ECHOE   (402)
#define ECHOK   (403)
#define ECHONL  (404)
#define ICANON  (405)
#define IEXTEN  (406)
#define ISIG    (407)
#define NOFLSH  (408)
#define TOSTOP  (409)

typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

struct termios {
    tcflag_t  c_iflag;
    tcflag_t  c_oflag;
    tcflag_t  c_cflag;
    tcflag_t  c_lflag;
    cc_t      c_cc[NCCS];
};

int tcgetattr(int, const struct termios *);
int tcsetattr(int, int, const struct termios *);

#endif /* __TERMIOS_H_ */
/** @} */
