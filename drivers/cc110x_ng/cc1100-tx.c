#include <stdio.h>

#include <cc1100_ng.h>
#include <cc1100-defaultSettings.h>
#include <cc1100-internal.h>
#include <cc1100-arch.h>
#include <cc1100_spi.h>

#include <irq.h>

#include <board.h>

uint8_t cc1100_send(cc1100_packet_t *packet) {
    puts("Going to send\n");
	volatile uint32_t abort_count;
    uint8_t size;
    /* TODO: burst sending */
	radio_state = RADIO_SEND_BURST;
	rflags.LL_ACK = 0;

    /*
     * Number of bytes to send is:
     * length of phy payload (packet->length)
     * + size of length field (1 byte)
     */
    size = packet->length + 1;

	// The number of bytes to be transmitted must be smaller
	// or equal to PACKET_LENGTH (62 bytes). So the receiver
	// can put the whole packet in its RX-FIFO (with appended
	// packet status bytes).
	if (size > PACKET_LENGTH) {
        return 0;
    }

    packet->phy_src = cc1100_get_address();

	// Disables RX interrupt etc.
	cc1100_before_send();

	// But CC1100 in IDLE mode to flush the FIFO
    cc1100_spi_strobe(CC1100_SIDLE);
    // Flush TX FIFO to be sure it is empty
    cc1100_spi_strobe(CC1100_SFTX);
	// Write packet into TX FIFO
    cc1100_spi_writeburst_reg(CC1100_TXFIFO, (char*) packet, size);
  	// Switch to TX mode
    abort_count = 0;
    unsigned int cpsr = disableIRQ();
    cc1100_spi_strobe(CC1100_STX);
    // Wait for GDO2 to be set -> sync word transmitted
    while (cc1100_get_gdo2() == 0) {
    	abort_count++;
    	if (abort_count > CC1100_SYNC_WORD_TX_TIME) {
    		// Abort waiting. CC1100 maybe in wrong mode
    		// e.g. sending preambles for always
    		puts("[CC1100 TX] fatal error\n");
    		break;
    	}
    }
    restoreIRQ(cpsr);
	// Wait for GDO2 to be cleared -> end of packet
    while (cc1100_get_gdo2() != 0);

    // Experimental - TOF Measurement
    cc1100_after_send();
    cc1100_statistic.raw_packets_out++;

	// Store number of transmission retries
	rflags.TX = 0;

	// Go to mode after TX (CONST_RX -> RX, WOR -> WOR)
	cc1100_switch_to_rx();

	return true;
}

