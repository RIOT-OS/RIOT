#include <at86rf231.h>
#include <at86rf231_arch.h>

static void at86rf231_xmit(uint8_t *data, uint8_t length);
static void at86rf231_gen_pkt(uint8_t *buf, at86rf231_packet_t *packet);

static uint8_t sequenz_nr;

int16_t at86rf231_send(at86rf231_packet_t *packet)
{
  // Set missing frame information
  packet->frame.fcf.frame_ver = 0;
  if (packet->frame.src_pan_id == packet->frame.dest_pan_id) {
      packet->frame.fcf.panid_comp = 1;
  } else {
      packet->frame.fcf.panid_comp = 0;
  }

  if(packet->frame.fcf.src_addr_m == 2) {
      packet->frame.src_addr[1] = (uint8_t)(at86rf231_get_address() >> 8);
      packet->frame.src_addr[0] = (uint8_t)(at86rf231_get_address() & 0xFF);
  } else if (packet->frame.fcf.src_addr_m == 3) {
      packet->frame.src_addr[7] = (uint8_t)(at86rf231_get_address_long() >> 56);
      packet->frame.src_addr[6] = (uint8_t)(at86rf231_get_address_long() >> 48);
      packet->frame.src_addr[5] = (uint8_t)(at86rf231_get_address_long() >> 40);
      packet->frame.src_addr[4] = (uint8_t)(at86rf231_get_address_long() >> 32);
      packet->frame.src_addr[3] = (uint8_t)(at86rf231_get_address_long() >> 24);
      packet->frame.src_addr[2] = (uint8_t)(at86rf231_get_address_long() >> 16);
      packet->frame.src_addr[1] = (uint8_t)(at86rf231_get_address_long() >> 8);
      packet->frame.src_addr[0] = (uint8_t)(at86rf231_get_address_long() & 0xFF);
  }

  packet->frame.src_pan_id = at86rf231_get_pan();
  packet->frame.seq_nr = sequenz_nr;

  sequenz_nr += 1;

  // calculate size of the frame (payload + FCS) */
  packet->length = get_802154_hdr_len(&packet->frame) + packet->frame.payload_len + 1;

  if(packet->length > AT86RF231_MAX_PKT_LENGTH) {
      return -1;
  }

  // FCS is added in hardware
  uint8_t pkt[packet->length];

  /* generate pkt */
  at86rf231_gen_pkt(pkt, packet);

  // transmit packet
  at86rf231_xmit(pkt, packet->length);
}

static void at86rf231_xmit(uint8_t *data, uint8_t length)
{
  // Go to state PLL_ON
  at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__PLL_ON);

  // wait until it is on PLL_ON state
  uint8_t status;
  uint8_t max_wait = 100;   // TODO : move elsewhere, this is in 10us

  do
  {
    status = at86rf231_get_status();

    vtimer_usleep(10);

    if (!--max_wait)
    {
      printf("at86rf231 : ERROR : could not enter PLL_ON mode");
      break;
    }
  } 
  while ((status & AT86RF231_TRX_STATUS_MASK__TRX_STATUS) != AT86RF231_TRX_STATUS__PLL_ON);

  // copy the packet to the radio FIFO
  at86rf231_write_fifo(data, length);

  // Start TX
  at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__TX_START);
}

/**
 * @brief Static function to generate byte array from at86rf231 packet.
 *
 */
static void at86rf231_gen_pkt(uint8_t *buf, at86rf231_packet_t *packet)
{
    uint8_t index, offset;
    index = init_802154_frame(&packet->frame, &buf[1]);

    // add length for at86rf231
    buf[0] = packet->length+1;
    index++;

    offset = index;

    while(index < packet->length) {
        buf[index] = packet->frame.payload[index-offset+1];
        index += 1;
    }

    at86rf231_swap_fcf_bytes(buf);
}

