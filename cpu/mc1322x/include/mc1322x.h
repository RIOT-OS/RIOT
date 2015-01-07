/*
 * mc1322x.h - mc1322x specific definitions
 * Copyright (C) 2013, 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MC1322X_H
#define MC1322X_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------*/
/* System Management */
#define SW_RST_VAL       (0x87651234)

#define CRM_BASE         (0x80003000)

/* Structure-based CRM access */
struct CRM_struct {
    union {
        uint32_t SYS_CNTL;
        struct CRM_SYS_CNTL {
            uint32_t PWR_SOURCE:2;
            uint32_t PADS_1P8V_SEL:1;
            uint32_t :1;
            uint32_t JTAG_SECU_OFF:1;
            uint32_t XTAL32_EXISTS:1;
            uint32_t :2;
            uint32_t XTAL_CLKDIV:6;
            uint32_t :18;
        } SYS_CNTLbits;
    };
    union {
        uint32_t WU_CNTL;
        struct CRM_WU_CNTL {
            uint32_t TIMER_WU_EN:1;
            uint32_t RTC_WU_EN:1;
            uint32_t HOST_WAKE:1;
            uint32_t AUTO_ADC:1;
            uint32_t EXT_WU_EN:4;
            uint32_t EXT_WU_EDGE:4;
            uint32_t EXT_WU_POL:4;
            uint32_t TIMER_WU_IEN:1;
            uint32_t RTC_WU_IEN:1;
            uint32_t :2;
            uint32_t EXT_WU_IEN:4;
            uint32_t :4;
            uint32_t EXT_OUT_POL:4;
        } WU_CNTLbits;
    };
    union {
        uint32_t SLEEP_CNTL;
        struct CRM_SLEEP_CNTL {
            uint32_t HIB:1;
            uint32_t DOZE:1;
            uint32_t :2;
            uint32_t RAM_RET:2;
            uint32_t MCU_RET:1;
            uint32_t DIG_PAD_EN:1;
            uint32_t :24;
        } SLEEP_CNTLbits;
    };
    union {
        uint32_t BS_CNTL;
        struct CRM_BS_CNTL {
            uint32_t BS_EN:1;
            uint32_t WAIT4IRQ:1;
            uint32_t BS_MAN_EN:1;
            uint32_t :2;
            uint32_t ARM_OFF_TIME:6;
            uint32_t :18;
        } BS_CNTLbits;
    };
    union {
        uint32_t COP_CNTL;
        struct CRM_COP_CNTL {
            uint32_t COP_EN:1;
            uint32_t COP_OUT:1;
            uint32_t COP_WP:1;
            uint32_t :5;
            uint32_t COP_TIMEOUT:7;
            uint32_t :1;
            uint32_t COP_COUNT:7;
            uint32_t :9;
        } COP_CNTLbits;
    };
    uint32_t COP_SERVICE;
    union {
        uint32_t STATUS;
        struct CRM_STATUS {
            uint32_t SLEEP_SYNC:1;
            uint32_t HIB_WU_EVT:1;
            uint32_t DOZE_WU_EVT:1;
            uint32_t RTC_WU_EVT:1;
            uint32_t EXT_WU_EVT:4;
            uint32_t :1;
            uint32_t CAL_DONE:1;
            uint32_t COP_EVT:1;
            uint32_t :6;
            uint32_t VREG_BUCK_RDY:1;
            uint32_t VREG_1P8V_RDY:1;
            uint32_t VREG_1P5V_RDY:1;
            uint32_t :12;
        } STATUSbits;
    };
    union {
        uint32_t MOD_STATUS;
        struct CRM_MOD_STATUS {
            uint32_t ARM_EN:1;
            uint32_t MACA_EN:1;
            uint32_t ASM_EN:1;
            uint32_t SPI_EN:1;
            uint32_t GPIO_EN:1;
            uint32_t UART1_EN:1;
            uint32_t UART2_EN:1;
            uint32_t TMR_EN:1;
            uint32_t RIF_EN:1;
            uint32_t I2C_EN:1;
            uint32_t SSI_EN:1;
            uint32_t SPIF_EN:1;
            uint32_t ADC_EN:1;
            uint32_t :1;
            uint32_t JTA_EN:1;
            uint32_t NEX_EN:1;
            uint32_t :1;
            uint32_t AIM_EN:1;
            uint32_t :14;
        } MOD_STATUSbits;
    };
    uint32_t WU_COUNT;
    uint32_t WU_TIMEOUT;
    uint32_t RTC_COUNT;
    uint32_t RTC_TIMEOUT;
    uint32_t reserved1;
    union {
        uint32_t CAL_CNTL;
        struct CRM_CAL_CNTL {
            uint32_t CAL_TIMEOUT:16;
            uint32_t CAL_EN:1;
            uint32_t CAL_IEN:1;
            uint32_t :14;
        } CAL_CNTLbits;
    };
    uint32_t CAL_COUNT;
    union {
        uint32_t RINGOSC_CNTL;
        struct CRM_RINGOSC_CNTL {
            uint32_t ROSC_EN:1;
            uint32_t :3;
            uint32_t ROSC_FTUNE:5;
            uint32_t ROSC_CTUNE:4;
            uint32_t :19;
        } RINGOSC_CNTLbits;
    };
    union {
        uint32_t XTAL_CNTL;
        struct CRM_XTAL_CNTL {
            uint32_t :8;
            uint32_t XTAL_IBIAS_SEL:4;
            uint32_t :4;
            uint32_t XTAL_FTUNE:5;
            uint32_t XTAL_CTUNE:5;
            uint32_t :6;
        } XTAL_CNTLbits;
    };
    union {
        uint32_t XTAL32_CNTL;
        struct CRM_XTAL32_CNTL {
            uint32_t XTAL32_EN:1;
            uint32_t :3;
            uint32_t XTAL32_GAIN:2;
            uint32_t :26;
        } XTAL32_CNTLbits;
    };
    union {
        uint32_t VREG_CNTL;
        struct CRM_VREG_CNTL {
            uint32_t BUCK_EN:1;
            uint32_t BUCK_SYNC_REC_EN:1;
            uint32_t BUCK_BYPASS_EN:1;
            uint32_t VREG_1P5V_EN:2;
            uint32_t VREG_1P5V_SEL:2;
            uint32_t VREG_1P8V_EN:1;
            uint32_t BUCK_CLKDIV:4;
            uint32_t :20;
        } VREG_CNTLbits;
    };
    uint32_t reserved2;
    uint32_t SW_RST;
    uint32_t reserved3;
    uint32_t reserved4;
    uint32_t reserved5;
    uint32_t reserved6;
};

static volatile struct CRM_struct * const CRM = (void*) (CRM_BASE);


/*-----------------------------------------------------------------*/
/* TIMERS */

/* Timer registers are all 16-bit wide with 16-bit access only */
#define TMR_OFFSET      (0x20)
#define TMR_BASE        (0x80007000)
#define TMR0_BASE       (TMR_BASE)
#define TMR1_BASE       (TMR_BASE + TMR_OFFSET*1)
#define TMR2_BASE       (TMR_BASE + TMR_OFFSET*2)
#define TMR3_BASE       (TMR_BASE + TMR_OFFSET*3)

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

/*-----------------------------------------------------------------*/
/* Interrupts */
#define INTBASE        (0x80020000)

/* Structure-based ITC access */
#define __INTERRUPT_union(x)              \
                union {                   \
                        uint32_t x;       \
                        struct ITC_##x {  \
                        uint32_t ASM:1;   \
                        uint32_t UART1:1; \
                        uint32_t UART2:1; \
                        uint32_t CRM:1;   \
                        uint32_t I2C:1;   \
                        uint32_t TMR:1;   \
                        uint32_t SPIF:1;  \
                        uint32_t MACA:1;  \
                        uint32_t SSI:1;   \
                        uint32_t ADC:1;   \
                        uint32_t SPI:1;   \
                        uint32_t :21;     \
                        } x##bits; \
                };

struct ITC_struct {
        union {
                uint32_t INTCNTL;
                struct ITC_INTCNTL {
                        uint32_t :19;
                        uint32_t FIAD:1;
                        uint32_t NIAD:1;
                        uint32_t :11;
                } INTCNTLbits;
        };
        uint32_t NIMASK;
        uint32_t INTENNUM;
        uint32_t INTDISNUM;
        __INTERRUPT_union(INTENABLE);
        __INTERRUPT_union(INTTYPE);
        uint32_t reserved[4];
        uint32_t NIVECTOR;
        uint32_t FIVECTOR;
        __INTERRUPT_union(INTSRC);
        __INTERRUPT_union(INTFRC);
        __INTERRUPT_union(NIPEND);
        __INTERRUPT_union(FIPEND);
};
#undef __INTERRUPT_union

static volatile struct ITC_struct * const ITC = (void *) (INTBASE);

/* Old register definitions, for compatibility */
#ifndef REG_NO_COMPAT

#define INTCNTL_OFF           (0x0)
#define INTENNUM_OFF          (0x8)
#define INTDISNUM_OFF         (0xC)
#define INTENABLE_OFF        (0x10)
#define INTSRC_OFF           (0x30)
#define INTFRC_OFF           (0x34)
#define NIPEND_OFF           (0x38)

static volatile uint32_t * const INTCNTL   =   ((volatile uint32_t *) (INTBASE + INTCNTL_OFF));
static volatile uint32_t * const INTENNUM  =   ((volatile uint32_t *) (INTBASE + INTENNUM_OFF));
static volatile uint32_t * const INTDISNUM =   ((volatile uint32_t *) (INTBASE + INTDISNUM_OFF));
static volatile uint32_t * const INTENABLE =   ((volatile uint32_t *) (INTBASE + INTENABLE_OFF));
static volatile uint32_t * const INTSRC    =   ((volatile uint32_t *) (INTBASE + INTSRC_OFF));
static volatile uint32_t * const INTFRC    =   ((volatile uint32_t *) (INTBASE + INTFRC_OFF));
static volatile uint32_t * const NIPEND    =   ((volatile uint32_t *) (INTBASE + NIPEND_OFF));

enum interrupt_nums {
        INT_NUM_ASM = 0,
        INT_NUM_UART1,
        INT_NUM_UART2,
        INT_NUM_CRM,
        INT_NUM_I2C,
        INT_NUM_TMR,
        INT_NUM_SPIF,
        INT_NUM_MACA,
        INT_NUM_SSI,
        INT_NUM_ADC,
        INT_NUM_SPI,
};

#define global_irq_disable() (set_bit(*INTCNTL,20))
#define global_irq_enable()  (clear_bit(*INTCNTL,20))

#define enable_irq(irq)  (*INTENNUM  = irq)
#define disable_irq(irq) (*INTDISNUM = irq)

#define safe_irq_disable(x)  volatile uint32_t saved_irq; saved_irq = *INTENABLE; disable_irq(x)
#define irq_restore() *INTENABLE = saved_irq

#endif /* REG_NO_COMPAT */

/* Macro to safely disable all interrupts for a block of code.
   Use it like this:
   disable_int({
       asdf = 1234;
       printf("hi\r\n");
   });
*/
#define __int_top() volatile uint32_t saved_intenable
#define __int_disable() saved_intenable = ITC->INTENABLE; ITC->INTENABLE = 0
#define __int_enable() ITC->INTENABLE = saved_intenable
#define disable_int(x) do { \
        __int_top(); \
        __int_disable(); \
        x; \
        __int_enable(); } while(0)

extern void register_isr(uint8_t interrupt, void (*isr)(void));

extern void tmr_isr(void) __attribute__((weak));

extern void crm_isr(void) __attribute__((weak));
extern void rtc_isr(void) __attribute__((weak));
extern void kbi4_isr(void) __attribute__((weak));
extern void kbi5_isr(void) __attribute__((weak));
extern void kbi6_isr(void) __attribute__((weak));
extern void kbi7_isr(void) __attribute__((weak));

extern void cal_isr(void) __attribute__((weak));

extern void uart1_isr(void) __attribute__((weak));
extern void uart2_isr(void) __attribute__((weak));

extern void maca_isr(void) __attribute__((weak));

extern void asm_isr(void) __attribute__((weak));

extern void i2c_isr(void) __attribute__((weak));

extern void spif_isr(void) __attribute__((weak));

extern void ssi_isr(void) __attribute__((weak));

extern void adc_isr(void) __attribute__((weak));

extern void spi_isr(void) __attribute__((weak));

#ifdef __cplusplus
}
#endif

#endif /* MC1322X_H */
