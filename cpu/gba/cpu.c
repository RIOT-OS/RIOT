
/**
 * @ingroup     cpu_gba
 * @{
 */

#include <stdint.h>
#include "gba_irq.h"
#include "cpu.h"
#include "kernel_init.h"

#include "stdio_base.h"

void arm_reset(void)
{
    while (1)
    {
    }
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    extern void board_init(void);

    init_isr();

    /* board specific setup of i/o pins */
    board_init();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();
}

/** @} */
