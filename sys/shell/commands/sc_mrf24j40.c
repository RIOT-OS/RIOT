/*
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell command to read back the values inside the MRF24J40 radio module
 *
 * @author      <neo@nenaco.de>
 *
 * @}
 */

#include <stdio.h>
#include "mrf24j40.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"
#include "mrf24j40_params.h"
#include "periph/spi.h"
#include "periph/gpio.h"

static mrf24j40_t mrf24j40_devs[1];

// temporaer hier untergebracht - sollte eigentlich von auto-init uebernommen werden
const mrf24j40_params_t *p = &mrf24j40_params[0];

uint8_t shrt_addr_reg[64];
uint8_t lng_addr_reg[77];

/**
 * @read registers
 */
void read_mrf_registers(void)
{
	uint8_t i; 	/* Schleifenzaehler fuer Short-Address-Registers*/
	uint16_t k; /* Schleifenzaehler fuer Long-Address-Registers */

	for (i = 0x0; i <= 0x3f; i++)
	{
		shrt_addr_reg[i] = mrf24j40_reg_read_short(&mrf24j40_devs[0], i);
	}

	for (k = 0x200; k <= 0x24c; k++)
	{
//		lng_addr_reg[k - 0x200] = mrf_read_long(k);
		lng_addr_reg[k - 0x200] = mrf24j40_reg_read_long(&mrf24j40_devs[0], k);
	}

}


int _cmd_print_mrf24j40_regs(int argc, char **argv)
{
    (void) argc;
    (void) argv;


    read_mrf_registers();

    printf("################## Print Registers ###################\n");

    printf("\n\rShort-Address-Registers\r\n");
    printf("RXMCR     [0x00] = %x \r\n",shrt_addr_reg[0]);
    printf("PANIDL    [0x01] = %x \r\n",shrt_addr_reg[1]);
    printf("PANIDH    [0x02] = %x \r\n",shrt_addr_reg[2]);
    printf("SADRL     [0x03] = %x \r\n",shrt_addr_reg[3]);
    printf("SADRH     [0x04] = %x \r\n",shrt_addr_reg[4]);
    printf("EADR0     [0x05] = %x \r\n",shrt_addr_reg[5]);
    printf("EADR1     [0x06] = %x \r\n",shrt_addr_reg[6]);
    printf("EADR2     [0x07] = %x \r\n",shrt_addr_reg[7]);
    printf("EADR3     [0x08] = %x \r\n",shrt_addr_reg[8]);
    printf("EADR4     [0x09] = %x \r\n",shrt_addr_reg[9]);
    printf("EADR5     [0x0a] = %x \r\n",shrt_addr_reg[0xa]);
    printf("EADR6     [0x0b] = %x \r\n",shrt_addr_reg[0xb]);
    printf("EADR7     [0x0c] = %x \r\n",shrt_addr_reg[0xc]);
    printf("RXFLUSH   [0x0d] = %x \r\n\n",shrt_addr_reg[0xd]);

    printf("ORDER     [0x10] = %x \r\n",shrt_addr_reg[0x10]);
    printf("TXMCR     [0x11] = %x \r\n",shrt_addr_reg[0x11]);
    printf("ACKTMOUT  [0x12] = %x \r\n",shrt_addr_reg[0x12]);
    printf("ESLOTG1   [0x13] = %x \r\n",shrt_addr_reg[0x13]);
    printf("SYMTICKL  [0x14] = %x \r\n",shrt_addr_reg[0x14]);
    printf("SYMTICKH  [0x15] = %x \r\n",shrt_addr_reg[0x15]);
    printf("PACON0    [0x16] = %x \r\n",shrt_addr_reg[0x16]);
    printf("PACON1    [0x17] = %x \r\n",shrt_addr_reg[0x17]);
    printf("PACON2    [0x18] = %x \r\n",shrt_addr_reg[0x18]);
    printf("TXBCON0   [0x1a] = %x \r\n",shrt_addr_reg[0x1a]);
    printf("TXNCON    [0x1b] = %x \r\n",shrt_addr_reg[0x1b]);
    printf("TXG1CON   [0x1c] = %x \r\n",shrt_addr_reg[0x1c]);
    printf("TXG2CON   [0x1d] = %x \r\n",shrt_addr_reg[0x1d]);
    printf("ESLOTG23  [0x1e] = %x \r\n",shrt_addr_reg[0x1e]);
    printf("ESLOTG45  [0x1f] = %x \r\n\n",shrt_addr_reg[0x1f]);
    printf("ESLOTG67  [0x20] = %x \r\n",shrt_addr_reg[0x20]);
    printf("TXPEND    [0x21] = %x \r\n",shrt_addr_reg[0x21]);
    printf("WAKECON   [0x22] = %x \r\n",shrt_addr_reg[0x22]);
    printf("FRMOFFSET [0x23] = %x \r\n",shrt_addr_reg[0x23]);
    printf("TXSTAT    [0x24] = %x \r\n",shrt_addr_reg[0x24]);
    printf("TXBCON1   [0x25] = %x \r\n",shrt_addr_reg[0x25]);
    printf("GATECLK   [0x26] = %x \r\n",shrt_addr_reg[0x26]);
    printf("TXTIME    [0x27] = %x \r\n",shrt_addr_reg[0x27]);
    printf("HSYMTMRL  [0x28] = %x \r\n",shrt_addr_reg[0x28]);
    printf("HSYMTMRH  [0x29] = %x \r\n",shrt_addr_reg[0x29]);
    printf("SOFTRST   [0x2a] = %x \r\n",shrt_addr_reg[0x2a]);

    printf("SECCON0   [0x2c] = %x \r\n",shrt_addr_reg[0x2c]);
    printf("SECCON1   [0x2d] = %x \r\n",shrt_addr_reg[0x2d]);
    printf("TXSTBL    [0x2e] = %x \r\n\n",shrt_addr_reg[0x2e]);

    printf("RXSR      [0x30] = %x \r\n",shrt_addr_reg[0x30]);
    printf("INTSTAT   [0x31] = %x \r\n",shrt_addr_reg[0x31]);
    printf("INTCON    [0x32] = %x \r\n",shrt_addr_reg[0x32]);
    printf("GPIO      [0x33] = %x \r\n",shrt_addr_reg[0x33]);
    printf("TRISGPIO  [0x34] = %x \r\n",shrt_addr_reg[0x34]);
    printf("SLPACK    [0x35] = %x \r\n",shrt_addr_reg[0x35]);
    printf("RFCTL     [0x36] = %x \r\n",shrt_addr_reg[0x36]);
    printf("SECCR2    [0x37] = %x \r\n",shrt_addr_reg[0x37]);
    printf("BBREG0    [0x38] = %x \r\n",shrt_addr_reg[0x38]);
    printf("BBREG1    [0x39] = %x \r\n",shrt_addr_reg[0x39]);
    printf("BBREG2    [0x3a] = %x \r\n",shrt_addr_reg[0x3a]);
    printf("BBREG3    [0x3b] = %x \r\n",shrt_addr_reg[0x3b]);
    printf("BBREG4    [0x3c] = %x \r\n",shrt_addr_reg[0x3c]);

    printf("BBREG6    [0x3e] = %x \r\n",shrt_addr_reg[0x3e]);
    printf("CCAEDTH   [0x3f] = %x \r\r\n\n",shrt_addr_reg[0x3f]);

    printf("\n\rLong-Address-Registers\r\n");
    printf("RFCON0    [0x200]  = %x \r\n",lng_addr_reg[0]);
    printf("RFCON1    [0x201] = %x \r\n",lng_addr_reg[1]);
    printf("RFCON2    [0x202] = %x \r\n",lng_addr_reg[2]);
    printf("RFCON3    [0x203] = %x \r\n",lng_addr_reg[3]);

    printf("RFCON5    [0x205] = %x \r\n",lng_addr_reg[5]);
    printf("RFCON6    [0x206] = %x \r\n",lng_addr_reg[6]);
    printf("RFCON7    [0x207] = %x \r\n",lng_addr_reg[7]);
    printf("RFCON8    [0x208] = %x \r\n",lng_addr_reg[8]);
    printf("SLPCAL0   [0x209] = %x \r\n",lng_addr_reg[9]);
    printf("SLPCAL1   [0x20a] = %x \r\n",lng_addr_reg[0xa]);
    printf("SLPCAL2   [0x20b] = %x \r\n",lng_addr_reg[0xb]);

    printf("RFSTATE   [0x20f] = %x \r\n\n",lng_addr_reg[0xf]);
    printf("RSSI      [0x210] = %x \r\n",lng_addr_reg[0x10]);
    printf("SLPCON0   [0x211] = %x \r\n\n",lng_addr_reg[0x11]);

    printf("SLPCON1   [0x220] = %x \r\n",lng_addr_reg[0x20]);

    printf("WAKETIMEL [0x222] = %x \r\n",lng_addr_reg[0x22]);
    printf("WAKETIMEH [0x223] = %x \r\n",lng_addr_reg[0x23]);
    printf("REMCNTL   [0x224] = %x \r\n",lng_addr_reg[0x24]);
    printf("REMCNTH   [0x225] = %x \r\n",lng_addr_reg[0x25]);
    printf("MAINCNT0  [0x226] = %x \r\n",lng_addr_reg[0x26]);
    printf("MAINCNT1  [0x227] = %x \r\n",lng_addr_reg[0x27]);
    printf("MAINCNT2  [0x228] = %x \r\n",lng_addr_reg[0x28]);
    printf("MAINCNT3  [0x229] = %x \r\n",lng_addr_reg[0x29]);

    printf("TESTMODE  [0x22f] = %x \r\n\n",lng_addr_reg[0x2f]);
    printf("ASSOEADR0 [0x230] = %x \r\n",lng_addr_reg[0x30]);
    printf("ASSOEADR1 [0x231] = %x \r\n",lng_addr_reg[0x31]);
    printf("ASSOEADR2 [0x232] = %x \r\n",lng_addr_reg[0x32]);
    printf("ASSOEADR3 [0x233] = %x \r\n",lng_addr_reg[0x33]);
    printf("ASSOEADR4 [0x234] = %x \r\n",lng_addr_reg[0x34]);
    printf("ASSOEADR5 [0x235] = %x \r\n",lng_addr_reg[0x35]);
    printf("ASSOEADR6 [0x236] = %x \r\n",lng_addr_reg[0x36]);
    printf("ASSOEADR7 [0x237] = %x \r\n",lng_addr_reg[0x37]);
    printf("ASSOSADR0 [0x238] = %x \r\n",lng_addr_reg[0x38]);
    printf("ASSOSADR1 [0x239] = %x \r\n\n",lng_addr_reg[0x39]);

    printf("UPNONCE0  [0x240] = %x \r\n",lng_addr_reg[0x40]);
    printf("UPNONCE1  [0x241] = %x \r\n",lng_addr_reg[0x41]);
    printf("UPNONCE2  [0x242] = %x \r\n",lng_addr_reg[0x42]);
    printf("UPNONCE3  [0x243] = %x \r\n",lng_addr_reg[0x43]);
    printf("UPNONCE4  [0x244] = %x \r\n",lng_addr_reg[0x44]);
    printf("UPNONCE5  [0x245] = %x \r\n",lng_addr_reg[0x45]);
    printf("UPNONCE6  [0x246] = %x \r\n",lng_addr_reg[0x46]);
    printf("UPNONCE7  [0x247] = %x \r\n",lng_addr_reg[0x47]);
    printf("UPNONCE8  [0x248] = %x \r\n",lng_addr_reg[0x48]);
    printf("UPNONCE9  [0x249] = %x \r\n",lng_addr_reg[0x49]);
    printf("UPNONCE10 [0x24a] = %x \r\n",lng_addr_reg[0x4a]);
    printf("UPNONCE11 [0x24b] = %x \r\n",lng_addr_reg[0x4b]);
    printf("UPNONCE12 [0x24c] = %x \r\n",lng_addr_reg[0x4c]);


    return 0;
}




