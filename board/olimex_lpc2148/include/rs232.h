//rs232.h
//#include <iolpc2138.h>

#include "lpc214x.h"

//#define OSCILLATOR_CLOCK_FREQUENCY  14745600      //in MHz
#define OSCILLATOR_CLOCK_FREQUENCY  12000000      //in MHz

//get real processor clock frequency
unsigned int processorClockFrequency(void);
//get peripheral clock frequency
unsigned int peripheralClockFrequency(void);

/**** UART0 ****/
//initialize UART0 interface
void UART0Initialize(unsigned int baud);
//write char to UART0 (RS232);
void UART0WriteChar(int ch0);
//read char from RS232
unsigned char UART0ReadChar(void);

//this function read/write char from RS232,
//but they not wait to read/write
unsigned char UART0ReadChar_nostop(void);
void UART0WriteChar_nostop(unsigned char ch0);


/**** UART1 ****/
//initialize UART0 interface
void UART1Initialize(unsigned int baud);
//write char to UART0 (RS232);
void UART1WriteChar(int ch0);
//read char from RS232
unsigned char UART0ReadChar(void);

//this function read/write char from RS232,
//but they not wait to read/write
unsigned char UART1ReadChar_nostop(void);
void UART1WriteChar_nostop(unsigned char ch0);

