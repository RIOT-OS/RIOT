// See LICENSE for license details.

#ifndef VENDOR_PLIC_H
#define VENDOR_PLIC_H


// 32 bits per source
#define PLIC_PRIORITY_OFFSET            (0x0000)
#define PLIC_PRIORITY_SHIFT_PER_SOURCE  2
// 1 bit per source (1 address)
#define PLIC_PENDING_OFFSET             (0x1000)
#define PLIC_PENDING_SHIFT_PER_SOURCE   0

//0x80 per target
#define PLIC_ENABLE_OFFSET              (0x2000)
#define PLIC_ENABLE_SHIFT_PER_TARGET    7


#define PLIC_THRESHOLD_OFFSET           (0x200000)
#define PLIC_CLAIM_OFFSET               (0x200004)
#define PLIC_THRESHOLD_SHIFT_PER_TARGET 12
#define PLIC_CLAIM_SHIFT_PER_TARGET     12

#define PLIC_MAX_SOURCE                 1023
#define PLIC_SOURCE_MASK                0x3FF

#define PLIC_MAX_TARGET                 15871
#define PLIC_TARGET_MASK                0x3FFF

#endif /* VENDOR_PLIC_H */
