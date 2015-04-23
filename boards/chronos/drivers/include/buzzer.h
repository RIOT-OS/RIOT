/*
 * Copyright 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#ifdef __cplusplus
extern "C" {
#endif

void buzzer_beep(uint8_t pitch, uint16_t duration);

#ifdef __cplusplus
}
#endif

#endif /* BUZZER_H */
