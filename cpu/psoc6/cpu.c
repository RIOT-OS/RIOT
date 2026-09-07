#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "irq_arch.h"
#include "kernel_init.h"
#include "periph/uart.h"
#include "vendor/system_psoc6.h"

extern void (* const _isr_vectors[])(void);

#define NUM_VECTORS             (16U + CPU_IRQ_NUMOF)
#define PSOC6_CM4_VECTKEY       (0x05FAUL)
#define PSOC6_CM4_PWR_RESET     (0x1UL)
#define PSOC6_CM4_PWR_ENABLED   (0x3UL)

__attribute__((aligned(256)))
static void (*ram_vectors[NUM_VECTORS])(void);

static bool _vectors_in_ram;

// copy vector table to RAM so CM0+ can patch IRQ mux handlers at runtime
static void _vectors_relocate_to_ram(void)
{
    if (_vectors_in_ram) {
        return;
    }

    memcpy(ram_vectors, _isr_vectors, sizeof(ram_vectors));
    SCB->VTOR = (uint32_t)ram_vectors;
    __DSB();
    __ISB();
    _vectors_in_ram = true;
}

// update one IRQ vector in the RAM table used by CM0+ 
void cpu_patch_irq_vector(IRQn_Type irqn, void (*handler)(void))
{
    if (((int)irqn < 0) || ((unsigned)irqn >= CPU_IRQ_NUMOF)) {
        return;
    }

    _vectors_relocate_to_ram();
    ram_vectors[16U + (unsigned)irqn] = handler;
    __DSB();
    __ISB();
}

/* Select the correct device description for the CY8C624ABZI. */
const cy_stc_device_t cy_deviceIpBlockCfgPSoC6_02 = {
    0x40200000UL,
    0x40240000UL,
    0x40000000UL,
    0UL,
    0x40230000UL,
    0x40300000UL,
    0x40310000UL,
    0x409F0000UL,
    0x40220000UL,
    0x40100000UL,
    0x409D0000UL,
    0x20U,
    0x20U,
    0x20U,
    0x20U,
    0x20U,
    0x20U,
    0x20U,
    0x20U,
    0x20U,
    0x10U,
    0x10U,
    16U,
    16U,
    29U,
    29U,
    128U,
    23,
    117,
    1023,
    6U,
    2U,
    6U,
    0U,
    54U,
    4U,
    16U,
    32U,
    0U,
    0U,
    0xC07FUL,
    1024U,
    0U,
    0U,
    0U,
    0U,
    0U,
    25U,
    50U,
    75U,
    100U,
    125U,
    0U,
    0U,
    0U,
    (uint16_t)offsetof(DW_V2_Type, CH_STRUCT),
    (uint16_t)sizeof(DW_CH_STRUCT_V2_Type),
    (uint8_t)DW_CH_STRUCT_V2_CH_CTL_PRIO_Pos,
    (uint8_t)DW_CH_STRUCT_V2_CH_CTL_PREEMPTABLE_Pos,
    (uint8_t)DW_V2_STATUS_CH_IDX_Pos,
    DW_V2_STATUS_CH_IDX_Msk,
    (uint16_t)offsetof(PERI_V2_Type, TR_CMD),
    (uint16_t)PERI_V2_TR_CMD_GROUP_SEL_Msk,
    (uint16_t)offsetof(PERI_V2_Type, TR_GR),
    (uint16_t)sizeof(PERI_TR_GR_V2_Type),
    (uint8_t)PERI_V2_DIV_CMD_DIV_SEL_Msk,
    (uint8_t)PERI_V2_DIV_CMD_TYPE_SEL_Pos,
    (uint8_t)PERI_V2_DIV_CMD_PA_DIV_SEL_Pos,
    (uint8_t)PERI_V2_DIV_CMD_PA_TYPE_SEL_Pos,
    (uint16_t)offsetof(PERI_V2_Type, DIV_8_CTL),
    (uint16_t)offsetof(PERI_V2_Type, DIV_16_CTL),
    (uint16_t)offsetof(PERI_V2_Type, DIV_16_5_CTL),
    (uint16_t)offsetof(PERI_V2_Type, DIV_24_5_CTL),
    (uint8_t)offsetof(GPIO_PRT_V2_Type, INTR_CFG),
    (uint8_t)offsetof(GPIO_PRT_V2_Type, CFG),
    (uint8_t)offsetof(GPIO_PRT_V2_Type, CFG_IN),
    (uint8_t)offsetof(GPIO_PRT_V2_Type, CFG_OUT),
    (uint8_t)offsetof(GPIO_PRT_V2_Type, CFG_SIO),
    offsetof(CPUSS_V2_Type, CM0_CLOCK_CTL),
    offsetof(CPUSS_V2_Type, CM4_CLOCK_CTL),
    offsetof(CPUSS_V2_Type, CM4_STATUS),
    offsetof(CPUSS_V2_Type, CM0_STATUS),
    offsetof(CPUSS_V2_Type, CM4_PWR_CTL),
    offsetof(CPUSS_V2_Type, TRIM_RAM_CTL),
    offsetof(CPUSS_V2_Type, TRIM_ROM_CTL),
    offsetof(CPUSS_V2_Type, SYSTICK_CTL),
    (uint16_t)offsetof(CPUSS_V2_Type, CM0_NMI_CTL),
    (uint16_t)offsetof(CPUSS_V2_Type, CM4_NMI_CTL),
    (uint16_t)offsetof(CPUSS_V2_Type, ROM_CTL),
    (uint16_t)offsetof(CPUSS_V2_Type, RAM0_CTL0),
    (uint16_t)offsetof(CPUSS_V2_Type, RAM1_CTL0),
    (uint16_t)offsetof(CPUSS_V2_Type, RAM2_CTL0),
    (uint16_t)offsetof(CPUSS_V2_Type, RAM0_PWR_MACRO_CTL),
    (uint16_t)offsetof(CPUSS_V2_Type, RAM1_PWR_CTL),
    (uint16_t)offsetof(CPUSS_V2_Type, RAM2_PWR_CTL),
    (uint16_t)sizeof(IPC_STRUCT_V2_Type),
    offsetof(IPC_STRUCT_V2_Type, LOCK_STATUS),
};

const cy_stc_device_t *cy_device = &cy_deviceIpBlockCfgPSoC6_02;

#if !defined(CPU_CORE_CORTEX_M0PLUS)
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t REMAP;
    volatile uint32_t COMP[];
} psoc6_fpb_t;

#define PSOC6_FPB_BASE              (0xE0002000UL)
#define PSOC6_FPB                   ((psoc6_fpb_t *)PSOC6_FPB_BASE)
#define PSOC6_FPB_CTRL_KEY_Msk      (1UL << 1)
#define PSOC6_FPB_CTRL_NUM_CODE_Pos (4U)
#define PSOC6_FPB_CTRL_NUM_CODE_Msk (0xFUL << PSOC6_FPB_CTRL_NUM_CODE_Pos)
#define PSOC6_FPB_CTRL_NUM_LIT_Pos  (8U)
#define PSOC6_FPB_CTRL_NUM_LIT_Msk  (0xFUL << PSOC6_FPB_CTRL_NUM_LIT_Pos)
#endif

static inline uint32_t _cm4_get_pwr_mode(void)
{
    return (CPUSS->CM4_PWR_CTL & 0x3U);
}

static inline uint32_t _enter_critical(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static inline void _exit_critical(uint32_t primask)
{
    if ((primask & 1U) == 0U) {
        __enable_irq();
    }
}

static inline void _cm4_wait_pwr_done(void)
{
    while ((CPUSS->CM4_STATUS & CPUSS_CM4_STATUS_PWR_DONE_Msk) == 0UL) {
    }
}

static inline void _cm4_set_pwr_mode(uint32_t mode)
{
    CPUSS->CM4_PWR_CTL = ((uint32_t)PSOC6_CM4_VECTKEY << 16) | (mode & 0x3U);
}

#if !defined(CPU_CORE_CORTEX_M0PLUS)
/** Clear stale debug state left behind by a probe before RIOT starts. */
static void _cm4_clear_debug_traps(void)
{
    uint32_t demcr = CoreDebug->DEMCR;

#ifdef CoreDebug_DEMCR_MON_EN_Msk
    demcr &= ~CoreDebug_DEMCR_MON_EN_Msk;
#endif
#ifdef CoreDebug_DEMCR_MON_PEND_Msk
    demcr &= ~CoreDebug_DEMCR_MON_PEND_Msk;
#endif
#ifdef CoreDebug_DEMCR_MON_STEP_Msk
    demcr &= ~CoreDebug_DEMCR_MON_STEP_Msk;
#endif
#ifdef CoreDebug_DEMCR_VC_HARDERR_Msk
    demcr &= ~CoreDebug_DEMCR_VC_HARDERR_Msk;
#endif
#ifdef CoreDebug_DEMCR_VC_INTERR_Msk
    demcr &= ~CoreDebug_DEMCR_VC_INTERR_Msk;
#endif
#ifdef CoreDebug_DEMCR_VC_BUSERR_Msk
    demcr &= ~CoreDebug_DEMCR_VC_BUSERR_Msk;
#endif
#ifdef CoreDebug_DEMCR_VC_MMERR_Msk
    demcr &= ~CoreDebug_DEMCR_VC_MMERR_Msk;
#endif
#ifdef CoreDebug_DEMCR_VC_NOCPERR_Msk
    demcr &= ~CoreDebug_DEMCR_VC_NOCPERR_Msk;
#endif
#ifdef CoreDebug_DEMCR_VC_CHKERR_Msk
    demcr &= ~CoreDebug_DEMCR_VC_CHKERR_Msk;
#endif
#ifdef CoreDebug_DEMCR_VC_STATERR_Msk
    demcr &= ~CoreDebug_DEMCR_VC_STATERR_Msk;
#endif
#ifdef CoreDebug_DEMCR_TRCENA_Msk
    demcr &= ~CoreDebug_DEMCR_TRCENA_Msk;
#endif

    CoreDebug->DEMCR = demcr;
    SCB->DFSR = SCB->DFSR;

    uint32_t fp_ctrl = PSOC6_FPB->CTRL;
    uint32_t num_code = (fp_ctrl & PSOC6_FPB_CTRL_NUM_CODE_Msk) >> PSOC6_FPB_CTRL_NUM_CODE_Pos;
    uint32_t num_lit = (fp_ctrl & PSOC6_FPB_CTRL_NUM_LIT_Msk) >> PSOC6_FPB_CTRL_NUM_LIT_Pos;
    uint32_t num_comp = num_code + num_lit;

    PSOC6_FPB->CTRL = PSOC6_FPB_CTRL_KEY_Msk;
    for (uint32_t i = 0; i < num_comp; i++) {
        PSOC6_FPB->COMP[i] = 0U;
    }
    (void)PSOC6_FPB->CTRL;

    uint32_t num_dwt_comp = (DWT->CTRL & DWT_CTRL_NUMCOMP_Msk) >> DWT_CTRL_NUMCOMP_Pos;
    for (uint32_t i = 0; i < num_dwt_comp; i++) {
        (&DWT->COMP0)[i * 4U] = 0U;
        (&DWT->MASK0)[i * 4U] = 0U;
        (&DWT->FUNCTION0)[i * 4U] = 0U;
    }
}
#endif

#if defined(CPU_CORE_CORTEX_M0PLUS)
/** Start the CM4 application from flash and leave CM0+ in a parked state. */
static void _enable_cm4(uint32_t vector_table_base)
{
    if ((vector_table_base & 0x3FFUL) != 0UL) {
        while (1) {
        }
    }

    uint32_t primask = _enter_critical();

    if (_cm4_get_pwr_mode() == PSOC6_CM4_PWR_ENABLED) {
        _cm4_set_pwr_mode(PSOC6_CM4_PWR_RESET);
        _cm4_wait_pwr_done();
    }

    CPUSS->CM4_VECTOR_TABLE_BASE = vector_table_base;
    _cm4_set_pwr_mode(PSOC6_CM4_PWR_ENABLED);
    _cm4_wait_pwr_done();
    _exit_critical(primask);
}
#endif

void cpu_init(void)
{
    cortexm_init();

#if defined(CPU_CORE_CORTEX_M0PLUS)
    _vectors_relocate_to_ram();
    _enable_cm4(0x10002000);

    __disable_irq();
    while (1) {
        __WFE();
    }
#else
    _cm4_clear_debug_traps();
    uart_init(UART_DEV(0), 115200, NULL, NULL);
    irq_enable();
#endif
}
