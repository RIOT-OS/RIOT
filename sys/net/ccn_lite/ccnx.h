/*
 * @f ccnx.h
 * @b CCN constants
 *
 * Copyright (C) 2011, Christian Tschudin, University of Basel
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * File history:
 * 2011-03-13 created
 */

// ----------------------------------------------------------------------

#define CCN_DEFAULT_MTU         PAYOAD_SIZE

// ----------------------------------------------------------------------

#define CCN_TT_TAG              1
#define CCN_TT_DTAG             2
#define CCN_TT_ATTR             3
#define CCN_TT_DATTR            4
#define CCN_TT_BLOB             5
#define CCN_TT_UDATA            6

#define CCN_DTAG_ANY            13
#define CCN_DTAG_NAME           14
#define CCN_DTAG_COMPONENT      15
#define CCN_DTAG_CONTENT        19
#define CCN_DTAG_SIGNEDINFO     20
#define CCN_DTAG_INTEREST       26
#define CCN_DTAG_KEY            27
#define CCN_DTAG_KEYLOCATOR     28
#define CCN_DTAG_KEYNAME        29
#define CCN_DTAG_SIGNATURE      37
#define CCN_DTAG_TIMESTAMP      39
#define CCN_DTAG_TYPE           40
#define CCN_DTAG_NONCE          41
#define CCN_DTAG_SCOPE          42
#define CCN_DTAG_EXCLUDE        43
#define CCN_DTAG_ANSWERORIGKIND 47
#define CCN_DTAG_WITNESS        53
#define CCN_DTAG_SIGNATUREBITS  54
#define CCN_DTAG_FRESHNESS      58
#define CCN_DTAG_FINALBLOCKID   59
#define CCN_DTAG_PUBPUBKDIGEST  60
#define CCN_DTAG_PUBCERTDIGEST  61
#define CCN_DTAG_CONTENTOBJ     64
#define CCN_DTAG_ACTION         73
#define CCN_DTAG_FACEID         74
#define CCN_DTAG_IPPROTO        75
#define CCN_DTAG_HOST           76
#define CCN_DTAG_PORT           77
#define CCN_DTAG_FWDINGFLAGS    79
#define CCN_DTAG_FACEINSTANCE   80
#define CCN_DTAG_FWDINGENTRY    81
#define CCN_DTAG_MINSUFFCOMP    83
#define CCN_DTAG_MAXSUFFCOMP    84
#define CCN_DTAG_SEQNO          256
#define CCN_DTAG_CCNPDU         17702112

// eof
