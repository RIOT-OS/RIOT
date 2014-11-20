/*
 * maca.h - The MACA driver for the Redbee EconoTag
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MACA_H_
#define MACA_H_
#include <stdint.h>

#include "radio/types.h"

#include "maca_packet.h"

/*********************************************************/
/* function definitions                                  */
/*********************************************************/

/* functions for the initialization and turning on and off of the MACA */
void maca_init ( void );
void maca_on ( void );
void maca_off ( void );

/* function to check if MACA is running right */
void maca_check ( void );

/* functions to configure MACA */
void maca_set_power ( uint8_t power );
void maca_set_channel ( uint8_t channel );
radio_address_t maca_set_address (radio_address_t addr );
radio_address_t maca_get_address ( void );
uint16_t maca_set_pan(uint16_t pan);
uint16_t maca_get_pan(void);

/* get Link Quality Indicator */
extern uint8_t (*get_lqi) ( void );


/* interface to configure demodulator */
#define MACA_DEMOD_DCD 1 /* -96dBm, 22.2mA */
#define MACA_DEMOD_NCD 0 /* -100dBm, 24.2mA */
void set_demodulator_type ( uint8_t type );

/* macro for setting checksum filtering */
extern volatile uint8_t fcs_mode;
#define MACA_SET_FCS_MODE(x) fcs_mode = (x)

/* MACA_SET_PRM_MODE(PROMISC) to disable address filtering */
/* MACA_SET_PRM_MODE(AUTOACK) to enable address filtering AND autoack */
extern volatile uint8_t prm_mode;
#define MACA_SET_PRM_MODE(x) prm_mode = (x)

/* functions working on maca_packet_t */
void maca_set_tx_packet ( volatile maca_packet_t *packet );
volatile maca_packet_t* maca_get_rx_packet ( void );
volatile maca_packet_t* maca_get_free_packet ( void );
void maca_free_packet ( volatile maca_packet_t *packet );
void maca_free_all_packets ( void );

extern volatile maca_packet_t *maca_rx_head, *maca_tx_head;
extern volatile uint32_t maca_entry;

extern void maca_rx_callback ( volatile maca_packet_t *packet ) __attribute__((weak));
extern void maca_tx_callback ( volatile maca_packet_t *packet ) __attribute__((weak));

/* internal lowlevel MACA functions */
void _maca_reset ( void );
void _maca_init_phy ( void );
void _maca_flyback_init ( void );
void _maca_resume_maca_sync ( void );
void _maca_radio_init ( void );
uint32_t _maca_init_from_flash ( uint32_t address );

#define MACA_MAX_PACKET_SIZE (MACA_MAX_PAYLOAD_SIZE + 2) /* +2 for checksum */

/*********************************************************/
/* register definitions                                  */
/*********************************************************/

#define MACA_BASE_ADDRESS       (0x80004000)

struct MACA_struct {
    union {
        // Reset the MACA Module
        uint32_t RESET;
        struct MACA_RESET {
            uint32_t RST:1;             // MACA Reset
            uint32_t CLK_ON:1;          // MACA Clk on/off
            uint32_t :30;
        } RESETbits;
    };
    union {
        // Write: random generator seed
        // Read: get random data
        uint32_t RANDOM;
        struct MACA_RANDOM {
            uint32_t :32;
        } RANDOMbits;
    };
    union {
        // Control register of the MACA
        uint32_t CONTROL;
        struct MACA_CONTROL {
            uint32_t SEQUENCE :3;       // set new mode of operation (see MACA_CONTROL_SEQUENCE_*)
            uint32_t MODE :2;           // transmission access mode (see MACA_CONTROL_MODE_*)
            uint32_t TM :1;             // test mode (see MACA_CONTROL_TM_*)
            uint32_t :1;                // LFSR reserved
            uint32_t AUTO :1;           // restart Rx automatically (see MACA_CONTROL_AUTO_*)
            uint32_t BCN :1;            // filter beacon only (see MACA_CONTROL_BCN_*)
            uint32_t ASAP :1;           // start action sequence ASAP (see MACA_CONTROL_ASAP_*)
            uint32_t REL :1;            // clk selector (see MACA_CONTROL_REL_*)
            uint32_t PRM :1;            // promiscuous mode (see MACA_CONTROL_PRM_*)
            uint32_t NOFC :1;           // no frame check (see MACA_CONTROL_NOFC_*)
            uint32_t ROLE :1;           // current role
            uint32_t :1;                // reserved
            uint32_t RSTO :1;           // reset slot offset (see MACA_CONTROL_RSTO_*)
            uint32_t PRE_COUNT :4;      // preamble repeat counter (btween 0-15)
            uint32_t ISM :1;            // ISM reserved
            uint32_t :11;
        } CONTROLbits;
    };
    union {
        uint32_t STATUS;
        struct
        {
            uint32_t COMPLETE_CODE :4;  // complete code (see MACA_STATUS_COMPLETECODE_*)
            uint32_t :8;                // reserved
            uint32_t OVR :1;            // Rx buffer overrun (see MACA_STATUS_OVR_*)
            uint32_t BUSY :1;           // channel busy detection (see MACA_STATUS_BUSY_*)
            uint32_t CRC :1;            // checksum failed  (see MACA_STATUS_CRC_*)
            uint32_t TO :1;             // time-out (see MACA_STATUS_TO_*)
            uint32_t :16;               // reserved
        } STATUSbits;
    };
    union {
        uint32_t FRMPND;
        struct
        {
            uint32_t PND :1;            // Ack Frame Pending Status (see MACA_FRMPND_PND_*)
            uint32_t :31;               // reserved
        } FRMPNbits;
    };
    union {
        uint32_t MC1322x_ID;
        struct
        {
            uint32_t MC1322x_ID :8;     // the 9bit MC1322x ID (see MACA_MC1322x_ID_*)
            uint32_t :24;               // reserved
        } MC1322x_IDbits;
    };
    union {
        uint32_t TMREN;
        struct
        {
            uint32_t STRT :1;           // enable start clk circuit (see MACA_TMREN_STRT_*)
            uint32_t CPL :1;            // enable complete clk circuit (see MACA_TMREN_CPL_*)
            uint32_t SFT :1;            // enable soft complete clk circ (see MACA_TMREN_SFT_*)
            uint32_t :29;               // reserved
        } TMRENbits;
    };
    union {
        uint32_t TMRDIS;
        struct
        {
            uint32_t STRT :1;           // disable start clk circuit (see MACA_TMRDIS_STRT_*)
            uint32_t CPL :1;            // disable complete clk circuit (see MACA_TMRDIS_CPL_*)
            uint32_t SFT :1;            // disable soft complete clk circ (see MACA_TMRDIS_SFT_*)
            uint32_t STRT_OFF :1;       // abort start clk (see MACA_TMRDIS_STRTOFF_*)
            uint32_t CPL_OFF :1;        // abort complete clk (see MACA_TMRDIS_CPLOFF_*)
            uint32_t SFT_OFF :1;        // abort soft complete clk (see MACA_TMRDIS_SFT_OFF_*)
            uint32_t :26;               // reserved
        } TMRDISbits;
    };
    uint32_t CLK;                   // sets a new absolute clock value
    // Ensure that the timers are not active
    uint32_t STARTCLK;              // see (9.7.10)
    uint32_t CPLCLK;                // see (9.7.11)
    uint32_t SFTCLK;                // see (9.7.12)
    uint32_t CLKOFFSET;             // see (9.7.13)
    uint32_t RELCLK;                // see (9.7.14)
    uint32_t CPLTIM;                // see (9.7.15)
    union {
        uint32_t SLOTOFFSET;        // see (9.7.16)
        struct
        {
            uint32_t TX_SLOT_OFFSET :12;
            uint32_t :4;
            uint32_t RX_SLOT_OFFSEt :12;
            uint32_t :4;
        } SLOTOFFSETbits;
    };
    uint32_t TIMESTAMP;             // see (9.7.17)
    uint32_t DMARX;                 // see (9.7.18)
    uint32_t DMATX;                 // see (9.7.19)
    uint32_t DMAPOLL;               // see (9.7.20)
    union {
        uint32_t TXLEN;             // see (9.7.21)
        struct
        {
            uint32_t TX_LEN :15;    // Tx payload length
            uint32_t :17;           // reserved
        } TXLENbits;
    };
    union {
        uint32_t TXSEQNR;           // see (9.7.22)
        struct
        {
            uint32_t TXSEQN :8;     // Tx sequ. number
            uint32_t :24;           // reserved
        } TXSEQNRbits;
    };
    union {
        uint32_t SETRXLVL;          // see (9.7.23)
        struct
        {
            uint32_t FIFO_LVL :16;  // FIFO level
            uint32_t :16;           // reserved
        } SETRXLVLbits;
    };
    union {
        uint32_t GETRXLVL;          // see (9.7.24)
        struct
        {
            uint32_t RECVBYTES :16; // received bytes
            uint32_t :16;           // reserved
        } GETRXLVLbits;
    };
    union {
        uint32_t IRQ;               // read only see (9.7.25)
        struct
        {
            uint32_t ACPL :1;       // Action Complete Interrupt
            uint32_t POLL :1;       // Poll Indication Interrupt
            uint32_t DI :1;         // Data Indication Interrupt
            uint32_t :6;            // reserved
            uint32_t LVL :1;        // FIFO Level Interrupt
            uint32_t SFT :1;        // Soft Complete Clk Interrupt
            uint32_t FLT :1;        // Filter Failed Interrupt
            uint32_t CRC :1;        // Checksum Failed Interrupt
            uint32_t CM :1;         // Complete Clock Interrupt
            uint32_t SYNC :1;       // Sync Detected Interrupt
            uint32_t STRT :1;       // Action Started Interrupt
            uint32_t :16;           // reserved
        } IRQbits;
    };
    union {
        uint32_t CLRIRQ;            // write only see (9.7.26)
        // write 1 to clear
        struct
        {
            uint32_t ACPL :1;       // Clear Action Complete Interrupt
            uint32_t POLL :1;       // Clear Poll Indication Interrupt
            uint32_t DI :1;         // Clear Data Indication Interrupt
            uint32_t :6;            // reserved
            uint32_t LVL :1;        // Clear FIFO Level Interrupt
            uint32_t SFT :1;        // Clear Soft Complete Clk Interrupt
            uint32_t FLT :1;        // Clear Filter Failed Interrupt
            uint32_t CRC :1;        // Clear Checksum Failed Interrupt
            uint32_t CM :1;         // Clear Complete Clock Interrupt
            uint32_t SYNC :1;       // Clear Sync Detected Interrupt
            uint32_t STRT :1;       // Clear Action Started Interrupt
            uint32_t :16;           // reserved
        } CLRIRQbits;
    };
    union {
        uint32_t SETIRQ;            // write only see (9.7.27)
        // write 1 to set IRQ
        struct
        {
            uint32_t ACPL :1;       // Trigger Action Complete Interrupt
            uint32_t POLL :1;       // Trigger Poll Indication Interrupt
            uint32_t DI :1;         // Trigger Data Indication Interrupt
            uint32_t :6;            // reserved
            uint32_t LVL :1;        // Trigger FIFO Level Interrupt
            uint32_t SFT :1;        // Trigger Soft Complete Clk Interrupt
            uint32_t FLT :1;        // Trigger Filter Failed Interrupt
            uint32_t CRC :1;        // Trigger Checksum Failed Interrupt
            uint32_t CM :1;         // Trigger Complete Clock Interrupt
            uint32_t SYNC :1;       // Trigger Sync Detected Interrupt
            uint32_t STRT :1;       // Trigger Action Started Interrupt
            uint32_t :16;           // reserved
        } SETIRQbits;
    };
    union {
        uint32_t MASKIRQ;           // write only see (9.7.28)
        // write 1 to enable IRQ
        struct
        {
            uint32_t ACPL :1;       // Enable Action Complete Interrupt
            uint32_t POLL :1;       // Enable Poll Indication Interrupt
            uint32_t DI :1;         // Enable Data Indication Interrupt
            uint32_t WU :1;         // reserved but needed??
            uint32_t RST :1;        // reserved but needed??
            uint32_t :4;            // reserved
            uint32_t LVL :1;        // Enable FIFO Level Interrupt
            uint32_t SFT :1;        // Enable Soft Complete Clk Interrupt
            uint32_t FLT :1;        // Enable Filter Failed Interrupt
            uint32_t CRC :1;        // Enable Checksum Failed Interrupt
            uint32_t CM :1;         // Enable Complete Clock Interrupt
            uint32_t SYNC :1;       // Enable Sync Detected Interrupt
            uint32_t STRT :1;       // Enable Action Started Interrupt
            uint32_t :16;           // reserved
        } MASKIRQbits;
    };
    union {
        uint32_t MACPANID;          // see (9.7.29)
        struct
        {
            uint32_t PANID :16;     // MAC PAN ID for the 802.15.4 std network
        } MACPANIDbits;
    };
    union {
        uint32_t MAC16ADDR;         // see (9.7.30)
        struct
        {
            uint32_t ADDR :16;     // reflects the MAC short address
        } MAC16ADDRbits;
    };
    uint32_t MAC64HI;               // see (9.7.31)
    uint32_t MAC64LO;               // see (9.7.32)
    union {
        uint32_t FLTREJ;            // see (9.7.33)
        struct
        {
            uint32_t BCN :1;        // Reject Beacon packets
            uint32_t DATA :1;       // Reject Data packets
            uint32_t ACK :1;        // Reject ACK packets
            uint32_t CMD :1;        // Reject MAC CMD packets
            uint32_t :4;            // reserved
            uint32_t POLL :1;       // Accept POLL packets
            uint32_t :7;            // reserved
            uint32_t FC_MASK :16;   // Frame Control Mask
        } FLTREJbits;
    };
    union {
        uint32_t CLKDIV;            // see (9.7.34)
        struct
        {
            uint32_t Divider :16;   // prescale value for the MACA transmit clk
            uint32_t :16;           // reserved
        } CLKDIVbits;
    };
    union {
        uint32_t WARMUP;            // see (9.7.35)
        struct
        {
            uint32_t Rx_WARMUP :12; // receiver warmup time
            uint32_t :4;            // reserved
            uint32_t Tx_WARMUP :12; // transmitter warmuptime
            uint32_t :4;            // reserved
        } WARMUPbits;
    };
    uint32_t PREAMBLE;              // see (9.7.36)
    uint32_t FRAMESYNC0;            // see (9.7.37)
    uint32_t FRAMESYNC1;            // see (9.7.38)
    union {
        uint32_t TXACKDELAY;        // see (9.7.39)
        struct
        {
            uint32_t TXACKDELAY :12;    // Tx Acknowledgement Delay
            uint32_t :4;                // reserved
            uint32_t TXPOLLDELAY :12;   // Tx Poll Delay
            uint32_t :4;                // reserved
        } TXACKDELAYbits;
    };
    union {
        uint32_t RXACKDELAY;        // see (9.7.40)
        struct
        {
            uint32_t RXACKDELAY :12;    // Rx Acknowledgement Delay
            uint32_t :4;                // reserved
            uint32_t RXAUTODELAY :12;   // time to disable receiver before restarting
            uint32_t :4;                // reserved
        } RXACKDELAYbits;
    };
    union {
        uint32_t EOFDELAY;        // see (9.7.41)
        struct
        {
            uint32_t EOFDELAY :12;      // End Of Frame Delay
            uint32_t :20;               // reserved
        } EOFDELAYbits;
    };
    union {
        uint32_t CCADELAY;        // see (9.7.42)
        struct
        {
            uint32_t CCADELAY :12;      // CCA Delay
            uint32_t :4;                // reserved
            uint32_t CCALENGTH :12;     // Length of time to perform CCA
            uint32_t :4;                // reserved
        } CCADELAYbits;
    };
    union {
        uint32_t RXEND;         // see (9.7.43)
        struct
        {
            uint32_t RXACK_END :12;     // Rx Acknowledgement Window End in Normal Mode
            uint32_t :4;                // reserved
            uint32_t RXSLOTTED_END :12; // Rx Acknowledgement Window End in slotted CSMA-CA mode
            uint32_t :4;                // reserved
        } RXENDbits;
    };
    union {
        uint32_t TXCCADELAY;    // see (9.7.44)
        struct
        {
            uint32_t TXCCADELAY :12;    // Delay from end of CCA to Tx start
            uint32_t :20;               // reserved
        } TXCCADELAYbits;
    };
    uint32_t KEY3;              // see (9.7.45)
    uint32_t KEY2;              // see (9.7.45)
    uint32_t KEY1;              // see (9.7.45)
    uint32_t KEY0;              // see (9.7.45)
    union {
        uint32_t OPTIONS;       // see (9.7.46)
        struct
        {
            uint32_t POLL :1;       // enables writing to DMAPOLL
            uint32_t PLL_TX :1;     // responde to PLL unlock only while transmition
            uint32_t PLL_IGNORE :1; // ignore PLL unlock signal
            uint32_t SEED_KEY :1;   // generation can be seeded
            uint32_t :28;           // reserved
        } OPTIONSbits;
    };

};

static volatile struct MACA_struct * const MACA = (void *) (MACA_BASE_ADDRESS + 0x04); // +4 because base + 0x00 is reserved

#define MACA_CONTROL_SEQUENCE_NOP 0
#define MACA_CONTROL_SEQUENCE_ABORT 1
#define MACA_CONTROL_SEQUENCE_WAIT 2
#define MACA_CONTROL_SEQUENCE_TX 3
#define MACA_CONTROL_SEQUENCE_RX 4
#define MACA_CONTROL_SEQUENCE_TXPOLL 5
#define MACA_CONTROL_SEQUENCE_CCA 6
#define MACA_CONTROL_SEQUENCE_ED 7

#define MACA_CONTROL_MODE_NOCCA 0
#define MACA_CONTROL_MODE_NONSLOTTED_CSMACA 1
#define MACA_CONTROL_MODE_SLOTTED_CSMACA 2
#define MACA_CONTROL_MODE_RESERVED 3

#define MACA_CONTROL_TM_NORMAL 0
#define MACA_CONTROL_TM_TM 1

#define MACA_CONTROL_AUTO_OFF 0
#define MACA_CONTROL_AUTO_ON 1

#define MACA_CONTROL_BCN_OFF 0
#define MACA_CONTROL_BCN_ON 1

#define MACA_CONTROL_ASAP_OFF 0
#define MACA_CONTROL_ASAP_ON 1

#define MACA_CONTROL_REL_ABSOLUTE 0
#define MACA_CONTROL_REL_RELATIVE 1

#define MACA_CONTROL_PRM_OFF 0
#define MACA_CONTROL_PRM_ON 1

#define MACA_CONTROL_NOFC_OFF 0
#define MACA_CONTROL_USE_FCS 0
#define MACA_CONTROL_NOFC_ON 1
#define MACA_CONTROL_NO_FCS 1

#define MACA_CONTROL_ROLE_NOPAN 0
#define MACA_CONTROL_ROLE_PAN 1

#define MACA_CONTROL_RSTO_DONOT_RST 0
#define MACA_CONTROL_RSTO_DO_RST 1

#define MACA_STATUS_COMPLETECODE_SUCCESS 0
#define MACA_STATUS_COMPLETECODE_TIMEOUT 1
#define MACA_STATUS_COMPLETECODE_CHANBUSY 2
#define MACA_STATUS_COMPLETECODE_CRC_FAIL 3
#define MACA_STATUS_COMPLETECODE_ABORTED 4
#define MACA_STATUS_COMPLETECODE_NOACK 5
#define MACA_STATUS_COMPLETECODE_NODATA 6
#define MACA_STATUS_COMPLETECODE_LATESTART 7
#define MACA_STATUS_COMPLETECODE_EXTTIMEOUT 8
#define MACA_STATUS_COMPLETECODE_EXTPNDTIMEOUT 9
#define MACA_STATUS_COMPLETECODE_PLLUNLOCK 12
#define MACA_STATUS_COMPLETECODE_EXTABORT 13
#define MACA_STATUS_COMPLETECODE_NOTCOMPLETE 14
#define MACA_STATUS_COMPLETECODE_DMABUSERROR 15

#define MACA_STATUS_OVR_FALSE 0
#define MACA_STATUS_OVR_TRUE 1

#define MACA_STATUS_BUSY_FALSE 0
#define MACA_STATUS_BUSY_TRUE 1

#define MACA_STATUS_CRC_OK 0
#define MACA_STATUS_CRC_ERROR 1

#define MACA_STATUS_TO_FALSE 0
#define MACA_STATUS_TO_TRUE 1

#define MACA_FRMPND_PND_NODATA 0
#define MACA_FRMPND_PND_DATAAVAIL 1

#define MACA_MC1322x_ID_ENGINEERING 0
#define MACA_MC1322x_ID_MC13224_1 1
#define MACA_MC1322x_ID_MC13224_2 9
#define MACA_MC1322x_ID_MC13226 17


#define MACA_TMREN_STRT_ENABLE 1

#define MACA_TMREN_CPL_ENABLE 1

#define MACA_TMREN_SFT_ENABLE 1

#define MACA_TMRDIS_STRT_DISABLE 1

#define MACA_TMRDIS_CPL_DISABLE 1

#define MACA_TMRDIS_SFT_DISABLE 1

#define MACA_TMRDIS_STRTOFF_ABORT 1

#define MACA_TMRDIS_CPLOFF_ABORT 1

#define MACA_TMRDIS_SFTOFF_ABORT 1



#endif // MACA_H_
