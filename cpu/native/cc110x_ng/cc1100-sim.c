#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <inttypes.h>
#include <string.h>

#include "debug.h"

#include "cc110x_ng.h"
#include "cc110x-internal.h" /* CC1100_READ_BURST etc. */
#include "tap.h"
#include "cc1100sim.h"

#define CC1100_EVENT_SET_POWER      0x01
#define CC1100_EVENT_SET_CHANNEL    0x02

/**
 * simplified states according to cc1100.pdf figure 5, page 23
 */
#define STATE_IDLE      0x00
/* SPWD || WOR -> STATE_SLEEP
 * SXOFF -> STATE_XTALOFF
 * SCAL -> STATE_FREQ_SCAL
 * SRX || STX || WOR -> STATE_FREQ_SST */
#define STATE_SLEEP     0x01 /* CSn=0 -> STATE_IDLE */
#define STATE_XTALOFF   0x02 /* CSn=0 -> STATE_IDLE */
#define STATE_FREQ_SCAL 0x03 /* -> STATE_IDLE */

#define STATE_FREQ_SST  0x04
/* SFSTXON -> STATE_FREQ_SON
 * STX -> STATE_TX
 * SRX||WOR -> STATE_RX */
#define STATE_FREQ_SON  0x05 /* STX -> STATE_TX */
#define STATE_TX        0x06
/* TXOFF_MODE = 11 || SRX -> STATE_RX
 * TXOFF_MODE = 01 -> STATE_FREQ_SON
 * STATE_TX_FFUF */
#define STATE_TX_FFUF   0x07 /* SFTX -> STATE_IDLE */
#define STATE_RX        0x08
/* RXOFF_MODE = 10 || STX -> STATE_TX
 * RXOFF_MODE = 00 -> STATE_FREQ_SON
 * -> STATE_TX_FFUF */
#define STATE_RX_FFOF   0x09 /* SFRX -> STATE_IDLE */


/**
 * status byte state bits according to cc1100.pdf, page 26, table 17
 */
#define M_IDLE            0x00
#define M_RX              0x10
#define M_TX              0x20
#define M_FSTXON          0x30
#define M_CALIBRATE       0x40
#define M_SETTLING        0x50
#define M_RXFIFO_OVERFLOW 0x60
#define M_TXFIFO_UNDERFLOW 0x70

#define MASK_STATE                  0x70
#define MASK_CHIP_RDYn              0x80
#define MASK_FIFO_BYTES_AVAILABLE   0x0F

#if 0
static uint8_t internal_state; /* according to table 17 */
static uint8_t _native_cc1100_spi; /* the pins */
/* one byte goes in, one byte goes out */
static uint8_t _native_cc1100_serial_io;
#endif

uint8_t addr; /* as sent in header */
uint8_t _native_cc110x_state;


uint8_t configuration_registers[((0x2E)+1)]; /* address 0x00 - 0x2E */
uint8_t configuration_registers_idx; /* for burst access */
uint8_t status_registers[((0x3D-0x30)+1)]; /* address 0x30 - 0x3D */
/* command strobes reside at the addresses 0x30 to 0x3D when writing */
uint8_t patable[8]; /* address 0x3E */
uint8_t patable_idx; /* for burst access */

uint8_t tx_fifo[64];
uint8_t tx_fifo_idx;
uint8_t rx_fifo[64];
uint8_t rx_fifo_idx;

int native_cc110x_gd0;
int native_cc110x_gd1;
int native_cc110x_gd2;
int native_cc110x_gd0_enabled;
int native_cc110x_gd2_enabled;

/* static uint8_t address_space[0xFF]; */
/* static uint8_t address_space_pointer; */

/* network layer handlers for cc1100 events: */
struct cc1100_callback_t {
    void (*func)(void);
};
static struct cc1100_callback_t _native_cc1100_callbacks[255];

/* cc1100 default configuration */
unsigned char cc1100_reset_configuration_regs[] = {
    0x29, /* IOCFG2 */
    0x2E, /* IOCFG1 */
    0x3F, /* IOCFG0 */
    0x07, /* FIFOTHR */
    0xD3, /* SYNC1 */
    0x91, /* SYNC0 */
    0xFF, /* PKTLEN */
    0x04, /* PKTCTRL1 */
    0x45, /* PKTCTRL0 */
    0x00, /* ADDR */
    0x00, /* CHANNR */
    0x0F, /* FSCTRL1 */
    0x00, /* FSCTRL0 */
    0x1E, /* FREQ2 */
    0xC4, /* FREQ1 */
    0xEC, /* FREQ0 */
    0x2C, /* MDMCFG4 */
    0x22, /* MDMCFG3 */
    0x02, /* MDMCFG2 */
    0x22, /* MDMCFG1 */
    0xF8, /* MDMCFG0 */
    0x47, /* DEVIATN */
    0x07, /* MCSM2 */
    0x30, /* MCSM1 */
    0x04, /* MCSM0 */
    0x66, /* FOCCFG */
    0x66, /* BSCFG */
    0x03, /* AGCCTRL2 */
    0x40, /* AGCCTRL1 */
    0x91, /* AGCCTRL0 */
    0x87, /* WOREVT1 */
    0x6B, /* WOREVT0 */
    0xF8, /* WORCTRL */
    0x56, /* FREND1 */
    0x10, /* FREND0 */
    0xA9, /* FSCAL3 */
    0x0A, /* FSCAL2 */
    0x20, /* FSCAL1 */
    0x0D, /* FSCAL0 */
    0x41, /* RCCTRL1 */
    0x00, /* RCCTRL0 */
    0x59, /* FSTEST */
    0x7F, /* PTEST */
    0x3F, /* AGCTEST */
    0x88, /* TEST2 */
    0x31, /* TEST1 */
    0x0B  /* TEST0 */
};

/* cc1100 default status */
unsigned char cc1100_reset_status_regs[] = {
    0x00, /* PARTNUM */
    0x03, /* VERSION */
    0x00, /* FREQEST */
    0x00, /* LQI */
    0x00, /* RSSI */
    0x00, /* MARCSTATE */
    0x00, /* WORTIME1 */
    0x00, /* WORTIME0 */
    0x00, /* PKTSTATUS */
    0x00, /* VCO_VC_DAC */
    0x00, /* TXBYTES */
    0x00, /* RXBYTES */
    0x00, /* RCCTRL1_STATUS */
    0x00  /* RCCTRL0_STATUS */
};

void do_sres(void)
{
    int i;

    for (i = 0; i < 0x2E; i++) {
        configuration_registers[i] = cc1100_reset_configuration_regs[i];
    }
    for (i = 0; i < (0x3D - 0x30); i++) {
        status_registers[i] = cc1100_reset_status_regs[i];
    }
    for (i = 1; i < 8; i++) {
        patable[i] = 0;
    }
}

uint8_t do_strobe(void)
{
    DEBUG("issue strobe command\n");
    switch (addr) {
        case CC1100_SRES:
            do_sres();
            break;
        case CC1100_SFSTXON:
            //warnx("do_strobe: not implemented: CC1100_SFSTXON");
            break;
        case CC1100_SXOFF:
            //warnx("do_strobe: not implemented: CC1100_SXOFF");
            break;
        case CC1100_SCAL:
            //warnx("do_strobe: not implemented: CC1100_SCAL");
            break;
        case CC1100_SRX:
            //warnx("do_strobe: not implemented: CC1100_SRX");
            break;
        case CC1100_STX:
            //warnx("do_strobe: not properly implemented: CC1100_STX");
            if (((status_registers[CC1100_TXBYTES - 0x30] & 0x7F) > 0) && (send_buf() == -1)) {
                native_cc110x_gd2 = 1;
            }
            else {
                native_cc110x_gd2 = 1;
            }
            break;
        case CC1100_SIDLE:
            //warnx("do_strobe: not implemented: CC1100_SIDLE ");
            break;
        case CC1100_SAFC:
            //warnx("do_strobe: not implemented: CC1100_SAFC");
            break;
        case CC1100_SWOR:
            //warnx("do_strobe: not implemented: CC1100_SWOR");
            break;
        case CC1100_SPWD:
            //warnx("do_strobe: not implemented: CC1100_SPWD");
            break;
        case CC1100_SFRX:
            status_registers[CC1100_RXBYTES - 0x30] = 0x00;
            rx_fifo_idx = 0x00;
            break;
        case CC1100_SFTX:
            status_registers[CC1100_TXBYTES - 0x30] = 0x00;
            tx_fifo_idx = 0x00;
            break;
        case CC1100_SWORRST:
            //warnx("do_strobe: not implemented: CC1100_SWORRST");
            break;
        case CC1100_SNOP:
            //warnx("do_strobe: not implemented: CC1100_SNOP");
            break;
        default:
            errx(EXIT_FAILURE, "do_strobe: internal error");
    }
    return addr;
}

uint8_t parse_header(uint8_t c)
{
    DEBUG("parse_header(0x%02X): ", c);

    addr = c & 0x3F; /* bits [5..0] */
    uint8_t mode = c & 0xC0; /* bits [76]*/

    /* set access mode, set status bute (c) */
    switch (mode) {
        case CC1100_READ_BURST:
            /* status  registers can only be read single */
            if ((addr >= 0x30) && (addr <= 0x3D)) {
                DEBUG(" CC1100_READ_SINGLE");
                _native_cc110x_state = STATE_READ_S;
            }
            else {
                DEBUG(" CC1100_READ_BURST");
                _native_cc110x_state = STATE_READ_B;
            }
            c = status_registers[CC1100_RXBYTES - 0x30] - rx_fifo_idx;
            break;
        case CC1100_WRITE_BURST:
            DEBUG(" CC1100_WRITE_BURST");
            _native_cc110x_state = STATE_WRITE_B;
            c = 0x0F - status_registers[CC1100_TXBYTES - 0x30];
            break;
        case CC1100_READ_SINGLE:
            DEBUG(" CC1100_READ_SINGLE");
            _native_cc110x_state = STATE_READ_S;
            c = status_registers[CC1100_RXBYTES - 0x30] - rx_fifo_idx;
            break;
        default:
            DEBUG(" CC1100_WRITE_SINGLE");
            _native_cc110x_state = STATE_WRITE_S;
            c = 0x0F - status_registers[CC1100_TXBYTES - 0x30];
    }
    if (addr <= 0x2E) {
        DEBUG(" configuration register");
    }
    else if ((addr >= 0x30) && (addr <= 0x3D)) {
        if ((_native_cc110x_state == STATE_WRITE_B) || (_native_cc110x_state == STATE_WRITE_S)) {
            DEBUG(" strobe command");
            /* strobe commands get executed directly */
            do_strobe();
        }
        else {
            DEBUG(" status register");
        }
    }
    else if (addr == 0x3E) {
        DEBUG(" patable");
    }
    else if (addr == 0x3F) {
        if ((_native_cc110x_state == STATE_WRITE_B) || (_native_cc110x_state == STATE_WRITE_S)) {
            DEBUG(" TX");
        }
        else {
            DEBUG(" RX");
        }
    }
    else {
        errx(EXIT_FAILURE, "parse_header: unhandled addr: 0x%02X", addr);
    }
    DEBUG("==\n");

    return c;
}

uint8_t read_single(uint8_t c)
{
    DEBUG("read_single\n");
    if (addr <= 0x2E) {
        DEBUG("read configuration_registers\n");
        return configuration_registers[addr++];
    }
    else if ((addr >= 0x30) && (addr <= 0x3D)) {
        DEBUG("read status_registers\n");
        if (addr == 0x3B) {
            DEBUG("\n\n\t\treturning RXBYTES: %d\n\n", status_registers[(addr) - 0x30]);
        }
        else {
            DEBUG("\n\n\t\treturning: %d\n\n", status_registers[(addr) - 0x30]);
        }
        return status_registers[(addr++) - 0x30];
    }
    else if (addr == 0x3E) {
        DEBUG("read patable\n");
        if (patable_idx > 7) {
            patable_idx = 0;
        }
        return patable[patable_idx++];
    }
    else if (addr == 0x3F) {
        DEBUG("read rx fifo\n");
        int off = (status_registers[CC1100_RXBYTES - 0x30] - rx_fifo_idx);
        switch (off) {
            case 0:
                /* CRC OK */
                return CRC_OK;
            case -1:
                /* CRC OK */
                return 0xFF;
        }
        if (rx_fifo_idx >= status_registers[CC1100_RXBYTES - 0x30]) {
            //warnx("read_single: buffer empty");
        }
        return rx_fifo[rx_fifo_idx++];
    }
    else {
        errx(EXIT_FAILURE, "read_single: unhandled addr: 0x%02X", addr);
    }
    return c;
}

uint8_t read_burst(uint8_t c)
{
    DEBUG(" read_burst: ");
    if (c != 0xFF) {
        warnx("read_burst: unexpected value");
    }

    if (addr <= 0x2E) {
        DEBUG("read configuration_registers\n");
        return configuration_registers[addr++];
    }
    else if ((addr >= 0x30) && (addr <= 0x3D)) {
        DEBUG("read status_registers\n");
        return status_registers[(addr++) - 0x30];
    }
    else if (addr == 0x3E) {
        DEBUG("read patable\n");
        if (patable_idx > 7) {
            patable_idx = 0;
        }
        return patable[patable_idx++];
    }
    else if (addr == 0x3F) {
        DEBUG("read rx fifo\n");
        int off = (status_registers[CC1100_RXBYTES - 0x30] - rx_fifo_idx);
        switch (off) {
            case 0:
                /* CRC OK */
                return CRC_OK;
            case -1:
                /* CRC OK */
                return 0xFF;
        }
        if (rx_fifo_idx >= status_registers[CC1100_RXBYTES - 0x30]) {
            //warnx("read_burst: buffer empty");
        }
        return rx_fifo[rx_fifo_idx++];
    }
    else {
        errx(EXIT_FAILURE, "write_single: unhandled addr: 0x%02X", addr);
    }
}

void do_write_conf(uint8_t c)
{
    DEBUG("write configuration register\n");
    configuration_registers[addr] = c;
    if (_native_cc1100_callbacks[addr].func != NULL) {
        DEBUG("calling cc1100 callback for %i\n", addr);
        _native_cc1100_callbacks[addr].func();
    }
}

void do_write_patable(uint8_t c)
{
    DEBUG("write patable\n");
    patable[patable_idx] = c;
}
void do_write_tx(uint8_t c)
{
    DEBUG("write TX\n");
    if (status_registers[CC1100_TXBYTES - 0x30] == CC1100_FIFO_LENGTH) {
        errx(EXIT_FAILURE, "do_write_tx: buffer too small");
    }
    tx_fifo[tx_fifo_idx++] = c;
    status_registers[CC1100_TXBYTES - 0x30]++;
}

uint8_t write_single(uint8_t c)
{
    DEBUG("write_single\n");

    if (addr <= 0x2E) {
        do_write_conf(c);
    }
    else if ((addr >= 0x30) && (addr <= 0x3D)) {
        do_strobe();
    }
    else if (addr == 0x3E) {
        do_write_patable(c);
    }
    else if (addr == 0x3F) {
        do_write_tx(c);
    }
    else {
        errx(EXIT_FAILURE, "write_single: unhandled addr: 0x%02X", addr);
    }

    return 0;
}

uint8_t write_burst(uint8_t c)
{
    DEBUG("write_burst\n");

    if (addr <= 0x2E) {
        do_write_conf(c);
        addr++;
    }
    else if ((addr >= 0x30) && (addr <= 0x3D)) {
        do_strobe();
        addr++;
    }
    else if (addr == 0x3E) {
        DEBUG("write patable\n");
    }
    else if (addr == 0x3F) {
        do_write_tx(c);
    }
    else {
        errx(EXIT_FAILURE, "write_single: unhandled addr: 0x%02X", addr);
    }

    return 0;
}

uint8_t do_txrx(uint8_t c)
{
    switch (_native_cc110x_state) {
        case STATE_SEL:
            c = parse_header(c);
            break;
        case STATE_WRITE_B:
            c = write_burst(c);
            break;
        case STATE_READ_B:
            c = read_burst(c);
            break;
        case STATE_READ_S:
            c = read_single(c);
            _native_cc110x_state = STATE_SEL;
            break;
        case STATE_WRITE_S:
            c = write_single(c);
            _native_cc110x_state = STATE_SEL;
            break;
        case STATE_NULL:
            //warnx("received command(?) in NULL state");
            c = 0x00;
        default:
            errx(EXIT_FAILURE, "funny cc110x_ng state");
    }
    return c;
}

void _native_cc1100_register_callback(int event, void *cb)
{
    _native_cc1100_callbacks[event].func = cb;
}

void _native_cc1100_handle_packet(unsigned char *buf, int size)
{ 
    unsigned char dst_addr;

    dst_addr = buf[1];

    /* packet automation */

    /* monitor mode */
    if ((configuration_registers[CC1100_PKTCTRL1] & 0x03) == 0x00) {
        DEBUG("_native_cc1100_handle_packet: not filtering address\n");
    }
    /* address filter */
    else {
        /* own addr check */
        if (dst_addr == configuration_registers[CC1100_ADDR]) {
            DEBUG("_native_cc1100_handle_packet: accept packet, addressed to us\n");
        }
        /* 0x00 only broadcast */
        else if (
                ((configuration_registers[CC1100_PKTCTRL1] & 0x03) == 0x02) &&
                (dst_addr == 0x00)
                ) {
            DEBUG("_native_cc1100_handle_packet: accept packet, broadcast\n");
        }
        /* 0x00 only broadcast */
        else if (
                ((configuration_registers[CC1100_PKTCTRL1] & 0x03) == 0x03) &&
                ((dst_addr == 0x00) || (dst_addr == 0xFF))
                ) {
            DEBUG("_native_cc1100_handle_packet: accept packet, broadcast\n");
        }
        else {
            DEBUG("_native_cc1100_handle_packet: discard packet addressed to someone else\n");
            return;
        }
    }
    /* length filter */
    /* variable packet length */
    if ((configuration_registers[CC1100_PKTCTRL0] & 0x03) == 0x01) {
        if (size > configuration_registers[CC1100_PKTLEN]) {
            DEBUG("_native_cc1100_handle_packet: discard packet longer than CC1100_PKTLEN\n");
            return;
        }
        else {
            DEBUG("_native_cc1100_handle_packet: accept packet <= CC1100_PKTLEN\n");
        }
    }
    /* fixed packet length */
    else if ((configuration_registers[CC1100_PKTCTRL0] & 0x03) == 0x00) {
        if (size != configuration_registers[CC1100_PKTLEN]) {
            DEBUG("_native_cc1100_handle_packet: discard packet, size differs from CC1100_PKTLEN\n");
            return;
        }
        else {
            DEBUG("_native_cc1100_handle_packet: accept packet == CC1100_PKTLEN\n");
        }
    }
    else {
        errx(EXIT_FAILURE, "_native_cc1100_handle_packet: packet length mode not supported");
    }

    /* copy packet to rx_fifo */
    /* XXX: handle overflow */
    rx_fifo_idx = 0;
    memcpy(rx_fifo, buf, size);
    status_registers[CC1100_RXBYTES - 0x30] = size;
    DEBUG("_native_cc1100_handle_packet: got %d bytes payload\n", size);

    /* toggle interrupt */
    cc110x_gdo2_irq();
}

#if 0 /* future ahead */

void step_state_idle(void)
{
    switch (addr) {
        /* configuration registers: */
        case CC1100_IOCFG2:
        case CC1100_IOCFG1:
        case CC1100_IOCFG0:
        case CC1100_FIFOTHR:
        case CC1100_SYNC1:
        case CC1100_SYNC0:
        case CC1100_PKTLEN:
        case CC1100_PKTCTRL1:
        case CC1100_PKTCTRL0:
        case CC1100_ADDR:
        case CC1100_CHANNR:
        case CC1100_FSCTRL1:
        case CC1100_FSCTRL0:
        case CC1100_FREQ2:
        case CC1100_FREQ1:
        case CC1100_FREQ0:
        case CC1100_MDMCFG4:
        case CC1100_MDMCFG3:
        case CC1100_MDMCFG2:
        case CC1100_MDMCFG1:
        case CC1100_MDMCFG0:
        case CC1100_DEVIATN:
        case CC1100_MCSM2:
        case CC1100_MCSM1:
        case CC1100_MCSM0:
        case CC1100_FOCCFG:
        case CC1100_BSCFG:
        case CC1100_AGCCTRL2:
        case CC1100_AGCCTRL1:
        case CC1100_AGCCTRL0:
        case CC1100_WOREVT1:
        case CC1100_WOREVT0:
        case CC1100_WORCTRL:
        case CC1100_FREND1:
        case CC1100_FREND0:
        case CC1100_FSCAL3:
        case CC1100_FSCAL2:
        case CC1100_FSCAL1:
        case CC1100_FSCAL0:
        case CC1100_RCCTRL1:
        case CC1100_RCCTRL0:
        case CC1100_FSTEST:
        case CC1100_PTEST:
        case CC1100_AGCTEST:
        case CC1100_TEST2:
        case CC1100_TEST1:
        case CC1100_TEST0:
        /* command strobes */
        case CC1100_SRES:
        case CC1100_SFSTXON:
        case CC1100_SXOFF:
        case CC1100_SCAL:
        case CC1100_SRX:
        case CC1100_STX:
        case CC1100_SIDLE:
        case CC1100_SAFC:
        case CC1100_SWOR:
        case CC1100_SPWD:
        case CC1100_SFRX:
        case CC1100_SFTX:
        case CC1100_SWORRST:
        case CC1100_SNOP:
        /* patable */
        case CC1100_PATABLE:
        /* tx/rx */
        case CC1100_RXFIFO: /* == CC1100_TXFIFO */
        default:
            errx(EXIT_FAILURE, "step_state_idle: unhandled addr: %d", addr);
    }
}

void step_state_sleep(void)
{
    ;
}

void step_state_xtaloff(void)
{
    ;
}

void step_state_freq_scal(void)
{
    ;
}

void step_state_freq_sst(void)
{
    ;
}

void step_state_freq_son(void)
{
    ;
}

void step_state_tx(void)
{
    ;
}

void step_state_rx(void)
{
    ;
}

void step_state_tx_ffuf(void)
{
    ;
}

void step_state_rx_ffof(void)
{
    ;
}

void step(void)
{
    switch (internal_state) {
        case (STATE_IDLE):
            step_state_idle();
            break;

        case (STATE_SLEEP):
            step_state_sleep();
            break;

        case (STATE_XTALOFF):
            step_state_xtaloff();
            break;

        case (STATE_FREQ_SCAL):
            step_state_freq_scal();
            break;

        case (STATE_FREQ_SST):
            step_state_freq_sst();
            break;

        case (STATE_FREQ_SON):
            step_state_freq_son();
            break;

        case (STATE_TX):
            step_state_tx();
            break;

        case (STATE_RX):
            step_state_rx();
            break;

        case (STATE_TX_FFUF):
            step_state_tx_ffuf();
            break;

        case (STATE_RX_FFOF):
            step_state_rx_ffof();
            break;

        default:
            errx(EXIT_FAILURE, "internal error in CC1100 state machine");
    }
}

void _native_cc1100_receive(uint8_t *buf, int len)
{
    ;
}

#endif
