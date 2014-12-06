#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
#include "thread.h"


/*----------- Transmission Settings -----------*/
/* PutData strings for datatransmission */
#define PutData0 "p,"
#define PutData1 "&n="
#define PutData2 "&s="
#define PutData3 "&v="
#define PutData4 "&t=now" 

/* CMD strings for commands */
#define cmdTestSms0 "c,t"
#define cmdTestSms1 "&n="
#define cmdTestSms2 "&t=now"

/* Node UDP Stack Buffer size */
#define udp_stack_buffer 1664
#define payload_thread_buffer 512
#define MSG_RCV_BUFFER_SIZE 64

/* Sleep definitions for transmission interval */
#define SLEEP_SECONDS 10 // 5 seconds
/*^^^^^^^^^^^ Transmission Settings ^^^^^^^^^^^*/

/*----------- ADC Weight Settings -----------*/
#define AdcResolution (65535)
#define gainCorrection (1.04888f)
#define gainFactor (95.286f)
#define microVoltPrGram (0.0000002f)
#define AdcReferenceVoltage (2.055f)
/* samples collected for averaging */
#define accumulateNum 200 
/* Delay for the RelayCtrl circuit to become ready */
#define RelayCtrlDelay 100000 // 100ms
/*^^^^^^^^^^^ ADC Weight Settings ^^^^^^^^^^^*/


extern kernel_pid_t udp_node_thread_pid;

#endif /* CONFIGURATION_H_ */