#ifndef _S5P4418_UART_H_
#define _S5P4418_UART_H_

//------------------------------------------------------------------------------
//  includes
//------------------------------------------------------------------------------
#include <stdint.h>
#include "types.h"
#include "s5p4418_chip.h"

#ifdef	__cplusplus
extern "C"
{
#endif

#define S5P4418_UART_FIFO_DEPTH	32

#define UTRSR_MASK          0x03
#define UTIIR_MASK          0x1f
#define UTFR_MASK           0xff
#define UTDMACR_MASK        0x3f

#define UARTDR_MASK         0xff
#define UARTECR_MASK        0xff
#define UARTRSR_MASK        0x0f
#define UARTLCR_H_MASK      0x7f
#define UARTLCR_M_MASK      0xff
#define UARTLCR_L_MASK      0xff
#define UARTCR_MASK         0x7f
#define UARTCR_newMASK      0xffff
#define UARTFR_MASK         0xf8
#define UARTILPR_MASK       0xff
#define UARTIBRD_MASK       0xffff
#define UARTFBRD_MASK       0x3f
#define UARTLCR_H_new_MASK  0xff
#define UARTIFLS_MASK       0x3f
#define UARTIMSC_MASK       0x7ff
#define UARTRIS_MASK        0xfff
#define UARTMIS_MASK        0xfff
#define UARTICR_MASK        0x7ff
#define UARTDMACR_MASK      0x07
#define MASK_IDREG          0x0ff

#define UARTTCR_MASK        0x07
#define UARTITIP_MASK       0xff
#define UARTITOP_MASK       0xffff
#define UARTTDR_MASK        0xfff

/* For RSTMODE_REG Register */
#define RSTMODE_ENABLE      0x01
#define PCLK_ENABLE         0x02
#define PCLKGEN             0x03
#define REFCLKGEN           0x04
#define REFCLKON            0x05
#define PCLKON              0x06

/* For UTIIR Register */
#define UT_MSINT            0x01
#define UT_RXINT            0x02
#define UT_TXINT            0x04
#define UT_RTISINT          0x08
#define UT_UARTINTR         0x10
#define UT_UARTEINTR        0x20

/* For UT_FORCED_ERROR Register */
#define FORCED_PARITY_ERR   0x01
#define FORCED_FRAMING_ERR  0x02
#define RX_JITTER_BITS      0x0c
#define RX_JITTER_SIGN      0x10
#define TX_JITTER_BITS      0x60
#define TX_JITTER_SIGN      0x80

/* For UTFR Register */
#define UTRXFIFO_EMPTY      0x01
#define UTRXFIFO_HALFFULL   0x02
#define UTRXFIFO_FULL       0x04
#define UTTXFIFO_EMPTY      0x08
#define UTTXFIFO_HALFEMPTY  0x10
#define UTTXFIFO_FULL       0x20
#define UTTXBUSY            0x40
#define UTRXBUSY            0x80

/* For UTRSR Register */
#define UTPE                0x02

/* For UTDMACR Register */
#define TXDMASREQ           0x04
#define RXDMASREQ           0x08
#define TXDMABREQ           0x10
#define RXDMABREQ           0x20

/* For UARTFR Register */
#define UART_CTS            0x01
#define UART_DSR            0x02
#define UART_DCD            0x04
#define UART_UBUSY          0x08
#define UART_RXFE           0x10
#define UART_TXFF           0x20
#define UART_RXFF           0x40
#define UART_TXFE           0x80
#define UART_RI             0x100

/* For UARTIFLS Register */
#define UART_RXFEIGHT       0x00
#define UART_RXFQUART       0x08
#define UART_RXFHALF        0x10
#define UART_RXF3QUART      0x18
#define UART_RXF7EIGHT      0x20
#define UART_TXFEIGHT       0x00
#define UART_TXFQUART       0x01
#define UART_TXFHALF        0x02
#define UART_TXF3QUART      0x03
#define UART_TXF7EIGHT      0x04

/* UARTRIS Register */
#define UART_RIRINT         0x01
#define UART_CTSRINT        0x02
#define UART_DCDRINT        0x04
#define UART_DSRRINT        0x08
#define UART_RXRINT         0x10
#define UART_TXRINT         0x20
#define UART_RTRINT         0x40
#define UART_FERINT         0x80
#define UART_PERINT         0x100
#define UART_BERINT         0x200
#define UART_OERINT         0x400

/* UARTMIS Register */
#define UART_RIMINT         0x01
#define UART_CTSMINT        0x02
#define UART_DCDMINT        0x04
#define UART_DSRMINT        0x08
#define UART_RXMINT         0x10
#define UART_TXMINT         0x20
#define UART_RTMINT         0x40
#define UART_FEMINT         0x80
#define UART_PEMINT         0x100
#define UART_BEMINT         0x200
#define UART_OEMINT         0x400

/* UARTDMACR Register */
#define UART_RXDMAE         0x01
#define UART_TXDMAE         0x02
#define DMAONERR            0x04

//------------------------------------------------------------------------------
/// @brief  UART register set structure
//------------------------------------------------------------------------------
typedef struct
{
    volatile uint32_t DR;               // 0x00    // Data Register
    volatile uint32_t RSR_ECR;          // 0x04    // Receive Status Register / Error Clear Register
    volatile uint32_t __Reserved0[(0x18-0x08)/4];  // 0x08~0x14 Reserved
    volatile uint32_t FR;               // 0x18    // Flag Register
    volatile uint32_t __Reserved1;      // 0x1C    // Reserved
    volatile uint32_t ILPR;             // 0x20    // IrDA Low Power Counter Register
    volatile uint32_t IBRD;             // 0x24    // IntegerBaud Rate Register
    volatile uint32_t FBRD;             // 0x28    // Fractional Baud Rate Register
    volatile uint32_t LCR_H;            // 0x2C    // Line Control Register
    volatile uint32_t CR;               // 0x30    // Control Register
    volatile uint32_t IFLS;             // 0x34    // Interrupt FIFO Level Select Register
    volatile uint32_t IMSC;             // 0x38    // Interrupt Mask Set/Clear Register
    volatile uint32_t RIS;              // 0x3C    // Raw Interrupt Status Register
    volatile uint32_t MIS;              // 0x40    // Masked Interrupt Status Register
    volatile uint32_t ICR;              // 0x44    // Interrupt Clear Register
    volatile uint32_t DMACR;            // 0x48    // DMA Control Register
    volatile uint32_t __Reserved2[(0x80-0x4C)/4];  // 0x4C~0x7C Reserved
    volatile uint32_t TCR;              // 0x80    // Test Control Register
    volatile uint32_t ITIP;             // 0x84    // Integration Test Input Register
    volatile uint32_t ITOP;             // 0x88    // Integration Test Output Register
    volatile uint32_t TDR;              // 0x8C    // Test Data Register
    volatile uint32_t __Reserved4[(0xFD0-0x90)/4]; // 0x90~0xFCC  Reserved
    volatile uint32_t __Reserved5[(0xFE0-0xFD0)/4];// 0xFD0~0xFDC Reserved
    volatile uint32_t PeriphID0;        // 0xFE0   // PeriphID0 Register
    volatile uint32_t PeriphID1;        // 0xFE4   // PeriphID1 Register
    volatile uint32_t PeriphID2;        // 0xFE8   // PeriphID2 Register
    volatile uint32_t PeriphID3;        // 0xFEC   // PeriphID3 Register
    volatile uint32_t PCellID0;         // 0xFF0   // PCellID0 Register
    volatile uint32_t PCellID1;         // 0xFF4   // PCellID1 Register
    volatile uint32_t PCellID2;         // 0xFF8   // PCellID2 Register
    volatile uint32_t PCellID3;         // 0xFFC   // PCellID3 Register
}uart_typedef;


/// @brief	UART interrupt for interrupt interface
enum
{
    S5P4418_UART_INT_RIM    = 0UL,  // RI Modem Interrupt
    S5P4418_UART_INT_CTSM   = 1UL,  // CTS Modem Interrupt
    S5P4418_UART_INT_DCDM   = 2UL,  // DCD Modem Interrupt
    S5P4418_UART_INT_DSRM   = 3UL,  // DSR Modem Interrupt
    S5P4418_UART_INT_RX     = 4UL,  // Receive Interrupt
    S5P4418_UART_INT_TX     = 5UL,  // Transmit Interrupt
    S5P4418_UART_INT_RT     = 6UL,  // Receive Timeout Interrupt
    S5P4418_UART_INT_FE     = 7UL,  // Framing Error Interrupt
    S5P4418_UART_INT_PE     = 8UL,  // Parity Error Interrupt
    S5P4418_UART_INT_BE     = 9UL,  // Break Error Interrupt
    S5P4418_UART_INT_OE     = 10UL  // Overrun Error Interrupt
};

enum
{
    S5P4418_UART_CR_UARTEN  = 0UL,
    S5P4418_UART_CR_SIREN   = 1UL,
    S5P4418_UART_CR_IIRLP   = 2UL,
    S5P4418_UART_CR_LPE     = 7UL,
    S5P4418_UART_CR_TXE     = 8UL,
    S5P4418_UART_CR_RXE     = 9UL,
    S5P4418_UART_CR_DTR     = 10UL,
    S5P4418_UART_CR_RTS     = 11UL,
    S5P4418_UART_CR_OUT1    = 12UL,
    S5P4418_UART_CR_OUT2    = 13UL,
    S5P4418_UART_CR_RTSEN   = 14UL,
    S5P4418_UART_CR_CTSEN   = 15UL
};

/// @brief Parity generation/check type
typedef enum
{
    S5P4418_UART_PARITYMODE_NONE = 0UL, // No Parity
    S5P4418_UART_PARITYMODE_ODD  = 1UL, // Odd Parity
    S5P4418_UART_PARITYMODE_EVEN = 2UL, // Even Parity
    S5P4418_UART_PARITYMODE_FONE = 3UL, // Force 1 Parity
    S5P4418_UART_PARITYMODE_FZ   = 4UL  // Force 0 Parity
}S5P4418_UART_PARITYMODE;

typedef enum
{
    S5P4418_UART_ERRSTAT_FRAME   = 1UL<<0,  // Frame Error
    S5P4418_UART_ERRSTAT_PARITY  = 1UL<<1,  // Parity Error
    S5P4418_UART_ERRSTAT_BREAK   = 1UL<<2,  // Break Signal receive
    S5P4418_UART_ERRSTAT_OVERRUN = 1UL<<3   // Overrun Error

}S5P4418_UART_ERRSTAT ;

typedef enum
{
    S5P4418_UART_FLAG_CTS   = 1UL<<0,   // Clear to send
    S5P4418_UART_FLAG_DSR   = 1UL<<1,   // Data set ready
    S5P4418_UART_FLAG_DCD   = 1UL<<2,   // Data carrier detect
    S5P4418_UART_FLAG_BUSY  = 1UL<<3,   // UART Busy
    S5P4418_UART_FLAG_RXFE  = 1UL<<4,   // Receive FIFO empty
    S5P4418_UART_FLAG_TXFF  = 1UL<<5,   // Transmit FIFO full
    S5P4418_UART_FLAG_RXFF  = 1UL<<6,   // Receive FIFO full
    S5P4418_UART_FLAG_TXFE  = 1UL<<7,   // Transmit FIFO empty
    S5P4418_UART_FLAG_RI    = 1UL<<8    // Ring indicator

}S5P4418_UART_FLAG;

typedef enum
{
    S5P4418_UART_RXDMAE     = 1UL<<0,   // Clear to send
    S5P4418_UART_TXDMAE     = 1UL<<1,   // Clear to send
    S5P4418_UART_DMAONERR   = 1UL<<2,   // Clear to send

}S5P4418_UART_DMA;

typedef enum
{
    S5P4418_UART_FIFOLEVEL1_8   = (S5P4418_UART_FIFO_DEPTH*1/8), // 1/8n
    S5P4418_UART_FIFOLEVEL2_8   = (S5P4418_UART_FIFO_DEPTH*2/8), // 1/4n
    S5P4418_UART_FIFOLEVEL4_8   = (S5P4418_UART_FIFO_DEPTH*4/8), // 1/2n
    S5P4418_UART_FIFOLEVEL6_8   = (S5P4418_UART_FIFO_DEPTH*6/8), // 3/4n
    S5P4418_UART_FIFOLEVEL7_8   = (S5P4418_UART_FIFO_DEPTH*7/8), // 7/8n
    S5P4418_UART_FIFOLEVEL_ERR  = 0xFFFFFFFFUL
}S5P4418_UART_FIFOLEVEL;

typedef enum
{
    S5P4418_UART_DATABIT_5   = 0UL, // 5 bits
    S5P4418_UART_DATABIT_6   = 1UL, // 6 bits
    S5P4418_UART_DATABIT_7   = 2UL, // 7 bits
    S5P4418_UART_DATABIT_8   = 4UL, // 8 bits
    S5P4418_UART_DATABIT_ERR = 0xFFFFFFFFUL
}S5P4418_UART_DATABIT;

/* UART channel description */
#define UART0  ((uart_typedef *) S5P4418_UART0_BASE)
#define UART1  ((uart_typedef *) S5P4418_UART1_BASE)
#define UART2  ((uart_typedef *) S5P4418_UART2_BASE)
#define UART3  ((uart_typedef *) S5P4418_UART3_BASE)

#ifdef	__cplusplus
}
#endif

#endif // _S5P4418_UART_H_
