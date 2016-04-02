/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 *
 * @addtogroup      <module_name>
 * @{
 *
 * @file
 * @brief           
 *
 * @author          Attilio Dona'
 */
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include "board.h"
#include "xtimer.h"

#define MORSE_LED_ON   LED_RED_ON
#define MORSE_LED_OFF  LED_RED_OFF

//Define the LED Pin
#define PIN_OUT        9
//Define unit length in ms
#define UNIT_LENGTH    100 //250

#define delay(MILLISEC) xtimer_usleep(MILLISEC*1000)

//Build a struct with the morse code mapping
static const struct {const char letter, *code;} MorseMap[] =
{
    { 'A', ".-" },
    { 'B', "-..." },
    { 'C', "-.-." },
    { 'D', "-.." },
    { 'E', "." },
    { 'F', "..-." },
    { 'G', "--." },
    { 'H', "...." },
    { 'I', ".." },
    { 'J', ".---" },
    { 'K', ".-.-" },
    { 'L', ".-.." },
    { 'M', "--" },
    { 'N', "-." },
    { 'O', "---" },
    { 'P', ".--." },
    { 'Q', "--.-" },
    { 'R', ".-." },
    { 'S', "..." },
    { 'T', "-" },
    { 'U', "..-" },
    { 'V', "...-" },
    { 'W', ".--" },
    { 'X', "-..-" },
    { 'Y', "-.--" },
    { 'Z', "--.." },
    { ' ', "     " }, //Gap between word, seven units

    { '1', ".----" },
    { '2', "..---" },
    { '3', "...--" },
    { '4', "....-" },
    { '5', "....." },
    { '6', "-...." },
    { '7', "--..." },
    { '8', "---.." },
    { '9', "----." },
    { '0', "-----" },

    { '-', "-....-" },
    { '.', ".-.-.-" },
    { ',', "--..--" },
    { '?', "..--.." },
    { '!', "-.-.--" },
    { ':', "---..." },
    { ';', "-.-.-." },
    { '(', "-.--." },
    { ')', "-.--.-" },
    { '"', ".-..-." },
    { '@', ".--.-." },
    { '&', ".-..." },
};


static char morse_word[64];


int encode(const char* string)
{
  size_t i, j;
  char *buffptr = morse_word;

  memset(morse_word, 0, 64);

  for( i = 0; string[i]; ++i )
  {
    for( j = 0; j < sizeof MorseMap / sizeof *MorseMap; ++j )
    {
      if( toupper((unsigned char)string[i]) == MorseMap[j].letter )
      {
          memcpy(buffptr, MorseMap[j].code, strlen(MorseMap[j].code));
          buffptr += strlen(MorseMap[j].code);
          break;
      }
    }
    memcpy(buffptr, " ", 1); //Add trailing space to separate the chars
    buffptr++;
  }
  return (buffptr - morse_word);
}


void morse(const char* namespace, int err_code, int detail) {
    char string[64];
    if (detail) {
        sprintf(string, "%s%d:%d", namespace, err_code, detail);
    } else {
        sprintf(string, "%s%d", namespace, err_code);
    }

    int len = encode(string);


    while (true) {
        for (int i = 0; i <= len; i++) {
            switch (morse_word[i]) {
                case '.': //dit
                    MORSE_LED_ON;
                    delay(UNIT_LENGTH);
                    MORSE_LED_OFF;
                    delay(UNIT_LENGTH);

                    break;

                case '-': //dah
                    MORSE_LED_ON;
                    delay(UNIT_LENGTH*3);
                    MORSE_LED_OFF;
                    delay(UNIT_LENGTH);

                    break;

                case ' ': //gap
                    delay(3*UNIT_LENGTH);
            }
        }
        delay(UNIT_LENGTH*7);
    }

}
