/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <reent.h>

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 * @brief           Device I/O helpers for UARTs on K60.
 *
 * @author          Joakim Gebart <joakim.gebart@eistec.se>
 */
#ifndef DEVIO_UART_H_
#define DEVIO_UART_H_

#ifdef __cplusplus
extern "C"
{
#endif

long uart0_write_r(struct _reent *r, int fd, const char *ptr, int len);
long uart1_write_r(struct _reent *r, int fd, const char *ptr, int len);
long uart2_write_r(struct _reent *r, int fd, const char *ptr, int len);
long uart3_write_r(struct _reent *r, int fd, const char *ptr, int len);
long uart4_write_r(struct _reent *r, int fd, const char *ptr, int len);
long uart0_read_r(struct _reent *r, int fd, char *ptr, int len);

#ifdef __cplusplus
}
#endif

#endif /* !defined(DEVIO_UART_H_) */
/** @} */
