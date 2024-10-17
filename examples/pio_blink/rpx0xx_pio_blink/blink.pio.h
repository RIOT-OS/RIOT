#ifndef PIO_BLINK_H
#define PIO_BLINK_H

#include <stdbool.h>
#include "periph/pio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* blink */
#define PIO_BLINK_SIDESET_COUNT       0
#define PIO_BLINK_SIDESET_OPTIONAL    false
#define PIO_BLINK_SIDESET_PINDIRS     false
#define PIO_BLINK_WRAP_BOTTOM         0
#define PIO_BLINK_WRAP_TOP            10

#define PIO_BLINK_PROGRAM { \
    /* .wrap_target */ \
    0xff00, /*  0: set    pins, 0                [31] */ \
    0xbf42, /*  1: nop                           [31] */ \
    0xbf42, /*  2: nop                           [31] */ \
    0xbf42, /*  3: nop                           [31] */ \
    0xbf42, /*  4: nop                           [31] */ \
    0xff01, /*  5: set    pins, 1                [31] */ \
    0xbf42, /*  6: nop                           [31] */ \
    0xbf42, /*  7: nop                           [31] */ \
    0xbf42, /*  8: nop                           [31] */ \
    0xbf42, /*  9: nop                           [31] */ \
    0x0000  /* 10: jmp    0                           */ \
    /* .wrap */ \
}
#define PIO_BLINK_PROGRAM_LENGTH 11
#ifndef PIO_BLINK_LABEL_entry_point
#define PIO_BLINK_LABEL_entry_point 0
#endif
#define PIO_BLINK_PROGRAM_CONF { \
    PIO_BLINK_LABEL_entry_point, \
    PIO_BLINK_WRAP_BOTTOM, \
    PIO_BLINK_WRAP_TOP, \
    PIO_BLINK_SIDESET_COUNT, \
    PIO_BLINK_SIDESET_OPTIONAL, \
    PIO_BLINK_SIDESET_PINDIRS \
}

static inline pio_program_t pio_blink_create_program(void)
{
    pio_program_t pro = {
        .instr_numof = PIO_BLINK_PROGRAM_LENGTH,
        .location = PIO_PROGRAM_NOT_LOADED
    };
    return pro;
}
#ifdef __cplusplus
}
#endif
#endif /* PIO_BLINK_H */
