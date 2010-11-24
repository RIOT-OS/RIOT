#include <stdlib.h>

#include <cc1100_ng.h>
#include <cc1100-arch.h>
#include <cc1100-config.h>
#include <cc1100-defaultSettings.h>
#include <cc1100-internal.h>
#include <cc1100_spi.h>

#include <transceiver.h>
#include <hwtimer.h>
#include <msg.h>

#define RX_BUF_SIZE (10)

/* some externals */
extern uint8_t pa_table[];				///< PATABLE with available output powers
extern uint8_t pa_table_index;          ///< Current PATABLE Index

/* global variables */

rx_buffer_t cc1100_rx_buffer[RX_BUF_SIZE];		    ///< RX buffer
cc1100_statistic_t cc1100_statistic;

volatile cc1100_flags rflags;		                ///< Radio control flags
volatile uint8_t radio_state = RADIO_UNKNOWN;		///< Radio state

static volatile uint8_t rx_buffer_next;	    ///< Next packet in RX queue

static uint8_t radio_address;		                ///< Radio address
static uint8_t radio_channel;		                ///< Radio channel

static int transceiver_pid;                         ///< the transceiver thread pid

/* internal function prototypes */
static uint8_t receive_packet_variable(uint8_t *rxBuffer, uint8_t length);
static uint8_t receive_packet(uint8_t *rxBuffer, uint8_t length);
static void reset(void);
static void power_up_reset(void);
static void write_register(uint8_t r, uint8_t value);

/*---------------------------------------------------------------------------*/
// 								Radio Driver API
/*---------------------------------------------------------------------------*/
void cc1100_init(int tpid) {
    transceiver_pid = tpid;

	rx_buffer_next = 0;

    /* Initialize SPI */
	cc1100_spi_init();

	/* Load driver & reset */
	power_up_reset();

    /* Write configuration to configuration registers */
    cc1100_spi_writeburst_reg(0x00, cc1100_conf, CC1100_CONF_SIZE);

	/* Write PATABLE (power settings) */
	cc1100_spi_write_reg(CC1100_PATABLE, pa_table[pa_table_index]);

	/* Initialize Radio Flags */
	rflags.RSSI         = 0x00;
	rflags.LL_ACK       = 0;
	rflags.CAA          = 0;
	rflags.CRC          = 0;
	rflags.SEQ          = 0;
	rflags.MAN_WOR      = 0;
	rflags.KT_RES_ERR   = 0;
	rflags.TX           = 0;
	rflags.WOR_RST      = 0;

	/* Set default channel number */
	radio_channel = CC1100_DEFAULT_CHANNR;
}

void cc1100_disable_interrupts(void) {
	cc1100_gdo2_disable();
	cc1100_gdo0_disable();
}

void cc1100_gdo0_irq(void) {
	// Air was not free -> Clear CCA flag
	rflags.CAA = false;
	// Disable carrier sense detection (GDO0 interrupt)
	cc1100_gdo0_disable();
}

void cc1100_gdo2_irq(void) {
	cc1100_rx_handler();
}

void cc1100_rx_handler(void) {
    uint8_t res = 0;

	// Possible packet received, RX -> IDLE (0.1 us)
	rflags.CAA      = 0;
	rflags.MAN_WOR  = 0;
	cc1100_statistic.packets_in++;

	res = receive_packet((uint8_t*)&(cc1100_rx_buffer[rx_buffer_next].packet), sizeof(cc1100_packet_t));
	if (res)
	{
        // If we are sending a burst, don't accept packets.
		// Only ACKs are processed (for stopping the burst).
		// Same if state machine is in TX lock.
		if (radio_state == RADIO_SEND_BURST || rflags.TX)
		{
			cc1100_statistic.packets_in_while_tx++;
			return;
		}
        cc1100_rx_buffer[rx_buffer_next].rssi = rflags.RSSI;
        cc1100_rx_buffer[rx_buffer_next].lqi = rflags.LQI;

		// Valid packet. After a wake-up, the radio should be in IDLE.
		// So put CC1100 to RX for WOR_TIMEOUT (have to manually put
		// the radio back to sleep/WOR).
		cc1100_spi_write_reg(CC1100_MCSM0, 0x08);	// Turn off FS-Autocal
		cc1100_spi_write_reg(CC1100_MCSM2, 0x07);	// Configure RX_TIME (until end of packet)
        cc1100_spi_strobe(CC1100_SRX);
        hwtimer_wait(IDLE_TO_RX_TIME);
        radio_state = RADIO_RX;
        
        if (++rx_buffer_next == RX_BUF_SIZE) {
            rx_buffer_next = 0;
        }
        if (transceiver_pid) {
            msg m;  
            m.type = (uint16_t) RCV_PKT;
            m.content.value = TRANSCEIVER_CC1100;
            msg_send_int(&m, transceiver_pid);
        }
        return;
    }
	else
	{
		// No ACK received so TOF is unpredictable
		rflags.TOF = 0;

		// CRC false or RX buffer full -> clear RX FIFO in both cases
		cc1100_spi_strobe(CC1100_SIDLE);	// Switch to IDLE (should already be)...
		cc1100_spi_strobe(CC1100_SFRX);		// ...for flushing the RX FIFO

		// If packet interrupted this nodes send call,
		// don't change anything after this point.
		if (radio_state == RADIO_AIR_FREE_WAITING)
		{
			cc1100_spi_strobe(CC1100_SRX);
			hwtimer_wait(IDLE_TO_RX_TIME);
			return;
		}
		// If currently sending, exit here (don't go to RX/WOR)
		if (radio_state == RADIO_SEND_BURST)
		{
			cc1100_statistic.packets_in_while_tx++;
			return;
		}

		// No valid packet, so go back to RX/WOR as soon as possible
		cc1100_switch_to_rx();
	}
}

uint8_t cc1100_get_buffer_pos(void) {
    return (rx_buffer_next-1);
}

uint8_t cc1100_set_address(radio_address_t address) {
	if ((address < MIN_UID) || (address > MAX_UID)) {
		return 0;
	}

	uint8_t id = (uint8_t) address;
	if (radio_state != RADIO_UNKNOWN) {
		write_register(CC1100_ADDR, id);
	}

	radio_address = id;
	return 0;
}

void cc1100_setup_rx_mode(void) {
	// Stay in RX mode until end of packet
	cc1100_spi_write_reg(CC1100_MCSM2, 0x07);
	cc1100_switch_to_rx();
}

void cc1100_switch_to_rx(void) {
	radio_state = RADIO_RX;
	cc1100_spi_strobe(CC1100_SRX);
}

void cc1100_wakeup_from_rx(void) {
	if (radio_state != RADIO_RX) return;
	cc1100_spi_strobe(CC1100_SIDLE);
	radio_state = RADIO_IDLE;
}

void switch_to_pwd(void) {
    cc1100_wakeup_from_rx();
	cc1100_spi_strobe(CC1100_SPWD);
	radio_state = RADIO_PWD;
}
/*---------------------------------------------------------------------------*/
/*               Internal functions                                          */
/*---------------------------------------------------------------------------*/

static uint8_t receive_packet_variable(uint8_t *rxBuffer, uint8_t length) {
	uint8_t status[2];
	uint8_t packetLength = 0;

	/* Any bytes available in RX FIFO? */
	if ((cc1100_spi_read_status(CC1100_RXBYTES) & BYTES_IN_RXFIFO))
	{
		// Read length byte (first byte in RX FIFO)
        packetLength = cc1100_spi_read_reg(CC1100_RXFIFO);
		// Read data from RX FIFO and store in rxBuffer
        if (packetLength <= length)
		{
			// Put length byte at first position in RX Buffer
			rxBuffer[0] = packetLength;

			// Read the rest of the packet
			cc1100_spi_readburst_reg(CC1100_RXFIFO, (char*)rxBuffer+1, packetLength);

            // Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI)
			cc1100_spi_readburst_reg(CC1100_RXFIFO, (char*)status, 2);

			// Store RSSI value of packet
			rflags.RSSI = status[I_RSSI];

			// MSB of LQI is the CRC_OK bit
			rflags.CRC = (status[I_LQI] & CRC_OK) >> 7;
			if (!rflags.CRC) {
                cc1100_statistic.packets_in_crc_fail++;
            }

			// Bit 0-6 of LQI indicates the link quality (LQI)
			rflags.LQI = status[I_LQI] & LQI_EST;

			return rflags.CRC;
        }
        /* too many bytes in FIFO */
		else {
			// RX FIFO get automatically flushed if return value is false
            return 0;
        }
	}
    /* no bytes in RX FIFO */
	else {
		// RX FIFO get automatically flushed if return value is false
		return 0;
	}
}

static uint8_t receive_packet(uint8_t *rxBuffer, uint8_t length) {
	uint8_t pkt_len_cfg = cc1100_spi_read_reg(CC1100_PKTCTRL0) & PKT_LENGTH_CONFIG;
	if (pkt_len_cfg == VARIABLE_PKTLEN)
	{
		return receive_packet_variable(rxBuffer, length);
	}
	// Fixed packet length not supported.
	// RX FIFO get automatically flushed if return value is false
	return 0;
}

/*---------------------------------------------------------------------------*/
// 							CC1100 reset functionality
/*---------------------------------------------------------------------------*/

static void reset(void) {
	cc1100_wakeup_from_rx();
	cc1100_spi_select();
	cc1100_spi_strobe(CC1100_SRES);
	hwtimer_wait(RTIMER_TICKS(10));
}

static void power_up_reset(void) {
	cc1100_spi_unselect();
	cc1100_spi_cs();
	cc1100_spi_unselect();
	hwtimer_wait(RESET_WAIT_TIME);
	reset();
	radio_state = RADIO_IDLE;
}
  
static void write_register(uint8_t r, uint8_t value) {
	// Save old radio state
	uint8_t old_state = radio_state;

	/* Wake up from WOR/RX (if in WOR/RX, else no effect) */
	cc1100_wakeup_from_rx();
	cc1100_spi_write_reg(r, value);

	// Have to put radio back to WOR/RX if old radio state
	// was WOR/RX, otherwise no action is necessary
	if ((old_state == RADIO_WOR) || (old_state == RADIO_RX)) {
		cc1100_switch_to_rx();
	}
}
    
static int rd_set_mode(int mode) {
	int result;

	// Get current radio mode
	if ((radio_state == RADIO_UNKNOWN) || (radio_state == RADIO_PWD)) {
		result = RADIO_MODE_OFF;
	}
	else {
		result = RADIO_MODE_ON;
	}

	switch (mode) {
		case RADIO_MODE_ON:
			cc1100_init_interrupts();			// Enable interrupts
			cc1100_setup_rx_mode();				// Set chip to desired mode
			break;
		case RADIO_MODE_OFF:
			cc1100_disable_interrupts();		// Disable interrupts
			cc1100_switch_to_pwd();					// Set chip to power down mode
			break;
		case RADIO_MODE_GET:
			// do nothing, just return current mode
		default:
			// do nothing
			break;
	}

	// Return previous mode
	return result;
}


