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

extern int download_begin(char *file);
extern void soft_reboot_begin(void);
extern void hard_reset_to_bootloader(void);
extern void hard_reset_to_user_code(void);
extern void download_cancel(const char *mesg);
extern void download_rx_term(const unsigned char *buf, int num);
extern void download_rx_port(const unsigned char *buf, int num);
extern void download_timer(void);
