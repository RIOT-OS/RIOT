/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    cpu_stm32_periph_fmc STM32 FMC/FSMC peripheral driver
 * @ingroup     cpu_stm32
 *
 * @{
 *
 * The `periph_fmc` module implements a driver for STM32 FMC/FSMC peripherals.
 * It supports
 * - NOR Flashes,
 * - PSRAMs/SRAMs,
 * - SDRAMs, and
 * - Display Controllers with MCU 8-/16-bit parallel interface.
 *
 * NAND Flashes are not yet supported.
 *
 * To use the FMC/FSMC, the `periph_fmc` module has to be enabled.
 * To keep required data structures and resulting code as small as possible,
 * a couple of pseudomodules are defined that have to be used in addition to
 * the `periph_fmc` module to enable supported features. These are:
 *
 * | Module | Feature |
 * |:-----------------------|:----------------------------------------|
 * | `periph_fmc_nor_sram`  | enable NOR Flash and PSRAM/SRAM support |
 * | `periph_fmc_sdram`     | enable SDRAM support                    |
 * | `periph_fmc_16bit`     | enable 16-bit support                   |
 * | `periph_fmc_32bit`     | enable 32-bit support                   |
 * \n
 *
 * The board has then to define
 * - the corresponding features according to the connected external device,
 * - the peripheral configuration of the FMC/FSMC of type @ref fmc_conf_t,
 * - the configuration of the FMC banks which describe the connected external
 *   devices.
 *
 * As examples for such configurations, see @ref boards_stm32l496g-disco
 * (FMC with Display and SRAM) or @ref boards_stm32f429i-disc1 (FMC with SDRAM).
 *
 * To use the RAM connected to the FMC as heap, the board has also to define
 * @ref FMC_RAM_ADDR and @ref FMC_RAM_LEN.
 *
 * @file
 * @brief           Specific FMC definitions for the STM32
 *
 * @author          Gunar Schorcht <gunar@schorcht.net>
 */


#include <stdint.h>

#include "cpu.h"
#include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Gives the configuration of n-th bank
 *
 * This macro gives a pointer to the n-th entry of type @ref fmc_bank_conf_t of
 * the banks configured by the board in the @ref fmc_bank_config array. n is in
 * the range 0 ... @ref FMC_BANK_NUMOF - 1.
 */
#ifndef FMC_BANK_CONFIG
#define FMC_BANK_CONFIG(n)  (&fmc_bank_config[n])
#endif

/**
 * @brief   Number of data pins used
 *
 * The number of configured data pins depends on the memory with the maximum
 * data bus width. The maximum data bus width used has to be specified by the
 * pseudomodules `periph_fmc_16bit` and `periph_fmc_32bit`. 8 bit data bus
 * width is the default.
 */
#if MODULE_PERIPH_FMC_32BIT || DOXYGEN
#define FMC_DATA_PIN_NUMOF      (32)
#elif MODULE_PERIPH_FMC_16BIT
#define FMC_DATA_PIN_NUMOF      (16)
#else
#define FMC_DATA_PIN_NUMOF      (8)
#endif

/**
 * @brief   Number of address pins used
 *
 * The number of configured address pins depends on the memory types used.
 * NORs, PSRAMs and SRAMs (module `periph_fmc_nor_sram`) use up to 26 address
 * signals A0...A25 if address/data multiplexing is not used. SDRAMs (module
 * `periph_fmc_sdram`) use only up to 13 address signals A0...A12.
 * NANDs (module `periph_fmc_sdram`) don't need separate address signals.
 * The type of used memories is specified by the corresponding pseudomodules.
 */
#if MODULE_PERIPH_FMC_NOR_SRAM || DOXYGEN
#define FMC_ADDR_PIN_NUMOF      (26)
#elif MODULE_PERIPH_FMC_SDRAM
#define FMC_ADDR_PIN_NUMOF      (13)
#else
#define FMC_ADDR_PIN_NUMOF      (0)
#endif

#if DOXYGEN
/**
 * @brief   Start address of the heap for the FMC RAM
 *
 * This variable has to be defined in `Makefile.include` of the board
 * definition to use the a RAM connected to the FMC as heap. It has to
 * correspond to the @ref fmc_bank_conf_t::address in FMC bank configuration.
 *
 * @note `FMC_RAM_ADDR` is not a macro and cannot be used in C code.
 *       It is just defined for documentation purpose.
 */
#define FMC_RAM_ADDR    0x60000000

/**
 * @brief   Length of the heap for the FMC RAM
 *
 * This variable has to be defined in `Makefile.include` of the board
 * definition to use the a RAM connected to the FMC as heap. It has to
 * correspond to the @ref fmc_bank_conf_t::size in FMC bank configuration.
 * Since it is used in linker script, it has to be defined in kByte with
 * suffix `K`, e.g. `1024K`.
 *
 * @note `FMC_RAM_SIZE` is not a macro and cannot be used in C code.
 *       It is just defined for documentation purpose.
 */
#define FMC_RAM_LEN     1024K
#endif

/**
 * @name    Type declarations for NOR Flash, PSRAM and SRAM
 * @{
 */
/**
 * @brief   Memory access modes for NOR/PSRAM/SRAM in extended mode
 */
typedef enum {
    FMC_MODE_A = 0,             /**< Access mode A */
    FMC_MODE_B = 1,             /**< Access mode B */
    FMC_MODE_C = 2,             /**< Access mode C */
    FMC_MODE_D = 3,             /**< Access mode D */
} fmc_access_mode_t;

/**
 * @brief   Timing configuration for NOR/PSRAM/SRAM
 *
 * Used timing configuration parameters depend on used access mode. Please refer
 * the reference manual of the respective MCU for details.
 */
typedef struct {
    fmc_access_mode_t mode;     /**< Access Mode used (only used if
                                     @ref fmc_nor_sram_bank_conf_t::ext_mode is true) */
    uint8_t clk_div;            /**< Clock divide ratio, FMC_CLK = HCLK / (DIV + 1) */
    uint8_t addr_setup;         /**< Address setup time [0..15], default 15 */
    uint8_t addr_hold;          /**< Address hold time [0..15], default 15 */
    uint8_t data_setup;         /**< Data setup time [0..15], default 15 */
    uint8_t data_latency;       /**< Data latency for synchronous access [0..15],
                                     default 15 (only used in read timing) */
    uint8_t bus_turnaround;     /**< Bus turnaround phase duration [0..15], default 15 */
} fmc_nor_sram_timing_t;

/**
 * @brief   Bank configuration structure for NOR/PSRAM/SRAM
 */
typedef struct {
    uint8_t sub_bank;               /**< Bank1 has 4 subbanks 1..4 */
    bool mux_enable;                /**< Multiplexed address/data signals used
                                         (only valid for PSRAMs and NORs */
    bool wait_enable;               /**< Wait signal used for synchronous
                                         access */
    bool ext_mode;                  /**< Extended mode used (separate read and
                                         write timings) */
    fmc_nor_sram_timing_t r_timing; /**< Read timings (also used for write if
                                         @ref fmc_nor_sram_bank_conf_t::ext_mode is false) */
    fmc_nor_sram_timing_t w_timing; /**< Write timings (only used if
                                         @ref fmc_nor_sram_bank_conf_t::ext_mode is true) */
} fmc_nor_sram_bank_conf_t;
/** @} */

/**
 * @name    Type declarations for SDRAMs
 * @{
 */
/**
 * @brief   SDRAM Burst Length as an exponent of a power of two
 */
typedef enum {
    FMC_BURST_LENGTH_1 = 0,     /* Burst length is 1 */
    FMC_BURST_LENGTH_2 = 1,     /* Burst length is 2 */
    FMC_BURST_LENGTH_4 = 2,     /* Burst length is 4 */
    FMC_BURST_LENGTH_8 = 3,     /* Burst length is 8 */
    FMC_BURST_LENGTH_16 = 4,    /* Burst length is 16 */
    FMC_BURST_LENGTH_32 = 5,    /* Burst length is 32 */
    FMC_BURST_LENGTH_64 = 6,    /* Burst length is 64 */
    FMC_BURST_LENGTH_FULL = 7,  /* Burst length is full page */
} fmc_bust_length_t;

/**
 * @brief   Timing configuration for SDRAM
 */
typedef struct {
    uint8_t row_to_col_delay;   /**< Row to column delay in SDCLK clock
                                     cycles [1..16], delay between Activate
                                     and Read/Write command */
    uint8_t row_precharge;      /**< Row precharge delay in SDCLK clock
                                     cycles [1..15], delay between Precharge
                                     and another command */
    uint8_t recovery_delay;     /**< Recovery delay in SDCLK clock
                                     cycles [1..15], delay between Write and
                                     Precharge command */
    uint8_t row_cylce;          /**< Row cycle delay in SDCLK clock
                                     cycles [1..15], delay between Refresh and
                                     Activate command */
    uint8_t self_refresh;       /**< Self refresh time in SDCLK clock
                                     cycles [1..15] */
    uint8_t exit_self_refresh;  /**< Exit self-refresh delay in SDCLK clock
                                     cycles [1..15], delay between Self-Refresh
                                     and Activate command */
    uint8_t load_mode_register; /**< Load Mode Register to Activate delay in
                                     SDCLK clock cycles [1..15], delay between
                                     Load Mode Register and Activate command */
    uint8_t refresh_period;     /**< Refresh period in milliseconds */
} fmc_sdram_timing_t;

/**
 * @brief   Bank configuration structure for SDRAM
 */
typedef struct {
    uint8_t clk_period;          /**< CLK period [0,2,3] (0 - disabled, n * HCLK cycles) */
    uint8_t row_bits;            /**< Number row address bits [11..13] */
    uint8_t col_bits;            /**< Number column address bits [8..11]*/
    uint8_t cas_latency;         /**< CAS latency in SDCLK clock cycles [1..3] */
    uint8_t read_delay;          /**< Delay for reading data after CAS latency in HCLKs [0..2] */
    bool four_banks;             /**< SDRAM has four internal banks */
    bool write_protect;          /**< Write protection enabled */
    bool burst_read;             /**< Burst read mode enabled */
    bool burst_write;            /**< Burst write mode enabled */
    bool burst_interleaved;      /**< Burst mode interleaved, otherwise sequential */
    fmc_bust_length_t burst_len; /**< Burst length as an exponent of a power of two */
    fmc_sdram_timing_t timing;   /**< SDRAM Timing configuration */
} fmc_sdram_bank_conf_t;
/** @} */

/**
 * @name    Type declarations that are common for all memories
 * @{
 */
/**
 * @brief   FMC GPIO configuration type
 */
typedef struct {
    gpio_t pin;                 /**< GPIO pin */
    gpio_af_t af;               /**< Alternate function */
} fmc_gpio_t;

/**
 * @brief   FMC peripheral configuration
 *
 * The GPIOs are defined depending on used memory type according to the
 * FMC pin definition in Table 12 of section 4 in the
 * [Datasheet for STM32F765xx, STM32F767xx, STM32F768Ax, STM32F769xx](https://www.st.com/resource/en/datasheet/stm32f767zi.pdf).
 * Which memory types are used is defined by the pseudomodules
 * `periph_fmc_nor_sram`, `periph_fmc_nand` and `periph_fmc_sdram`
 *
 * @note For easier handling the configuration structure does not take
 *       multiplexed address/data bits into account.
 */
typedef struct {
    uint8_t bus;                         /**< AHB/APB bus */
    uint32_t rcc_mask;                   /**< Bit in clock enable register */
    fmc_gpio_t data[FMC_DATA_PIN_NUMOF]; /**< Data pins D0 ... */
#if FMC_ADDR_PIN_NUMOF || DOXYGEN
    fmc_gpio_t addr[FMC_ADDR_PIN_NUMOF]; /**< Address pins A0 ... if any */
#endif
    /* signals used by all kind of memories */
    fmc_gpio_t nbl0_pin;                 /**< NBL0 pin */
    fmc_gpio_t nbl1_pin;                 /**< NBL1 pin */
    fmc_gpio_t nbl2_pin;                 /**< NBL2 pin */
    fmc_gpio_t nbl3_pin;                 /**< NBL3 pin */
    fmc_gpio_t nwait_pin;                /**< NWAIT pin */
#if MODULE_PERIPH_FMC_NOR_SRAM
    /* NORs, PSRAMs, and SRAMs use CLK, NOE, NWE, NE, and NADV signals **/
    fmc_gpio_t clk_pin;                  /**< CLK pin */
    fmc_gpio_t noe_pin;                  /**< NOE pin (output enable / read enable) */
    fmc_gpio_t nwe_pin;                  /**< NWE pin (write enable) */
    fmc_gpio_t ne1_pin;                  /**< NE1 pin (subbank 1 enable) */
    fmc_gpio_t ne2_pin;                  /**< NE2 pin (subbank 2 enable) */
    fmc_gpio_t ne3_pin;                  /**< NE3 pin (subbank 3 enable) */
    fmc_gpio_t ne4_pin;                  /**< NE4 pin (subbank 4 enable) */
    fmc_gpio_t nadv_pin;                 /**< NADV pin (address valid) */
#endif /* MODULE_PERIPH_FMC_NORSRAM */
#if MODULE_PERIPH_FMC_SDRAM
    /* SDRAMs use BAx, CLK, RAS, CAS, WE, ... signals **/
    fmc_gpio_t ba0_pin;                  /**< BA0 pin */
    fmc_gpio_t ba1_pin;                  /**< BA1 pin */
    fmc_gpio_t sdclk_pin;                /**< SDCLK pin */
    fmc_gpio_t sdnwe_pin;                /**< SDWE pin */
    fmc_gpio_t sdnras_pin;               /**< SDNRAS pin */
    fmc_gpio_t sdncas_pin;               /**< SDNCAS pin */
    fmc_gpio_t sdcke0_pin;               /**< SDCKE0 pin (SDRAM bank 5 clock enable) */
    fmc_gpio_t sdcke1_pin;               /**< SDCKE1 pin (SDRAM bank 6 clock enable) */
    fmc_gpio_t sdne0_pin;                /**< SDNE0 pin (SDRAM bank 5 enable) */
    fmc_gpio_t sdne1_pin;                /**< SDNE1 pin (SDRAM bank 6 enable) */
#endif /* MODULE_PERIPH_FMC_SDRAM */
} fmc_conf_t;

/**
 * @brief   Memory banks
 */
typedef enum {
    FMC_BANK_1 = 1,             /**< Bank 1 is always available and used for NOR, PSRAM, SRAM */
#if defined(FMC_Bank2_3_R_BASE)
    FMC_BANK_2 = 2,             /**< Bank2 is used for NAND if available */
#endif
#if defined(FMC_Bank2_3_R_BASE) || defined(FMC_Bank3_R_BASE)
    FMC_BANK_3 = 3,             /**< Bank3 is used for NAND if available */
#endif
#if defined(FMC_Bank4_R_BASE)
    FMC_BANK_4 = 4,             /**< Bank4 is used for PC Card if available */
#endif
#if defined(FMC_Bank5_6_R_BASE)
    FMC_BANK_5 = 5,             /**< Bank5 is used for SDRAM if available */
    FMC_BANK_6 = 6,             /**< Bank6 is used for SDRAM if available */
#endif
} fmc_bank_t;

/**
 * @brief   Memory types supported by the FMC controller
 */
typedef enum {
    FMC_SRAM = 0,               /**< SRAM */
    FMC_PSRAM = 1,              /**< PSRAM */
    FMC_NOR = 2,                /**< NOR Flash */
    FMC_NAND = 3,               /**< NAND Flash */
    FMC_SDRAM = 4,              /**< SDRAM Controller used */
} fmc_mem_type_t;

/**
 * @brief   Memory data bus widths
 */
typedef enum {
    FMC_BUS_WIDTH_8BIT = 0,     /**< 8 bit data bus width */
    FMC_BUS_WIDTH_16BIT = 1,    /**< 16 bit data bus width */
    FMC_BUS_WIDTH_32BIT = 2,    /**< 32 bit data bus width */
} fmc_bus_width_t;

/**
 * @brief   Bank configuration structure
 */
typedef struct {
    fmc_bank_t bank;                /**< Bank1 .. Bank4/Bank6 (maximum) */
    fmc_mem_type_t mem_type;        /**< Type of memory */
    fmc_bus_width_t data_width;     /**< Data bus width */
    uint32_t address;               /**< Address of the memory bank */
    uint32_t size;                  /**< Size in bytes of the memory bank */
    union {
        fmc_nor_sram_bank_conf_t nor_sram;  /* Configuration in case of NOR/PSRAM/SRAM */
        fmc_sdram_bank_conf_t sdram;        /* Configuration in case of SDRAM */
    };
} fmc_bank_conf_t;

typedef uint8_t fmc_bank_id_t;  /**< FMC bank identifier */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
