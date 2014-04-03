#ifndef CONSTANTS_H_
#define CONSTANTS_H_

/* RFC5498 */
#define MANET_PORT	269

/* in seconds */
#define HELLO_REFRESH_INTERVAL	2
#define TC_REFRESH_INTERVAL	5
#define HOLD_TIME		(3 * TC_REFRESH_INTERVAL)

#define TC_HOP_LIMIT	16

#define HYST_SCALING	0.4
#define HYST_LOW	0.3
#define HYST_HIGH	0.8

/* 1s */
#define MAX_JITTER	1000000

#define FLOODING_MPR_SELECTOR 1
#define ROUTING_MPR_SELECTOR  2

#define RFC5444_TLV_NODE_NAME 42

/* NHDP message TLV array index */
enum {
	IDX_TLV_ITIME,			/* Interval time */
	IDX_TLV_VTIME,			/* validity time */
	IDX_TLV_NODE_NAME,		/* name of the node */
};

/* NHDP address TLV array index */
enum {
	IDX_ADDRTLV_LOCAL_IF,		/* is local if */
	IDX_ADDRTLV_LINK_STATUS,	/* link status TODO */
	IDX_ADDRTLV_MPR,		/* neighbor selected as mpr */
	IDX_ADDRTLV_METRIC,		/* incomming link metric */
	IDX_ADDRTLV_NODE_NAME,		/* 'name' of a node from graph.gv */
};

#endif /* CONSTANTS_H_ */
