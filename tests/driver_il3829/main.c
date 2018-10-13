#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "xtimer.h"

#include "il3829.h"
#include "il3829_pictures.h"
#include "il3829_params.h"

const uint16_t riot_32_height = sizeof(riot_logo_32)/sizeof(riot_logo_32[0]);

/* Draw the large RIOT logo with a full refresh */
void draw_riot(il3829_t *dev) {
    il3829_init_full(dev);
    il3829_activate(dev);
    il3829_write_buffer(dev, (uint8_t*)riot_icon_200, sizeof riot_icon_200);
    il3829_update_full(dev);
    il3829_deactivate(dev);
}

/* Draw a small RIOT logo with a partial refresh */
void draw_small_riot(il3829_t *dev, uint8_t x, uint16_t y) {
    il3829_init_part(dev);
    il3829_activate(dev);
    il3829_set_area(dev, x, x + 32, y, y + riot_32_height);
    il3829_write_buffer(dev, (uint8_t*)riot_logo_32, sizeof riot_logo_32);
    il3829_update_part(dev);
    il3829_deactivate(dev);
}

int main(void)
{
    il3829_t dev;
    int init = il3829_init(&dev, &il3829_params[0],
                           DISPLAY_X, DISPLAY_Y, IL3829_Y_INC_X_INC);
    if (init != 0) {
        printf("IL3829 INIT FAILED: %i\n", init);
        return 1;
    }

    xtimer_ticks32_t last_wakeup = xtimer_now();
    while (1) {
        /* Set both RAM buffers to the RIOT logo */
        draw_riot(&dev);
        draw_riot(&dev);
        xtimer_periodic_wakeup(&last_wakeup, 3 * US_PER_SEC);

        /* Draw small RIOT logos on the display */
        for(int y = 0; y < dev.size_y; y+=riot_32_height) {
            for(int x = 0; x < dev.size_x; x+=32) {
                draw_small_riot(&dev, x, y);
                xtimer_periodic_wakeup(&last_wakeup, 500 * US_PER_MS);
            }
        }
    }

    return 0;
}
