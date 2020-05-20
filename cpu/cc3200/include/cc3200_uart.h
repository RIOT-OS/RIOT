/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_cc3200_uart CC3200 UART
 * @ingroup         cpu_cc3200_regs
 * @{
 *
 * @file
 * @brief           CC3200 UART interface
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 */

#ifndef CC3200_UART_H
#define CC3200_UART_H

#include "cc3200.h"
#include "vendor/hw_memmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief UART Interrupt Masks
 * @{
 */
#define UART_INT_DMATX 0x20000 /**< DMA Tx Done interrupt Mask */
#define UART_INT_DMARX 0x10000 /**< DMA Rx Done interrupt Mask */
#define UART_INT_EOT 0x800     /**< End of transfer interrupt Mask */
#define UART_INT_OE 0x400      /**< Overrun Error Interrupt Mask */
#define UART_INT_BE 0x200      /**< Break Error Interrupt Mask */
#define UART_INT_PE 0x100      /**< Parity Error Interrupt Mask */
#define UART_INT_FE 0x080      /**< Framing Error Interrupt Mask */
#define UART_INT_RT 0x040      /**< Receive Timeout Interrupt Mask */
#define UART_INT_TX 0x020      /**< Transmit Interrupt Mask */
#define UART_INT_RX 0x010      /**< Receive Interrupt Mask */
#define UART_INT_CTS 0x002     /**< CTS Modem Interrupt Mask */
/** @} */

/**
 * @brief UART Configurations
 * @{
 */
#define UART_CONFIG_WLEN_MASK 0x00000060 /**< Mask for extracting word length \
                                          */
#define UART_CONFIG_WLEN_8 0x00000060    /**< 8 bit data */
#define UART_CONFIG_WLEN_7 0x00000040    /**< 7 bit data */
#define UART_CONFIG_WLEN_6 0x00000020    /**< 6 bit data */
#define UART_CONFIG_WLEN_5 0x00000000    /**< 5 bit data */
#define UART_CONFIG_STOP_MASK 0x00000008 /**< Mask for extracting stop bits */
#define UART_CONFIG_STOP_ONE 0x00000000  /**< One stop bit */
#define UART_CONFIG_STOP_TWO 0x00000008  /**< Two stop bits */
#define UART_CONFIG_PAR_MASK 0x00000086  /**< Mask for extracting parity */
#define UART_CONFIG_PAR_NONE 0x00000000  /**< No parity */
#define UART_CONFIG_PAR_EVEN 0x00000006  /**< Even parity */
#define UART_CONFIG_PAR_ODD 0x00000002   /**< Odd parity */
#define UART_CONFIG_PAR_ONE 0x00000082   /**< Parity bit is one */
#define UART_CONFIG_PAR_ZERO 0x00000086  /**< Parity bit is zero */
/** @} */

/**
 * @brief UART control register
 * based on the cc2538_uart_t definitions (UART reg memory layout is shared
 * between cc3200 and cc2538)
 * @{
 */
typedef struct cc3200_uart_t {
    cc3200_reg_t dr; /**< UART Data Register */
    union {
        cc3200_reg_t rsr; /**< UART receive status response */
        cc3200_reg_t ecr; /**< UART error clear */
    } cc3200_uart_dr; /**< UART receive status response or error clear reg */
    cc3200_reg_t RESERVED1[4];
    union {
        cc3200_reg_t raw; /**< RAW flag register */
        struct {
            cc3200_reg_t CTS : 1;        /**< Clear to send (UART1 only) */
            cc3200_reg_t DSR : 1;        /**< Data Set Ready */
            cc3200_reg_t DCD : 1;        /**< Data Carrier Detect */
            cc3200_reg_t BUSY : 1;       /**< UART busy */
            cc3200_reg_t RXFE : 1;       /**< UART receive FIFO empty */
            cc3200_reg_t TXFF : 1;       /**< UART transmit FIFO full */
            cc3200_reg_t RXFF : 1;       /**< UART receive FIFO full */
            cc3200_reg_t TXFE : 1;       /**< UART transmit FIFO empty */
            cc3200_reg_t RI : 1;         /**< UART ring indicator */
            cc3200_reg_t RESERVED1 : 23; /**< Reserved bits */
        } bits; /**< UART configuration flags as a Bitfield  */
    } flags;    /**< UART configuration flags */

    cc3200_reg_t RESERVED2; /**< Reserved byte */
    cc3200_reg_t ILPR;      /**< UART IrDA Low-Power Register */
    cc3200_reg_t IBRD;      /**< UART Integer Baud-Rate Divisor */
    cc3200_reg_t FBRD;      /**< UART Fractional Baud-Rate Divisor */

    /**
     * @brief Line control register
     */
    union {
        cc3200_reg_t raw; /**< UART Line Control Register */
        struct {
            cc3200_reg_t BRK : 1;       /**< UART send break */
            cc3200_reg_t PEN : 1;       /**< UART parity enable */
            cc3200_reg_t EPS : 1;       /**< UART even parity select */
            cc3200_reg_t STP2 : 1;      /**< UART two stop bits select */
            cc3200_reg_t FEN : 1;       /**< UART enable FIFOs */
            cc3200_reg_t WLEN : 2;      /**< UART word length */
            cc3200_reg_t SPS : 1;       /**< UART stick parity select */
            cc3200_reg_t RESERVED : 24; /**< Reserved bits */
        } bits;
    } LCRH;

    /**
     * @brief Control register
     */
    union {
        cc3200_reg_t raw; /**< UART Control */
        struct {
            cc3200_reg_t UARTEN : 1;     /**< UART enable */
            cc3200_reg_t SIREN : 1;      /**< UART SIR enable */
            cc3200_reg_t SIRLP : 1;      /**< UART SIR low-power mode */
            cc3200_reg_t RESERVED11 : 1; /**< Reserved bits */
            cc3200_reg_t EOT : 1;        /**< End of transmission */
            cc3200_reg_t HSE : 1;        /**< High-speed enable */
            cc3200_reg_t LIN : 1;        /**< LIN mode enable */
            cc3200_reg_t LBE : 1;        /**< UART loop back enable */
            cc3200_reg_t TXE : 1;        /**< UART transmit enable */
            cc3200_reg_t RXE : 1;        /**< UART receive enable */
            cc3200_reg_t RESERVED12 : 4; /**< Reserved bits */
            cc3200_reg_t RTSEN : 1; /**< U1RTS Hardware flow control enable */
            cc3200_reg_t CTSEN : 1; /**< U1CTS Hardware flow control enable */
            cc3200_reg_t RESERVED13 : 16; /**< Reserved bits */
        } bits;
    } CTL;

    /**
     * @brief Interrupt FIFO level select register
     */
    union {
        cc3200_reg_t raw; /**< UART interrupt FIFO Level Select */
        struct {
            cc3200_reg_t TXIFLSEL : 3;  /**< UART transmit interrupt FIFO level
                                           select */
            cc3200_reg_t RXIFLSEL : 3;  /**< UART receive interrupt FIFO level
                                           select */
            cc3200_reg_t RESERVED : 26; /**< Reserved bits */
        } bits;
    } IFLS;

    /**
     * @brief Interrupt mask register
     */
    union {
        cc3200_reg_t raw; /**< UART Interrupt Mask */
        struct {
            cc3200_reg_t RESERVED3 : 4; /**< Reserved bits */
            cc3200_reg_t RXIM : 1;      /**< UART receive interrupt mask */
            cc3200_reg_t TXIM : 1;      /**< UART transmit interrupt mask */
            cc3200_reg_t RTIM : 1; /**< UART receive time-out interrupt mask */
            cc3200_reg_t FEIM : 1; /**< UART framing error interrupt mask */
            cc3200_reg_t PEIM : 1; /**< UART parity error interrupt mask */
            cc3200_reg_t BEIM : 1; /**< UART break error interrupt mask */
            cc3200_reg_t OEIM : 1; /**< UART overrun error interrupt mask */
            cc3200_reg_t RESERVED2 : 1; /**< Reserved bits */
            cc3200_reg_t NINEBITM : 1;  /**< 9-bit mode interrupt mask */
            cc3200_reg_t LMSBIM : 1; /**< LIN mode sync break interrupt mask */
            cc3200_reg_t LME1IM : 1; /**< LIN mode edge 1 interrupt mask */
            cc3200_reg_t LME5IM : 1; /**< LIN mode edge 5 interrupt mask */
            cc3200_reg_t RESERVED1 : 16; /**< Reserved bits */
        } bits;
    } IM;

    cc3200_reg_t RIS; /**< UART Raw Interrupt Status */

    /**
     * @brief Masked interrupt status register
     */
    union {
        cc3200_reg_t raw; /**< UART Masked Interrupt Status */
        struct {
            cc3200_reg_t RESERVED8 : 4; /**< Reserved bits */
            cc3200_reg_t RXMIS : 1; /**< UART receive masked interrupt status */
            cc3200_reg_t TXMIS : 1; /**< UART transmit masked interrupt status
                                     */
            cc3200_reg_t RTMIS : 1; /**< UART receive time-out masked interrupt
                                       status */
            cc3200_reg_t FEMIS : 1; /**< UART framing error masked interrupt
                                       status */
            cc3200_reg_t PEMIS : 1; /**< UART parity error masked interrupt
                                       status */
            cc3200_reg_t BEMIS : 1; /**< UART break error masked interrupt
                                       status */
            cc3200_reg_t OEMIS : 1; /**< UART overrun error masked interrupt
                                       status */
            cc3200_reg_t RESERVED9 : 1;  /**< Reserved bits */
            cc3200_reg_t NINEBITMIS : 1; /**< 9-bit mode masked interrupt status
                                          */
            cc3200_reg_t LMSBMIS : 1; /**< LIN mode sync break masked interrupt
                                         status */
            cc3200_reg_t LME1MIS : 1; /**< LIN mode edge 1 masked interrupt
                                         status */
            cc3200_reg_t LME5MIS : 1; /**< LIN mode edge 5 masked interrupt
                                         status */
            cc3200_reg_t RESERVED10 : 16; /**< Reserved bits */
        } bits;
    } MIS;

    cc3200_reg_t ICR;            /**< UART Interrupt Clear Register */
    cc3200_reg_t DMACTL;         /**< UART DMA Control */
    cc3200_reg_t RESERVED3[17];  /**< Reserved addresses */
    cc3200_reg_t LCTL;           /**< UART LIN Control */
    cc3200_reg_t LSS;            /**< UART LIN Snap Shot */
    cc3200_reg_t LTIM;           /**< UART LIN Timer */
    cc3200_reg_t RESERVED4[2];   /**< Reserved addresses */
    cc3200_reg_t NINEBITADDR;    /**< UART 9-bit self Address */
    cc3200_reg_t NINEBITAMASK;   /**< UART 9-bit self Address Mask */
    cc3200_reg_t RESERVED5[965]; /**< Reserved addresses */
    cc3200_reg_t PP;             /**< UART Peripheral Properties */
    cc3200_reg_t RESERVED6;      /**< Reserved addresses */
    cc3200_reg_t CC;             /**< UART Clock Configuration */
    cc3200_reg_t RESERVED7[13];  /**< Reserved addresses */
} cc3200_uart_t;
/** @} */

#define UART0 (cc3200_uart_t *)(UARTA0_BASE) /**< UART0 Instance */
#define UART1 (cc3200_uart_t *)(UARTA1_BASE) /**< UART1 Instance */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC3200_UART_H */
       /** @} */