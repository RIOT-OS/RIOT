/*
 * mc1322x.h - mc1322x specific definitions
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#ifndef MC1322X_H
#define MC1322X_H 

#define F_CPU   (24000000)				///< CPU target speed in Hz

/* Timer registers are all 16-bit wide with 16-bit access only */
#define TMR_OFFSET      (0x20)
#define TMR_BASE        (0x80007000)
#define TMR0_BASE       (TMR_BASE)
#define TMR1_BASE       (TMR_BASE + TMR_OFFSET*1)
#define TMR2_BASE       (TMR_BASE + TMR_OFFSET*2)
#define TMR3_BASE       (TMR_BASE + TMR_OFFSET*3)

/* Structure-based register definitions */
/* Example use:
	TMR2->CTRL = 0x1234;
	TMR2->CTRLbits = (struct TMR_CTRL) {
		.DIR = 1,
		.OUTPUT_MODE = 2,
	};
	TMR2->CTRLbits.PRIMARY_CNT_SOURCE = 3;
*/

struct TMR_struct {
	uint16_t COMP1;
	uint16_t COMP2;
	uint16_t CAPT;
	uint16_t LOAD;
	uint16_t HOLD;
	uint16_t CNTR;
	union {
		uint16_t CTRL;
		struct TMR_CTRL {
			uint16_t OUTPUT_MODE:3;
			uint16_t CO_INIT:1;
			uint16_t DIR:1;
			uint16_t LENGTH:1;
			uint16_t ONCE:1;
			uint16_t SECONDARY_CNT_SOURCE:2;
			uint16_t PRIMARY_CNT_SOURCE:4;
			uint16_t COUNT_MODE:3;
		} CTRLbits;
	};
	union {
		uint16_t SCTRL;
		struct TMR_SCTRL {
			uint16_t OEN:1;
			uint16_t OPS:1;
			uint16_t FORCE:1;
			uint16_t VAL:1;
			uint16_t EEOF:1;
			uint16_t MSTR:1;
			uint16_t CAPTURE_MODE:2;
			uint16_t INPUT:1;
			uint16_t IPS:1;
			uint16_t IEFIE:1;
			uint16_t IEF:1;
			uint16_t TOFIE:1;
			uint16_t TOF:1;
			uint16_t TCFIE:1;
			uint16_t TCF:1;
		} SCTRLbits;
	};
	uint16_t CMPLD1;
	uint16_t CMPLD2;
	union {
		uint16_t CSCTRL;
		struct TMR_CSCTRL {
			uint16_t CL1:2;
			uint16_t CL2:2;
			uint16_t TCF1:1;
			uint16_t TCF2:1;
			uint16_t TCF1EN:1;
			uint16_t TCF2EN:1;
			uint16_t :5;
			uint16_t FILT_EN:1;
			uint16_t DBG_EN:2;
		} CSCTRLbits;
	};

	uint16_t reserved[4];

	union {
		uint16_t ENBL;
		struct TMR_ENBL {
			union {
				struct {
					uint16_t ENBL:4;
				};
				struct {
					uint16_t ENBL3:1;
					uint16_t ENBL2:1;
					uint16_t ENBL1:1;
					uint16_t ENBL0:1;
				};
			};
			uint16_t :12;
		} ENBLbits;
	};
};

static volatile struct TMR_struct * const TMR0 = (void *) (TMR0_BASE);
static volatile struct TMR_struct * const TMR1 = (void *) (TMR1_BASE);
static volatile struct TMR_struct * const TMR2 = (void *) (TMR2_BASE);
static volatile struct TMR_struct * const TMR3 = (void *) (TMR3_BASE);

/* Get timer pointer from timer number */
#define TMR_ADDR(x) (*(volatile struct TMR_struct *)(((uint32_t)(x) * TMR_OFFSET) + TMR_BASE))

/* Get timer number from the timer pointer. */
#define TMR_NUM(x) (((uint32_t)(x) - TMR_BASE) / TMR_OFFSET)

/* Used to compute which enable bit to set for a particular timer, e.g.
     TMR0.ENBL |= TMR_ENABLE_BIT(TMR2);
   Helpful when you're using macros to define timers
*/
#define TMR_ENABLE_BIT(x) (1 << TMR_NUM(x))

#define TMR0_PIN GPIO_08
#define TMR1_PIN GPIO_09
#define TMR2_PIN GPIO_10
#define TMR3_PIN GPIO_11

#define TMR_REGOFF_COMP1    (0x0)
#define TMR_REGOFF_COMP2    (0x2)
#define TMR_REGOFF_CAPT     (0x4)
#define TMR_REGOFF_LOAD     (0x6)
#define TMR_REGOFF_HOLD     (0x8)
#define TMR_REGOFF_CNTR     (0xa)
#define TMR_REGOFF_CTRL     (0xc)
#define TMR_REGOFF_SCTRL    (0xe)
#define TMR_REGOFF_CMPLD1   (0x10)
#define TMR_REGOFF_CMPLD2   (0x12)
#define TMR_REGOFF_CSCTRL   (0x14)
#define TMR_REGOFF_ENBL     (0x1e)

/* one enable register to rule them all */
#define TMR_ENBL     (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_ENBL))

/* Timer 0 registers */
#define TMR0_COMP1   (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_COMP1))
#define TMR0_COMP_UP TMR0_COMP1
#define TMR0_COMP2   (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_COMP2))
#define TMR0_COMP_DOWN TMR0_COMP2
#define TMR0_CAPT    (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_CAPT))
#define TMR0_LOAD    (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_LOAD))
#define TMR0_HOLD    (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_HOLD))
#define TMR0_CNTR    (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_CNTR))
#define TMR0_CTRL    (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_CTRL))
#define TMR0_SCTRL   (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_SCTRL))
#define TMR0_CMPLD1  (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_CMPLD1))
#define TMR0_CMPLD2  (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_CMPLD2))
#define TMR0_CSCTRL  (*(volatile uint16_t *) (TMR0_BASE + TMR_REGOFF_CSCTRL))

/* Timer 1 registers */
#define TMR1_COMP1   (*(volatile uint16_t *) (TMR1_BASE + TMR_REGOFF_COMP1))
#define TMR1_COMP_UP TMR1_COMP1
#define TMR1_COMP2   (*(volatile uint16_t *) (TMR1_BASE + TMR_REGOFF_COMP2))
#define TMR1_COMP_DOWN TMR1_COMP2
#define TMR1_CAPT    (*(volatile uint16_t *) (TMR1_BASE + TMR_REGOFF_CAPT))
#define TMR1_LOAD    (*(volatile uint16_t *) (TMR1_BASE + TMR_REGOFF_LOAD))
#define TMR1_HOLD    (*(volatile uint16_t *) (TMR1_BASE + TMR_REGOFF_HOLD))
#define TMR1_CNTR    (*(volatile uint16_t *) (TMR1_BASE + TMR_REGOFF_CNTR))
#define TMR1_CTRL    (*(volatile uint16_t *) (TMR1_BASE + TMR_REGOFF_CTRL))
#define TMR1_SCTRL   (*(volatile uint16_t *) (TMR1_BASE + TMR_REGOFF_SCTRL))
#define TMR1_CMPLD1  (*(volatile uint16_t *) (TMR1_BASE + TMR_REGOFF_CMPLD1))
#define TMR1_CMPLD2  (*(volatile uint16_t *) (TMR1_BASE + TMR_REGOFF_CMPLD2))
#define TMR1_CSCTRL  (*(volatile uint16_t *) (TMR1_BASE + TMR_REGOFF_CSCTRL))

/* Timer 2 registers */
#define TMR2_COMP1   (*(volatile uint16_t *) (TMR2_BASE + TMR_REGOFF_COMP1))
#define TMR2_COMP_UP TMR2_COMP1
#define TMR2_COMP2   (*(volatile uint16_t *) (TMR2_BASE + TMR_REGOFF_COMP2))
#define TMR2_COMP_DOWN TMR2_COMP2
#define TMR2_CAPT    (*(volatile uint16_t *) (TMR2_BASE + TMR_REGOFF_CAPT))
#define TMR2_LOAD    (*(volatile uint16_t *) (TMR2_BASE + TMR_REGOFF_LOAD))
#define TMR2_HOLD    (*(volatile uint16_t *) (TMR2_BASE + TMR_REGOFF_HOLD))
#define TMR2_CNTR    (*(volatile uint16_t *) (TMR2_BASE + TMR_REGOFF_CNTR))
#define TMR2_CTRL    (*(volatile uint16_t *) (TMR2_BASE + TMR_REGOFF_CTRL))
#define TMR2_SCTRL   (*(volatile uint16_t *) (TMR2_BASE + TMR_REGOFF_SCTRL))
#define TMR2_CMPLD1  (*(volatile uint16_t *) (TMR2_BASE + TMR_REGOFF_CMPLD1))
#define TMR2_CMPLD2  (*(volatile uint16_t *) (TMR2_BASE + TMR_REGOFF_CMPLD2))
#define TMR2_CSCTRL  (*(volatile uint16_t *) (TMR2_BASE + TMR_REGOFF_CSCTRL))

/* Timer 3 registers */
#define TMR3_COMP1   (*(volatile uint16_t *) (TMR3_BASE + TMR_REGOFF_COMP1))
#define TMR3_COMP_UP TMR3_COMP1
#define TMR3_COMP2   (*(volatile uint16_t *) (TMR3_BASE + TMR_REGOFF_COMP2))
#define TMR3_COMP_DOWN TMR3_COMP2
#define TMR3_CAPT    (*(volatile uint16_t *) (TMR3_BASE + TMR_REGOFF_CAPT))
#define TMR3_LOAD    (*(volatile uint16_t *) (TMR3_BASE + TMR_REGOFF_LOAD))
#define TMR3_HOLD    (*(volatile uint16_t *) (TMR3_BASE + TMR_REGOFF_HOLD))
#define TMR3_CNTR    (*(volatile uint16_t *) (TMR3_BASE + TMR_REGOFF_CNTR))
#define TMR3_CTRL    (*(volatile uint16_t *) (TMR3_BASE + TMR_REGOFF_CTRL))
#define TMR3_SCTRL   (*(volatile uint16_t *) (TMR3_BASE + TMR_REGOFF_SCTRL))
#define TMR3_CMPLD1  (*(volatile uint16_t *) (TMR3_BASE + TMR_REGOFF_CMPLD1))
#define TMR3_CMPLD2  (*(volatile uint16_t *) (TMR3_BASE + TMR_REGOFF_CMPLD2))
#define TMR3_CSCTRL  (*(volatile uint16_t *) (TMR3_BASE + TMR_REGOFF_CSCTRL))
#define TMR(num, reg)  CAT2(TMR,num,_##reg)
#endif /* MC1322X_H */
