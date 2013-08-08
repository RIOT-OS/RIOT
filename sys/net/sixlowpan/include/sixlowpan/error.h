/**
 * 6LoWPAN error codes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowpan/error.h
 * @brief   6LoWPAN error codes
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @}
 */

#ifndef SIXLOWPAN_ERROR_H
#define SIXLOWPAN_ERROR_H

#ifndef SUCCESS
#define SUCCESS                 (0)
#endif

#define SIXLOWERROR_ARRAYFULL   (132)   // array is full
#define SIXLOWERROR_NULLPTR     (133)   // null pointer error
#define SIXLOWERROR_VALUE       (134)   // illegal value
#define SIXLOWERROR_ADDRESS     (135)   // illegal address
#define SIXLOWERROR_DISPATCH    (136)   // lowpan dispatch unknown
#define SIXLOWERROR_FSTFRAG     (137)   // first lowpan fragment not received
#define SIXLOWERROR_INVFRAG     (138)   // invalid fragment received
#define SIXLOWERROR_SCI         (139)   // source context not found
#define SIXLOWERROR_DCI         (140)   // destination context not found
#define SIXLOWERROR_CSUM        (141)   // wrong checksum

#endif /* SIXLOWPAN_ERROR_H */
