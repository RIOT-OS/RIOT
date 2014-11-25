/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * The IRQ handler of x86 boards.
 *
 * @ingroup x86-irq
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__PIC__H__
#define CPU__X86__PIC__H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the Programmable Interrupt Controller.
 *
 * This function is called during initialization by x86_startup().
 * You must not call this function on your own accord.
 */
void x86_init_pic(void);

#define X86_IRQ_COUNT (0x10) /**< Number of IRQ lines */

/**
 * @brief   Offset of the IRQ master in the interrupt numbers.
 *
 * x86_interrupts.c expects this value to be `0x20`. Do not change.
 */
#define PIC_MASTER_INTERRUPT_BASE (0x20)
/**
 * @brief   Offset of the IRQ slave in the interrupt numbers.
 *
 * The whole IRQ subsystem expects the IRQ slave numbers to come immediately after the master.
 */
#define PIC_SLAVE_INTERRUPT_BASE (PIC_MASTER_INTERRUPT_BASE + 8)

#define PIC_MASTER (0x20)
#define PIC_SLAVE (0xA0)

#define PIC_COMMAND (0x00)
#define PIC_DATA (0x01)
#define PIC_IMR (0x01)

#define PIC_EOI 0x20 /**< End-of-interrupt command code */

#define PIC_READ_IRR                0x0a    /**< OCW3 irq ready next CMD read */
#define PIC_READ_ISR                0x0b    /**< OCW3 irq service next CMD read */

#define PIC_ICW1_ICW4       (0x01) /**< ICW4 (not) needed */
#define PIC_ICW1_SINGLE     (0x02) /**< Single (cascade) mode */
#define PIC_ICW1_INTERVAL4  (0x04) /**< Call address interval 4 (8) */
#define PIC_ICW1_LEVEL      (0x08) /**< Level triggered (edge) mode */
#define PIC_ICW1_INIT       (0x10) /**< Initialization - required! */

#define PIC_ICW4_8086       (0x01) /**< 8086/88 (MCS-80/85) mode */
#define PIC_ICW4_AUTO       (0x02) /**< Auto (normal) EOI */
#define PIC_ICW4_BUF_SLAVE  (0x08) /**< Buffered mode/slave */
#define PIC_ICW4_BUF_MASTER (0x0C) /**< Buffered mode/master */
#define PIC_ICW4_SFNM       (0x10) /**< Special fully nested (not) */

#define PIC_NUM_PIT                   (0x0) /**< IRQ line of the Programmable Interrupt Controller **/
#define PIC_NUM_KEYBOARD_CONTROLLER_1 (0x1) /**< IRQ line of the first PS/2 port **/
#define PIC_NUM_SLAVE                 (0x2) /**< not a valid IRQ line! */
#define PIC_NUM_RS232_2_4             (0x3) /**< IRQ line of COM 2+4 **/
#define PIC_NUM_RS232_1_3             (0x4) /**< IRQ line of COM 1+2 **/
#define PIC_NUM_LPT2                  (0x5) /**< IRQ line of the secondary printer or soundcard (available for PCI) **/
#define PIC_NUM_FLOPPY                (0x6) /**< IRQ line of the floppy drive. **/
#define PIC_NUM_LPT1                  (0x7) /**< IRQ line of the parallel port (available for PCI) **/

#define PIC_NUM_RTC                   (0x8) /**< IRQ line of the Real Time Clock **/
#define PIC_NUM_9                     (0x9) /**< Free to use IRQ line (available for PCI) **/
#define PIC_NUM_ATA_4                 (0xa) /**< Free to use IRQ line (available for PCI) **/
#define PIC_NUM_ATA_3                 (0xb) /**< Free to use IRQ line (available for PCI) **/
#define PIC_NUM_KEYBOARD_CONTROLLER_2 (0xc) /**< IRQ line of the second PS/2 port **/
#define PIC_NUM_FPU                   (0xd) /**< not a valid IRQ line! */
#define PIC_NUM_ATA_1                 (0xe) /**< IRQ line of the primary IDE controller **/
#define PIC_NUM_ATA_2                 (0xf) /**< IRQ line of the secondary IDQ controller **/

#define PIC_MASK_PIT                   (1 << PIC_NUM_PIT)
#define PIC_MASK_KEYBOARD_CONTROLLER_1 (1 << PIC_NUM_KEYBOARD_CONTROLLER_1)
#define PIC_MASK_SLAVE                 (1 << PIC_NUM_SLAVE)
#define PIC_MASK_RS232_2_4             (1 << PIC_NUM_RS232_2_4)
#define PIC_MASK_RS232_1_3             (1 << PIC_NUM_RS232_1_3)
#define PIC_MASK_LPT2                  (1 << PIC_NUM_LPT2)
#define PIC_MASK_FLOPPY                (1 << PIC_NUM_FLOPPY)
#define PIC_MASK_LPT1                  (1 << PIC_NUM_LPT1)

#define PIC_MASK_RTC                   (1 << PIC_NUM_RTC)
#define PIC_MASK_9                     (1 << PIC_NUM_9)
#define PIC_MASK_ATA_4                 (1 << PIC_NUM_ATA_4)
#define PIC_MASK_ATA_3                 (1 << PIC_NUM_ATA_3)
#define PIC_MASK_KEYBOARD_CONTROLLER_2 (1 << PIC_NUM_KEYBOARD_CONTROLLER_2)
#define PIC_MASK_FPU                   (1 << PIC_NUM_FPU)
#define PIC_MASK_ATA_1                 (1 << PIC_NUM_ATA_1)
#define PIC_MASK_ATA_2                 (1 << PIC_NUM_ATA_2)

/**
 * @brief   Callback handler if there was an interrupt on this IRQ line.
 * @param   irq_num   IRQ line in question.
 *
 * This callback is called out of the interrupt handler (inISR() == true).
 * Lengthy operations should be handled in a dedicated thread; use msg_send_int().
 * You must no enable interrupt inside the handler.
 */
typedef void (*x86_irq_handler_t)(uint8_t irq_num);

/**
 * @brief   Set callback function for an IRQ line.
 * @param   handler   The callback function, or NULL to do nothing.
 *
 * Setting a handler does not enable the interrupt.
 * Use x86_pic_enable_irq() or x86_pic_disable_irq() accordingly.
 *
 * This function must only be called with interrupts disabled.
 *
 * Beware on unsetting interrupt handlers:
 * The PIC default handler will still send an EOI.
 * Especially the keyboard controller does not like it,
 * if it is told that everything was done but it wasn't.
 * A raised #GP might be the least of your problems.
 */
void x86_pic_set_handler(unsigned irq, x86_irq_handler_t handler);

/**
 * @brief   Set the enabled IRQs
 *
 * Beware: this is the exact opposite of masking IRQs.
 *
 * This function should only be called by other subsystems like the PCI subsystem.
 */
void x86_pic_set_enabled_irqs(uint16_t mask);

/**
 * @brief   Enable (unmask) an IRQ
 *
 * This function should only be called by other subsystems like the PCI subsystem.
 */
void x86_pic_enable_irq(unsigned num);

/**
 * @brief   Disable (mask) an IRQ
 *
 * This function should only be called by other subsystems like the PCI subsystem.
 */
void x86_pic_disable_irq(unsigned num);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
