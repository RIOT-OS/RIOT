/**
 * 6LoWPAN constants, data structs, and prototypes related to ICMP
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowpan/icmp.h
 * @brief   6LoWPAN ICMP related header
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#ifndef SIXLOWPAN_ICMP_H
#define SIXLOWPAN_ICMP_H

/* icmp message types rfc4443 */
#define ICMP_PARA_PROB                 	(4)
#define ICMP_ECHO_REQ                   (128)
#define ICMP_ECHO_REPL                  (129)
/* icmp message types rfc4861 4.*/
#define ICMP_RTR_ADV                   	(134)
#define ICMP_RTR_SOL                   	(133)
#define ICMP_NBR_ADV                   	(136)
#define ICMP_NBR_SOL                   	(135)
#define ICMP_REDIRECT                  	(137)	/* will be filtered out by the border router */
#define ICMP_RPL_CONTROL                (155)

#endif /* SIXLOWPAN_ICMP_H */
