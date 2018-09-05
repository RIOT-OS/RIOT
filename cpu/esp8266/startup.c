/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define ENABLE_DEBUG  0
#include "debug.h"

#include <string.h>
#include <stdlib.h>

#include "kernel_init.h"
#include "log.h"
#include "periph/init.h"

#include "c_types.h"
#include "spi_flash.h"

#include "board.h"
#include "common.h"
#include "exceptions.h"
#include "syscalls.h"
#include "tools.h"
#include "thread_arch.h"

#include "esp/iomux_regs.h"
#include "esp/spi_regs.h"
#include "esp/xtensa_ops.h"
#include "sdk/sdk.h"

#if MODULE_ESP_GDBSTUB
#include "esp-gdbstub/gdbstub.h"
#endif

extern void board_init(void);
extern void board_print_config(void);

uint32_t hwrand (void);

#ifdef MODULE_NEWLIB_SYSCALLS_DEFAULT
/* initialization as it should be called from newlibc */
extern void _init(void);
#endif

extern uint8_t _bss_start;
extern uint8_t _bss_end;
extern uint8_t _sheap;
extern uint8_t _eheap;

#ifdef MODULE_ESP_SDK

#include "sdk/ets_task.h"

/**
 * @brief System main loop called by the ETS
 *
 * This function is called by ETS after all initializations has been
 * finished to start periodic processing of defined ETS tasks. We
 * overwrite this ETS function to take over the control and to start RIOT.
 */
void ets_run(void)
{
    #if ENABLE_DEBUG
    register uint32_t *sp __asm__ ("a1");
    ets_uart_printf("_stack      %p\n", sp);
    ets_uart_printf("_bss_start  %p\n", &_bss_start);
    ets_uart_printf("_bss_end    %p\n", &_bss_end);
    ets_uart_printf("_heap_start %p\n", &_sheap);
    ets_uart_printf("_heap_end   %p\n", &_eheap);
    ets_uart_printf("_heap_free  %lu\n", get_free_heap_size());
    #endif

    /* init min task priority */
    ets_task_min_prio = 0;

    #ifdef MODULE_NEWLIB_SYSCALLS_DEFAULT
    _init();
    #endif

    ets_tasks_init();

    /* enable interrupts used by ETS/SDK */
    #ifndef MODULE_ESP_SDK_INT_HANDLING
    ets_isr_unmask(BIT(ETS_FRC2_INUM));
    ets_isr_unmask(BIT(ETS_WDEV_INUM));
    ets_isr_unmask(BIT(ETS_WDT_INUM));
    #endif

    #ifdef MODULE_ESP_GDBSTUB
    gdbstub_init();
    #endif

    #if ENABLE_DEBUG==0
    /* disable SDK messages */
    system_set_os_print(0);
    #endif

    #ifdef CONTEXT_SWITCH_BY_INT
    extern void IRAM thread_yield_isr(void* arg);
    ets_isr_attach(ETS_SOFT_INUM, thread_yield_isr, NULL);
    ets_isr_unmask(BIT(ETS_SOFT_INUM));
    #endif

    /* does not return */
    kernel_init();
}

/**
 * @brief Initialize the CPU, the board and the peripherals
 *
 * This function is called by ESP8266 SDK when all system initializations
 * has been finished.
 */
void system_init(void)
{
    LOG_INFO("\nStarting ESP8266 CPU with ID: %08x", system_get_chip_id());
    LOG_INFO("\nSDK Version %s\n\n", system_get_sdk_version());

    /* avoid reconnection all the time */
    wifi_station_disconnect();

    /* set exception handlers */
    init_exceptions ();

    /* init random number generator */
    srand(hwrand());

    /* init flash drive */
    extern void flash_drive_init (void);
    flash_drive_init();

    /* trigger static peripheral initialization */
    periph_init();

    /* trigger board initialization */
    board_init();

    /* print the board config */
    board_print_config();
}


/**
 * @brief   Entry point in user space after a system reset
 *
 * This function is called after system reset by the ESP8266 SDK. In this
 * functions following steps are neccessary:
 *
 * 1. Reinit system timer as microsecond timer (precision is 500 us)
 * 2. Set the UART parameters for serial output
 * 3. Set the system initialization callback
 */

void IRAM user_init (void)
{
    syscalls_init ();
    thread_isr_stack_init ();

    /* run system in high performance mode */
    system_update_cpu_freq(160);

    /* reinit system timer as microsecond timer */
    system_timer_reinit ();

    /* setup the serial communication */
    uart_div_modify(0, UART_CLK_FREQ / STDIO_UART_BAUDRATE);

    /* once the ETS initialization is done we can start with our code as callback */
    system_init_done_cb(system_init);

    /* keep wifi interface in null mode per default */
    wifi_set_opmode_current (0);
}

#else /* MODULE_ESP_SDK */

#include "esp/dport_regs.h"
#include "esp/phy_info.h"
#include "esp/spiflash.h"

/**
 * @brief   Defines the structure of the file header in SPI flash
 *
 * @see https://github.com/espressif/esptool/wiki/Firmware-Image-Format
 */
typedef struct __attribute__((packed))
{
    uint8_t  magic;    /* always 0xe9 */
    uint8_t  segments; /* number of segments */
    uint8_t  mode;     /* 0 - qio, 1 - qout, 2 - dio, 3 - dout */
    uint8_t  speed:4;  /* 0 - 40 MHz, 1 - 26 MHz, 2 - 20 MHz, 15 - 80 MHz */
    uint8_t  size:4;   /* 0 - 512 kB, 1 - 256 kB, 2 - 1 MB, 3 - 2 MB, 4 - 4 MB */

} _spi_flash_header;

#define SPI_FLASH_SECTOR_SIZE 4096

struct s_info {
    uint32 ap_ip;    /* +00 */
    uint32 ap_mask;  /* +04 */
    uint32 ap_gw;    /* +08 */
    uint32 st_ip;    /* +0C */
    uint32 st_mask;  /* +10 */
    uint32 st_gw;    /* +14 */
    uint8 ap_mac[6]; /* +18 */
    uint8 st_mac[6]; /* +1E */
}  __attribute__((packed, aligned(4)));

static struct s_info info;

enum rst_reason {
    REASON_DEFAULT_RST      = 0,
    REASON_WDT_RST          = 1,
    REASON_EXCEPTION_RST    = 2,
    REASON_SOFT_WDT_RST     = 3,
    REASON_SOFT_RESTART     = 4,
    REASON_DEEP_SLEEP_AWAKE = 5,
    REASON_EXT_SYS_RST      = 6
};

struct rst_info{
    uint32 reason;
    uint32 exccause;
    uint32 epc1;
    uint32 epc2;
    uint32 epc3;
    uint32 excvaddr;
    uint32 depc;
};

/**
 * @brief   System configuration store formats
 *
 * source https://github.com/pvvx/esp8266web
 * (c) PV` 2015
 *
 * @{
 */

/* SDK 2.0.0 */
#define wifi_config_size 0x494 /* 1172 bytes */
#define g_ic_size (wifi_config_size + 532) /* 1704 bytes */

struct ets_store_wifi_hdr { /* Sector flash addr flashchip->chip_size-0x1000  (0x4027F000) */
    uint8  bank;      /* +00 = 0, 1 WiFi config flash addr: */
                      /*       0 - flashchip->chip_size-0x3000 (0x7D000), */
                      /*       1 - flashchip->chip_size-0x2000 */
    uint8  unused[3]; /* +01 = 0xff, 0xff, 0xff */
    uint32 flag;      /* +04 = 0x55aa55aa */
    uint32 wr_cnt;    /* +08 = 0x00000001 */
    uint32 len[2];    /* +12 = 0x00000020, 0xffffffff */
    uint32 chk[2];    /* +20 = 0x000000ed, 0xffffffff */
    uint32 flag2;     /* +28 = 0xaa55aa55 */
};

static const uint8_t
ets_store_wifi_hdr_default[sizeof(struct ets_store_wifi_hdr)] =
{
    0x00, 0xff, 0xff, 0xff, 0xaa, 0x55, 0xaa, 0x55,
    0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xed, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0x55, 0xaa, 0x55, 0xaa
};

struct s_wifi_store { /* WiFi config flash addr: flashchip->chip_size - 0x3000 or -0x2000 */
    /* SDK >= 2.0.0 */
    uint8    boot_info[8];     /* +000   0x000         (boot_info[1]) boot_version */
    uint8    wfmode[4];        /* +008   0x008 */
    uint32   st_ssid_len;      /* +012   0x00c */
    uint8    st_ssid[32];      /* +016   0x010 */
    uint8    field_048[7];     /* +048   0x030 */
    uint8    st_passw[64];     /* +055   0x037 */
    uint8    field_119;        /* +119   0x077 */
    uint8    data_120[32];     /* +120   0x078 */
    uint8    field_152[17];    /* +152   0x098 */
    uint8    field_169;        /* +169   0x0a9 */
    uint8    field_170[6];     /* +170   0x0aa */
    uint32   ap_ssid_len;      /* +176   0x0b0 */
    uint8    ap_ssid[32];      /* +180   0x0b4 */
    uint8    ap_passw[64];     /* +212   0x0d4 */
    uint8    field_276[32];    /* +276   0x114 */
    uint8    field_308;        /* +308   0x134 */
    uint8    wfchl;            /* +309   0x135 */
    uint8    field_310;        /* +310   0x136 */
    uint8    field_311;        /* +311   0x137 */
    uint8    field_312;        /* +312   0x138 */
    uint8    field_313;        /* +313   0x139 */
    uint8    field_314;        /* +314   0x13a */
    uint8    field_315;        /* +315   0x13b */
    uint16   field_316;        /* +316   0x13c */
    uint8    field_318[2];     /* +318   0x13e */
    uint32   st1ssid_len;      /* +320   0x140 */
    uint8    st1ssid[32];      /* +324   0x144 */
    uint8    st1passw[64];     /* +356   0x164 */
    uint8    field_420[400];   /* +420   0x1a4 */
    uint32   field_820[3];     /* +820   0x334 */
    uint8    field_832[4];     /* +832   0x340     wifi_station_set_auto_connect */
    uint32   phy_mode;         /* +836   0x344 */
    uint8    field_840[36];    /* +840   0x348 */
    uint16   beacon;           /* +876   0x36c     876+532 g_ic+1408 */
    uint8    field_878[2];     /* +878   0x36e */
    uint32   field_880;        /* +880   0x370 */
    uint32   field_884;        /* +884   0x374 */
    uint32   field_888;        /* +888   0x378 */
    uint8    field_892[284];   /* +892   0x37c     ... 1176  0x498 */
};

struct s_g_ic {
    uint32    boot_info;        /* +0000 g_ic+0    0x3FFF2324 boot_version? */
    uint32    field_004;        /* +0004 g_ic+4 */
    uint32    field_008;        /* +0008 g_ic+8 */
    uint32    field_00C;        /* +000C g_ic+12 */
    struct netif **netif1;      /* +0010 g_ic+16 */
    struct netif **netif2;      /* +0014 g_ic+20 */
    uint32    field_018;        /* +0018 g_ic+24 */
    uint32    field_01C;        /* +001C g_ic+28 */
    uint32    field_020;        /* +0020 g_ic+32 */
    uint32    field_024;        /* +0024 g_ic+36 */
    uint32    field_028;        /* +0028 g_ic+40 */
    uint8     field_02C[84];    /* +002C g_ic+44 */
    uint32    field_080;        /* +0080 g_ic+128 */
    uint8     field_084[200];   /* +0084 g_ic+132 */
    /* [0x12c] */
    void *    field_14C;        /* +014C g_ic+332 */
    uint32    ratetable;        /* +0150 g_ic+336 */
    uint8     field_154[44];    /* +0154 g_ic+340 */
    uint32    field_180;        /* +0180 g_ic+384 */
    /* [0x170..0x180] wifi_get_user_ie */
    void *    field_184;        /* +0184 g_ic+388 user_ie_manufacturer_recv_cb */
    uint32    field_188;        /* +0188 g_ic+392 */
    uint32    field_18C;        /* +018C g_ic+396 */
    uint32    field_190;        /* +0190 g_ic+400 */
    uint32    field_194;        /* +0194 g_ic+404 */
    uint32    field_198;        /* +0198 g_ic+408 */
    uint32    field_19C;        /* +019C g_ic+412 */
    uint32    field_1A0;        /* +01A0 g_ic+416 */
    uint32    field_1A4;        /* +01A4 g_ic+420 */
    uint32    field_1A8;        /* +01A8 g_ic+424 */
    uint32    field_1AC;        /* +01AC g_ic+428 */
    uint32    field_1B0;        /* +01B0 g_ic+432 */
    uint32    field_1B4;        /* +01B4 g_ic+436 */
    uint32    field_1B8;        /* +01B8 g_ic+440 */
    uint32    field_1BC;        /* +01BC g_ic+444 */
    uint32    field_1C0;        /* +01C0 g_ic+448 */
    uint32    field_1C4;        /* +01C4 g_ic+452 */
    uint32    field_1C8[19];    /* +01C8 g_ic+456 ...532 */
    struct s_wifi_store wifi_store;    /* g_ic+??? */
};

typedef union _u_g_ic{
    struct s_g_ic g;
    uint8  c[g_ic_size];
    uint16 w[g_ic_size/2];
    uint32 d[g_ic_size/4];
} u_g_ic;

static u_g_ic g_ic;

/** }@ */

/**
 * Following functions are from https://github.com/pvvx/esp8266web
 * (c) PV` 2015
 *
 * @{
 */
void read_macaddr_from_otp(uint8_t* mac)
{
    /* fixed prefix */
    mac[0] = 0x18;
    mac[1] = 0xfe;
    mac[2] = 0x34;

    if ((!(DPORT.OTP_CHIPID & (1 << 15))) ||
        ((DPORT.OTP_MAC0 == 0) && (DPORT.OTP_MAC1 == 0))) {
        mac[3] = 0x18;
        mac[4] = 0xfe;
        mac[5] = 0x34;
    }
    else {
        mac[3] = (DPORT.OTP_MAC1 >> 8) & 0xff;
        mac[4] = DPORT.OTP_MAC1 & 0xff;
        mac[5] = (DPORT.OTP_MAC0 >> 24) & 0xff;
    }
}

int ICACHE_FLASH_ATTR flash_data_check(uint8 *check_buf)
{
    uint32 cbuf[32];
    uint32 *pcbuf = cbuf;
    uint8 *pbuf = check_buf;
    int i = 27;
    while (i--) {
        *pcbuf = pbuf[0] | (pbuf[1]<<8) | (pbuf[2]<<16) | (pbuf[3]<<24);
        pcbuf++;
        pbuf+=4;
    }
    cbuf[24] = (DPORT.OTP_MAC1 & 0xFFFFFFF) | ((DPORT.OTP_CHIPID & 0xF000) << 16);
    cbuf[25] = (DPORT.OTP_MAC2 & 0xFFFFFFF) | (DPORT.OTP_MAC0 & 0xFF000000);
    pcbuf = cbuf;
    uint32 xsum = 0;
    do {
        xsum += *pcbuf++;
    } while (pcbuf != &cbuf[26]);
    xsum ^= 0xFFFFFFFF;
    if (cbuf[26] != xsum) {
        return 1;
    }
    return 0;
}

/** }@ */


/**
 * @brief   Startup function hook placed at 0x40100000
 */
void __attribute__((section(".UserEnter.text"))) NORETURN _call_user_start_hook(void)
{
    __asm__ volatile (
        "   vectors_base:      .word   0x40100000  \n"  /* must contain entry point */
        "                                          \n"
        "   _call_user_start:                      \n"  /* system startup function */
        "              .global _call_user_start    \n"
        "              l32r    a2, vectors_base    \n"  /* set vector base */
        "              wsr     a2, vecbase         \n"
        "              call0   cpu_user_start      \n"  /* call startup function */
        );
    UNREACHABLE();
}

void ICACHE_FLASH_ATTR start_phase2 (void);

/**
 * @brief   Startup function
 *
 * This function is the entry point in the user application. It is called
 * after a system reset to startup the system.
 */
void __attribute__((noreturn)) IRAM cpu_user_start (void)
{
    register uint32_t *sp __asm__ ("a1"); (void)sp;

    /* PHASE 1: startup in SDK */

    struct rst_info rst_if = { .reason = 0 };
    system_rtc_mem_read(0, &rst_if, sizeof(struct rst_info));

    /**
     * Setup the serial communication speed. After cold start UART_CLK_FREQ is
     * only 26/40 (65 %). So the baud rate would be only 74880. To have 115200
     * at the beginning of the cold start, we have to set it to 177231 baud.
     * This is changed later in function system_set_pll.
     */
    system_set_pll(1); /* parameter is fix (from esp_init_data_default.bin byte 48) */

    #if 0
    if (rst_if.reason > REASON_DEFAULT_RST) {
        /* warm start */
        uart_div_modify(0, UART_CLK_FREQ / STDIO_UART_BAUDRATE);
    }
    else {
        /* cold start */
        uart_div_modify(0, UART_CLK_FREQ / 177231);
    }
    #else
    uart_div_modify(0, UART_CLK_FREQ / STDIO_UART_BAUDRATE);
    #endif

    /* flush uart_tx_buffer */
    ets_uart_printf("                                                     \n");

    #if ENABLE_DEBUG
    ets_uart_printf("reset reason: %d %d\n", rst_if.reason, rtc_get_reset_reason());
    ets_uart_printf("exccause=%ld excvaddr=%08lx\n", rst_if.exccause, rst_if.excvaddr);
    ets_uart_printf("epc1=%08lx epc2=%08lx epc3=%08lx\n", rst_if.epc1, rst_if.epc2, rst_if.epc3);
    ets_uart_printf("depc=%ld\n", rst_if.depc);
    ets_uart_printf("_stack      %p\n", sp);
    ets_uart_printf("_bss_start  %p\n", &_bss_start);
    ets_uart_printf("_bss_end    %p\n", &_bss_end);
    ets_uart_printf("_heap_start %p\n", &_sheap);
    ets_uart_printf("_heap_end   %p\n", &_eheap);
    ets_uart_printf("_heap_free  %lu\n", get_free_heap_size());
    #endif

    uint32_t flash_sectors;
    uint32_t flash_size;

    _spi_flash_header flash_header;

    SPI(0).USER0 |= SPI_USER0_CS_SETUP;
    SPIRead(0, (uint32_t*)&flash_header, 4);

    assert (flash_header.magic == 0xe9);

    /* SPI flash sectors a 4.096 byte */
    switch (flash_header.size) {
        case 0:  flash_sectors =  128; break; /* 512 kByte */
        case 1:  flash_sectors =   64; break; /* 256 kByte */
        case 2:  flash_sectors =  256; break; /*   1 MByte */
        case 3:  flash_sectors =  512; break; /*   2 MByte */
        case 4:  flash_sectors = 1024; break; /*   4 MByte */
        default: flash_sectors =  128; break; /* default 512 kByte */
    }

    flash_size = flash_sectors * SPI_FLASH_SECTOR_SIZE;

    flashchip->chip_size = flash_size;
    flashchip->sector_size = SPI_FLASH_SECTOR_SIZE;

    /*
     * SPI flash speed params
     * speed = 80MHz / clkdiv_pre / clkcnt_N
     */
    uint32_t clkdiv_pre = 1;      /* default 40 MHz = 80 MHz / 1 / 2 */
    uint32_t clkcnt_N = 2;

    switch (flash_header.speed) {
        case  0: clkdiv_pre = 1;  /* 40 MHz = 80 MHz / 1 / 2 */
                 clkcnt_N   = 2;
                 break;

        case  1: clkdiv_pre = 1;  /* 26 MHz = 80 MHz / 1 / 3 */
                 clkcnt_N   = 3;
                 break;

        case  2: clkdiv_pre = 1;  /* 20 MHz = 80 MHz / 1 / 4 */
                 clkcnt_N   = 4;
                 break;

        case 15: clkdiv_pre = 0;  /* clock is equal to system clock */
                 break;

        default: break;
    }

    if (clkdiv_pre) {
        IOMUX.CONF &= ~IOMUX_CONF_SPI0_CLOCK_EQU_SYS_CLOCK;
        SPI(0).CLOCK = VAL2FIELD_M(SPI_CLOCK_DIV_PRE, clkdiv_pre - 1) |
                       VAL2FIELD_M(SPI_CLOCK_COUNT_NUM, clkcnt_N - 1) |
                       VAL2FIELD_M(SPI_CLOCK_COUNT_HIGH, clkcnt_N/2 - 1) |
                       VAL2FIELD_M(SPI_CLOCK_COUNT_LOW, clkcnt_N - 1);
    }
    else {
        IOMUX.CONF   |= IOMUX_CONF_SPI0_CLOCK_EQU_SYS_CLOCK;
        SPI(0).CLOCK |= SPI_CLOCK_EQU_SYS_CLOCK;
    }

    ets_uart_printf("Flash size: %lu byte, speed %d MHz",
                    flash_size, clkdiv_pre ? 80 / clkdiv_pre / clkcnt_N : 80);

    switch (flash_header.mode) {
        case 0:  ets_printf(", mode QIO\n"); break;
        case 1:  ets_printf(", mode QOUT\n"); break;
        case 2:  ets_printf(", mode DIO\n"); break;
        case 3:  ets_printf(", mode DOUT\n"); break;
        default: ets_printf("\n");
    }

    ets_uart_printf("\nStarting ESP8266 CPU with ID: %08x\n\n", system_get_chip_id());

    /* clear .bss to avoid startup problems because of compiler optimization options */
    memset(&_bss_start, 0x0, &_bss_end-&_bss_start);

    /**
      * Following parts of code are partially from or inspired by the
      * following projects:
      *
      * [esp8266web](https://github.com/pvvx/esp8266web)
      * (c) PV` 2015
      *
      * [esp-open-rtos](https://github.com/SuperHouse/esp-open-rtos.git).
      * Copyright (C) 2015 Superhouse Automation Pty Ltd
      * BSD Licensed as described in the file LICENSE
      *
      * @{
      */
    struct ets_store_wifi_hdr  binfo;
    struct s_wifi_store        wscfg;

    /* load boot info (32 byte) from last sector and */
    uint32_t binfo_addr = flash_size - SPI_FLASH_SECTOR_SIZE;
    SPIRead (binfo_addr, (uint32_t*)&binfo, sizeof(binfo));

    /* load the system config (1176 byte) from last 3 or 2 sectors */
    uint32_t wscfg_addr = flash_size - (binfo.bank ? 2 : 3) * SPI_FLASH_SECTOR_SIZE;
    SPIRead (wscfg_addr, (uint32_t*)&wscfg, sizeof(wscfg));

    Cache_Read_Enable(0, 0, 1);

    #if ENABLE_DEBUG
    printf("boot_inf sector @0x%x\n", flash_size - SPI_FLASH_SECTOR_SIZE);
    esp_hexdump(&binfo, sizeof(binfo), 'b', 16);
    #endif

    LOG_INFO("load boot_inf 0x%x, len %d, chk %02x\n",
             binfo_addr, sizeof(binfo),
             system_get_checksum((uint8_t*)&binfo, sizeof(binfo)));
    LOG_INFO("load wifi_cfg 0x%x, len %d, chk %02x\n",
             wscfg_addr, sizeof(wscfg),
             system_get_checksum((uint8_t*)&wscfg, sizeof(wscfg)));

    /* check whether boot_inf sector could be loaded */
    if (binfo.bank > 0 && binfo.flag == 0xffffffff) {
        LOG_INFO("no boot_inf sector @0x%x, write a default one to flash\n", binfo_addr);
        memcpy (&binfo, ets_store_wifi_hdr_default, sizeof(binfo));
        spi_flash_write (binfo_addr, (uint32_t*)&binfo, sizeof(binfo));

    }

    /* check the checksum */
    if (binfo.flag == 0x55aa55aa &&
        binfo.chk[binfo.bank] == system_get_checksum((uint8_t*)&wscfg, binfo.len[binfo.bank])) {
        /* checksum test is ok */
    }
    else {
        /* checksum error but continue */
        LOG_INFO("flash check sum error @0x%x\n", wscfg_addr);

        /* check whether there is no wifi_cfg sector */
        uint8_t* wscfg_sec = (uint8_t*)&wscfg;
        size_t i = 0;
        for ( ; i < sizeof(wscfg); i++) {
            if (wscfg_sec[i] != 0xff) {
                break;
            }
        }

        /* no data different from 0xff found, we assume that the flash was erased */
        if (i == sizeof(wscfg)) {
            /* TODO write a default wifi_cfg sector automatically into the flash in that case */
            LOG_INFO("\nno wifi_cfg sector found, use following command:\n"
                     "esptool.py write_flash 0x%x $(RIOT_CPU)/bin/wifi_cfg_default.bin\n\n",
                     wscfg_addr);
        }
    }

    memcpy(&g_ic.g.wifi_store, &wscfg, sizeof(wscfg));
    uart_tx_flush(0);
    uart_tx_flush(1);

    init_exceptions ();

    /* PHASE 2: sdk_init in SDK */

    start_phase2();

    /** }@ */

    /* run system in high performance mode */
    /* TODO system_update_cpu_freq(160); */

    /* PHASE 3: start RIOT-OS kernel */

    /* init random number generator */
    srand(hwrand());

    /* init flash drive */
    extern void flash_drive_init (void);
    flash_drive_init();

    /* trigger static peripheral initialization */
    periph_init();

    /* initialize the board and startup the kernel */
    board_init();

    /* print the board config */
    board_print_config();

    #ifdef MODULE_NEWLIB_SYSCALLS_DEFAULT
    _init();
    #endif

    #ifdef MODULE_ESP_GDBSTUB
    gdbstub_init();
    #endif

    #ifdef CONTEXT_SWITCH_BY_INT
    extern void IRAM thread_yield_isr(void* arg);
    ets_isr_attach(ETS_SOFT_INUM, thread_yield_isr, NULL);
    ets_isr_unmask(BIT(ETS_SOFT_INUM));
    #endif

    /* startup the kernel */
    kernel_init();

    /* should not be reached */
    UNREACHABLE() ;
}


void start_phase2 (void)
{
    uint32_t flash_size = flashchip->chip_size;
    struct rst_info rst_if;

    system_rtc_mem_read(0, &rst_if, sizeof(struct rst_info));

    /**
      * Following parts of code are partially from or inspired by the
      * following projects:
      *
      * [esp8266web](https://github.com/pvvx/esp8266web)
      * (c) PV` 2015
      *
      * [esp-open-rtos](https://github.com/SuperHouse/esp-open-rtos.git).
      * Copyright (C) 2015 Superhouse Automation Pty Ltd
      * BSD Licensed as described in the file LICENSE
      *
      * @{
      */

    /* changes clock freqeuncy and should be done before system_set_pll */
    sleep_reset_analog_rtcreg_8266();

    /* set correct system clock and adopt UART frequency */
    system_set_pll(1); /* parameter is fixed (from esp_init_data_default.bin byte 48) */
    uart_div_modify(0, UART_CLK_FREQ / STDIO_UART_BAUDRATE);
    uart_tx_flush(0);
    uart_tx_flush(1);

    syscalls_init ();
    thread_isr_stack_init ();

    read_macaddr_from_otp(info.st_mac);
    LOG_INFO("MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
             info.st_mac[0], info.st_mac[1], info.st_mac[2],
             info.st_mac[3], info.st_mac[4], info.st_mac[5]);

    /* load esp_init_data_default.bin */
    uint8_t* pbuf = malloc(1024); (void)pbuf;
    uint32_t phy_info_addr = flash_size - 4 * SPI_FLASH_SECTOR_SIZE;

    sdk_phy_info_t* phy_info = (sdk_phy_info_t*)pbuf;
    spi_flash_read (phy_info_addr, (uint32_t*)phy_info, sizeof(sdk_phy_info_t));

    LOG_INFO("load phy_info 0x%x, len %d, chk %02x\n",
             phy_info_addr, sizeof(sdk_phy_info_t),
             system_get_checksum((uint8_t*)phy_info, sizeof(sdk_phy_info_t)));

    /* load rf_cal_sec (default rf_cal_sec = flash_sectors - 5) */
    uint32_t rf_cal_sec = user_rf_cal_sector_set();
    uint32_t rf_cal_sec_addr = rf_cal_sec * SPI_FLASH_SECTOR_SIZE;
    spi_flash_read (rf_cal_sec_addr, (uint32_t*)(pbuf + 128), 628);

    LOG_INFO("rf_cal_sec=%d\n", rf_cal_sec);
    LOG_INFO("load rf_cal 0x%x, len %d, chk %02x\n",
             rf_cal_sec_addr, 628, system_get_checksum(pbuf + 128, 628));
    LOG_INFO("reset reason: %d %d\n", rst_if.reason, rtc_get_reset_reason());

    #if ENABLE_DEBUG
    printf("phy_info sector @0x%x\n", phy_info_addr);
    esp_hexdump(pbuf, 128, 'b', 16);

    printf("rf_cal sector @0x%x\n", rf_cal_sec_addr);
    /* esp_hexdump(pbuf+128, 628, 'b', 16); */
    #endif

    sdk_phy_info_t default_phy_info;
    get_default_phy_info(&default_phy_info);

    if (phy_info->version != default_phy_info.version) {
        /* check whether there is no phy_info sector */
        uint8_t* phy_info_sec = (uint8_t*)phy_info;
        size_t i = 0;
        for ( ; i < sizeof(sdk_phy_info_t); i++) {
            if (phy_info_sec[i] != 0xff) {
                break;
            }
        }

        /* no data different from 0xff found, we assume that the flash was erased */
        if (i == sizeof(sdk_phy_info_t)) {
            /* write a default default phy_info sector into the flash */
            LOG_INFO("no phy_info sector found @0x%x, "
                     "writing esp_init_data_default.bin into flash\n",
                     phy_info_addr);
            spi_flash_write (phy_info_addr,
                             (uint32_t*)&default_phy_info, sizeof(sdk_phy_info_t));
        }

        LOG_INFO("set default esp_init_data!\n");
        memcpy(pbuf, &default_phy_info, sizeof(sdk_phy_info_t));
    }

    extern uint8_t* phy_rx_gain_dc_table;
    extern uint8_t  phy_rx_gain_dc_flag;

    phy_rx_gain_dc_table = &pbuf[0x100];
    phy_rx_gain_dc_flag  = 0;

    int xflg = (flash_data_check(&pbuf[128]) != 0 ||
                phy_check_data_table(phy_rx_gain_dc_table, 125, 1) != 0) ? 1 : 0;

    if (rst_if.reason != REASON_DEEP_SLEEP_AWAKE) {
        phy_afterwake_set_rfoption(1);
        if (xflg == 0) {
            write_data_to_rtc(pbuf+128);
        }
    }

    g_ic.c[491] = ((phy_info->freq_correct_mode & 7 )== 3) ? 1 : 0;
    pbuf[0xf8] = 0;

    /* clear RTC memory */
    memset(&rst_if, 0, sizeof(struct rst_info));
    system_rtc_mem_write(0, &rst_if, sizeof(struct rst_info));

    uart_tx_flush(0);
    uart_tx_flush(1);

    if (register_chipv6_phy(pbuf)) {
        LOG_ERROR ("register_chipv6_phy failed");
    }

    free (pbuf);

    /** @} */
}

#endif /* MODULE_ESP_SDK */
