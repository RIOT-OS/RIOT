#ifndef __DISPATCH_VALUES_H
#define __DISPATCH_VALUES_H

// identifies a valid sixlowpan packet (mask for the MSB)
#define VALID_LOWPAN_DISPATCH_HEADER (0x3)

// identifies a clocksync evaluation beacon
#define CLOCKSYNC_EVAL_PROTOCOL_DISPATCH (0x19)

// identifies a topology beacon
#define NETWORK_TOPOLOGY_DISPATCH (0x20)

// identifies a GTSP beacon
#define GTSP_PROTOCOL_DISPATCH (0x21)

// identifies a FTSP beacon
#define FTSP_PROTOCOL_DISPATCH (0x22)

// identifies a PULSESYNC beacon
#define PULSESYNC_PROTOCOL_DISPATCH (0x23)


#endif /* __DISPATCH_VALUES_H */
