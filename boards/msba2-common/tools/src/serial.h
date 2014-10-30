/*
 * LPC 2000 Loader, http://www.pjrc.com/arm/lpc2k_pgm
 * Copyright (c) 2004, PJRC.COM, LLC, <paul@pjrc.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef SERIAL_H
#define SERIAL_H

extern char *baud_rate;

int open_serial_port(const char *port_name);
int write_serial_port(const void *buf, int num);
void input_flush_serial_port(void);
int read_serial_port_nb(unsigned char *buf, int bufsize);
int read_serial_port(unsigned char *buf, int bufsize);
void close_serial_port(void);
void send_break_signal(void);
int set_baud(const char *baud_name);
int serial_port_fd(void);
void set_rts(int val);
void set_dtr(int val);
void change_baud(const char *baud_name);

#endif // SERIAL_H
