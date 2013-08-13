#ifndef AT86RF231_H_
#define AT86RF231_H_

#include <stdio.h>
#include <stdint.h>

#include <ieee802154/ieee802154_frame.h>

#include <at86rf231_settings.h>

#define AT86RF231_MAX_PKT_LENGTH 127
#define AT86RF231_MAX_DATA_LENGTH 118

/**
 *  Structure to represent a at86rf231 packet.
 */
typedef struct __attribute__ ((packed)) {
	/* @{ */
  uint8_t length;  			      /** < the length of the frame of the frame including fcs*/
  ieee802154_frame_t frame;   /** < the ieee802154 frame */
  int8_t rssi;                /** < the rssi value */
  uint8_t crc;                /** < 1 if crc was successfull, 0 otherwise */
  uint8_t lqi;                /** < the link quality indicator */
  /* @} */
} at86rf231_packet_t;


void at86rf231_init(int tpid);
//void at86rf231_reset(void);
void at86rf231_rx(void);
void at86rf231_rx_handler(void);

int16_t at86rf231_send(at86rf231_packet_t *packet);

uint8_t at86rf231_set_channel(uint8_t channel);
uint8_t at86rf231_get_channel(void);

uint16_t at86rf231_set_pan(uint16_t pan);
uint16_t at86rf231_get_pan(void);

uint16_t at86rf231_set_address(uint16_t address);
uint16_t at86rf231_get_address(void);
uint64_t at86rf231_get_address_long(void);
uint64_t at86rf231_set_address_long(uint64_t address);


void at86rf231_set_monitor(uint8_t mode);



void at86rf231_swap_fcf_bytes(uint8_t *buf);

enum
{
    RF86RF231_MAX_TX_LENGTH = 125,
    RF86RF231_MAX_RX_LENGTH = 127,
    RF86RF231_MIN_CHANNEL = 11,
    RF86RF231_MAX_CHANNEL = 26
};

extern at86rf231_packet_t at86rf231_rx_buffer[AT86RF231_RX_BUF_SIZE];

#endif

