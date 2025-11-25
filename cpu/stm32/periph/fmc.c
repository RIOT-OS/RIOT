/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_fmc
 * @{
 *
 * @file
 * @brief       FMC peripheral driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <assert.h>

#include "log.h"
#include "periph/gpio.h"
#include "pm_layered.h"

#define ENABLE_DEBUG       0
#include "debug.h"

#ifndef FMC_BASE
#define FMC_BASE    (0x60000000UL)
#endif

#if !defined(FMC_Bank1_R_BASE) && defined(FSMC_Bank1_R_BASE)
/* FSMC Symbols are defined, mapping is required */

#define FMC_Bank1_R_BASE    FSMC_Bank1_R_BASE   /* (FMC_R_BASE + 0x0000UL) */
#define FMC_Bank1_TypeDef   FSMC_Bank1_TypeDef

#define FMC_Bank1E_R_BASE   FSMC_Bank1E_R_BASE  /* (FMC_R_BASE + 0x0104UL) */
#define FMC_Bank1E_TypeDef  FSMC_Bank1E_TypeDef

#if defined(FSMC_Bank2_3_R_BASE)
#define FMC_Bank2_3_R_BASE  FSMC_Bank2_3_R_BASE /* (FMC_R_BASE + 0x0060UL) */
#define FMC_Bank2_3_TypeDef FSMC_Bank2_3_TypeDef
#endif

#if defined(FSMC_Bank4_R_BASE)
#define FMC_Bank4_R_BASE    FSMC_Bank4_R_BASE   /* (FMC_R_BASE + 0x00A0UL) */
#define FMC_Bank4_TypeDef   FSMC_Bank4_TypeDef
#endif

#endif /* !defined(FMC_Bank1_R_BASE) && defined(FSMC_Bank1_R_BASE) */

#if !defined(FMC_Bank1) && defined(FMC_Bank1_R_BASE)
#define FMC_Bank1           ((FMC_Bank1_TypeDef *) FMC_Bank1_R_BASE)
#endif
#if !defined(FMC_Bank1E) && defined(FMC_Bank1E_R_BASE)
#define FMC_Bank1E          ((FMC_Bank1E_TypeDef *) FMC_Bank1E_R_BASE)
#endif
#if !defined(FMC_Bank2_3) && defined(FMC_Bank2_3_R_BASE)
#define FMC_Bank2_3         ((FMC_Bank3_TypeDef *) FMC_Bank2_3_R_BASE)
#endif
#if !defined(FMC_Bank3) && defined(FMC_Bank3_R_BASE)
#define FMC_Bank3           ((FMC_Bank3_TypeDef *) FMC_Bank3_R_BASE)
#endif
#if !defined(FMC_Bank4) && defined(FMC_Bank4_R_BASE)
#define FMC_Bank4           ((FMC_Bank4_TypeDef *) FMC_Bank3_R_BASE)
#endif
#if !defined(FMC_Bank5_6) && defined(FMC_Bank5_6_R_BASE)
#define FMC_Bank5_6         ((FMC_Bank5_6_TypeDef *) FMC_Bank5_6_R_BASE)
#endif

#if defined(FMC_BCR1_MBKEN)
/* if CMSIS header define FMC_BCR1_* macros instead of FMC_BCRx_*,
 * mapping is needed */
#define FMC_BCRx_MBKEN          FMC_BCR1_MBKEN
#define FMC_BCRx_MBKEN_Pos      FMC_BCR1_MBKEN_Pos
#define FMC_BCRx_MUXEN          FMC_BCR1_MUXEN
#define FMC_BCRx_MUXEN_Pos      FMC_BCR1_MUXEN_Pos
#define FMC_BCRx_MTYP_Msk       FMC_BCR1_MTYP_Msk
#define FMC_BCRx_MTYP_Pos       FMC_BCR1_MTYP_Pos
#define FMC_BCRx_MWID_Msk       FMC_BCR1_MWID_Msk
#define FMC_BCRx_MWID_Pos       FMC_BCR1_MWID_Pos
#define FMC_BCRx_FACCEN         FMC_BCR1_FACCEN
#define FMC_BCRx_FACCEN_Pos     FMC_BCR1_FACCEN_Pos
#define FMC_BCRx_WAITEN         FMC_BCR1_WAITEN
#define FMC_BCRx_WAITEN_Pos     FMC_BCR1_WAITEN_Pos
#define FMC_BCRx_EXTMOD         FMC_BCR1_EXTMOD
#define FMC_BCRx_EXTMOD_Pos     FMC_BCR1_EXTMOD_Pos

#define FMC_BTRx_ADDSET_Pos     FMC_BTR1_ADDSET_Pos
#define FMC_BTRx_ADDHLD_Pos     FMC_BTR1_ADDHLD_Pos
#define FMC_BTRx_DATAST_Pos     FMC_BTR1_DATAST_Pos
#define FMC_BTRx_DATLAT_Pos     FMC_BTR1_DATLAT_Pos
#define FMC_BTRx_BUSTURN_Pos    FMC_BTR1_BUSTURN_Pos
#define FMC_BTRx_CLKDIV_Pos     FMC_BTR1_CLKDIV_Pos
#endif /* defined(FMC_BCR1_MBKEN) */

static void _fmc_init_gpio_common(void)
{
    for (unsigned i = 0; i < FMC_DATA_PIN_NUMOF; i++) {
        assert(gpio_is_valid(fmc_config.data[i].pin));
        gpio_init(fmc_config.data[i].pin, GPIO_OUT);
        gpio_init_af(fmc_config.data[i].pin, fmc_config.data[i].af);
    }

#if FMC_ADDR_PIN_NUMOF
    for (unsigned i = 0; i < FMC_ADDR_PIN_NUMOF; i++) {
        if (gpio_is_valid(fmc_config.addr[i].pin)) {
            gpio_init(fmc_config.addr[i].pin, GPIO_OUT);
            gpio_init_af(fmc_config.addr[i].pin, fmc_config.addr[i].af);
        }
    }
#endif /* FMC_ADDR_PIN_NUMOF */

    if (gpio_is_valid(fmc_config.nbl0_pin.pin)) {
        gpio_init(fmc_config.nbl0_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.nbl0_pin.pin, fmc_config.nbl0_pin.af);
    }
    if (gpio_is_valid(fmc_config.nbl1_pin.pin)) {
        gpio_init(fmc_config.nbl1_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.nbl1_pin.pin, fmc_config.nbl1_pin.af);
    }
    if (gpio_is_valid(fmc_config.nbl2_pin.pin)) {
        gpio_init(fmc_config.nbl2_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.nbl2_pin.pin, fmc_config.nbl2_pin.af);
    }
    if (gpio_is_valid(fmc_config.nbl3_pin.pin)) {
        gpio_init(fmc_config.nbl3_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.nbl3_pin.pin, fmc_config.nbl3_pin.af);
    }
    if (gpio_is_valid(fmc_config.nwait_pin.pin)) {
        gpio_init(fmc_config.nwait_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.nwait_pin.pin, fmc_config.nwait_pin.af);
    }
}

#if MODULE_PERIPH_FMC_NOR_SRAM
static void _fmc_init_gpio_nor_sram(uint8_t sub_bank, fmc_mem_type_t type)
{
    /* at least either NOE or NWE has to be valid */
    assert(gpio_is_valid(fmc_config.noe_pin.pin) ||
           gpio_is_valid(fmc_config.nwe_pin.pin));

    if (gpio_is_valid(fmc_config.clk_pin.pin)) {
        gpio_init(fmc_config.clk_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.clk_pin.pin, fmc_config.clk_pin.af);
    }

    if (gpio_is_valid(fmc_config.noe_pin.pin)) {
        gpio_init(fmc_config.noe_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.noe_pin.pin, fmc_config.noe_pin.af);
    }

    if (gpio_is_valid(fmc_config.nwe_pin.pin)) {
        gpio_init(fmc_config.nwe_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.nwe_pin.pin, fmc_config.nwe_pin.af);
    }

    if (type == FMC_NOR) {
        /* NADV has to be valid for NOR Flash memories */
        assert(gpio_is_valid(fmc_config.nadv_pin.pin));
        gpio_init(fmc_config.nadv_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.nadv_pin.pin, fmc_config.nadv_pin.af);
    }

    /* the corresponding NE pin has to be valid */
    switch (sub_bank) {
    case 1:
        assert(gpio_is_valid(fmc_config.ne1_pin.pin));
        gpio_init(fmc_config.ne1_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.ne1_pin.pin, fmc_config.ne1_pin.af);
        break;
    case 2:
        assert(gpio_is_valid(fmc_config.ne2_pin.pin));
        gpio_init(fmc_config.ne2_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.ne2_pin.pin, fmc_config.ne2_pin.af);
        break;
    case 3:
        assert(gpio_is_valid(fmc_config.ne3_pin.pin));
        gpio_init(fmc_config.ne3_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.ne3_pin.pin, fmc_config.ne3_pin.af);
        break;
    case 4:
        assert(gpio_is_valid(fmc_config.ne4_pin.pin));
        gpio_init(fmc_config.ne4_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.ne4_pin.pin, fmc_config.ne4_pin.af);
        break;
    }
}

static void _fmc_init_nor_sram_bank(const fmc_bank_conf_t *bank_conf)
{
    DEBUG("[%s] subbank %u\n", __func__, bank_conf->nor_sram.sub_bank);

    const fmc_nor_sram_bank_conf_t *conf = &bank_conf->nor_sram;
    assert((bank_conf->nor_sram.sub_bank) >= 1 &&
           (bank_conf->nor_sram.sub_bank) <= 4);

    _fmc_init_gpio_nor_sram(conf->sub_bank, bank_conf->mem_type);

    uint32_t *bcr1 = (uint32_t *)(FMC_Bank1_R_BASE);
    uint32_t *bcr = (uint32_t *)(FMC_Bank1_R_BASE + ((conf->sub_bank - 1) * 8));
    uint32_t *btr = (uint32_t *)(FMC_Bank1_R_BASE + ((conf->sub_bank - 1) * 8) + 0x04);
    uint32_t *bwtr = (uint32_t *)(FMC_Bank1E_R_BASE + ((conf->sub_bank - 1) * 8));

    /* disable Memory Bank 1 */
    *bcr &= ~FMC_BCRx_MBKEN;

    /* set read timing */
    *btr = conf->r_timing.mode
         | (conf->r_timing.addr_setup << FMC_BTRx_ADDSET_Pos)
         | (conf->r_timing.addr_hold << FMC_BTRx_ADDHLD_Pos)
         | (conf->r_timing.data_setup << FMC_BTRx_DATAST_Pos)
         | (conf->r_timing.data_latency << FMC_BTRx_DATLAT_Pos)
         | (conf->r_timing.bus_turnaround << FMC_BTRx_BUSTURN_Pos)
         | (conf->r_timing.clk_div << FMC_BTRx_CLKDIV_Pos);

    if (conf->mux_enable) {
        /* enable Extended mode if set and set write timings */
        *bcr |= FMC_BCRx_EXTMOD;
        *bwtr = conf->w_timing.mode
              | (conf->w_timing.addr_setup << FMC_BTRx_ADDSET_Pos)
              | (conf->w_timing.addr_hold << FMC_BTRx_ADDHLD_Pos)
              | (conf->w_timing.data_setup << FMC_BTRx_DATAST_Pos)
              | (conf->w_timing.bus_turnaround << FMC_BTRx_BUSTURN_Pos);
    }
    else {
        *bcr &= ~FMC_BCRx_EXTMOD;
    }

#ifdef FMC_BCR1_WFDIS
    /* disable Write FIFO for all subbanks */
    *bcr1 |= FMC_BCR1_WFDIS;
#else
    (void)bcr1;
#endif

    /* set memory Type */
    *bcr &= ~FMC_BCRx_MTYP_Msk;
    *bcr |= bank_conf->mem_type << FMC_BCRx_MTYP_Pos;

    /* set Memory Type to 16 bit (reset default) */
    *bcr &= ~FMC_BCRx_MWID_Msk;
    *bcr |= bank_conf->data_width << FMC_BCRx_MWID_Pos;

    /* clear Write Wait, Flash Access and Address/Data Multiplexing */
    *bcr &= ~(FMC_BCRx_WAITEN | FMC_BCRx_FACCEN | FMC_BCRx_MUXEN);

    /* enable WAIT signal if set */
    *bcr |= (conf->wait_enable) << FMC_BCRx_WAITEN_Pos;

    /* enable NOR Flash memory access in case of NOR Flash */
    *bcr |= (bank_conf->mem_type == FMC_NOR) << FMC_BCRx_FACCEN_Pos;

    /* enable MUX in case of PSRAM or NOR if set */
    *bcr |= (((bank_conf->mem_type == FMC_PSRAM) ||
              (bank_conf->mem_type == FMC_NOR)) &&
             conf->mux_enable) << FMC_BCRx_MUXEN_Pos;

    /* leave all other values at reset default 0x000030d2 */
    /* enable Memory Bank 1 */
    *bcr |= FMC_BCRx_MBKEN;         /* Memory Bank Enable = enabled */

    DEBUG("[%s] FMC BCRx=%08lx BTRx=%08lx BWTR=%08lx\n",
          __func__, *bcr, *btr, *bwtr);
}
#endif /* MODULE_PERIPH_FMC_NOR_SRAM */

#if MODULE_PERIPH_FMC_SDRAM
/**
 * @brief   SDRAM Command Mode Register definition
 */
typedef union {
    struct __attribute__((__packed__)) {
        uint32_t command:3;         /**< Command mode */
        uint32_t target:2;          /**< Command target */
        uint32_t auto_refresh:4;    /**< Number of auto-refresh cycles */
        uint32_t mode_reg:13;       /**< Mode register */
        uint32_t reserved:10;       /**< reserved bits */
    };
    uint32_t value;
} fmc_sdram_sdcmr_t;

/**
 * @brief   SDRAM Mode Register definition
 */
typedef union __attribute__((__packed__)) {
    struct {
        uint16_t burst_len:3;   /**< Burst length (0..6: 2^burst_len, 7: full page) */
        uint16_t burst_type:1;  /**< Burst type (0: sequential, 1: interleaved) */
        uint16_t cas_latency:3; /**< CAS latency (1, 2, 3, other values are reserved) */
        uint16_t op_mode:2;     /**< Operating mode (must be 0) */
        uint16_t write_burst:1; /**< Write burst mode (0: use read burst length, 1: no bursts) */
        uint16_t unused:6;      /**< unused */
    };
    uint16_t value;
} fmc_sdram_modereg_t;

/**
 * @brief   SDRAM Command Mode
 */
typedef enum {
    FMC_CMD_NORMAL_MODE = 0,            /**< Normal Mode */
    FMC_CMD_CLK_ENABLE = 1,             /**< Clock Configuration Enable */
    FMC_CMD_PALL = 2,                   /**< All Bank Precharge command */
    FMC_CMD_AUTO_REFRESH = 3,           /**< Auto-refresh command */
    FMC_CMD_LOAD_MODE = 4,              /**< Load Mode Register */
    FMC_CMD_SELF_REFRESH = 5,           /**< Self-refresh command */
    FMC_CMD_POWER_DOWN = 6,             /**< Power-down command */
    FMC_CMD_RESERVED = 7,               /**< reserved */
} fmc_sdram_cmd_t;

static void _fmc_init_gpio_sdram(fmc_bank_t bank)
{
    /* CLK, RAS, CAS, BA0, BA1 and WE must be valid */
    assert(gpio_is_valid(fmc_config.sdclk_pin.pin) &&
           gpio_is_valid(fmc_config.sdnras_pin.pin) &&
           gpio_is_valid(fmc_config.sdncas_pin.pin) &&
           gpio_is_valid(fmc_config.sdnwe_pin.pin) &&
           gpio_is_valid(fmc_config.ba0_pin.pin) &&
           gpio_is_valid(fmc_config.ba1_pin.pin));

    gpio_init(fmc_config.ba0_pin.pin, GPIO_OUT);
    gpio_init(fmc_config.ba1_pin.pin, GPIO_OUT);
    gpio_init(fmc_config.sdclk_pin.pin, GPIO_OUT);
    gpio_init(fmc_config.sdnras_pin.pin, GPIO_OUT);
    gpio_init(fmc_config.sdncas_pin.pin, GPIO_OUT);
    gpio_init(fmc_config.sdnwe_pin.pin, GPIO_OUT);
    gpio_init_af(fmc_config.ba0_pin.pin, fmc_config.ba0_pin.af);
    gpio_init_af(fmc_config.ba1_pin.pin, fmc_config.ba1_pin.af);
    gpio_init_af(fmc_config.sdclk_pin.pin, fmc_config.sdclk_pin.af);
    gpio_init_af(fmc_config.sdnras_pin.pin, fmc_config.sdnras_pin.af);
    gpio_init_af(fmc_config.sdncas_pin.pin, fmc_config.sdncas_pin.af);
    gpio_init_af(fmc_config.sdnwe_pin.pin, fmc_config.sdnwe_pin.af);

    /* corresponding NE pin and CKE pin must be valid */
    if (bank == FMC_BANK_5) {
        assert(gpio_is_valid(fmc_config.sdne0_pin.pin));
        assert(gpio_is_valid(fmc_config.sdcke0_pin.pin));
        gpio_init(fmc_config.sdne0_pin.pin, GPIO_OUT);
        gpio_init(fmc_config.sdcke0_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.sdne0_pin.pin, fmc_config.sdne0_pin.af);
        gpio_init_af(fmc_config.sdcke0_pin.pin, fmc_config.sdcke0_pin.af);
    }
    else {
        assert(gpio_is_valid(fmc_config.sdne1_pin.pin));
        assert(gpio_is_valid(fmc_config.sdcke1_pin.pin));
        gpio_init(fmc_config.sdne1_pin.pin, GPIO_OUT);
        gpio_init(fmc_config.sdcke1_pin.pin, GPIO_OUT);
        gpio_init_af(fmc_config.sdne1_pin.pin, fmc_config.sdne1_pin.af);
        gpio_init_af(fmc_config.sdcke1_pin.pin, fmc_config.sdcke1_pin.af);
    }
}

static void _fmc_init_sdram_bank(const fmc_bank_conf_t *bank_conf)
{
    _fmc_init_gpio_sdram(bank_conf->bank);

    const fmc_sdram_bank_conf_t *conf = &bank_conf->sdram;

    uint8_t bank = (bank_conf->bank == FMC_BANK_5) ? 0 : 1;
    uint32_t sdcr;

    /* following settings are always configured in SDCR1 independent on the
     * bank and are don't care in SDCR2 */
    sdcr = FMC_Bank5_6->SDCR[0];

    /* set read delay */
    assert(conf->read_delay <= 2);
    sdcr &= ~FMC_SDCR1_RPIPE_Msk;
    sdcr |= conf->read_delay << FMC_SDCR1_RPIPE_Pos;

    /* clear and set RBURST if enabled */
    sdcr &= ~FMC_SDCR1_RBURST;
    sdcr |= conf->burst_read << FMC_SDCR1_RBURST_Pos;

    /* set clock period */
    assert(conf->clk_period <= 3);
    sdcr &= ~FMC_SDCR1_SDCLK_Msk;
    sdcr |= conf->clk_period << FMC_SDCR1_SDCLK_Pos;

    /* write back register SDCR1 */
    FMC_Bank5_6->SDCR[0] = sdcr;

    /* other settings are done in SDCRx for bannk x */
    sdcr = FMC_Bank5_6->SDCR[bank];

    /* set number of row and column bits and the data bus width */
    assert((conf->row_bits >= 11) && (conf->row_bits <= 13));
    assert((conf->col_bits >= 8) && (conf->col_bits <= 11));
    sdcr &= ~(FMC_SDCR1_NR_Msk | FMC_SDCR1_NC_Msk | FMC_SDCR1_MWID_Msk);
    sdcr |= (conf->row_bits - 11) << FMC_SDCR1_NR_Pos;
    sdcr |= (conf->col_bits - 8) << FMC_SDCR1_NC_Pos;
    sdcr |= bank_conf->data_width << FMC_SDCR1_MWID_Pos;

    /* set CAS latency */
    assert((conf->cas_latency >= 1) && (conf->cas_latency <= 3));
    sdcr &= ~FMC_SDCR1_CAS_Msk;
    sdcr |= conf->cas_latency << FMC_SDCR1_CAS_Pos;

    /* clear and set NB and WP */
    sdcr &= ~(FMC_SDCR1_NB | FMC_SDCR1_WP);
    sdcr |= conf->four_banks << FMC_SDCR1_NB_Pos;
    sdcr |= conf->write_protect << FMC_SDCR1_WP_Pos;

    /* write back register SDCRx */
    FMC_Bank5_6->SDCR[bank] = sdcr;

    uint32_t sdtr = 0;

    assert((conf->timing.row_to_col_delay - 1) <= 15);
    assert((conf->timing.row_precharge - 1) <= 15);
    assert((conf->timing.recovery_delay - 1) <= 15);
    assert((conf->timing.row_cylce - 1) <= 15);
    assert((conf->timing.self_refresh - 1) <= 15);
    assert((conf->timing.exit_self_refresh - 1) <= 15);
    assert((conf->timing.load_mode_register - 1) <= 15);

    /* following settings are always configured in SDTR1 independent on the
     * bank and are don't care in SDTR2 */
    sdtr = FMC_Bank5_6->SDTR[0];

    sdtr &= ~FMC_SDTR1_TRP;
    sdtr |= (conf->timing.row_precharge - 1) << FMC_SDTR1_TRP_Pos;

    sdtr &= ~FMC_SDTR1_TRC;
    sdtr |= (conf->timing.row_cylce - 1) << FMC_SDTR1_TRC_Pos;

    /* write back register SDCRx */
    FMC_Bank5_6->SDTR[0] = sdtr;

    /* other settings are done in SDTRx for bannk x */
    sdtr = FMC_Bank5_6->SDTR[0];

    sdtr &= ~FMC_SDTR1_TRCD;
    sdtr |= (conf->timing.row_to_col_delay - 1) << FMC_SDTR1_TRCD_Pos;

    sdtr &= ~FMC_SDTR1_TWR;
    sdtr |= (conf->timing.recovery_delay - 1) << FMC_SDTR1_TWR_Pos;

    sdtr &= ~FMC_SDTR1_TRAS;
    sdtr |= (conf->timing.self_refresh - 1) << FMC_SDTR1_TRAS_Pos;

    sdtr &= ~FMC_SDTR1_TXSR;
    sdtr |= (conf->timing.exit_self_refresh - 1) << FMC_SDTR1_TXSR_Pos;

    sdtr &= ~FMC_SDTR1_TMRD;
    sdtr |= (conf->timing.load_mode_register - 1) << FMC_SDTR1_TMRD_Pos;

    /* write register SDTRx */
    FMC_Bank5_6->SDTR[bank] = sdtr;

    DEBUG("[%s] DMC SDCR%d=%08lx SDTR%d=%08lx SDCMR=%08lx SDRTR=%08lx SDSR=%08lx\n",
          __func__,
          bank, FMC_Bank5_6->SDCR[bank],
          bank, FMC_Bank5_6->SDTR[bank],
          FMC_Bank5_6->SDCMR, FMC_Bank5_6->SDRTR, FMC_Bank5_6->SDSR);

    /* Initialization sequence according to the reference manual */

    fmc_sdram_sdcmr_t sdcmr = { .target = (bank) ? 0b01 : 0b10,
                                .auto_refresh = 1,
                                .mode_reg = 0 };

    /* enable clock (SDCKE HIGH) */
    sdcmr.command = FMC_CMD_CLK_ENABLE;
    FMC_Bank5_6->SDCMR = sdcmr.value;

    /* wait at least 100 us */
    uint32_t count = 0xffffff;
    while (count--) {}

    /* issue Precharge All command */
    sdcmr.command = FMC_CMD_PALL;
    FMC_Bank5_6->SDCMR = sdcmr.value;

    /* issue the typical number of Auto-refresh commands */
    sdcmr.command = FMC_CMD_AUTO_REFRESH;
    sdcmr.auto_refresh = 8;
    FMC_Bank5_6->SDCMR = sdcmr.value;

    /* load mode register */
    fmc_sdram_modereg_t modreg = {
        .burst_len = 0, // conf->burst_len,
        .burst_type = conf->burst_interleaved,
        .cas_latency = conf->cas_latency,
        .op_mode = 0,
        .write_burst = (conf->burst_write) ? 0 : 1,
    };
    sdcmr.command = FMC_CMD_LOAD_MODE;
    sdcmr.auto_refresh = 1;
    sdcmr.mode_reg = modreg.value;
    FMC_Bank5_6->SDCMR = sdcmr.value;

    /* set refresh timer register */
    uint32_t cycles;
    cycles = CLOCK_AHB / KHZ(1) / conf->clk_period;     /* SDCLK cycles per ms */
    cycles = cycles * conf->timing.refresh_period;      /* SDCLK cycles per refresh period */
    cycles = cycles / (1 << conf->row_bits);            /* SDCLK cycles per row */
    cycles = ((cycles - 20) > 41) ? cycles - 20 : 41;   /* cycles - margin > min 41 */
    FMC_Bank5_6->SDRTR &= FMC_SDRTR_COUNT_Msk;
    FMC_Bank5_6->SDRTR |= cycles << FMC_SDRTR_COUNT_Pos;
}
#endif

void fmc_init_bank(fmc_bank_id_t bank)
{
    DEBUG("[%s] bank id %u\n", __func__, bank);

    assert(bank < FMC_BANK_NUMOF);

    const fmc_bank_conf_t *conf = &fmc_bank_config[bank];

    /* ensure that configured bank is valid */
    switch (conf->bank) {
    case 1: break;
#if defined(FMC_Bank2_3_R_BASE)
    case 2: break;
#endif
#if defined(FMC_Bank2_3_R_BASE) || defined(FMC_Bank3_R_BASE)
    case 3: break;
#endif
#if defined(FMC_Bank4_R_BASE)
    case 4: break;
#endif
#if defined(FMC_Bank5_6_R_BASE)
    case 5:
    case 6: break;
#endif
    default: assert(false);
    }

    if (conf->bank == FMC_BANK_1) {
#if MODULE_PERIPH_FMC_NOR_SRAM
        /* bank 1 has to be NOR, PSRAM or SRAM */
        assert((conf->mem_type == FMC_SRAM) ||
               (conf->mem_type == FMC_PSRAM) ||
               (conf->mem_type == FMC_NOR));
        _fmc_init_nor_sram_bank(conf);
#else
        LOG_WARNING("NOR/PSRAM/SRAM configured but not enabled by feature periph_fmc_sdram\n");
#endif
    }

#if defined(FMC_Bank5_6_R_BASE)
    else if ((conf->bank == FMC_BANK_5) || (conf->bank == FMC_BANK_6)) {
#if MODULE_PERIPH_FMC_SDRAM
        _fmc_init_sdram_bank(conf);
#else
        LOG_WARNING("SDRAM configured but not enabled by feature periph_fmc_nor_sram\n");
#endif
    }
#endif

    else {
        LOG_ERROR("[fmc] Bank %u not supported\n", conf->bank);
        assert(false);
    }
}

#include "periph_cpu.h"

void fmc_init(void)
{
    DEBUG("[%s]\n", __func__);

    periph_clk_en(fmc_config.bus, fmc_config.rcc_mask);

    _fmc_init_gpio_common();
    for (unsigned i = 0; i < FMC_BANK_NUMOF; i++) {
        fmc_init_bank(i);
    }
}
