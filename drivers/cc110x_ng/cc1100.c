#include <cc1100_ng.h>
#include <cc1100-arch.h>
#include <cc1100-config.h>
#include <cc1100-defaultSettings.h>
#include <cc1100-internal.h>
#include <cc1100_spi.h>

#include <hwtimer.h>

//#define ENABLE_DEBUG    (1)
#include <debug.h>

/* some externals */
extern uint8_t pa_table[];				///< PATABLE with available output powers
extern uint8_t pa_table_index;          ///< Current PATABLE Index

/* global variables */

cc1100_statistic_t cc1100_statistic;

volatile cc1100_flags rflags;		                ///< Radio control flags
volatile uint8_t radio_state = RADIO_UNKNOWN;		///< Radio state

static uint8_t radio_address;		                ///< Radio address
static uint8_t radio_channel;		                ///< Radio channel

int transceiver_pid;                         ///< the transceiver thread pid

/* internal function prototypes */
static int rd_set_mode(int mode);
static void reset(void);
static void power_up_reset(void);
static void write_register(uint8_t r, uint8_t value);

/*---------------------------------------------------------------------------*/
// 								Radio Driver API
/*---------------------------------------------------------------------------*/
void cc1100_init(int tpid) {
    transceiver_pid = tpid;
    DEBUG("Transceiver PID: %i\n", transceiver_pid);

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
	cc1100_set_channel(CC1100_DEFAULT_CHANNR);
    DEBUG("CC1100 initialized and set to channel %i\n", radio_channel);

	// Switch to desired mode (WOR or RX)
	rd_set_mode(RADIO_MODE_ON);
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

uint8_t cc1100_get_buffer_pos(void) {
    return (rx_buffer_next-1);
}

radio_address_t cc1100_get_address() {
    return radio_address;
}

radio_address_t cc1100_set_address(radio_address_t address) {
	if ((address < MIN_UID) || (address > MAX_UID)) {
		return 0;
	}

	uint8_t id = (uint8_t) address;
	if (radio_state != RADIO_UNKNOWN) {
		write_register(CC1100_ADDR, id);
	}

	radio_address = id;
	return radio_address;
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
	if (radio_state != RADIO_RX) {
        return;
    }
    DEBUG("CC1100 going to idle\n");
	cc1100_spi_strobe(CC1100_SIDLE);
	radio_state = RADIO_IDLE;
}

char* cc1100_get_marc_state(void) {
	uint8_t state;

	// Save old radio state
	uint8_t old_state = radio_state;

	// Read content of status register
	state = cc1100_spi_read_status(CC1100_MARCSTATE) & MARC_STATE;

	// Make sure in IDLE state.
	// Only goes to IDLE if state was RX/WOR
	cc1100_wakeup_from_rx();

	// Have to put radio back to WOR/RX if old radio state
	// was WOR/RX, otherwise no action is necessary
	if (old_state == RADIO_WOR || old_state == RADIO_RX) {
		cc1100_switch_to_rx();
	}

	switch (state)
	{
		// Note: it is not possible to read back the SLEEP or XOFF state numbers
		// because setting CSn low will make the chip enter the IDLE mode from the
		// SLEEP (0) or XOFF (2) states.
		case 1: return "IDLE";
		case 3: case 4: case 5: return "MANCAL";
		case 6: case 7: return "FS_WAKEUP";
		case 8: case 12: return "CALIBRATE";
		case 9: case 10: case 11: return "SETTLING";
		case 13: case 14: case 15: return "RX";
		case 16: return "TXRX_SETTLING";
		case 17: return "RXFIFO_OVERFLOW";
		case 18: return "FSTXON";
		case 19: case 20: return "TX";
		case 21: return "RXTX_SETTLING";
		case 22: return "TXFIFO_UNDERFLOW";
		default: return "UNKNOWN";
	}
}

char* cc1100_state_to_text(uint8_t state) {
	switch (state)
	{
		case RADIO_UNKNOWN:
			return "Unknown";
		case RADIO_AIR_FREE_WAITING:
			return "CS";
		case RADIO_WOR:
			return "WOR";
		case RADIO_IDLE:
			return "IDLE";
		case RADIO_SEND_BURST:
			return "TX BURST";
		case RADIO_RX:
			return "RX";
		case RADIO_SEND_ACK:
			return "TX ACK";
		case RADIO_PWD:
			return "PWD";
		default:
			return "unknown";
	}
}


void cc1100_print_config(void) {
	printf("Current radio state:          %s\r\n", cc1100_state_to_text(radio_state));
	printf("Current MARC state:           %s\r\n", cc1100_get_marc_state());
	printf("Current channel number:       %u\r\n", radio_channel);
}

void switch_to_pwd(void) {
    cc1100_wakeup_from_rx();
	cc1100_spi_strobe(CC1100_SPWD);
	radio_state = RADIO_PWD;
}
    
/*---------------------------------------------------------------------------*/
int16_t cc1100_set_channel(uint8_t channr) {
	uint8_t state = cc1100_spi_read_status(CC1100_MARCSTATE) & MARC_STATE;
	if ((state != 1) && (channr > MAX_CHANNR)) {
        return 0;
    }
	write_register(CC1100_CHANNR, channr*10);
	radio_channel = channr;
	return radio_channel;
}

int16_t cc1100_get_channel(void) {
    return radio_channel;
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
            DEBUG("Enabling rx mode\n");
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


