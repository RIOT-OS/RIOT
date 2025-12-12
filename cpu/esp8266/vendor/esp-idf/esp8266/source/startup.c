// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#include "sdkconfig.h"

#include "nvs_flash.h"
#ifndef RIOT_OS
#include "tcpip_adapter.h"
#else
#include "buildinfo/cpu.h"
#endif

#include "esp_log.h"
#include "esp_image_format.h"
#include "esp_phy_init.h"
#include "esp_wifi_osi.h"
#include "esp_heap_caps_init.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "internal/esp_wifi_internal.h"
#include "internal/esp_system_internal.h"

#define FLASH_MAP_ADDR 0x40200000
#define FLASH_MAP_SIZE 0x00100000

extern void chip_boot(void);
extern int esp_rtc_init(void);
extern int mac_init(void);
extern int base_gpio_init(void);
extern int watchdog_init(void);
extern int wifi_timer_init(void);
extern int wifi_nvs_init(void);
extern esp_err_t esp_pthread_init(void);
extern void phy_get_bb_evm(void);

static void user_init_entry(void *param)
{
    extern void app_main(void);

    phy_get_bb_evm();

#ifdef CPU_ESP8266
    /* initialize newlib system calls */
    extern void syscalls_init (void);
    syscalls_init ();
#endif

    if (nvs_flash_init() != 0) {
        assert(0);
    }
    if (wifi_nvs_init() != 0) {
        assert(0);
    }
    if (esp_rtc_init() != 0) {
        assert(0);
    }
    if (mac_init() != 0) {
        assert(0);
    }
    if (base_gpio_init() != 0) {
        assert(0);
    };

    esp_phy_load_cal_and_init(0);

#ifdef MODULE_ESP_WIFI_ANY
    if (wifi_timer_init() != 0) {
        assert(0);
    }
#endif

    esp_wifi_set_rx_pbuf_mem_type(WIFI_RX_PBUF_DRAM);

#if CONFIG_RESET_REASON
    esp_reset_reason_init();
#endif

#ifdef CONFIG_TASK_WDT
    esp_task_wdt_init();
#endif

#ifdef CONFIG_ENABLE_PTHREAD
    if (esp_pthread_init() != 0) {
        assert(0);
    }
#endif

#ifdef RIOT_OS
    /* initialize RIOT for ESP8266 */
    wifi_os_init();
    /* start RIOT kernel */
    wifi_os_start();
#else
    app_main();

    wifi_task_delete(NULL);
#endif
}

void call_user_start(size_t start_addr)
{
    int i;
    int *p;

    extern int _bss_start, _bss_end;

    esp_image_header_t *head = (esp_image_header_t *)(FLASH_MAP_ADDR + (start_addr & (FLASH_MAP_SIZE - 1)));
    esp_image_segment_header_t *segment = (esp_image_segment_header_t *)((uintptr_t)head + sizeof(esp_image_header_t));

    for (i = 0; i < 3; i++) {
        segment = (esp_image_segment_header_t *)((uintptr_t)segment + sizeof(esp_image_segment_header_t) + segment->data_len);

        uint32_t *dest = (uint32_t *)segment->load_addr;
        uint32_t *src = (uint32_t *)((uintptr_t)segment + sizeof(esp_image_segment_header_t));
        uint32_t size = segment->data_len / sizeof(uint32_t);

        while (size--)
            *dest++ = *src++;
    }

    /*
     * When finish copying IRAM program, the exception vect must be initialized.
     * And then user can load/store data which is not aligned by 4-byte.
     */
    __asm__ __volatile__(
        "movi       a0, 0x40100000\n"
        "wsr        a0, vecbase\n"
        : : :"memory");

#ifndef CONFIG_BOOTLOADER_INIT_SPI_FLASH
    chip_boot();
#endif

    /* clear bss data */
    for (p = &_bss_start; p < &_bss_end; p++)
        *p = 0;

    __asm__ __volatile__(
        "rsil       a2, 2\n"
        "movi       a1, _chip_interrupt_tmp\n"
        : : :"memory");

    heap_caps_init();

#ifdef RIOT_OS
    /* in case of RIOT, user_init_entry is called directly and not from a task */
    user_init_entry(NULL);
#else
    wifi_os_init();

    if (wifi_task_create(user_init_entry, "uiT", CONFIG_MAIN_TASK_STACK_SIZE, NULL, wifi_task_get_max_priority()) == NULL) {
        assert(0);
    }

    wifi_os_start();
#endif
}
