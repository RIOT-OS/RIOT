/*
 * maca.c - The MACA driver for the Redbee EconoTag
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <stdio.h>

#include "maca.h"
#include "maca_packet.h"
#include "nvm.h"
#include "mc1322x.h"

// number of packets in the maca_packet_pool
#ifndef MACA_NUM_PACKETS
#define MACA_NUM_PACKETS 32
#endif

/* for 250kHz clock */
#define MACA_CLOCK_DIV 95
/* (32 chips/sym) * (sym/4bits) * (8bits/byte) = (64 chips/byte)  */
/* (8 chips/clk) * (byte/64 chips) = byte/8clks */
#define CLK_PER_BYTE 8

#ifndef RECV_SOFTIMEOUT
#define RECV_SOFTIMEOUT (1024*128*CLK_PER_BYTE)
#endif

#ifndef CPL_TIMEOUT
#define CPL_TIMEOUT (2*128*CLK_PER_BYTE)
#endif



static volatile maca_packet_t maca_packet_pool[MACA_NUM_PACKETS];

static volatile maca_packet_t *maca_free_head;
static volatile maca_packet_t *maca_rx_end;
static volatile maca_packet_t *maca_tx_end;
static volatile maca_packet_t *maca_dma_tx;
static volatile maca_packet_t *maca_dma_rx;

volatile maca_packet_t *maca_rx_head;
volatile maca_packet_t *maca_tx_head;

static volatile maca_packet_t maca_dummy_ack;

uint8_t (*get_lqi)(void) = (void *) 0x0000e04d;

#define MACA_NO_POST 0
#define MACA_TX_POST 1
#define MACA_RX_POST 2
#define MACA_MAX_POST 3
static volatile uint8_t maca_last_post = MACA_NO_POST;

void maca_init ( void ) {
    _maca_reset();
    _maca_radio_init();
    _maca_flyback_init();
    _maca_init_phy();
    maca_set_channel ( 0 );
    maca_set_power ( 0 );

    maca_free_head = 0;
    maca_tx_head = 0;
    maca_rx_head = 0;
    maca_rx_end = 0;
    maca_tx_end = 0;
    maca_dma_rx = 0;
    maca_dma_tx = 0;

    maca_free_all_packets();

    // promiscuous, no CCA
    MACA->CONTROLbits.PRM = MACA_CONTROL_PRM_ON;
    MACA->CONTROLbits.MODE = MACA_CONTROL_MODE_NOCCA;

    enable_irq ( INT_NUM_MACA );
    ITC->INTFRCbits.MACA = 1;
}

void _maca_reset ( void ) {
    volatile uint32_t c;

    MACA->RESETbits.RST = 1;

    for ( c = 0; c < 100; c++ ) {
        continue;
    }

    MACA->RESETbits.CLK_ON = 1;
    MACA->CONTROLbits.SEQUENCE = MACA_CONTROL_SEQUENCE_NOP;

    for ( c = 0; c < 400000; c++ ) {
        continue;
    }

    // clear all interrupts
    MACA->CLRIRQ = 0xffff;
}

void maca_on ( void ) {

    // turn on the radio regulators
    CRM->VREG_CNTLbits.BUCK_EN = 0x0;
    CRM->VREG_CNTLbits.BUCK_SYNC_REC_EN = 0x0;
    CRM->VREG_CNTLbits.BUCK_BYPASS_EN = 0x0;
    CRM->VREG_CNTLbits.VREG_1P5V_EN = 0x3;
    CRM->VREG_CNTLbits.VREG_1P5V_SEL = 0x3;
    CRM->VREG_CNTLbits.VREG_1P8V_EN = 0x0;
    CRM->VREG_CNTLbits.BUCK_CLKDIV = 0xf;

    // reinitialize phy
    _maca_reset();
    _maca_init_phy();

    enable_irq ( INT_NUM_MACA );
    ITC->INTFRCbits.MACA = 0x1;
}

void maca_off ( void ) {
    disable_irq ( INT_NUM_MACA );

    // turn off radio regulators
    CRM->VREG_CNTL = 0x00000f00;

    MACA->RESETbits.RST = 0x1;
}

void maca_free_all_packets ( void ) {
    volatile uint32_t i;
    safe_irq_disable ( INT_NUM_MACA );

    maca_free_head = 0;

    for ( i = 0; i < MACA_NUM_PACKETS; i++ ) {
        maca_free_packet ( ( volatile maca_packet_t * ) & ( maca_packet_pool[i] ) );
    }

    maca_rx_head = 0;
    maca_rx_end = 0;
    maca_tx_head = 0;
    maca_tx_end = 0;

    irq_restore();

    // force MACA interrupt if there are pending irqs
    if ( ITC->NIPENDbits.MACA ) {
        ITC->INTFRCbits.MACA = 0x1;
    }
}

void _maca_bound_check ( volatile maca_packet_t *packet ) {
    volatile uint32_t i;

    if ( !packet || packet == &maca_dummy_ack ) {
        return;
    }

    for ( i = 0; i < MACA_NUM_PACKETS; i++ ) {
        if ( packet == &maca_packet_pool[i] ) {
            return;
        }
    }

    // bad packet bounds! next, nirvana...
    printf ( "bad packet bounds! Halting... \n" );

    while ( 1 ) {
        continue;
    }
}

void maca_free_packet ( volatile maca_packet_t *packet ) {
    safe_irq_disable ( INT_NUM_MACA );

    // _maca_bound_check( packet );

    if ( !packet || packet == &maca_dummy_ack ) {
        return;
    }

    // _maca_bound_check( maca_free_head );

    packet->length = 0;
    packet->offset = 0;
    packet->left = maca_free_head;
    packet->right = 0;

    maca_free_head = packet;

    // _maca_bound_check( maca_free_head );

    irq_restore();

    if ( ITC->NIPENDbits.MACA ) {
        ITC->INTFRCbits.MACA = 0x1;
    }
}

volatile maca_packet_t *maca_get_free_packet ( void ) {
    volatile maca_packet_t *packet;

    safe_irq_disable ( INT_NUM_MACA );
    // _maca_bound_check( maca_free_head );

    packet = maca_free_head;

    if ( packet ) {
        maca_free_head = packet->left;
        maca_free_head->right = 0;
    }

    // _maca_bound_check( maca_free_head );

    irq_restore();

    if ( ITC->NIPENDbits.MACA ) {
        ITC->INTFRCbits.MACA = 0x1;
    }

    return packet;
}

volatile maca_packet_t *maca_get_rx_packet ( void ) {
    volatile maca_packet_t *packet;
    safe_irq_disable ( INT_NUM_MACA );

    // maca_bound_check ( maca_rx_head );

    packet = maca_rx_head;

    if ( packet ) {
        maca_rx_head = packet->left;
        maca_rx_head->right = 0;
    }

    irq_restore ();

    if ( ITC->NIPENDbits.MACA ) {
        ITC->INTFRCbits.MACA = 0x1;
    }

    return packet;
}

void maca_set_tx_packet ( volatile maca_packet_t *packet ) {
    safe_irq_disable ( INT_NUM_MACA );

    // maca_bound_check ( packet );

    if ( !packet ) {
        return;
    }

    if ( !maca_tx_head ) {
        // start a new queue
        maca_tx_end = packet;
        maca_tx_end->left = 0;
        maca_tx_end->right = 0;
        maca_tx_head = maca_tx_end;
    }
    else {
        // add packet to the end of the queue
        maca_tx_end->left = packet;
        packet->right = maca_tx_end;
        // move the queue
        maca_tx_end = packet;
        maca_tx_end->left = 0;
    }

    irq_restore ();

    if ( ITC->NIPENDbits.MACA || maca_last_post == MACA_NO_POST ) {
        ITC->INTFRCbits.MACA = 0x1;
    }

    if ( maca_last_post == MACA_RX_POST ) {
        MACA->SFTCLK = MACA->CLK + CLK_PER_BYTE;
    }

    return;
}

void _maca_post_receive ( void ) {
    maca_last_post = MACA_RX_POST;

    // from the libmc1322x implementation:
    // "this sets the rxlen field
    //  this is undocumented but very important
    //  you will not receive anything without setting it"
    MACA->TXLEN = ( MACA_MAX_PACKET_SIZE<<16 );

    if ( !maca_dma_rx ) {
        maca_dma_rx = maca_get_free_packet ();

        if ( !maca_dma_rx ) {
            MACA->SFTCLK = MACA->CLK + RECV_SOFTIMEOUT;
            MACA->TMRENbits.SFT = 0x1;

            // no free buffers, so don't start reception
            enable_irq ( INT_NUM_MACA );
            return;
        }
    }

    // maca_bound_check ( maca_dma_rx );
    // maca_bound_check ( maca_dma_tx );

    // load starting addess
    MACA->DMARX = ( uint32_t ) & ( maca_dma_rx->data[0] );
    // wind up a timer for timeout
    MACA->SFTCLK = MACA->CLK + RECV_SOFTIMEOUT;
    // enable the soft complete clk circuit
    MACA->TMRENbits.SFT = 0x1;
    // start the receive sequence
    MACA->CONTROLbits.ASAP = MACA_CONTROL_ASAP_ON;
    MACA->CONTROLbits.PRE_COUNT = 0x4;
    MACA->CONTROLbits.NOFC = MACA_CONTROL_NOFC_OFF; // aka use FCS
    MACA->CONTROLbits.PRM = MACA_CONTROL_PRM_ON;
    MACA->CONTROLbits.AUTO = MACA_CONTROL_AUTO_ON;
    MACA->CONTROLbits.SEQUENCE = MACA_CONTROL_SEQUENCE_RX;
}

void _maca_post_transmit ( void ) {
    safe_irq_disable ( INT_NUM_MACA );

    maca_last_post = MACA_TX_POST;
    maca_dma_tx = maca_tx_head;

    MACA->TXSEQNRbits.TXSEQN = maca_dma_tx->data[2];
    MACA->TXLENbits.TX_LEN = ( uint16_t ) ( ( maca_dma_tx->length ) + 2 );
    // set rx length to ACK length
    MACA->TXLEN = ( uint32_t ) ( 3<<16 );
    // load data starting address
    MACA->DMATX = ( uint32_t ) & ( maca_dma_tx->data[0 + maca_dma_tx->offset] );

    if ( !maca_dma_rx ) {
        maca_dma_rx = maca_get_free_packet();

        if ( !maca_dma_rx ) {
            printf ( "tried to fill MACA->DMARX in maca_post_tx but there are no packet buffers left" );
            maca_dma_rx = &maca_dummy_ack;
        }
    }

    // maca_bound_check ( maca_dma_rx );
    // maca_bound_check ( maca_dma_tx );

    MACA->DMARX = ( uint32_t ) & ( maca_dma_rx->data[0] );

    // disable soft timeout clk and start clk
    MACA->TMRDISbits.SFT = MACA_TMRDIS_SFT_DISABLE;
    MACA->TMRDISbits.CPL = MACA_TMRDIS_CPL_DISABLE;

    // set cpl clk to a long range and enable it
    // for the case MACA locks
    MACA->CPLCLK = MACA->CLK + CPL_TIMEOUT;
    MACA->TMRENbits.CPL = 0x1;

    irq_restore ();

    MACA->CONTROLbits.ASAP = MACA_CONTROL_ASAP_ON;
    MACA->CONTROLbits.PRE_COUNT = 0x4;
    MACA->CONTROLbits.PRM = MACA_CONTROL_PRM_ON;
    MACA->CONTROLbits.MODE = MACA_CONTROL_MODE_NOCCA;
    MACA->CONTROLbits.SEQUENCE = MACA_CONTROL_SEQUENCE_TX;

}

void _maca_free_tx_head ( void ) {
    volatile maca_packet_t *packet;
    safe_irq_disable ( INT_NUM_MACA );

    packet = maca_tx_head;
    maca_tx_head = maca_tx_head->left;

    if ( maca_tx_head == 0 ) {
        maca_tx_end = 0;
    }

    maca_free_packet ( packet );

    irq_restore ();

    if ( ITC->NIPENDbits.MACA ) {
        ITC->INTFRCbits.MACA = 1;
    }

    return;
}

void _maca_add_to_rx ( volatile maca_packet_t *packet ) {
    safe_irq_disable ( INT_NUM_MACA );

    if ( !packet ) {
        printf ( "packet null passed to _maca_add_to_rx\n" );
        return;
    }

    packet->offset = 1; // first byte is the length

    if ( maca_rx_head == 0 ) {
        // start a new queue
        maca_rx_end = packet;
        maca_rx_end->left = 0;
        maca_rx_end->right = 0;
        maca_rx_head = maca_rx_end;
    }
    else {
        // enqueue packet
        maca_rx_end->left = packet;
        packet->right = maca_rx_end;
        maca_rx_end = packet;
        maca_rx_end->left = 0;
    }

    irq_restore();

    if ( ITC->NIPENDbits.MACA ) {
        ITC->INTFRCbits.MACA = 1;
    }

    return;
}

void _maca_decode_status ( void ) {
    volatile uint32_t code;

    code = MACA->STATUSbits.COMPLETE_CODE;

    switch ( code ) {
    case MACA_STATUS_COMPLETECODE_ABORTED: {
        printf ( "maca: aborted\n" );
        _maca_resume_maca_sync ();
        break;
    }

    case MACA_STATUS_COMPLETECODE_NOTCOMPLETE: {
        printf ( "maca: not completed\n" );
        _maca_resume_maca_sync ();
        break;
    }

    case MACA_STATUS_COMPLETECODE_TIMEOUT: {
        printf ( "maca: timeout\n" );
        _maca_resume_maca_sync ();
        break;
    }

    case MACA_STATUS_COMPLETECODE_NOACK: {
        printf ( "maca: no ack\n" );
        _maca_resume_maca_sync ();
        break;
    }

    case MACA_STATUS_COMPLETECODE_EXTTIMEOUT: {
        printf ( "maca: ext timeout\n" );
        _maca_resume_maca_sync ();
        break;
    }

    case MACA_STATUS_COMPLETECODE_EXTPNDTIMEOUT: {
        printf ( "maca: ext pnd timeout\n" );
        _maca_resume_maca_sync ();
        break;
    }
    case MACA_STATUS_COMPLETECODE_SUCCESS: {
        printf ( "maca: success\n" );
        _maca_resume_maca_sync ();
        break;
    }

    default: {
        printf ( "maca status: %x", MACA->STATUSbits.COMPLETE_CODE );
        _maca_resume_maca_sync ();
    }
    }
}

void maca_isr ( void ) {
    //maca_entry++;

    if ( MACA->STATUSbits.OVR ) {
        printf ( "maca overrun\n" );
    }

    if ( MACA->STATUSbits.BUSY ) {
        printf ( "maca busy\n" );
    }

    if ( MACA->STATUSbits.CRC ) {
        printf ( "maca crc error\n" );
    }

    if ( MACA->STATUSbits.TO ) {
        printf ( "maca timeout\n" );
    }

    if ( MACA->IRQbits.DI ) {
        MACA->CLRIRQbits.DI = 0x1;
        maca_dma_rx->length = MACA->GETRXLVLbits.RECVBYTES - 2; // packet length doesn't include length
        maca_dma_rx->lqi = get_lqi();
        maca_dma_rx->rx_time = MACA->TIMESTAMP;

        // does the packet need an ACK?
        if ( MACA->CONTROLbits.PRM == MACA_CONTROL_PRM_OFF &&
                ( maca_dma_rx->data[1] & 0x20 ) ) {
            volatile uint32_t wait_clk;
            wait_clk = MACA->CLK + 200;

            while ( MACA->CLK < wait_clk ) {
                continue;
            }
        }

        if ( maca_rx_callback != 0 ) {
            maca_rx_callback ( maca_dma_rx );
        }

        _maca_add_to_rx ( maca_dma_rx );
        maca_dma_rx = 0;
    }

    // filter faild
    if ( MACA->IRQbits.FLT ) {
        printf ( "maca filter faild\n" );
        _maca_resume_maca_sync ();
        MACA->CLRIRQbits.FLT = 0x1;
    }

    // CRC checksum faild
    if ( MACA->IRQbits.CRC ) {
        printf ( "maca crc checksum faild\n" );
        _maca_resume_maca_sync ();
        MACA->CLRIRQbits.CRC = 0x1;
    }

    // softclock interrupt
    if ( MACA->IRQbits.SFT ) {
        MACA->CLRIRQbits.SFT = 0x1;
    }

    // poll interrupt
    if ( MACA->IRQbits.POLL ) {
        MACA->CLRIRQbits.POLL = 0x1;
    }

    // action complete interrupt
    if ( MACA->IRQbits.ACPL ) {
        if ( maca_last_post == MACA_TX_POST ) {
            maca_tx_head->status = MACA->STATUSbits.COMPLETE_CODE;

            if ( maca_tx_callback != 0 ) {
                maca_tx_callback ( maca_tx_head );
            }

            maca_dma_tx = 0x0;
            _maca_free_tx_head ();
            maca_last_post = MACA_NO_POST;
        }

        _maca_resume_maca_sync ();
        MACA->CLRIRQbits.ACPL = 0x1;
    }

    _maca_decode_status ();

    // this should never happen ...
    if ( MACA->IRQ != 0 ) {
        printf ( "MACA->IRQ is %x", (unsigned int) MACA->IRQ );
    }

    if ( maca_tx_head != 0 ) {
        _maca_post_transmit ();
    }
    else {
        _maca_post_receive ();
    }
}

void _maca_init_phy ( void ) {
    MACA->CLKDIV = MACA_CLOCK_DIV;
    MACA->WARMUP = 0x00180012;
    MACA->EOFDELAY = 0x00000004;
    MACA->CCADELAY = 0x001a0022;
    MACA->TXCCADELAY = 0x00000025;
    MACA->FRAMESYNC0 = 0x000000a7;
    MACA->CLK = 0x00000008;
    MACA->RXACKDELAY = 0x0000001e;
    MACA->RXEND = 0x000000b4;
    MACA->TXACKDELAY = 0x00000044;
    MACA->MASKIRQbits.ACPL = 0x1;
    MACA->MASKIRQbits.CM = 0x1;
    MACA->MASKIRQbits.FLT = 0x1;
    MACA->MASKIRQbits.CRC = 0x1;
    MACA->MASKIRQbits.DI = 0x1;
    MACA->MASKIRQbits.SFT = 0x1;
    MACA->MASKIRQbits.RST = 0x1;
    MACA->SLOTOFFSET = 0x00350000;
}

#define MTOC_BASE 0x80009a00 //Modem Tracking Oscillator Controller
void _maca_flyback_init ( void ) {
    uint32_t val8, tmp;

    val8 = * ( volatile uint32_t * ) ( MTOC_BASE+8 );
    tmp = val8 | 0x0000f7df;
    * ( volatile uint32_t * ) ( MTOC_BASE+8 ) = tmp;
    * ( volatile uint32_t * ) ( MTOC_BASE+12 ) = 0x00ffffff;
    * ( volatile uint32_t * ) ( MTOC_BASE+16 ) = ( ( ( uint32_t ) 0x00ffffff ) >>12 );
    * ( volatile uint32_t * ) ( MTOC_BASE ) = 16;
}

#define MACA_MAX_SEQ_1 2
const uint32_t maca_addr_seq_1[MACA_MAX_SEQ_1] = {
    0x80003048,
    0x8000304c,
};

const uint32_t maca_data_seq_1[MACA_MAX_SEQ_1] = {
    0x00000f78,
    0x00607707,
};


#define MACA_MAX_SEQ_2 2
const uint32_t maca_addr_seq_2[MACA_MAX_SEQ_2] = {
    0x8000a050,
    0x8000a054,
};

const uint32_t maca_data_seq_2[MACA_MAX_SEQ_2] = {
    0x0000047b,
    0x0000007b,
};

#define MACA_MAX_CAL3_SEQ_1 3
const uint32_t maca_addr_cal3_seq_1[MACA_MAX_CAL3_SEQ_1] = { 0x80009400,0x80009a04,0x80009a00, };
const uint32_t maca_data_cal3_seq_1[MACA_MAX_CAL3_SEQ_1] = {0x00020017,0x8185a0a4,0x8c900025, };

#define MACA_MAX_CAL3_SEQ_2 2
const uint32_t maca_addr_cal3_seq_2[MACA_MAX_CAL3_SEQ_2] = { 0x80009a00,0x80009a00,};
const uint32_t maca_data_cal3_seq_2[MACA_MAX_CAL3_SEQ_2] = { 0x8c900021,0x8c900027,};

#define MACA_MAX_CAL3_SEQ_3 1
const uint32_t maca_addr_cal3_seq_3[MACA_MAX_CAL3_SEQ_3] = { 0x80009a00 };
const uint32_t maca_data_cal3_seq_3[MACA_MAX_CAL3_SEQ_3] = { 0x8c900000 };

#define MACA_MAX_CAL5 4
const uint32_t maca_addr_cal5[MACA_MAX_CAL5] = {
    0x80009400,
    0x8000a050,
    0x8000a054,
    0x80003048,
};
const uint32_t maca_data_cal5[MACA_MAX_CAL5] = {
    0x00000017,
    0x00000000,
    0x00000000,
    0x00000f00,
};

#define MACA_MAX_DATA 43
const uint32_t maca_addr_reg_rep[MACA_MAX_DATA] = {
    0x80004118,0x80009204,0x80009208,0x8000920c,
    0x80009210,0x80009300,0x80009304,0x80009308,
    0x8000930c,0x80009310,0x80009314,0x80009318,
    0x80009380,0x80009384,0x80009388,0x8000938c,
    0x80009390,0x80009394,0x8000a008,0x8000a018,
    0x8000a01c,0x80009424,0x80009434,0x80009438,
    0x8000943c,0x80009440,0x80009444,0x80009448,
    0x8000944c,0x80009450,0x80009460,0x80009464,
    0x8000947c,0x800094e0,0x800094e4,0x800094e8,
    0x800094ec,0x800094f0,0x800094f4,0x800094f8,
    0x80009470,0x8000981c,0x80009828
};

const uint32_t maca_data_reg_rep[MACA_MAX_DATA] = {
    0x00180012,0x00000605,0x00000504,0x00001111,
    0x0fc40000,0x20046000,0x4005580c,0x40075801,
    0x4005d801,0x5a45d800,0x4a45d800,0x40044000,
    0x00106000,0x00083806,0x00093807,0x0009b804,
    0x000db800,0x00093802,0x00000015,0x00000002,
    0x0000000f,0x0000aaa0,0x01002020,0x016800fe,
    0x8e578248,0x000000dd,0x00000946,0x0000035a,
    0x00100010,0x00000515,0x00397feb,0x00180358,
    0x00000455,0x00000001,0x00020003,0x00040014,
    0x00240034,0x00440144,0x02440344,0x04440544,
    0x0ee7fc00,0x00000082,0x0000002a
};

uint8_t ctov[16] = {
    0x0b,0x0b,0x0b,0x0a,
    0x0d,0x0d,0x0c,0x0c,
    0x0f,0x0e,0x0e,0x0e,
    0x11,0x10,0x10,0x0f
};

#define _INIT_CTOV_WORD_1 0x00dfbe77
#define _INIT_CTOV_WORD_2 0x023126e9
uint8_t get_ctov ( uint32_t r0, uint32_t r1 ) {
    r0 = r0 * _INIT_CTOV_WORD_1;
    r0 += ( r1 << 22 );
    r0 += _INIT_CTOV_WORD_2;

    r0 = ( uint32_t ) ( ( ( int32_t ) r0 ) >>25 );
    return ( uint8_t ) r0;
}

static uint8_t _ram_values[4];

void _maca_radio_init ( void ) {
    volatile uint32_t c;

    // sequence 1
    for ( c = 0; c < MACA_MAX_SEQ_1; c++ ) {
        if ( ( uint32_t ) maca_addr_seq_1[c] != CRM->VREG_CNTL ) {
            * ( ( volatile uint32_t * ) ( maca_addr_seq_1[c] ) ) = maca_data_seq_1[c];
        }
    }

    // sequence 1 delay
    for ( c = 0; c < 0x161a8; c++ ) {
        continue;
    }

    // sequence 2
    for ( c = 0; c < MACA_MAX_SEQ_2; c++ ) {
        * ( ( volatile uint32_t * ) ( maca_addr_seq_2[c] ) ) = maca_data_seq_2[c];
    }

    // modem value
    * ( ( volatile uint32_t * ) 0x80009000 ) = 0x80050100;

    // cal 3 seq 1
    for ( c = 0; c < MACA_MAX_CAL3_SEQ_1; c++ ) {
        * ( ( volatile uint32_t * ) ( maca_addr_cal3_seq_1[c] ) ) = maca_data_cal3_seq_1[c];
    }

    // cal 3 delay
    for ( c = 0; c < 0x11194; c++ ) {
        continue;
    }

    // cal 3 seq 2
    for ( c = 0; c < MACA_MAX_CAL3_SEQ_2; c++ ) {
        * ( ( volatile uint32_t * ) ( maca_addr_cal3_seq_2[c] ) ) = maca_data_cal3_seq_2[c];
    }

    // cal 3 delay
    for ( c = 0; c < 0x11194; c++ ) {
        continue;
    }

    // cal 3 seq 3
    for ( c = 0; c < MACA_MAX_CAL3_SEQ_3; c++ ) {
        * ( ( volatile uint32_t * ) ( maca_addr_cal3_seq_3[c] ) ) = maca_data_cal3_seq_3[c];
    }

    // cal 5
    for ( c = 0; c < MACA_MAX_CAL5; c++ ) {
        if ( ( uint32_t ) maca_addr_cal5[c] != CRM->VREG_CNTL ) {
            * ( ( volatile uint32_t * ) ( maca_addr_cal5[c] ) ) = maca_data_cal5[c];
        }
    }

    /*reg replacment */
    for ( c = 0; c < MACA_MAX_DATA; c++ ) {
        * ( ( volatile uint32_t * ) ( maca_addr_reg_rep[c] ) ) = maca_data_reg_rep[c];
    }

    _maca_init_from_flash ( 0x1f000 );

    for ( c=0; c<4; c++ ) {
        printf ( "  0x%02x\n\r", _ram_values[c] );
    }

    for ( c=0; c<16; c++ ) {
        ctov[c] = get_ctov ( c, _ram_values[3] );
    }
}

const uint32_t PSMVAL[19] = {
    0x0000080f, 0x0000080f, 0x0000080f,
    0x0000080f, 0x0000081f, 0x0000081f,
    0x0000081f, 0x0000080f, 0x0000080f,
    0x0000080f, 0x0000001f, 0x0000000f,
    0x0000000f, 0x00000816, 0x0000001b,
    0x0000000b, 0x00000802, 0x00000817,
    0x00000003,
};

const uint32_t PAVAL[19] = {
    0x000022c0, 0x000022c0, 0x000022c0,
    0x00002280, 0x00002303, 0x000023c0,
    0x00002880, 0x000029f0, 0x000029f0,
    0x000029f0, 0x000029c0, 0x00002bf0,
    0x000029f0, 0x000028a0, 0x00002800,
    0x00002ac0, 0x00002880, 0x00002a00,
    0x00002b00,
};

const uint32_t AIMVAL[19] = {
    0x000123a0, 0x000163a0, 0x0001a3a0,
    0x0001e3a0, 0x000223a0, 0x000263a0,
    0x0002a3a0, 0x0002e3a0, 0x000323a0,
    0x000363a0, 0x0003a3a0, 0x0003a3a0,
    0x0003e3a0, 0x000423a0, 0x000523a0,
    0x000423a0, 0x0004e3a0, 0x0004e3a0,
    0x0004e3a0,
};

// #define MRRF_BASE 0x80009400 // Modem Radio Receiver Functions
// void _set_demodulator_type ( uint8_t demod ) {
//     uint32_t val = *(uint32_t*)(MRRF_BASE);
//     if ( demod == MACA_DEMOD_NCD ) {
//         val = (val & ~1);
//     }
//     else {
//         val = (val | 1);
//     }
//     *(uint32_t*)(MRRF_BASE) = val;
// }

#define ADDR_POW1 0x8000a014
#define ADDR_POW2 (ADDR_POW1 + 12)
#define ADDR_POW3 (ADDR_POW1 + 64)

void maca_set_power ( uint8_t power ) {
    safe_irq_disable ( INT_NUM_MACA );

    * ( ( uint32_t * ) ( ADDR_POW1 ) ) = PSMVAL[power];

#ifdef USE_PA
    * ( ( uint32_t * ) ( ADDR_POW2 ) ) = 0xffffdfff & PAVAL[power]; /* single port */
#else
    * ( ( uint32_t * ) ( ADDR_POW2 ) ) = 0x00002000 | PAVAL[power]; /* dual port */
#endif

    * ( ( uint32_t * ) ( ADDR_POW3 ) ) = AIMVAL[power];
    irq_restore();

    if ( ITC->NIPENDbits.MACA ) {
        ITC->INTFRCbits.MACA = 1;
    }
}

const uint8_t VCODivI[16] = {
    0x2f, 0x2f, 0x2f, 0x2f,
    0x2f, 0x2f, 0x2f, 0x2f,
    0x2f, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30,
};

const uint32_t VCODivF[16] = {
    0x00355555, 0x006aaaaa, 0x00a00000, 0x00d55555,
    0x010aaaaa, 0x01400000, 0x01755555, 0x01aaaaaa,
    0x01e00000, 0x00155555, 0x004aaaaa, 0x00800000,
    0x00b55555, 0x00eaaaaa, 0x01200000, 0x01555555,
};

#define ADDR_CHAN1 0x80009800
#define ADDR_CHAN2 (ADDR_CHAN1+12)
#define ADDR_CHAN3 (ADDR_CHAN1+16)
#define ADDR_CHAN4 (ADDR_CHAN1+48)

void maca_set_channel ( uint8_t chan ) {
    volatile uint32_t tmp;
    safe_irq_disable ( INT_NUM_MACA );

    tmp = * ( uint32_t * ) ( ADDR_CHAN1 );
    tmp = tmp & 0xbfffffff;
    * ( ( uint32_t * ) ( ADDR_CHAN1 ) ) = tmp;

    * ( ( uint32_t * ) ( ADDR_CHAN2 ) ) = VCODivI[chan];
    * ( ( uint32_t * ) ( ADDR_CHAN3 ) ) = VCODivF[chan];

    tmp = * ( ( uint32_t * ) ( ADDR_CHAN4 ) );
    tmp = tmp | 2;
    * ( ( uint32_t * ) ( ADDR_CHAN4 ) ) = tmp;

    tmp = * ( ( uint32_t * ) ( ADDR_CHAN4 ) );
    tmp = tmp | 4;
    * ( ( uint32_t * ) ( ADDR_CHAN4 ) ) = tmp;

    tmp = tmp & 0xffffe0ff;
    tmp = tmp | ( ( ( ctov[chan] ) <<8 ) &0x1f00 );
    * ( ( uint32_t * ) ( ADDR_CHAN4 ) ) = tmp;

    irq_restore();

    if ( ITC->NIPENDbits.MACA ) {
        ITC->INTFRCbits.MACA = 1;
    }
}

radio_address_t maca_set_address (radio_address_t addr) {
    safe_irq_disable ( INT_NUM_MACA );

    MACA->MAC16ADDR = addr;

    irq_restore();

    if ( ITC->NIPENDbits.MACA ) {
        ITC->INTFRCbits.MACA = 1;
    }
    return MACA->MAC16ADDR;
}

radio_address_t maca_get_address ( void ) {
    return MACA->MAC16ADDR;
}

uint16_t maca_set_pan(uint16_t pan)
{
    safe_irq_disable(INT_NUM_MACA);

    MACA->MACPANIDbits.PANID = pan;

    irq_restore();

    if (ITC->NIPENDbits.MACA) {
        ITC->INTFRCbits.MACA = 1;
    }

    return MACA->MACPANIDbits.PANID;
}

uint16_t maca_get_pan(void)
{
    return MACA->MACPANIDbits.PANID;
}

#define MACA_ROM_END 0x0013ffff
#define MACA_ENTRY_EOF 0x00000e0f
uint32_t _exec_init_entry ( volatile uint32_t *entries, uint8_t *value_buffer ) {
    if ( entries[0] <= MACA_ROM_END ) {
        if ( entries[0] == 0 ) {
            /* do delay */
            printf ( "init_entry: delay 0x%08x\n", (unsigned int) entries[1] );

            for ( volatile uint32_t i=0; i < entries[1]; i++ ) {
                continue;
            }

            return 2;
        }
        else if ( entries[0] == 1 ) {
            /* do bit set/clear */
            printf ( "init_entry: bit set/clear 0x%08x 0x%08x 0x%08x\n", (unsigned int) entries[1],
                     (unsigned int) entries[2],
                     (unsigned int) entries[3] );
            * ( uint32_t * ) ( entries[2] ) = ( * ( uint32_t * ) ( entries[2] ) & entries[1] ) | ( entries[3] & entries[1] );
            return 4;
        }
        else if ( entries[0] >= 16 && entries[0] < 0xfff1 ) {
            /* store bytes in value_buffer */
            printf ( "init_entry: store in value_buffer 0x%02x position %d\n", (unsigned int) entries[1],
                     (unsigned int) ( entries[0]>>4 ) - 1 );
            value_buffer[ ( entries[0]>>4 )-1] = entries[1];
            return 2;
        }
        else if ( entries[0] == MACA_ENTRY_EOF ) {
            printf ( "init_entry: EOF \n" );
            return 0;
        }
        else {
            /* invalid */
            printf ( "init_entry: invalid code 0x%08x\n", (unsigned int) entries[0] );
            return 0;
        }
    }
    else {
        /* address not in ROM */
        /* store value in address command */
        printf ( "init_entry: address value pair - *0x%08x = 0x%08x\n", (unsigned int) entries[0],
                 (unsigned int) entries[1] );

        if ( entries[0] != CRM->VREG_CNTL ) {
            * ( uint32_t * ) ( entries[0] ) = entries[1];
        }
        else {
            printf ( "skipping CRM->VREG_CNTL\n" );
        }

        return 2;
    }
}

#define MACA_FLASH_INIT_MAGIC 0x00000abc
uint32_t _maca_init_from_flash ( uint32_t addr ) {
    nvm_type_t type = 0;
    nvm_err_t err;

    volatile uint32_t buffer[8];
    volatile uint32_t i = 0;
    volatile uint32_t j = 0;

    err = nvm_detect ( g_nvm_internal_interface_c, &type );
    printf ( "nvm_detect returned type 0x%08x err 0x%02x\n", type, err );

    nvm_setsvar ( 0 );
    err = nvm_read ( g_nvm_internal_interface_c, type, ( uint8_t * ) buffer, addr, 8 );
    i += 8;
    printf ( "nvm_read returned: 0x%02x\n", err );

    for ( j = 0; j < 4; j++ ) {
        printf ( "0x%08x\n", (unsigned int) buffer[j] );
    }

    if ( buffer[0] == MACA_FLASH_INIT_MAGIC ) {
        volatile uint32_t length;
        length = buffer[1] & 0x0000ffff;

        while ( i < ( length-4 ) ) {
            err = nvm_read ( g_nvm_internal_interface_c, type, ( uint8_t * ) buffer, addr+i, 32 );
            i += 4 * _exec_init_entry ( buffer, _ram_values );
        }
    }
    else {
        i = 0;
    }

    nvm_setsvar ( 1 );
    return i;
}

void _maca_resume_maca_sync ( void ) {
    volatile uint32_t clk;
    volatile uint32_t tsm_rx_steps;
    volatile uint32_t last_warmup_step;
    volatile uint32_t last_warmup_data;
    volatile uint32_t last_warmdown_step;
    volatile uint32_t last_warmdown_data;
    volatile uint32_t i;

    safe_irq_disable ( INT_NUM_MACA );

    /* manual TSM modem shutdown */
    /* read TSM_RX_STEPS */
    tsm_rx_steps = * ( volatile uint32_t * ) ( 0x80009204 );
    /* isolate the RX_WU_STEPS */
    /* shift left to align with 32-bit addressing */
    last_warmup_step = ( tsm_rx_steps & 0x1f ) << 2;
    /* Read "current" TSM step and save this value for later */
    last_warmup_data = ( * ( ( volatile uint32_t * ) ( 0x80009300 + last_warmup_step ) ) );

    /* isolate the RX_WD_STEPS */
    /* right-shift bits down to bit 0 position */
    /* left-shift to align with 32-bit addressing */
    last_warmdown_step = ( ( tsm_rx_steps & 0x1f00 ) >> 8 ) << 2;
    /* write "last warmdown data" to current TSM step to shutdown rx */
    last_warmdown_data = ( * ( ( volatile uint32_t * ) ( 0x80009300 + last_warmdown_step ) ) );
    ( * ( ( volatile uint32_t * ) ( 0x80009300 + last_warmup_step ) ) ) = last_warmdown_data;

    /* Abort */
    MACA->CONTROLbits.SEQUENCE = MACA_CONTROL_SEQUENCE_ABORT;

    /* Wait ~8us */
    for ( clk = MACA->CLK, i = 0; MACA->CLK - clk < 3 && i < 300; i++ ) {
        continue;
    }

    /* NOP */
    MACA->CONTROLbits.SEQUENCE = MACA_CONTROL_SEQUENCE_NOP;

    /* Wait ~8us */
    for ( clk = MACA->CLK, i = 0; MACA->CLK - clk < 3 && i < 300; i++ ) {
        continue;
    }

    /* restore original "last warmup step" data to TSM (VERY IMPORTANT!!!) */
    ( * ( ( volatile uint32_t * ) ( 0x80009300 + last_warmup_step ) ) ) = last_warmup_data;

    /* Clear all MACA interrupts - we should have gotten the ABORT IRQ */
    MACA->CLRIRQ = 0xffff;

    irq_restore();
}
