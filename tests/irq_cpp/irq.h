/*
* Copyright (C) 2021 Jens Wetterich <jens@wetterich-net.de>
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/
/**
 * @ingroup     tests
 * @brief       Dummy irq header to allow mocking. Prevents inclusion of the original IRQ header
 * @{
 *
 * @file
 *
 * @author  Jens Wetterich <jens@wetterich-net.de>
 */
#ifndef IRQ_H
#define IRQ_H
#ifdef __cplusplus
extern "C" {
#endif
unsigned irq_disable();
unsigned irq_enable();
int irq_is_enabled();
int irq_is_in();
void irq_restore(unsigned state);
#ifdef __cplusplus
}
#endif
#endif /* IRQ_H */
