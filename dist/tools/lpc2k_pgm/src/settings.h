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

#ifndef SETTINGS_H
#define SETTINGS_H

extern void init_settings(void);
extern const char *file_setting(void);
extern const char *port_setting(void);
extern const char *baud_setting(void);
extern const char *crystal_setting(void);
extern void new_file_setting(const char *new_file);
extern void new_port_setting(const char *new_port);
extern void new_baud_setting(const char *new_baud);
extern void new_crystal_setting(const char *new_xtal);

extern char *baud_list[];

#endif /* SETTINGS_H */
