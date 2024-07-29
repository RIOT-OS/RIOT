/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "log.h"
#include "ztimer.h"

#include "tusb.h"
#include "tinyusb.h"

/*
 * --------------------------------------------------------------------
 * MACRO CONSTANT TYPEDEF PROTYPES
 * --------------------------------------------------------------------
 */

/*
 * Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

/*
 * --------------------------------------------------------------------
 * BLINKING TASK
 * --------------------------------------------------------------------
 */
char led_thread_stack[THREAD_STACKSIZE_MAIN];

void *led_thread_impl(void *arg)
{
    (void)arg;

    while (1) {
        ztimer_sleep(ZTIMER_MSEC, blink_interval_ms);
#ifdef LED0_TOGGLE
        LED0_TOGGLE;
#else
        printf("Blinking with %"PRIu32" msec!\n", blink_interval_ms);
#endif
    }
}

void cdc_task(void);

/* ------------- MAIN ------------- */

int main(void)
{
    ztimer_sleep(ZTIMER_MSEC, 200);

    thread_create(led_thread_stack, sizeof(led_thread_stack),
                  THREAD_PRIORITY_MAIN + 1,
                  THREAD_CREATE_WOUT_YIELD,
                  led_thread_impl, NULL, "led");

    if (!IS_USED(MODULE_AUTO_INIT)) {
        /* If auto-initialization is not used (module `auto_init`),
         * initialize the tinyUSB stack including used peripherals and
         * start the tinyUSB thread. Auto-initialization is used by default. */
        tinyusb_setup();
    }

    while (1) {
        ztimer_sleep(ZTIMER_MSEC, 10);
        cdc_task();
    }

    return 0;
}

/*
 * --------------------------------------------------------------------
 * Device callbacks to be implemented
 * --------------------------------------------------------------------
 */

/*
 * Invoked when device is mounted
 */
void tud_mount_cb(void)
{
    printf("tinyUSB %s\n", __func__);
    blink_interval_ms = BLINK_MOUNTED;
}

/*
 * Invoked when device is unmounted
 */
void tud_umount_cb(void)
{
    printf("tinyUSB %s\n", __func__);
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

/*
 * Invoked when usb bus is suspended
 * remote_wakeup_en : if host allow us  to perform remote wakeup
 * Within 7ms, device must draw an average of current less than 2.5 mA from bus
 */
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    printf("tinyUSB %s\n", __func__);
    blink_interval_ms = BLINK_SUSPENDED;
}

/*
 * Invoked when usb bus is resumed
 */
void tud_resume_cb(void)
{
    printf("tinyUSB %s\n", __func__);
    blink_interval_ms = BLINK_MOUNTED;
}

/*
 * --------------------------------------------------------------------+
 * USB CDC
 * --------------------------------------------------------------------+
 */
void cdc_task(void)
{
    /*
     * connected() check for DTR bit
     * Most but not all terminal client set this when making connection
     */
    if ( tud_cdc_connected() ) {
        /* connected and there are data available */
        if ( tud_cdc_available() )
        {
           /* read data */
           char buf[64];
           uint32_t count = tud_cdc_read(buf, sizeof(buf));
           (void) count;

           /*
            * Echo back
            * Note: Skip echo by commenting out write() and write_flush()
            * for throughput test e.g
            *    $ dd if=/dev/zero of=/dev/ttyACM0 count=10000
            */
          tud_cdc_write(buf, count);
          tud_cdc_write_flush();
        }
    }
}

/*
 * Invoked when cdc when line state changed e.g connected/disconnected
 */
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    (void) itf;
    (void) rts;

    /* TODO set some indicator */
    if ( dtr ) {
        /* Terminal connected */
    }
    else {
        /* Terminal disconnected */
    }
}
