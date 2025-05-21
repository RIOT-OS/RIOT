/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup        cpu_cc2538_uart CC2538 UART
 * @ingroup         cpu_cc2538_regs
 * @{
 *
 * @file
 * @brief           CC2538 UART interface
 *
 * @author          Ian Martin <ian@locicontrols.com>
 */


#include "cc2538.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief UART component registers
 */
typedef struct {
    cc2538_reg_t DR;                      /**< UART Data Register */

    /**
     * @brief Status register
     */
    union {
        cc2538_reg_t RSR;                 /**< UART receive status and error clear */
        cc2538_reg_t ECR;                 /**< UART receive status and error clear */
    } cc2538_uart_dr;

    cc2538_reg_t RESERVED1[4];            /**< Reserved addresses */

    /**
     * @brief Flag register
     */
    union {
        cc2538_reg_t FR;                  /**< UART Flag Register */
        struct {
            cc2538_reg_t CTS        :  1; /**< Clear to send (UART1 only) */
            cc2538_reg_t RESERVED2  :  2; /**< Reserved bits */
            cc2538_reg_t BUSY       :  1; /**< UART busy */
            cc2538_reg_t RXFE       :  1; /**< UART receive FIFO empty */
            cc2538_reg_t TXFF       :  1; /**< UART transmit FIFO full */
            cc2538_reg_t RXFF       :  1; /**< UART receive FIFO full */
            cc2538_reg_t TXFE       :  1; /**< UART transmit FIFO empty */
            cc2538_reg_t RESERVED1  : 24; /**< Reserved bits */
        } FRbits;
    } cc2538_uart_fr;

    cc2538_reg_t RESERVED2;               /**< Reserved byte */
    cc2538_reg_t ILPR;                    /**< UART IrDA Low-Power Register */
    cc2538_reg_t IBRD;                    /**< UART Integer Baud-Rate Divisor */
    cc2538_reg_t FBRD;                    /**< UART Fractional Baud-Rate Divisor */

    /**
     * @brief Line control register
     */
    union {
        cc2538_reg_t LCRH;                /**< UART Line Control Register */
        struct {
            cc2538_reg_t BRK        :  1; /**< UART send break */
            cc2538_reg_t PEN        :  1; /**< UART parity enable */
            cc2538_reg_t EPS        :  1; /**< UART even parity select */
            cc2538_reg_t STP2       :  1; /**< UART two stop bits select */
            cc2538_reg_t FEN        :  1; /**< UART enable FIFOs */
            cc2538_reg_t WLEN       :  2; /**< UART word length */
            cc2538_reg_t SPS        :  1; /**< UART stick parity select */
            cc2538_reg_t RESERVED   : 24; /**< Reserved bits */
        } LCRHbits;
    } cc2538_uart_lcrh;

    /**
     * @brief Control register
     */
    union {
        cc2538_reg_t CTL;                 /**< UART Control */
        struct {
            cc2538_reg_t UARTEN     :  1; /**< UART enable */
            cc2538_reg_t SIREN      :  1; /**< UART SIR enable */
            cc2538_reg_t SIRLP      :  1; /**< UART SIR low-power mode */
            cc2538_reg_t RESERVED11 :  1; /**< Reserved bits */
            cc2538_reg_t EOT        :  1; /**< End of transmission */
            cc2538_reg_t HSE        :  1; /**< High-speed enable */
            cc2538_reg_t LIN        :  1; /**< LIN mode enable */
            cc2538_reg_t LBE        :  1; /**< UART loop back enable */
            cc2538_reg_t TXE        :  1; /**< UART transmit enable */
            cc2538_reg_t RXE        :  1; /**< UART receive enable */
            cc2538_reg_t RESERVED12 :  4; /**< Reserved bits */
            cc2538_reg_t RTSEN      :  1; /**< U1RTS Hardware flow control enable */
            cc2538_reg_t CTSEN      :  1; /**< U1CTS Hardware flow control enable */
            cc2538_reg_t RESERVED13 : 16; /**< Reserved bits */
        } CTLbits;
    } cc2538_uart_ctl;

    /**
     * @brief Interrupt FIFO level select register
     */
    union {
        cc2538_reg_t IFLS;                /**< UART interrupt FIFO Level Select */
        struct {
            cc2538_reg_t TXIFLSEL   :  3; /**< UART transmit interrupt FIFO level select */
            cc2538_reg_t RXIFLSEL   :  3; /**< UART receive interrupt FIFO level select */
            cc2538_reg_t RESERVED   : 26; /**< Reserved bits */
        } IFLSbits;
    } cc2538_uart_ifls;

    /**
     * @brief Interrupt mask register
     */
    union {
        cc2538_reg_t IM;                  /**< UART Interrupt Mask */
        struct {
            cc2538_reg_t RESERVED3  :  4; /**< Reserved bits */
            cc2538_reg_t RXIM       :  1; /**< UART receive interrupt mask */
            cc2538_reg_t TXIM       :  1; /**< UART transmit interrupt mask */
            cc2538_reg_t RTIM       :  1; /**< UART receive time-out interrupt mask */
            cc2538_reg_t FEIM       :  1; /**< UART framing error interrupt mask */
            cc2538_reg_t PEIM       :  1; /**< UART parity error interrupt mask */
            cc2538_reg_t BEIM       :  1; /**< UART break error interrupt mask */
            cc2538_reg_t OEIM       :  1; /**< UART overrun error interrupt mask */
            cc2538_reg_t RESERVED2  :  1; /**< Reserved bits */
            cc2538_reg_t NINEBITM   :  1; /**< 9-bit mode interrupt mask */
            cc2538_reg_t LMSBIM     :  1; /**< LIN mode sync break interrupt mask */
            cc2538_reg_t LME1IM     :  1; /**< LIN mode edge 1 interrupt mask */
            cc2538_reg_t LME5IM     :  1; /**< LIN mode edge 5 interrupt mask */
            cc2538_reg_t RESERVED1  : 16; /**< Reserved bits */
        } IMbits;
    } cc2538_uart_im;

    cc2538_reg_t RIS;                     /**< UART Raw Interrupt Status */

    /**
     * @brief Masked interrupt status register
     */
    union {
        cc2538_reg_t MIS;                 /**< UART Masked Interrupt Status */
        struct {
            cc2538_reg_t RESERVED8  :  4; /**< Reserved bits */
            cc2538_reg_t RXMIS      :  1; /**< UART receive masked interrupt status */
            cc2538_reg_t TXMIS      :  1; /**< UART transmit masked interrupt status */
            cc2538_reg_t RTMIS      :  1; /**< UART receive time-out masked interrupt status */
            cc2538_reg_t FEMIS      :  1; /**< UART framing error masked interrupt status */
            cc2538_reg_t PEMIS      :  1; /**< UART parity error masked interrupt status */
            cc2538_reg_t BEMIS      :  1; /**< UART break error masked interrupt status */
            cc2538_reg_t OEMIS      :  1; /**< UART overrun error masked interrupt status */
            cc2538_reg_t RESERVED9  :  1; /**< Reserved bits */
            cc2538_reg_t NINEBITMIS :  1; /**< 9-bit mode masked interrupt status */
            cc2538_reg_t LMSBMIS    :  1; /**< LIN mode sync break masked interrupt status */
            cc2538_reg_t LME1MIS    :  1; /**< LIN mode edge 1 masked interrupt status */
            cc2538_reg_t LME5MIS    :  1; /**< LIN mode edge 5 masked interrupt status */
            cc2538_reg_t RESERVED10 : 16; /**< Reserved bits */
        } MISbits;
    } cc2538_uart_mis;

    cc2538_reg_t ICR;                     /**< UART Interrupt Clear Register */
    cc2538_reg_t DMACTL;                  /**< UART DMA Control */
    cc2538_reg_t RESERVED3[17];           /**< Reserved addresses */
    cc2538_reg_t LCTL;                    /**< UART LIN Control */
    cc2538_reg_t LSS;                     /**< UART LIN Snap Shot */
    cc2538_reg_t LTIM;                    /**< UART LIN Timer */
    cc2538_reg_t RESERVED4[2];            /**< Reserved addresses */
    cc2538_reg_t NINEBITADDR;             /**< UART 9-bit self Address */
    cc2538_reg_t NINEBITAMASK;            /**< UART 9-bit self Address Mask */
    cc2538_reg_t RESERVED5[965];          /**< Reserved addresses */
    cc2538_reg_t PP;                      /**< UART Peripheral Properties */
    cc2538_reg_t RESERVED6;               /**< Reserved addresses */
    cc2538_reg_t CC;                      /**< UART Clock Configuration */
    cc2538_reg_t RESERVED7[13];           /**< Reserved addresses */
} cc2538_uart_t;

#define UART0_BASEADDR (cc2538_uart_t *)(&UART0_DR)      /**< UART0 Instance */
#define UART1_BASEADDR (cc2538_uart_t *)(&UART1_DR)      /**< UART1 Instance */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
