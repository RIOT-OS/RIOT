//******************************************************************************
//  eZ430 chronos hello world
//  Description: initializes lcd module and shows the string 'hi earth' on the
//               lcd display becuase 'hello world' is too long
//  Author: Felix Genicio
//******************************************************************************

#include  "cc430x613x.h"
#include <string.h>
#include <display.h>

void display1(void);
void display2(void);

int main(void)
{
  lcd_init();

  clear_display_all();

  uint8_t i = 0;
  uint16_t j;

  while(1) {
      if (i) {
          i = 0;
          display1();
      }
      else {
          i = 1;
          display2();
      }
      for (j = 1; j != 0; j++) {
          if (i) {
            display_symbol(LCD_ICON_BEEPER1, SEG_ON);
          }
          else {
            display_symbol(5, SEG_OFF);
          }
      }
  }
}

void display1(void) {
  display_chars(LCD_SEG_L1_3_0, (uint8_t*) "HI", SEG_ON);
  display_chars(LCD_SEG_L2_5_0, (uint8_t*) " EARTH", SEG_OFF);
}

void display2(void) {
  display_chars(LCD_SEG_L1_3_0, (uint8_t*) "HI", SEG_OFF);
  display_chars(LCD_SEG_L2_5_0, (uint8_t*) " EARTH", SEG_ON);
}

