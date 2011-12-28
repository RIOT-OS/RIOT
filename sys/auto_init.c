#include <stdint.h>
#include <stdio.h>
#include "board_uart0.h"
#include "rtc.h"
#include "diskio.h"
#include <auto_init.h>
#include <lpc2387-rtc.h>

#define ENABLE_DEBUG
#include <debug.h>

extern void main(void);

void auto_init(void) {
#ifdef MODULE_BOARD_DISPLAY
    extern void lcd_init();
    lcd_init();
    DEBUG("DISP OK");
#endif
#ifdef MODULE_DISPLAY_PUTCHAR
    extern void init_display_putchar();
    init_display_putchar();
    DEBUG("DISP OK");
#endif
#ifdef MODULE_HWTIMER
    DEBUG("Auto init hwtimer module.\n");
    hwtimer_init();
#endif
#ifdef MODULE_SWTIMER
    DEBUG("Auto init swtimer module.\n");
    swtimer_init();
#endif
#ifdef MODULE_UART0
    DEBUG("Auto init uart0 module.\n");
    board_uart0_init();
#endif
#ifdef MODULE_RTC
    DEBUG("Auto init rtc module.\n");
    rtc_init();
    rtc_enable();
#endif
#ifdef MODULE_SHT11
    DEBUG("Auto init SHT11 module.\n");
    sht11_init();
#endif
#ifdef MODULE_GPIOINT
    DEBUG("Auto init gpioint module.\n");
    gpioint_init();
#endif
#ifdef MODULE_CC110X
    DEBUG("Auto init CC1100 module.\n");
    cc1100_init();
#endif
#ifdef MODULE_LTC4150
    DEBUG("Auto init ltc4150 module.\n");
    ltc4150_init();
#endif
#ifdef MODULE_MCI
    DEBUG("Auto init mci module.\n");
    MCI_initialize();
#endif
#ifdef MODULE_PROFILING
    extern void profiling_init(void);
    profiling_init();
#endif
    main();
}
