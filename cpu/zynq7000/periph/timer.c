/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer driver implementation for the zynq7000 CPU
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"

#include "periph/timer.h"

#include "zynq_interrupts.h"
#include "periph_conf.h"
#include "hwtimer_cpu.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

typedef struct {
    void (*cb)(int);
} timer_conf_t;

/**
 * @brief Timer state memory
 */
timer_conf_t config[TIMER_NUMOF];

#define PRESCALER

#define COUNTER_CONTROL_INIT		0x38 /* enable, increment, match mode, reset, wave disabled */
#define COUNTER_CONTROL_DIS_MASK	0x1
#define COUNTER_CONTROL_RST_MASK	0x10
#define CHANNEL_1_MATCH_INT			0x2
#define CHANNEL_2_MATCH_INT			0x4
#define CHANNEL_3_MATCH_INT			0x8

#if TIMER_0_EN
void TIMER_0_ISR(void *arg);
#endif

#if TIMER_1_EN
void TIMER_1_ISR(void *arg);
#endif

#if TIMER_2_EN
void TIMER_2_ISR(void *arg);
#endif

#if TIMER_3_EN
void TIMER_3_ISR(void *arg);
#endif

/**
 * @brief Setup the given timer
 */
int timer_init(tim_t dev, unsigned int us_per_ticks, void (*callback)(int)) {

    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        if (!TIMER_0_DEV->counter_control_1 & COUNTER_CONTROL_DIS_MASK) {
            return 0;
        }
        /* set clk input source + prescaler */
        TIMER_0_DEV->clock_control_1 = (HWTIMER_PRESCALER_X << 1) | 1;

        /* reset timer */
        TIMER_0_DEV->counter_control_1 = COUNTER_CONTROL_INIT;

        /* disable channel interrupts */
        TIMER_0_DEV->interrupt_enable_1 = 0;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        if (!TIMER_1_DEV->counter_control_2 & COUNTER_CONTROL_DIS_MASK) {
            return 0;
        }
        /* set clk input source + prescaler */
        TIMER_1_DEV->clock_control_2 = (HWTIMER_PRESCALER_X << 1) | 1;;

        /* reset timer */
        TIMER_1_DEV->counter_control_2 = COUNTER_CONTROL_INIT;

        /* disable channel interrupts */
        TIMER_1_DEV->interrupt_enable_2 = 0;
        break;
#endif
#if TIMER_2_EN
    case TIMER_2:
        if (!TIMER_2_DEV->counter_control_3 & COUNTER_CONTROL_DIS_MASK) {
            return 0;
        }
        /* set clk input source + prescaler */
        TIMER_2_DEV->clock_control_3 = (HWTIMER_PRESCALER_X << 1) | 1;;

        /* reset timer */
        TIMER_2_DEV->counter_control_3 = COUNTER_CONTROL_INIT;

        /* disable channel interrupts */
        TIMER_2_DEV->interrupt_enable_3 = 0;
        break;
#endif
#if TIMER_3_EN
    case TIMER_3:
        if (!TIMER_3_DEV->counter_control_1 & COUNTER_CONTROL_DIS_MASK) {
            return 0;
        }
        /* set clk input source + prescaler */
        TIMER_3_DEV->clock_control_1 = (HWTIMER_PRESCALER_X << 1) | 1;;

        /* reset timer */
        TIMER_3_DEV->counter_control_1 = COUNTER_CONTROL_INIT;

        /* disable channel interrupts */
        TIMER_3_DEV->interrupt_enable_1 = 0;
        break;
#endif
    case TIMER_UNDEFINED:
    default:
        return -1;
    }

    /* save callback */
    config[dev].cb = callback;

    /* enable interrupts for given timer */
    timer_irq_enable(dev);

    timer_start(dev);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout) {
    return timer_set_absolute(dev, channel, timer_read(dev) + timeout);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value) {
    DEBUG("Setting timer %i channel %i to %i\n", dev, channel, value);

    /* set match value
     * enable interrupt */
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        switch (channel) {
        case 0:
            TIMER_0_DEV->match_1_counter_1 = value;
            TIMER_0_DEV->interrupt_enable_1 |= CHANNEL_1_MATCH_INT;
            break;
        case 1:
            TIMER_0_DEV->match_2_counter_1 = value;
            TIMER_0_DEV->interrupt_enable_1 |= CHANNEL_2_MATCH_INT;
            break;
        case 2:
            TIMER_0_DEV->match_3_counter_1 = value;
            TIMER_0_DEV->interrupt_enable_1 |= CHANNEL_3_MATCH_INT;
            break;
        default:
            return -1;
        }
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        switch (channel) {
        case 0:
            TIMER_1_DEV->match_1_counter_2 = value;
            TIMER_1_DEV->interrupt_enable_2 |= CHANNEL_1_MATCH_INT;
            break;
        case 1:
            TIMER_1_DEV->match_2_counter_2 = value;
            TIMER_1_DEV->interrupt_enable_2 |= CHANNEL_2_MATCH_INT;
            break;
        case 2:
            TIMER_1_DEV->match_3_counter_2 = value;
            TIMER_1_DEV->interrupt_enable_2 |= CHANNEL_3_MATCH_INT;
            break;
        default:
            return -1;
        }
        break;
#endif
#if TIMER_2_EN
    case TIMER_2:
        switch (channel) {
        case 0:
            TIMER_2_DEV->match_1_counter_3 = value;
            TIMER_2_DEV->interrupt_enable_3 |= CHANNEL_1_MATCH_INT;
            break;
        case 1:
            TIMER_2_DEV->match_2_counter_3 = value;
            TIMER_2_DEV->interrupt_enable_3 |= CHANNEL_2_MATCH_INT;
            break;
        case 2:
            TIMER_2_DEV->match_3_counter_3 = value;
            TIMER_2_DEV->interrupt_enable_3 |= CHANNEL_3_MATCH_INT;
            break;
        default:
            return -1;
        }
        break;
#endif
#if TIMER_3_EN
    case TIMER_3:
        switch (channel) {
        case 0:
            TIMER_3_DEV->match_1_counter_1 = value;
            TIMER_3_DEV->interrupt_enable_1 |= CHANNEL_1_MATCH_INT;
            break;
        case 1:
            TIMER_3_DEV->match_2_counter_1 = value;
            TIMER_3_DEV->interrupt_enable_1 |= CHANNEL_2_MATCH_INT;
            break;
        case 2:
            TIMER_3_DEV->match_3_counter_1 = value;
            TIMER_3_DEV->interrupt_enable_1 |= CHANNEL_3_MATCH_INT;
            break;
        default:
            return -1;
        }
        break;
#endif
    case TIMER_UNDEFINED:
    default:
        return -1;
    }

    return 1;
}

int timer_clear(tim_t dev, int channel) {
    /* get timer base register address */
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        switch (channel) {
        case 0:
            TIMER_0_DEV->interrupt_enable_1 &= ~CHANNEL_1_MATCH_INT;
            break;
        case 1:
            TIMER_0_DEV->interrupt_enable_1 &= ~CHANNEL_2_MATCH_INT;
            break;
        case 2:
            TIMER_0_DEV->interrupt_enable_1 &= ~CHANNEL_3_MATCH_INT;
            break;
        default:
            return -1;
        }
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        switch (channel) {
        case 0:
            TIMER_1_DEV->interrupt_enable_2 &= ~CHANNEL_1_MATCH_INT;
            break;
        case 1:
            TIMER_1_DEV->interrupt_enable_2 &= ~CHANNEL_2_MATCH_INT;
            break;
        case 2:
            TIMER_1_DEV->interrupt_enable_2 &= ~CHANNEL_3_MATCH_INT;
            break;
        default:
            return -1;
        }
        break;
#endif
#if TIMER_2_EN
    case TIMER_2:
        switch (channel) {
        case 0:
            TIMER_2_DEV->interrupt_enable_3 &= ~CHANNEL_1_MATCH_INT;
            break;
        case 1:
            TIMER_2_DEV->interrupt_enable_3 &= ~CHANNEL_2_MATCH_INT;
            break;
        case 2:
            TIMER_2_DEV->interrupt_enable_3 &= ~CHANNEL_3_MATCH_INT;
            break;
        default:
            return -1;
        }
        break;
#endif
#if TIMER_3_EN
    case TIMER_3:
        switch (channel) {
        case 0:
            TIMER_3_DEV->interrupt_enable_1 &= ~CHANNEL_1_MATCH_INT;
            break;
        case 1:
            TIMER_3_DEV->interrupt_enable_1 &= ~CHANNEL_2_MATCH_INT;
            break;
        case 2:
            TIMER_3_DEV->interrupt_enable_1 &= ~CHANNEL_3_MATCH_INT;
            break;
        default:
            return -1;
        }
        break;
#endif
    case TIMER_UNDEFINED:
    default:
        return -1;
    }

    return 1;
}

unsigned int timer_read(tim_t dev) {
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        return TIMER_0_DEV->counter_value_1;
#endif
#if TIMER_1_EN
    case TIMER_1:
        return TIMER_1_DEV->counter_value_2;
#endif
#if TIMER_2_EN
    case TIMER_2:
        return TIMER_2_DEV->counter_value_3;
#endif
#if TIMER_3_EN
    case TIMER_3:
        return TIMER_3_DEV->counter_value_1;
#endif
    default:
        return 0;
    }

}

void timer_stop(tim_t dev) {
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        TIMER_0_DEV->counter_control_1 |= COUNTER_CONTROL_DIS_MASK;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        TIMER_1_DEV->counter_control_2 |= COUNTER_CONTROL_DIS_MASK;
        break;
#endif
#if TIMER_2_EN
    case TIMER_2:
        TIMER_2_DEV->counter_control_3 |= COUNTER_CONTROL_DIS_MASK;
        break;
#endif
#if TIMER_3_EN
    case TIMER_3:
        TIMER_3_DEV->counter_control_1 |= COUNTER_CONTROL_DIS_MASK;
        break;
#endif
    case TIMER_UNDEFINED:
        break;
    }
}

void timer_start(tim_t dev) {
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        TIMER_0_DEV->counter_control_1 &= ~COUNTER_CONTROL_DIS_MASK;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        TIMER_1_DEV->counter_control_2 &= ~COUNTER_CONTROL_DIS_MASK;
        break;
#endif
#if TIMER_2_EN
    case TIMER_2:
        TIMER_2_DEV->counter_control_3 &= ~COUNTER_CONTROL_DIS_MASK;
        break;
#endif
#if TIMER_3_EN
    case TIMER_3:
        TIMER_3_DEV->counter_control_1 &= ~COUNTER_CONTROL_DIS_MASK;
        break;
#endif
    case TIMER_UNDEFINED:
        break;
    }
}

void timer_irq_enable(tim_t dev) {
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
    	register_interrupt_handler(TTC0_T0_INT_ID, INT_HIGH_LEVEL,&TIMER_0_ISR, NULL);
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
    	register_interrupt_handler(TTC0_T1_INT_ID, INT_HIGH_LEVEL,&TIMER_1_ISR, NULL);
        break;
#endif
#if TIMER_2_EN
    case TIMER_2:
    	register_interrupt_handler(TTC0_T2_INT_ID, INT_HIGH_LEVEL,&TIMER_2_ISR, NULL);
        break;
#endif
#if TIMER_3_EN
    case TIMER_3:
    	register_interrupt_handler(TTC1_T0_INT_ID, INT_HIGH_LEVEL,&TIMER_3_ISR, NULL);
        break;
#endif
    case TIMER_UNDEFINED:
        break;
    }
}

void timer_irq_disable(tim_t dev) {
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
    	unregister_interrupt_handler(TTC0_T0_INT_ID);
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
    	unregister_interrupt_handler(TTC0_T1_INT_ID);
        break;
#endif
#if TIMER_2_EN
    case TIMER_2:
    	unregister_interrupt_handler(TTC0_T2_INT_ID);
        break;
#endif
#if TIMER_3_EN
    case TIMER_3:
    	unregister_interrupt_handler(TTC1_T0_INT_ID);
        break;
#endif
    case TIMER_UNDEFINED:
        break;
    }
}

void timer_reset(tim_t dev) {
    switch (dev) {
#if TIMER_0_EN
    case TIMER_0:
        TIMER_0_DEV->counter_control_1 |= COUNTER_CONTROL_RST_MASK;
        break;
#endif
#if TIMER_1_EN
    case TIMER_1:
        TIMER_1_DEV->counter_control_2 |= COUNTER_CONTROL_RST_MASK;
        break;
#endif
#if TIMER_2_EN
    case TIMER_2:
        TIMER_2_DEV->counter_control_3 |= COUNTER_CONTROL_RST_MASK;
        break;
#endif
#if TIMER_3_EN
    case TIMER_3:
        TIMER_3_DEV->counter_control_1 |= COUNTER_CONTROL_RST_MASK;
        break;
#endif
    case TIMER_UNDEFINED:
        break;
    }
}

#if TIMER_0_EN
void TIMER_0_ISR(void *arg) {
    uint32_t isr = TIMER_0_DEV->interrupt_register_1;
    isr &= TIMER_0_DEV->interrupt_enable_1;

    if (isr & CHANNEL_1_MATCH_INT) {
        if (config[TIMER_0].cb) {
            TIMER_0_DEV->interrupt_enable_1 &= ~CHANNEL_1_MATCH_INT;
            config[TIMER_0].cb(0);
        }
    }
    if (isr & CHANNEL_2_MATCH_INT) {
        if (config[TIMER_0].cb) {
            TIMER_0_DEV->interrupt_enable_1 &= ~CHANNEL_2_MATCH_INT;
            config[TIMER_0].cb(1);
        }
    }
    if (isr & CHANNEL_3_MATCH_INT) {
        if (config[TIMER_0].cb) {
            TIMER_0_DEV->interrupt_enable_1 &= ~CHANNEL_3_MATCH_INT;
            config[TIMER_0].cb(2);
        }
    }
}
#endif /* TIMER_0_EN */

#if TIMER_1_EN
void TIMER_1_ISR(void *arg) {
    uint32_t isr = TIMER_1_DEV->interrupt_register_2;
    isr &= TIMER_1_DEV->interrupt_enable_2;

    if (isr & CHANNEL_1_MATCH_INT) {
        if (config[TIMER_1].cb) {
            TIMER_1_DEV->interrupt_enable_2 &= ~CHANNEL_1_MATCH_INT;
            config[TIMER_1].cb(0);
        }
    }
    if (isr & CHANNEL_2_MATCH_INT) {
        if (config[TIMER_1].cb) {
            TIMER_1_DEV->interrupt_enable_2 &= ~CHANNEL_2_MATCH_INT;
            config[TIMER_1].cb(1);
        }
    }
    if (isr & CHANNEL_3_MATCH_INT) {
        if (config[TIMER_1].cb) {
            TIMER_1_DEV->interrupt_enable_2 &= ~CHANNEL_3_MATCH_INT;
            config[TIMER_1].cb(2);
        }
    }
}
#endif /* TIMER_1_EN */

#if TIMER_2_EN
void TIMER_2_ISR(void *arg) {
    uint32_t isr = TIMER_2_DEV->interrupt_register_3;
    isr &= TIMER_2_DEV->interrupt_enable_3;

    if (isr & CHANNEL_1_MATCH_INT) {
        if (config[TIMER_2].cb) {
            TIMER_2_DEV->interrupt_enable_3 &= ~CHANNEL_1_MATCH_INT;
            config[TIMER_2].cb(0);
        }
    }
    if (isr & CHANNEL_2_MATCH_INT) {
        if (config[TIMER_2].cb) {
            TIMER_2_DEV->interrupt_enable_3 &= ~CHANNEL_2_MATCH_INT;
            config[TIMER_2].cb(1);
        }
    }
    if (isr & CHANNEL_3_MATCH_INT) {
        if (config[TIMER_2].cb) {
            TIMER_2_DEV->interrupt_enable_3 &= ~CHANNEL_3_MATCH_INT;
            config[TIMER_2].cb(2);
        }
    }
}
#endif /* TIMER_2_EN */

#if TIMER_3_EN
void TIMER_3_ISR(void *arg) {
    uint32_t isr = TIMER_3_DEV->interrupt_register_1;
    isr &= TIMER_3_DEV->interrupt_enable_1;

    if (isr & CHANNEL_1_MATCH_INT) {
        if (config[TIMER_3].cb) {
            TIMER_3_DEV->interrupt_enable_1 &= ~CHANNEL_1_MATCH_INT;
            config[TIMER_3].cb(0);
        }
    }
    if (isr & CHANNEL_2_MATCH_INT) {
        if (config[TIMER_3].cb) {
            TIMER_3_DEV->interrupt_enable_1 &= ~CHANNEL_2_MATCH_INT;
            config[TIMER_3].cb(1);
        }
    }
    if (isr & CHANNEL_3_MATCH_INT) {
        if (config[TIMER_3].cb) {
            TIMER_3_DEV->interrupt_enable_1 &= ~CHANNEL_3_MATCH_INT;
            config[TIMER_3].cb(2);
        }
    }
}
#endif /* TIMER_3_EN */

