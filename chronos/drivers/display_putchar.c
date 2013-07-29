#include <stdio.h>
#include <display.h>
#include <string.h>

extern int toupper(int c);
extern void (*_putchar)(int c);

static char display_buf[11];

void putchar_to_display(int c);

void init_display_putchar(void)
{
    memset(display_buf, '\0', 11);
    _putchar = putchar_to_display;
}

void putchar_to_display(int c)
{
    if (c == '\n') {
        display_buf[4] = 1;
        return;
    }

    if (display_buf[4]) {
        memset(display_buf, '\0', 11);
    }
    else {
        display_buf[0] = display_buf[1];
        display_buf[1] = display_buf[2];
        display_buf[2] = display_buf[3];
        display_buf[3] = display_buf[5];
        display_buf[5] = display_buf[6];
        display_buf[6] = display_buf[7];
        display_buf[7] = display_buf[8];
        display_buf[8] = display_buf[9];
    }

    display_buf[9] = toupper(c);

    clear_display_all();

    display_chars(LCD_SEG_L1_3_0, display_buf, SEG_ON);
    display_chars(LCD_SEG_L2_5_0, display_buf + 4, SEG_ON);
}
