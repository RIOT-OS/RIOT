/*
 * SPDX-FileCopyrightText: 2004 PJRC.COM, LLC, <paul@pjrc.com>
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef LPC2K_PGM_H
#define LPC2K_PGM_H

/* gets a name like "115200", sets baudrate accordingly. */
void change_baud(const char *baud_name);

/* called before/after using serial device, used to have terminal
 * close the device.
*/
void signal_terminal(void);

#endif /* LPC2K_PGM_H */
