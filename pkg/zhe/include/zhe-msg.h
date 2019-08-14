/* -*- mode: c; c-basic-offset: 4; fill-column: 95; -*- */
#ifndef ZHE_MSG_H
#define ZHE_MSG_H

#define ZHE_VERSION        1

#define MSCOUT              1
#define MHELLO              2
#define MOPEN               3
#define MACCEPT             4
#define MCLOSE              5
#define MDECLARE            6
#define MSDATA              7
#define MBDATA              8 /* FIXME: NIY */
#define MWDATA              9
#define MQUERY             10 /* FIXME: NIY */
#define MPULL              11 /* FIXME: NIY */
#define MPING              12
#define MPONG              13
#define MSYNCH             14
#define MACKNACK           15
#define MKEEPALIVE         16
#define MCONDUIT_CLOSE     17 /* FIXME: NIY */
#define MFRAGMENT          18 /* FIXME: NIY */
#define MCONDUIT           19
#define MMIGRATE           20 /* FIXME: NIY */
#define MSDDATA            21 /* FIXME: NIY */
#define MBDDATA            22 /* FIXME: NIY */
#define MWDDATA            23 /* FIXME: NIY */

#define MKIND            0x1f

#define MSFLAG             32
#define MMFLAG             32
#define MPFLAG             32
#define MRFLAG             64
#define MNFLAG             64
#define MCFLAG             64
#define MZFLAG            128
#define MAFLAG            128
#define MUFLAG            128

#define DRESOURCE           1
#define DPUB                2
#define DSUB                3
#define DSELECTION          4
#define DBINDID             5
#define DCOMMIT             6
#define DRESULT             7
#define DFRESOURCE          8
#define DFPUB               9
#define DFSUB              10
#define DFSELECTION        11
#define DKIND             0xf

#define DPFLAG             32
#define DGFLAG            128

#define CLR_INVALID_AUTH    1
#define CLR_UNSUPP_PROTO    2
#define CLR_OUT_OF_RES      3
#define CLR_UNSUPP_SEQLEN   4
#define CLR_UNSUPP_PARAM    5
#define CLR_INCOMPAT_DECL   6
#define CLR_ERROR         255

#define PROP_VENDORID       0
#define PROP_MAXCONDUITS    2
#define PROP_SEQLEN         4
#define PROP_RELIABILITY    6
#define PROP_DURABILITY     8
#define PROP_COMMITMODE    10
#define PROP_AUTHDATA      12
#define PROP_CLIENTHASH    14

#define MSCOUT_BROKER       1
#define MSCOUT_DURABILITY   2
#define MSCOUT_PEER         4
#define MSCOUT_CLIENT       8

#define SUBMODE_PUSH        1
#define SUBMODE_PULL        2
#define SUBMODE_PERIODPUSH  3
#define SUBMODE_PERIODPULL  4
#define SUBMODE_PUSHPULL    5
#define SUBMODE_MAX         SUBMODE_PUSHPULL

#endif
