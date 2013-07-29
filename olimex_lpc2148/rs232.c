//rs232.c
#include "rs232.h"

unsigned int processorClockFrequency(void)
{
    //return real processor clock speed
    return OSCILLATOR_CLOCK_FREQUENCY * (PLL0CON & 1 ? (PLL0CFG & 0xF) + 1 : 1);
}

unsigned int peripheralClockFrequency(void)
{
    //VPBDIV - determines the relationship between the processor clock (cclk)
    //and the clock used by peripheral devices (pclk).
    unsigned int divider = 0;

    switch (VPBDIV & 3) {
        case 0:
            divider = 4;
            break;

        case 1:
            divider = 1;
            break;

        case 2:
            divider = 2;
            break;
    }

    return processorClockFrequency() / divider;
}

/**** UART0 ****/
void UART1Initialize(unsigned int baud)
{
    unsigned int divisor = peripheralClockFrequency() / (16 * baud);

    //set Line Control Register (8 bit, 1 stop bit, no parity, enable DLAB)
    //  U0LCR_bit.WLS   = 0x3;    //8 bit
    //  U0LCR_bit.SBS   = 0x0;    //1 stop bit
    //  U0LCR_bit.PE    = 0x0;    //no parity
    //  U0LCR_bit.DLAB  = 0x1;    //enable DLAB
    //with one row
    U1LCR = 0x83;


    //devisor
    U1DLL = divisor & 0xFF;
    U1DLM = (divisor >> 8) & 0xFF;
    U1LCR &= ~0x80;

    //set functionalite to pins:  port0.0 -> TX0,  port0.1 -> RXD0
    //  PINSEL0_bit.P0_0 = 0x1;
    //  PINSEL0_bit.P0_1 = 0x1;
    //with one row
    PINSEL0 |= BIT16;
    PINSEL0 &= ~BIT17;

}

void UART1WriteChar(int ch0)
{
    while (!(U1LSR & BIT5));

    U1THR = ch0;
}

unsigned char UART0ReadChar(void)
{
    //when U0LSR_bit.DR is 1 - U0RBR contains valid data
    //  while (U0LSR_bit.DR == 0);
    return U0RBR;
}


