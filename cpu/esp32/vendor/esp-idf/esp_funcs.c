// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* PLEASE NOTE: This file is a collection of required functions from
   different files in ESP-IDF */

#define ENABLE_DEBUG  0
#include "debug.h"
#include "esp_common.h"
#include "log.h"

#include <stdarg.h>
#include <string.h>
#include <sys/time.h>

#include "esp_attr.h"
#include "sdk_conf.h"

#include "driver/periph_ctrl.h"
#include "esp32/esp_spiram.h"
#include "esp32/esp_system.h"
#include "freertos/FreeRTOS.h"
#include "heap/esp_heap_caps_init.h"
#include "log/esp_log.h"
#include "periph/hwrng.h"
#include "rom/rtc.h"
#include "rom/cache.h"
#include "rom/efuse.h"
#include "rom/uart.h"
#include "soc/cpu.h"
#include "soc/efuse_reg.h"
#include "soc/gpio_reg.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "xtensa/core-macros.h"
#include "xtensa/xtensa_api.h"

#include "syscalls.h"

/* source: /path/to/esp-idf/components/esp32/panic.c */
void IRAM_ATTR esp_panic_wdt_stop (void)
{
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
    WRITE_PERI_REG(RTC_CNTL_WDTFEED_REG, 1);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG0, RTC_WDT_STG_SEL_OFF);
    REG_CLR_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN);
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, 0);
}

/* source: /path/to/esp-idf/components/esp32/panic.c */
void _esp_error_check_failed(esp_err_t rc, const char *file, int line,
                             const char *function, const char *expression)
{
    ets_printf("ESP_ERROR_CHECK failed: esp_err_t 0x%x at 0x%08x\n",
               rc, (intptr_t)__builtin_return_address(0) - 3);
    #if 0 /* TODO */
    if (spi_flash_cache_enabled()) { /* strings may be in flash cache */
        ets_printf("file: \"%s\" line %d\nfunc: %s\nexpression: %s\n",
                   file, line, function, expression);
    }
    invoke_abort();
    #endif
    exit(1);

    while (1) {}
}

/*
 * provided by: /path/to/esp-idf/component/log/log.c
 */
uint32_t IRAM_ATTR esp_log_timestamp(void)
{
    return system_get_time() / USEC_PER_MSEC;
}


#if MODULE_ESP_LOG_TAGGED
#define ESP_LOG_PREFIX(letter, tag)  \
                        printf(LOG_COLOR_ ## letter #letter " (%d) [%s] ", \
                               system_get_time_ms(), tag)
#else
#define ESP_LOG_PREFIX(letter, tag)  \
                        printf(LOG_COLOR_ ## letter "[%s] ", tag)
#endif

#if MODULE_ESP_LOG_COLORED
#define ESP_LOG_SUFFIX()  printf(LOG_RESET_COLOR)
#else
#define ESP_LOG_SUFFIX()
#endif

/*
 * provided by: /path/to/esp-idf/component/log/log.c
 */
void IRAM_ATTR esp_log_write(esp_log_level_t level,
                             const char* tag, const char* format, ...)
{
    if ((unsigned)level > CONFIG_LOG_DEFAULT_LEVEL) {
        return;
    }

    char _printf_buf[PRINTF_BUFSIZ];
    const char* prefix = (strchr (format, ':') + 2);

    switch (level) {
        case LOG_NONE   : return;
        case LOG_ERROR  : ESP_LOG_PREFIX(E, tag); break;
        case LOG_WARNING: ESP_LOG_PREFIX(W, tag); break;
        case LOG_INFO   : ESP_LOG_PREFIX(I, tag); break;
        case LOG_DEBUG  : ESP_LOG_PREFIX(D, tag); break;
        case LOG_ALL    : ESP_LOG_PREFIX(V, tag); break;
    }

    va_list arglist;
    va_start(arglist, format);

    /* remove time and tag argument from argument list */
    va_arg(arglist, unsigned);
    va_arg(arglist, const char*);

    int ret = vsnprintf(_printf_buf, PRINTF_BUFSIZ, prefix, arglist);

    va_end(arglist);

    if (ret > 0) {
        printf ("%s", _printf_buf);
    }

    ESP_LOG_SUFFIX();
}

/*
 * provided by: /path/to/esp-idf/component/log/log.c
 */
void esp_log_level_set(const char* tag, esp_log_level_t level)
{
    /* TODO implementation */
}

static bool _spi_ram_initialized = false;

/*
 * source: /path/to/esp-idf/component/esp32/cpu_start.c
 */
void spi_ram_init(void)
{
    _spi_ram_initialized = false;

    #if CONFIG_SPIRAM_SUPPORT
    esp_spiram_init_cache();
    if (esp_spiram_init() == ESP_OK) {
        _spi_ram_initialized = true;
    }
    else {
        ESP_EARLY_LOGE("spi_ram", "Failed to init external SPI RAM\n");
        _spi_ram_initialized = false;
    }
    #else
    ESP_EARLY_LOGI("spi_ram", "External SPI RAM functions not enabled\n");
    #endif
}

/*
 * source: /path/to/esp-idf/component/esp32/cpu_start.c
 */
void spi_ram_heap_init(void)
{
    #if CONFIG_SPIRAM_SUPPORT

    #if CONFIG_SPIRAM_MEMTEST
    if (!esp_spiram_test()) {
        return;
    }
    #endif /* CONFIG_SPIRAM_MEMTEST */

    #if CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC
    esp_err_t r=esp_spiram_add_to_heapalloc();
    if (r != ESP_OK) {
        ESP_EARLY_LOGE("spi_ram", "External SPI RAM could not be added to heap!\n");
        abort();
    }

    #if CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL
    r=esp_spiram_reserve_dma_pool(CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL);
    if (r != ESP_OK) {
        ESP_EARLY_LOGE("spi_ram", "Could not reserve internal/DMA pool!\n");
        abort();
    }
    #endif /* CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL */

    #if CONFIG_SPIRAM_USE_MALLOC
    heap_caps_malloc_extmem_enable(CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL);
    #endif /* CONFIG_SPIRAM_USE_MALLOC */

    #endif /* CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC */

    #else  /* CONFIG_SPIRAM_SUPPORT */
    ESP_EARLY_LOGI("spi_ram", "External SPI RAM functions not enabled\n");
    #endif /* CONFIG_SPIRAM_SUPPORT */
}

static const char* TAG = "system_api";
static uint8_t base_mac_addr[6] = { 0 };

/*
 * source: /path/to/esp-idf/component/esp32/system_api.c
 */
esp_err_t esp_efuse_mac_get_default(uint8_t* mac)
{
    uint32_t mac_low;
    uint32_t mac_high;
    uint8_t efuse_crc;
    uint8_t calc_crc;

    mac_low = REG_READ(EFUSE_BLK0_RDATA1_REG);
    mac_high = REG_READ(EFUSE_BLK0_RDATA2_REG);

    mac[0] = mac_high >> 8;
    mac[1] = mac_high;
    mac[2] = mac_low >> 24;
    mac[3] = mac_low >> 16;
    mac[4] = mac_low >> 8;
    mac[5] = mac_low;

    efuse_crc = mac_high >> 16;

    calc_crc = esp_crc8(mac, 6);

    if (efuse_crc != calc_crc) {
         // Small range of MAC addresses are accepted even if CRC is invalid.
         // These addresses are reserved for Espressif internal use.
        if ((mac_high & 0xFFFF) == 0x18fe) {
            if ((mac_low >= 0x346a85c7) && (mac_low <= 0x346a85f8)) {
                return ESP_OK;
            }
        } else {
            ESP_LOGE(TAG, "Base MAC address from BLK0 of EFUSE CRC error, efuse_crc = 0x%02x; calc_crc = 0x%02x", efuse_crc, calc_crc);
            abort();
        }
    }
    return ESP_OK;
}

/*
 * source: /path/to/esp-idf/component/esp32/system_api.c
 */
esp_err_t esp_derive_mac(uint8_t* local_mac, const uint8_t* universal_mac)
{
    uint8_t idx;

    if (local_mac == NULL || universal_mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(local_mac, universal_mac, 6);
    for (idx = 0; idx < 64; idx++) {
        local_mac[0] = universal_mac[0] | 0x02;
        local_mac[0] ^= idx << 2;

        if (memcmp(local_mac, universal_mac, 6)) {
            break;
        }
    }

    return ESP_OK;
}

/*
 * source: /path/to/esp-idf/component/esp32/system_api.c
 */
esp_err_t esp_base_mac_addr_get(uint8_t *mac)
{
    uint8_t null_mac[6] = {0};

    if (memcmp(base_mac_addr, null_mac, 6) == 0) {
        ESP_LOGI(TAG, "Base MAC address is not set, read default base MAC address from BLK0 of EFUSE");
        return ESP_ERR_INVALID_MAC;
    }

    memcpy(mac, base_mac_addr, 6);

    return ESP_OK;
}

/*
 * source: /path/to/esp-idf/component/esp32/system_api.c
 */
esp_err_t esp_read_mac(uint8_t* mac, esp_mac_type_t type)
{
    uint8_t efuse_mac[6];

    if (mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (type < ESP_MAC_WIFI_STA || type > ESP_MAC_ETH) {
        ESP_LOGE(TAG, "mac type is incorrect");
        return ESP_ERR_INVALID_ARG;
    }

    _Static_assert(UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR \
            || UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR, \
            "incorrect NUM_MAC_ADDRESS_FROM_EFUSE value");

    if (esp_base_mac_addr_get(efuse_mac) != ESP_OK) {
        esp_efuse_mac_get_default(efuse_mac);
    }

    switch (type) {
    case ESP_MAC_WIFI_STA:
        memcpy(mac, efuse_mac, 6);
        break;
    case ESP_MAC_WIFI_SOFTAP:
        if (UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR) {
            memcpy(mac, efuse_mac, 6);
            mac[5] += 1;
        }
        else if (UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR) {
            esp_derive_mac(mac, efuse_mac);
        }
        break;
    case ESP_MAC_BT:
        memcpy(mac, efuse_mac, 6);
        if (UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR) {
            mac[5] += 2;
        }
        else if (UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR) {
            mac[5] += 1;
        }
        break;
    case ESP_MAC_ETH:
        if (UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR) {
            memcpy(mac, efuse_mac, 6);
            mac[5] += 3;
        }
        else if (UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR) {
            efuse_mac[5] += 1;
            esp_derive_mac(mac, efuse_mac);
        }
        break;
    default:
        ESP_LOGW(TAG, "incorrect mac type");
        break;
    }

    return ESP_OK;
}

/*
 * source: /path/to/esp-idf/component/esp32/system_api.c
 */
/* "inner" restart function for after RTOS, interrupts & anything else on this
 * core are already stopped. Stalls other core, resets hardware,
 * triggers restart.
*/
void IRAM_ATTR esp_restart_noos(void)
{
    // Disable interrupts
    xt_ints_off(0xFFFFFFFF);

    // Enable RTC watchdog for 1 second
    REG_WRITE(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
    REG_WRITE(RTC_CNTL_WDTCONFIG0_REG,
            RTC_CNTL_WDT_FLASHBOOT_MOD_EN_M |
            (RTC_WDT_STG_SEL_RESET_SYSTEM << RTC_CNTL_WDT_STG0_S) |
            (RTC_WDT_STG_SEL_RESET_RTC << RTC_CNTL_WDT_STG1_S) |
            (1 << RTC_CNTL_WDT_SYS_RESET_LENGTH_S) |
            (1 << RTC_CNTL_WDT_CPU_RESET_LENGTH_S) );
    REG_WRITE(RTC_CNTL_WDTCONFIG1_REG, rtc_clk_slow_freq_get_hz() * 1);

    // Reset and stall the other CPU.
    // CPU must be reset before stalling, in case it was running a s32c1i
    // instruction. This would cause memory pool to be locked by arbiter
    // to the stalled CPU, preventing current CPU from accessing this pool.
    const uint32_t core_id = 0;
    const uint32_t other_core_id = (core_id == 0) ? 1 : 0;
    esp_cpu_reset(other_core_id);
    esp_cpu_stall(other_core_id);

    // Other core is now stalled, can access DPORT registers directly
    esp_dport_access_int_abort();

    // Disable TG0/TG1 watchdogs
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_config0.en = 0;
    TIMERG0.wdt_wprotect=0;
    TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG1.wdt_config0.en = 0;
    TIMERG1.wdt_wprotect=0;

    // Flush any data left in UART FIFOs
    uart_tx_wait_idle(0);
    uart_tx_wait_idle(1);
    uart_tx_wait_idle(2);

    // Disable cache
    Cache_Read_Disable(0);
    Cache_Read_Disable(1);

    // 2nd stage bootloader reconfigures SPI flash signals.
    // Reset them to the defaults expected by ROM.
    WRITE_PERI_REG(GPIO_FUNC0_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC1_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC2_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC3_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC4_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC5_IN_SEL_CFG_REG, 0x30);

    // Reset wifi/bluetooth/ethernet/sdio (bb/mac)
    DPORT_SET_PERI_REG_MASK(DPORT_CORE_RST_EN_REG,
         DPORT_BB_RST | DPORT_FE_RST | DPORT_MAC_RST |
         DPORT_BT_RST | DPORT_BTMAC_RST | DPORT_SDIO_RST |
         DPORT_SDIO_HOST_RST | DPORT_EMAC_RST | DPORT_MACPWR_RST |
         DPORT_RW_BTMAC_RST | DPORT_RW_BTLP_RST);
    DPORT_REG_WRITE(DPORT_CORE_RST_EN_REG, 0);

    // Reset timer/spi/uart
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,
            DPORT_TIMERS_RST | DPORT_SPI_RST_1 | DPORT_UART_RST);
    DPORT_REG_WRITE(DPORT_PERIP_RST_EN_REG, 0);

    // Set CPU back to XTAL source, no PLL, same as hard reset
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_XTAL);

    // Clear entry point for APP CPU
    DPORT_REG_WRITE(DPORT_APPCPU_CTRL_D_REG, 0);

    // Reset CPUs
    if (core_id == 0) {
        // Running on PRO CPU: APP CPU is stalled. Can reset both CPUs.
        esp_cpu_reset(1);
        esp_cpu_reset(0);
    } else {
        // Running on APP CPU: need to reset PRO CPU and unstall it,
        // then reset APP CPU
        esp_cpu_reset(0);
        esp_cpu_unstall(0);
        esp_cpu_reset(1);
    }
    while(true) {
        ;
    }
}

/*
 * source: /path/to/esp-idf/components/wpa_supplicant/port/os_xtensa.c
 */
typedef long os_time_t;
struct os_time {
    os_time_t sec;
    os_time_t usec;
};

int os_get_time(struct os_time *t)
{
    return gettimeofday((struct timeval*) t, NULL);
}

/*
 * source: /path/to/esp-idf/components/wpa_supplicant/port/os_xtensa.c
 */
unsigned long os_random(void)
{
    return esp_random();
}

/*
 * provided by: /path/to/esp-idf/components/wpa_supplicant/port/os_xtensa.c
 */
int os_get_random(unsigned char *buf, size_t len)
{
    hwrng_read((void*)buf, len);
    return 0;
}

uint32_t esp_random(void)
{
    uint32_t tmp;
    hwrng_read((void*)&tmp, sizeof(uint32_t));
    return tmp;
}

/*
 * source: /path/to/esp-idf/components/lwip/netif/ethernet.c
 */
#define ETH_HWADDR_LEN    6

struct eth_addr {
   uint8_t addr[ETH_HWADDR_LEN];
};

#ifndef MODULE_LWIP_ETHERNET
const struct eth_addr ethbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};
#endif

#if MODULE_ESP_WIFI_ANY
/*
 * source: /path/to/esp-idf/components/smartconfig_ack.c
 */
#include "esp_smartconfig.h"
#include "smartconfig_ack.h"

void sc_ack_send(sc_ack_t *param)
{
    NOT_SUPPORTED();
}

/*
 * source: /path/to/esp-idf/components/smartconfig_ack.c
 */
void sc_ack_send_stop(void)
{
    NOT_SUPPORTED();
}
#endif

/*
 * source: /path/to/esp-idf/components/bootloader_support/src/bootloader_clock.c
 */
void bootloader_clock_configure(void)
{
    // ROM bootloader may have put a lot of text into UART0 FIFO.
    // Wait for it to be printed.
    // This is not needed on power on reset, when ROM bootloader is running at
    // 40 MHz. But in case of TG WDT reset, CPU may still be running at >80 MHZ,
    // and will be done with the bootloader much earlier than UART FIFO is empty.
    uart_tx_wait_idle(0);

    /* Set CPU to 80MHz. Keep other clocks unmodified. */
    rtc_cpu_freq_t cpu_freq = RTC_CPU_FREQ_80M;
#ifndef RIOT_VERSION
    /* On ESP32 rev 0, switching to 80MHz if clock was previously set to
     * 240 MHz may cause the chip to lock up (see section 3.5 of the errata
     * document). For rev. 0, switch to 240 instead if it was chosen in
     * menuconfig.
     */
    uint32_t chip_ver_reg = REG_READ(EFUSE_BLK0_RDATA3_REG);
    if ((chip_ver_reg & EFUSE_RD_CHIP_VER_REV1_M) == 0 &&
            CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ == 240) {
        cpu_freq = RTC_CPU_FREQ_240M;
    }
#endif

    rtc_clk_config_t clk_cfg = RTC_CLK_CONFIG_DEFAULT();
    clk_cfg.xtal_freq = CONFIG_ESP32_XTAL_FREQ;
    clk_cfg.cpu_freq = cpu_freq;
    clk_cfg.slow_freq = rtc_clk_slow_freq_get();
    clk_cfg.fast_freq = rtc_clk_fast_freq_get();
    rtc_clk_init(clk_cfg);
    /* As a slight optimization, if 32k XTAL was enabled in sdkconfig, we enable
     * it here. Usually it needs some time to start up, so we amortize at least
     * part of the start up time by enabling 32k XTAL early.
     * App startup code will wait until the oscillator has started up.
     */
#ifdef CONFIG_ESP32_RTC_CLOCK_SOURCE_EXTERNAL_CRYSTAL
    if (!rtc_clk_32k_enabled()) {
        rtc_clk_32k_bootstrap(CONFIG_ESP32_RTC_XTAL_BOOTSTRAP_CYCLES);
    }
#endif
}
