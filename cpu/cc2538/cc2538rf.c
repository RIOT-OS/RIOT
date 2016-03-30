/*
 * Copyright (C) 2015
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cc2538
 * @{
 *
 * @file
 * @brief       Implementation of public functions for CC2538RF drivers
 *
 * @author      Anon Mall <anon.mall@dai-labor.de>
 *
 * @}
 */


#include "xtimer.h"
#include "periph/cpuid.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "net/netdev2.h"
#include "core_cm3.h"
#include "cc2538rf.h"
#include "cc2538.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#define UDMA_ENABLED 0
#define CHECKSUM_LEN 2

/*Some Status Flags for the Radio */
//static uint8_t rf_flags;

/* Local RF Flags */
#define RX_ACTIVE     0x80
#define RF_MUST_RESET 0x40
#define RF_ON         0x01

//TODO check back with real value
#define _MAX_MHR_OVERHEAD   (25)

#define RESET_DELAY             (1U)        /* must be > 625ns */

cc2538rf_t* device;

static size_t _make_data_frame_hdr(cc2538rf_t *dev, uint8_t *buf,
                                   gnrc_netif_hdr_t *hdr)
{

  DEBUG("cc2538rf: making 802.15.4 header:\n");


    int pos = 0;

    /* we are building a data frame here */
    buf[0] = IEEE802154_FCF_TYPE_DATA;
    buf[1] = IEEE802154_FCF_VERS_V0;

    /* if AUTOACK is enabled, then we also expect ACKs for this packet */
    if (!(hdr->flags & GNRC_NETIF_HDR_FLAGS_BROADCAST) &&
        !(hdr->flags & GNRC_NETIF_HDR_FLAGS_MULTICAST) &&
        (dev->options & CC2538_RF_AUTOACK)) {
        DEBUG("cc23538rf: autoack enabled\n");
        buf[0] |= IEEE802154_FCF_ACK_REQ;
    }

    /* fill in destination PAN ID */
    pos = 3;
    buf[pos++] = (uint8_t)((dev->pan) & 0xff);
    buf[pos++] = (uint8_t)((dev->pan) >> 8);

    /* fill in destination address */
    if (hdr->flags &
        (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        DEBUG("cc23538rf: its a broadcast/multicast\n");
        buf[1] |= IEEE802154_FCF_DST_ADDR_SHORT;
        buf[pos++] = 0xff;
        buf[pos++] = 0xff;
    }
    else if (hdr->dst_l2addr_len == 2) {
        uint8_t *dst_addr = gnrc_netif_hdr_get_dst_addr(hdr);
        buf[1] |= IEEE802154_FCF_DST_ADDR_SHORT;
        buf[pos++] = dst_addr[1];
        buf[pos++] = dst_addr[0];
    }
    else if (hdr->dst_l2addr_len == 8) {
        buf[1] |= IEEE802154_FCF_DST_ADDR_LONG;
        uint8_t *dst_addr = gnrc_netif_hdr_get_dst_addr(hdr);
        for (int i = 7;  i >= 0; i--) {
            buf[pos++] = dst_addr[i];
        }
    }
    else {
        DEBUG("cc2538rf mac header: unsupported address length\n");
        /* unsupported address length */
        return 0;
    }

    /* fill in source PAN ID (if applicable) */
    if (dev->options & CC2538RF_OPT_USE_SRC_PAN) {
        DEBUG("cc2538rf: using src pan\n");
        buf[pos++] = (uint8_t)((dev->pan) & 0xff);
        buf[pos++] = (uint8_t)((dev->pan) >> 8);
    } else {
        buf[0] |= IEEE802154_FCF_PAN_COMP;
    }

    /* fill in source address*/
    if (dev->options & CC2538RF_OPT_SRC_ADDR_LONG) {
        buf[1] |= IEEE802154_FCF_SRC_ADDR_LONG;
        memcpy(&(buf[pos]), dev->addr_long, 8);
        pos += 8;
    }
    else {
        buf[1] |= IEEE802154_FCF_SRC_ADDR_SHORT;
        buf[pos++] = dev->addr_short[0];
        buf[pos++] = dev->addr_short[1];
    }


    /* set sequence number */
    buf[2] = dev->seq_nr++;
    /* return actual header length */
    return pos;
}

#if 0
/* TODO: generalize and move to ieee802154 */
/* TODO: include security header implications */
static size_t _get_frame_hdr_len(uint8_t *mhr)
{
    uint8_t tmp;
    size_t len = 3;

    /* figure out address sizes */
    tmp = (mhr[1] & IEEE802154_FCF_DST_ADDR_MASK);
    if (tmp == IEEE802154_FCF_DST_ADDR_SHORT) {
        len += 4;
    }
    else if (tmp == IEEE802154_FCF_DST_ADDR_LONG) {
        len += 10;
    }
    else if (tmp != IEEE802154_FCF_DST_ADDR_VOID) {
        return 0;
    }
    tmp = (mhr[1] & IEEE802154_FCF_SRC_ADDR_MASK);
    if (tmp == IEEE802154_FCF_SRC_ADDR_VOID) {
        return len;
    }
    else {
        if (!(mhr[0] & IEEE802154_FCF_PAN_COMP)) {
            len += 2;
        }
        if (tmp == IEEE802154_FCF_SRC_ADDR_SHORT) {
            return (len + 2);
        }
        else if (tmp == IEEE802154_FCF_SRC_ADDR_LONG) {
            return (len + 8);
        }
    }
    return 0;
}



/* TODO: generalize and move to (gnrc_)ieee802154 */
static gnrc_pktsnip_t *_make_netif_hdr(uint8_t *mhr)
{
    uint8_t tmp;
    uint8_t *addr;
    uint8_t src_len, dst_len;
    gnrc_pktsnip_t *snip;
    gnrc_netif_hdr_t *hdr;

    /* figure out address sizes */
    tmp = mhr[1] & IEEE802154_FCF_SRC_ADDR_MASK;
    if (tmp == IEEE802154_FCF_SRC_ADDR_SHORT) {
        src_len = 2;
    }
    else if (tmp == IEEE802154_FCF_SRC_ADDR_LONG) {
        src_len = 8;
    }
    else if (tmp == IEEE802154_FCF_SRC_ADDR_VOID) {
        src_len = 0;
    }
    else {
        return NULL;
    }
    tmp = mhr[1] & IEEE802154_FCF_DST_ADDR_MASK;
    if (tmp == IEEE802154_FCF_DST_ADDR_SHORT) {
        dst_len = 2;
    }
    else if (tmp == IEEE802154_FCF_DST_ADDR_LONG) {
        dst_len = 8;
    }
    else if (tmp == IEEE802154_FCF_DST_ADDR_VOID) {
        dst_len = 0;
    }
    else {
        return NULL;
    }
    /* allocate space for header */
    snip = gnrc_pktbuf_add(NULL, NULL, sizeof(gnrc_netif_hdr_t) + src_len + dst_len,
                           GNRC_NETTYPE_NETIF);
    if (snip == NULL) {
        return NULL;
    }
    /* fill header */
    hdr = (gnrc_netif_hdr_t *)snip->data;
    gnrc_netif_hdr_init(hdr, src_len, dst_len);
    if (dst_len > 0) {
        tmp = 5 + dst_len;
        addr = gnrc_netif_hdr_get_dst_addr(hdr);
        for (int i = 0; i < dst_len; i++) {
            addr[i] = mhr[5 + (dst_len - i) - 1];
        }
    }
    else {
        tmp = 3;
    }
    if (!(mhr[0] & IEEE802154_FCF_PAN_COMP)) {
        tmp += 2;
    }
    if (src_len > 0) {
        addr = gnrc_netif_hdr_get_src_addr(hdr);
        for (int i = 0; i < src_len; i++) {
            addr[i] = mhr[tmp + (src_len - i) - 1];
        }
    }
    return snip;
}
#endif

static int _send(gnrc_netdev_t *netdev, gnrc_pktsnip_t *pkt){


    DEBUG("cc2538rf: trying to send stuff now package:\n");

    //wait until previous transmittion is finished
    while(RFCORE_XREG_FSMSTAT1 & RFCORE_XREG_FSMSTAT1_TX_ACTIVE);

    //clearing TX FIFO
    cc2538rf_flush_tx_fifo();

    cc2538rf_t *dev = (cc2538rf_t *)netdev;
    gnrc_pktsnip_t *snip;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    memset(mhr, 0, IEEE802154_MAX_HDR_LEN);
    size_t len;


    if (pkt == NULL) {
        return -ENOMSG;
      }


    len = _make_data_frame_hdr(dev, mhr, (gnrc_netif_hdr_t *)pkt->data);
    for(int i = 0; i<len; i++){
      DEBUG("0x%x ", mhr[i]);
    }
        DEBUG("\n");


    if (dev == NULL) {
        gnrc_pktbuf_release(pkt);
        return -ENODEV;
    }

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        DEBUG("[cc2538rf]: First header was not generic netif header\n");
        gnrc_pktbuf_release(pkt);
        return -EBADMSG;
    }

    /* create 802.15.4 header */
    if (len == 0) {
        DEBUG("[cc2538rf] error: 802.15.4 header is empty\n");
        gnrc_pktbuf_release(pkt);
        return -ENOMSG;
    }


    /* check if packet (header + payload + FCS) fits into FIFO */
    snip = pkt->next;
    if ((gnrc_pkt_len(snip) + len + 2) > CC2538RF_MAX_PACKET_LEN) {
        printf("[cc2538rf] error: packet too large (%u byte) to be send\n",
               gnrc_pkt_len(snip) + len + 2);
        gnrc_pktbuf_release(pkt);
        DEBUG("cc2538rf: package overflow\n");
        return -EOVERFLOW;
    }
    DEBUG("cc2538rf: mac header len= %x, payload_len= %x\n", len, gnrc_pkt_len(snip));


    //TODO: prepare package for sending and send over fifo of cc2538rf
    DEBUG("cc2538rf: putting mac_header + payload_length+2 into RFDATA register\n");
    //RFCORE_SFR_RFDATA =  gnrc_pkt_len(snip) + len + 2;
    uint8_t package_size = gnrc_pkt_len(snip) + len + 2;
    cc2538rf_tx_load(&package_size, sizeof(package_size), 0);

    DEBUG("cc2538rf: putting stuff into the RFDATA register\n");
    cc2538rf_tx_load(mhr, len, 0);
    while(snip){
      cc2538rf_tx_load(snip->data, snip->size, 0);
      snip = snip->next;
    }

/*
    for(int i = 0; i<len; i++){
      RFCORE_SFR_RFDATA = mhr[i];
    }

    while(snip){
        for(int i = 0; i< snip->size; i++){
          RFCORE_SFR_RFDATA = ((uint8_t*)(snip->data))[i];
      }
      snip = snip->next;
    }
*/

    //read how much is in the TX FIFO
    DEBUG("cc2538rf: amount currently in the TX FIFO: %u\n", (uint8_t)RFCORE_XREG_TXFIFOCNT);

    //send transmit opcode to csp
    dev->state = NETOPT_STATE_TX;
    DEBUG("cc2538rf: trying to send over cc2538 RF DATA register\n");
    //set SET_RXENMASK_ON_TX to switch on RX after TX finished
    //RFCORE->XREG_FRMCTRL1 |= 0x01;
    cc2538rf_tx_exec();

    while(RFCORE_XREG_FSMSTAT1 & RFCORE_XREG_FSMSTAT1_TX_ACTIVE )
      DEBUG("cc2538rf: Still Sending\n");

    DEBUG("cc2538rf: sending complete\n");
    cc2538rf_flush_tx_fifo();
    dev->event_cb(NETDEV_EVENT_TX_COMPLETE, NULL);
    gnrc_pktbuf_release(pkt);
    dev->state = NETOPT_STATE_IDLE;

    return (int)len;
}


static int _add_event_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{
  DEBUG("cc2538rf: adding event cb:\n");
    if (dev == NULL) {
        return -ENODEV;
    }
    if (dev->event_cb) {
        return -ENOBUFS;
    }

    dev->event_cb = cb;
    return 0;
}


static int _rem_event_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{

  DEBUG("cc2538rf: removing event cb:\n");
  if (dev == NULL) {
      return -ENODEV;
  }
  if (dev->event_cb != cb) {
      return -ENOENT;
  }

  dev->event_cb = NULL;
  return 0;
}

//TODO: not implemented yet
static int _get(gnrc_netdev_t *device, netopt_t opt, void *val, size_t max_len)
{

  DEBUG("cc2538rf: getting options:\n");

    if(device == NULL) {
      return -ENODEV;
    }

    cc2538rf_t *dev = (cc2538rf_t *) device;

    switch (opt) {

      case NETOPT_ADDRESS:
      DEBUG("NETOPT_ADDRESS  ");
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)val) = cc2538rf_get_addr_short(dev);
            return sizeof(uint16_t);
      break;

      case NETOPT_ADDRESS_LONG:
      DEBUG("NETOPT_ADDRESS_LONG  ");
            if (max_len < sizeof(uint64_t)) {
                return -EOVERFLOW;
            }
            *((uint64_t *)val) = cc2538rf_get_addr_long(dev);
            return sizeof(uint64_t);

      break;

      case NETOPT_ADDR_LEN:
      DEBUG("NETOPT_ADDR_LEN  ");
      break;

      case NETOPT_SRC_LEN:
      DEBUG("NETOPT_SRC_LEN  ");
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            if (dev->options & CC2538RF_OPT_SRC_ADDR_LONG) {
                *((uint16_t *)val) = 8;
            }
            else {
                *((uint16_t *)val) = 2;
            }
            return sizeof(uint16_t);

      case NETOPT_NID:
      DEBUG("NETOPT_NID  ");
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)val) = dev->pan;
            return sizeof(uint16_t);

      case NETOPT_IPV6_IID:
      DEBUG("NETOPT_IPV6_IID  ");
            if (max_len < sizeof(eui64_t)) {
                return -EOVERFLOW;
            }
            if (dev->options & CC2538RF_OPT_SRC_ADDR_LONG) {
                uint64_t addr = cc2538rf_get_addr_long(dev);
                ieee802154_get_iid(val, (uint8_t *)&addr, 8);
            }
            else {
                uint16_t addr = cc2538rf_get_addr_short(dev);
                ieee802154_get_iid(val, (uint8_t *)&addr, 2);
            }
            return sizeof(eui64_t);
      break;

      case NETOPT_PROTO:
      DEBUG("NETOPT_PROTO  ");
            if (max_len < sizeof(gnrc_nettype_t)) {
                return -EOVERFLOW;
            }
            *((gnrc_nettype_t *)val) = dev->proto;
            return sizeof(gnrc_nettype_t);
      break;

      case NETOPT_CHANNEL:
      DEBUG("NETOPT_CHANNEL  ");
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            ((uint8_t *)val)[1] = 0;
            ((uint8_t *)val)[0] = cc2538rf_get_chan(dev);
            return sizeof(uint16_t);
      break;

      case NETOPT_TX_POWER:
      DEBUG("NETOPT_TX_POWER  ");
      break;

      case NETOPT_MAX_PACKET_SIZE:
      DEBUG("NETOPT_MAX_PACKET_SIZE  ");
            if (max_len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)val) = CC2538RF_MAX_PACKET_LEN - _MAX_MHR_OVERHEAD;
            return sizeof(uint16_t);
      break;

      case NETOPT_STATE:
      DEBUG("NETOPT_STATE  ");
      break;

      case NETOPT_PRELOADING:
      DEBUG("NETOPT_PRELOADING  ");
      break;

      case NETOPT_AUTOACK:
      DEBUG("NETOPT_AUTOACK  ");
      break;

      case NETOPT_RETRANS:
      DEBUG("NETOPT_RETRANS  ");
      break;

      case NETOPT_PROMISCUOUSMODE:
      DEBUG("NETOPT_PROMISCUOUSMODE  ");
      break;

      case NETOPT_RAWMODE:
      DEBUG("NETOPT_RAWMODE  ");
      break;

      case NETOPT_IS_CHANNEL_CLR:
      DEBUG("NETOPT_IS_CHANNEL_CLR  ");
      break;

      case NETOPT_RX_START_IRQ:
      DEBUG("NETOPT_RX_START_IRQ  ");
      break;

      case NETOPT_RX_END_IRQ:
      DEBUG("NETOPT_RX_END_IRQ  ");
      break;

      case NETOPT_TX_START_IRQ:
      DEBUG("NETOPT_TX_START_IRQ  ");
      break;

      case NETOPT_TX_END_IRQ:
      DEBUG("NETOPT_TX_END_IRQ  ");
      break;

      case NETOPT_CSMA:
      DEBUG("NETOPT_CSMA  ");
      break;

      case NETOPT_CSMA_RETRIES:
      DEBUG("NETOPT_CSMA_RETRIES  ");
      break;

      default:
      DEBUG("default");
      return -ENOTSUP;

    }

  return -1;
}

//TODO: not implemented yet
static int _set(gnrc_netdev_t *device, netopt_t opt, void *val, size_t len)
{
  cc2538rf_t* dev = (cc2538rf_t*) device;

  DEBUG("cc2538rf: setting options:\n");
  switch(opt){
    case NETOPT_ADDRESS_LONG:
      DEBUG("NETOPT_ADDRESS_LONG\n");
            if (len > sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            cc2538rf_set_addr_long(dev, *((uint64_t*)val));
            return sizeof(uint16_t);


    case NETOPT_ADDRESS:
      DEBUG("NETOPT_ADDRESS\n");
            if (len > sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            cc2538rf_set_addr_short(dev, *((uint16_t*)val));
            return sizeof(uint16_t);


    case NETOPT_NID:
      DEBUG("NETOPT_NID\n");
            if (len > sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            cc2538rf_set_pan(dev, *((uint16_t *)val));
            return sizeof(uint16_t);


    case NETOPT_SRC_LEN:
      DEBUG("NETOPT_SRC_LEN\n");
            if (len > sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            if (*((uint16_t *)val) == 2) {
                dev->options &= ~(CC2538RF_OPT_SRC_ADDR_LONG);
            }
            else if (*((uint16_t *)val) == 8) {
                dev->options |= CC2538RF_OPT_SRC_ADDR_LONG;
            }
            else {
                return -ENOTSUP;
            }
            return sizeof(uint16_t);

    default:
      DEBUG("DEFAULT\n");
      break;
  }

  if(device == NULL) {
    return -ENODEV;
  }


  return -1;
}

//TODO: not implemented yet
static void _isr_event(gnrc_netdev_t *device, uint32_t event_type)
{
  DEBUG("cc2538rf: isr event has been called\n");
}


const gnrc_netdev_driver_t cc2538rf_driver = {
    .send_data = _send,
    .add_event_callback = _add_event_cb,
    .rem_event_callback = _rem_event_cb,
    .get = _get,
    .set = _set,
    .isr_event = _isr_event,
};

/*
const netdev2_driver_t cc2538rf_driver_netdev2 = {
    .send = _send,
    .recv = _receive,
    .init = cc2538rf_init,
    .get = _get,
    .set = _set,
    .isr = _isr_event,
};
*/


int cc2538rf_init(cc2538rf_t *dev)
{
  DEBUG("cc2538rf: Init\n");

  //trying to reset whole rf in order not to reset the whole mote
  DEBUG("cc2538rf_init: current RXENABLE: %u \n", (uint8_t)RFCORE->XREG_RXENABLE);


  device = dev;
  dev->driver = &cc2538rf_driver;

  //setting device state to idle
  dev->idle_state = NETOPT_STATE_IDLE;
  //set seq number
  dev->seq_nr = 0;

  //setting clock for the rfcore
  SYS_CTRL->RCGCRFC = 1;
  SYS_CTRL->SCGCRFC = 1;
  SYS_CTRL->DCGCRFC = 1;

  //setting up calibration register (check with Register Settings Update from docu)
  RFCORE->XREG_TXFILTCFG = 0x09; /* TX anti-aliasing filter */
  RFCORE->XREG_AGCCTRL1 = 0x15;  /* AGC target value */
  RFCORE->XREG_FSCAL1 = 0x01;    /* Reduce the VCO leakage */
  ANA_REGS_IVCTRL = 0x0B;        /* Controls bias current */

  //setting cca threshold
  RFCORE->XREG_CCACTRL0 = CC2538RF_CCA_THRES;


  //setting up addresses
  uint64_t longAddress;
  cpuid_get((void*)&longAddress);
  cc2538rf_set_addr_long(dev, longAddress);


#if ENABLE_DEBUG
  uint8_t* longToShortAddress = (uint8_t*) &longAddress;
  DEBUG("cc2538rf: long address from flash: \n");
  for(int i = 0; i<8; i++){
    DEBUG("%x:", longToShortAddress[i]);
  }
  DEBUG("\n");

  DEBUG("cc2538rf: current long address:\n");
  for(int i = 0; i<8; i++){
    DEBUG("%x:", dev->addr_long[i]);
  }
  DEBUG("\n");
#endif

  uint16_t addr_short = (dev->addr_long[6]<<8) | dev->addr_long[7];
  cc2538rf_set_addr_short(dev, addr_short);

  DEBUG("cc2538rf: short address:\n");
  for(int i = 0; i<2; i++){
    DEBUG("%x:", dev->addr_short[i]);
  }
  DEBUG("\n");

  //setting pan id
  DEBUG("cc2538rf: setting pan id: %u\n", CC2538RF_DEFAULT_PANID);
  cc2538rf_set_pan(dev, CC2538RF_DEFAULT_PANID);

  //setting channel
  DEBUG("cc2538rf: setting channel: %u\n", CC2538RF_DEFAULT_CHANNEL);
  cc2538rf_set_chan(dev, CC2538RF_DEFAULT_CHANNEL);

#ifdef MODULE_GNRC_SIXLOWPAN
  DEBUG("cc2538rf: 6lowpan enabled \n");
  dev->proto = GNRC_NETTYPE_SIXLOWPAN;
#else
  dev->proto = GNRC_NETTYPE_UNDEF;
#endif

//set device options to use pan
dev->options |= CC2538RF_OPT_USE_SRC_PAN;
//set device options to use long address
dev->options |= CC2538RF_OPT_SRC_ADDR_LONG;


#ifdef MODULE_GNRC_SIXLOWPAN
    dev->proto = GNRC_NETTYPE_SIXLOWPAN;
#else
    dev->proto = GNRC_NETTYPE_UNDEF;
#endif





  DEBUG("cc2538rf_init: enabling interrupts\n");
  //activating alternate Vector map
  //TODO: doesn`t work, find out why
  SYS_CTRL->I_MAP = 0x1;
  DEBUG("cc2538rf_init: alternate interrupt table flag: %u\n", (uint8_t) SYS_CTRL_I_MAP);
  //enable fifop interrupts
  RFCORE->XREG_RFIRQM0 |= CC2538RF_IRQMASK_FIFOP;
  //enable interrupt when tx is done
  RFCORE->XREG_RFIRQM1 |= CC2538RF_IRQMASK_TXACKDONE;
  RFCORE->XREG_RFIRQM1 |= CC2538RF_IRQMASK_TXDONE;
  //enable rx pkt done
  RFCORE->XREG_RFIRQM0 |= CC2538RF_IRQMASK_RXPKTDONE;


  //enable interrupt in table for TX/RX
  NVIC_EnableIRQ(RF_RXTX_IRQn);

  /* Acknowledge all RF Error interrupts */
  RFCORE->XREG_RFERRM = CC2538RF_RFERRM_RFERRM;
  //enable interrupt in table for TX/RX Error
  NVIC_EnableIRQ(RF_ERR_IRQn);



  //setting up AUTOCRC
  RFCORE->XREG_FRMCTRL0 |= RFCORE_XREG_FRMCTRL0_AUTOCRC;

  //disable source matching
  RFCORE->XREG_SRCMATCH = 0;

  //Setup max fifo threshold
  RFCORE->XREG_FIFOPCTRL = CC2538RF_MAX_PACKET_LEN;

  //disable frame filtering
  RFCORE->XREG_FRMFILT0 &= ~0x01;

  //rf_flags |= RF_ON;
  RFCORE->XREG_TXPOWER = CC2538RF_TX_POWER;
  dev->state = NETOPT_STATE_IDLE;

  //put RF into RX mode
  RFCORE_SFR_RFST = CC2538_RF_CSP_OP_ISRXON;

  return 0;
}

void cc2538rf_reset(cc2538rf_t *dev)
{
    //TODO: Reset without losing all the address informations
    cc2538rf_init(dev);
}

bool cc2538rf_cca(cc2538rf_t *dev)
{
  DEBUG("cc2538rf_cca(): reset not implemented yet.\n");
  return false;
}

//Unessecary due to _send function
size_t cc2538rf_send(cc2538rf_t *dev, uint8_t *data, size_t len)
{
  DEBUG("cc2538rf_send(): reset not implemented yet.\n");
  return -1;
}

//Unessecary due to _send function
void cc2538rf_tx_prepare(cc2538rf_t *dev)
{
  DEBUG("cc2538rf_tx_prepare(): reset not implemented yet.\n");
}

size_t cc2538rf_tx_load(uint8_t *data,
                         size_t len, size_t offset)
{
  for(int i = 0+offset; i < len+offset; i++){
      RFCORE_SFR_RFDATA = data[i];
  }
  return 0;
}

void cc2538rf_tx_exec(void)
{
  RFCORE->SFR_RFST = CC2538_RF_CSP_OP_ISTXON;
}

size_t cc2538rf_rx_len(cc2538rf_t *dev)
{
  DEBUG("cc2538rf_rx_len(): reset not implemented yet.\n");
  return 0;
}

void cc2538rf_flush_tx_fifo(void)
{
    RFCORE->SFR_RFST = CC2538_RF_CSP_OP_ISFLUSHTX;
    RFCORE->SFR_RFST = CC2538_RF_CSP_OP_ISFLUSHTX;
}

void cc2538rf_flush_rx_fifo(void)
{
    RFCORE->SFR_RFST = CC2538_RF_CSP_OP_ISFLUSHRX;
    RFCORE->SFR_RFST = CC2538_RF_CSP_OP_ISFLUSHRX;
}

/* TODO: generalize and move to (gnrc_)ieee802154 */
static gnrc_pktsnip_t *_make_netif_hdr(uint8_t *mhr)
{
    uint8_t tmp;
    uint8_t *addr;
    uint8_t src_len, dst_len;
    gnrc_pktsnip_t *snip;
    gnrc_netif_hdr_t *hdr;

    /* figure out address sizes */
    tmp = mhr[1] & IEEE802154_FCF_SRC_ADDR_MASK;
    if (tmp == IEEE802154_FCF_SRC_ADDR_SHORT) {
        src_len = 2;
    }
    else if (tmp == IEEE802154_FCF_SRC_ADDR_LONG) {
        src_len = 8;
    }
    else if (tmp == IEEE802154_FCF_SRC_ADDR_VOID) {
        src_len = 0;
    }
    else {
        return NULL;
    }
    tmp = mhr[1] & IEEE802154_FCF_DST_ADDR_MASK;
    if (tmp == IEEE802154_FCF_DST_ADDR_SHORT) {
        dst_len = 2;
    }
    else if (tmp == IEEE802154_FCF_DST_ADDR_LONG) {
        dst_len = 8;
    }
    else if (tmp == IEEE802154_FCF_DST_ADDR_VOID) {
        dst_len = 0;
    }
    else {
        return NULL;
    }
    /* allocate space for header */
    snip = gnrc_pktbuf_add(NULL, NULL, sizeof(gnrc_netif_hdr_t) + src_len + dst_len,
                           GNRC_NETTYPE_NETIF);
    if (snip == NULL) {
        return NULL;
    }
    /* fill header */
    hdr = (gnrc_netif_hdr_t *)snip->data;
    gnrc_netif_hdr_init(hdr, src_len, dst_len);
    if (dst_len > 0) {
        tmp = 5 + dst_len;
        addr = gnrc_netif_hdr_get_dst_addr(hdr);
        for (int i = 0; i < dst_len; i++) {
            addr[i] = mhr[5 + (dst_len - i) - 1];
        }
    }
    else {
        tmp = 3;
    }
    if (!(mhr[0] & IEEE802154_FCF_PAN_COMP)) {
        tmp += 2;
    }
    if (src_len > 0) {
        addr = gnrc_netif_hdr_get_src_addr(hdr);
        for (int i = 0; i < src_len; i++) {
            addr[i] = mhr[tmp + (src_len - i) - 1];
        }
    }
    return snip;
}

/* TODO: generalize and move to ieee802154 */
/* TODO: include security header implications */
static size_t _get_frame_hdr_len(uint8_t *mhr)
{
    uint8_t tmp;
    size_t len = 3;

    /* figure out address sizes */
    tmp = (mhr[1] & IEEE802154_FCF_DST_ADDR_MASK);
    if (tmp == IEEE802154_FCF_DST_ADDR_SHORT) {
        len += 4;
    }
    else if (tmp == IEEE802154_FCF_DST_ADDR_LONG) {
        len += 10;
    }
    else if (tmp != IEEE802154_FCF_DST_ADDR_VOID) {
        return 0;
    }
    tmp = (mhr[1] & IEEE802154_FCF_SRC_ADDR_MASK);
    if (tmp == IEEE802154_FCF_SRC_ADDR_VOID) {
        return len;
    }
    else {
        if (!(mhr[0] & IEEE802154_FCF_PAN_COMP)) {
            len += 2;
        }
        if (tmp == IEEE802154_FCF_SRC_ADDR_SHORT) {
            return (len + 2);
        }
        else if (tmp == IEEE802154_FCF_SRC_ADDR_LONG) {
            return (len + 8);
        }
    }
    return 0;
}

void cc2538rf_rx_read(void)
{


  if((RFCORE->XREG_RXFIFOCNT) == 0){
    DEBUG("cc2538rf_rx_read: nothing in fifo. Should not happen\n");
    return;
  }

  if(RFCORE->XREG_RXFIRST == 0){
    DEBUG("cc2538rf_rx_read: package length is 0. Frame is invalid. Flushing FIFO\n");
    cc2538rf_flush_rx_fifo();
    return;
  }

  uint8_t mhr[IEEE802154_MAX_HDR_LEN];
  size_t pkt_len, hdr_len;
  gnrc_pktsnip_t *hdr, *payload = NULL;
  gnrc_netif_hdr_t *netif;
  int8_t rssi = 0;
  uint8_t lqi = 0;

  //packet length is the len - 2, as the FCS is attached at the end
  pkt_len = (uint8_t) RFCORE->SFR_RFDATA;

  /* abort here already if no event callback is registered */
  if (!device->event_cb) {
      return;
  }

  /* in raw mode, just read the binary dump into the packet buffer */
  if (device->options & NETOPT_RAWMODE) {
      DEBUG("cc2538rf_rx_read: sending to stack in RAW mode\n");
      payload = gnrc_pktbuf_add(NULL, NULL, pkt_len, GNRC_NETTYPE_UNDEF);
      if (payload == NULL ) {
          DEBUG("cc2538rf_rx_read: unable to allocate RAW data\n");
          return;
      }
      for(int i=0; i<pkt_len; i++){
        *(((uint8_t*)payload->data) + i) = (uint8_t) RFCORE->SFR_RFDATA;
      }
      device->event_cb(NETDEV_EVENT_RX_COMPLETE, payload);
      return;
  }

  /* get FCF field and compute 802.15.4 header length */
  mhr[0]=(uint8_t) RFCORE->SFR_RFDATA;
  mhr[1]=(uint8_t) RFCORE->SFR_RFDATA;
  hdr_len = _get_frame_hdr_len(mhr);
  if (hdr_len == 0) {
      DEBUG("cc2538rf_rx_read: unable parse incoming frame header\n");
      return;
  }



  /* read the rest of the header and parse the netif header from it */
  DEBUG("cc2538rf_rx_read: header with len 0x%x:\n", hdr_len);
  DEBUG("0x%x 0x%x ", mhr[0], mhr[1]);
  for(int i=2; i<hdr_len; i++){
    mhr[i]=(uint8_t) RFCORE->SFR_RFDATA;
    DEBUG("0x%x ", mhr[i]);
  }
  DEBUG("\n");
  hdr = _make_netif_hdr(mhr);
  if (hdr == NULL) {
      DEBUG("cc2538rf_rx_read: unable to allocate netif header\n");
      return;
  }

size_t dataLen = pkt_len - hdr_len - 2;
uint8_t frameData[dataLen];

  DEBUG("cc2538_rx_read: Reading frame data:\n");
  /* copy payload */
  for(int i=0; i<dataLen; i++){
    frameData[i] = (uint8_t) RFCORE->SFR_RFDATA;
    DEBUG("0x%x ", frameData[i]);
  }
  DEBUG("\n");


  //get the RSSI and LQI
  rssi = ((uint8_t) RFCORE->SFR_RFDATA) - RSSI_OFFSET;
  lqi = ((uint8_t) RFCORE->SFR_RFDATA)&0x7F;

  /* fill missing fields in netif header */
  netif = (gnrc_netif_hdr_t *)hdr->data;
  netif->if_pid = device->mac_pid;
  DEBUG("cc2538rf_rx_read: device->mac_pid = %i\n", device->mac_pid);
  netif->lqi = lqi;
  netif->rssi = rssi;


  DEBUG("cc2538rf_rx_read: header information:\n");
  DEBUG("src addr len: 0x%x\n", netif->src_l2addr_len);
  DEBUG("dst addr len: 0x%x\n", netif->dst_l2addr_len);

  /* allocate payload */
  DEBUG("cc2538rf_rx_read: device proto is: 0x%x\n", device->proto);
  payload = gnrc_pktbuf_add(hdr, NULL, dataLen, device->proto);
  if (payload == NULL) {
      DEBUG("cc2538rf_rx_read: unable to allocate incoming payload\n");
      gnrc_pktbuf_release(hdr);
      return;
  }

  uint8_t* payloadData = (uint8_t*) payload->data;
  for(int i=0; i<dataLen; i++){
    payloadData[i] = frameData[i];
  }
  /* finish up and send data to upper layers */
  device->event_cb(NETDEV_EVENT_RX_COMPLETE, payload);
}


uint8_t cc2538rf_get_chan(cc2538rf_t *dev)
{
  return dev->chan;
}


void cc2538rf_set_chan(cc2538rf_t *dev, uint8_t chan)
{
  uint8_t regChannel = 11+(5*(chan-11));
  RFCORE_XREG_FREQCTRL = regChannel;
  dev->chan = chan;

}

void cc2538rf_set_addr_long(cc2538rf_t *dev, uint64_t addr)
{
  cc2538_rfcore_t* rfcore = RFCORE;

  uint8_t *addr_cut = (uint8_t*) &addr;
  for(int i = 0; i<8; i++){
    //dev->addr_long[i] = (addr >> ((7-i)*8));
    dev->addr_long[i] = addr_cut[i];
  }

  rfcore->FFSM_EXT_ADDR0 = addr_cut[7];
  rfcore->FFSM_EXT_ADDR1 = addr_cut[6];
  rfcore->FFSM_EXT_ADDR2 = addr_cut[5];
  rfcore->FFSM_EXT_ADDR3 = addr_cut[4];
  rfcore->FFSM_EXT_ADDR4 = addr_cut[3];
  rfcore->FFSM_EXT_ADDR5 = addr_cut[2];
  rfcore->FFSM_EXT_ADDR6 = addr_cut[1];
  rfcore->FFSM_EXT_ADDR7 = addr_cut[0];

}


uint64_t cc2538rf_get_addr_long(cc2538rf_t *dev)
{
  uint64_t longAddr = 0x0;
  for(int i=0; i<8; i++){
    longAddr = (longAddr << 8) | dev->addr_long[i];
  }

  return longAddr;
}


void cc2538rf_set_addr_short(cc2538rf_t *dev, uint16_t addr)
{
    uint8_t *addr_cut = (uint8_t*) &addr;
    dev->addr_short[0] = addr_cut[0];
    dev->addr_short[1] = addr_cut[1];
    cc2538_rfcore_t* rfcore = RFCORE;
    rfcore->FFSM_SHORT_ADDR0 = addr_cut[1];
    rfcore->FFSM_SHORT_ADDR1 = addr_cut[0];
}


uint16_t cc2538rf_get_addr_short(cc2538rf_t *dev)
{
  uint16_t shortAddr = (dev->addr_short[1] << 8) | dev->addr_short[0];
  return shortAddr;
}


void cc2538rf_set_pan(cc2538rf_t *dev, uint16_t pan)
{
    RFCORE_FFSM_PAN_ID0 = (uint8_t) pan;
    RFCORE_FFSM_PAN_ID1 = (uint8_t) pan>>8;
    dev->pan = pan;

}


uint16_t cc2538rf_get_pan(cc2538rf_t *dev)
{
  return dev->pan;
}

//Interrupt Handler for RFCORE Interrupts
void handle_rfcoretxrx_isr(void)
{

  uint8_t irq0 = (uint8_t) RFCORE_SFR_RFIRQF0;
  uint8_t irq1 = (uint8_t) RFCORE_SFR_RFIRQF1;
  uint8_t fsmstat1 = (uint8_t) RFCORE->XREG_FSMSTAT1;
#if ENABLE_DEBUG
  DEBUG("cc2538rf_rfcoreinterrupt: RFIRQF0: 0x%x\n", irq0);
  DEBUG("cc2538rf_rfcoreinterrupt: RFIRQF1: 0x%x\n", irq1);
  DEBUG("cc2538rf_rfcoreinterrupt: FSMSTAT1: 0x%x\n", fsmstat1);
#endif

  //check fifo status
  if(fsmstat1 & 0x40)
  {
    DEBUG("cc2538rf_rfcoreinterrupt: FIFO FSMSTAT1\n");
  }

  //parse for flags
  if(irq1 & CC2538RF_IRQFLAG_TXDONE || irq1 & CC2538RF_IRQFLAG_TXACKDONE)
  {
    DEBUG("cc2538rf_rfcoreinterrupt: TX DONE\n");
    cc2538rf_flush_rx_fifo();
    //RFCORE->SFR_RFIRQF1 &= ~CC2538RF_IRQFLAG_TXDONE;
    //RFCORE->SFR_RFIRQF1 &= ~CC2538RF_IRQFLAG_TXACKDONE;
  }


  if(irq0 & CC2538RF_IRQFLAG_RXPKTDONE || irq0 & CC2538RF_IRQFLAG_FIFOP){
    DEBUG("cc2538rf_rfcoreinterrupt: Reading RX package\n");
#if ENABLE_DEBUG
    uint8_t rxfifocnt = (uint8_t)RFCORE->XREG_RXFIFOCNT;
#endif
    DEBUG("cc2538rf_rfcoreinterrupt: Current RXFIFOCNT: %u\n", rxfifocnt);

    cc2538rf_rx_read();
#if ENABLE_DEBUG
    rxfifocnt = (uint8_t)RFCORE->XREG_RXFIFOCNT;
#endif
    DEBUG("cc2538rf_rfcoreinterrupt: Current RXFIFOCNT after read: %u\n", rxfifocnt);
    /*
    DEBUG("cc2538rf_rfcoreinterrupt: flushing RX fifo\n");
    RFCORE_SFR_RFST = CC2538_RF_CSP_OP_ISFLUSHRX;
    RFCORE_SFR_RFST = CC2538_RF_CSP_OP_ISFLUSHRX;
#if ENABLE_DEBUG
    rxfifocnt = (uint8_t)RFCORE->XREG_RXFIFOCNT;
#endif
    DEBUG("cc2538rf_rfcoreinterrupt: Current RXFIFOCNT after flush: %u\n", rxfifocnt);
    */
    //RFCORE->SFR_RFIRQF0 &= ~CC2538RF_IRQFLAG_RXPKTDONE;
    //RFCORE->SFR_RFIRQF0 &= ~CC2538RF_IRQFLAG_FIFOP;
  }


  if(irq0 & CC2538RF_IRQFLAG_RXMASKZERO)
  {
    DEBUG("cc2538rf_rfcoreinterrupt: RX Disabled\n");
    //RFCORE_SFR_RFST = CC2538_RF_CSP_OP_ISRXON;
#if ENABLE_DEBUG
    uint8_t rxfifocnt = (uint8_t)RFCORE->XREG_RXFIFOCNT;
#endif
    DEBUG("cc2538rf_rfcoreinterrupt: Current RXFIFOCNT: %u\n", rxfifocnt);
    if(!(RFCORE_XREG_FSMSTAT1 & RFCORE_XREG_FSMSTAT1_TX_ACTIVE)){
      RFCORE->SFR_RFST = CC2538_RF_CSP_OP_ISRXON;
    }
    //RFCORE->SFR_RFIRQF1 &= ~CC2538RF_IRQFLAG_RXMASKZERO;
  }
  RFCORE->SFR_RFIRQF1 = 0;
  RFCORE->SFR_RFIRQF0 = 0;

  DEBUG("\n");
}

//Entrypoint for RFCORE Interrupts on Vector 42
void isr_rfcoretx(void)
{
  DEBUG("\n\ncc2538rf: GOT INTERRUPT RFCORE\n");
  handle_rfcoretxrx_isr();
}

//Entrypoint for Alternative RFCORE Interrupts on Vector 157
void isr_rfcoretx_alternate(void)
{
  DEBUG("\n\ncc2538rf: GOT INTERRUPT RFCORE ALTERNATE\n");
  handle_rfcoretxrx_isr();
}


void isr_ssi1(void)
{
  DEBUG("\n\ncc2538rf: GOT INTERRUPT isr_ssil\n");
}


void isr_rfcoreerr(void)
{
  DEBUG("\n\ncc2538rf: GOT INTERRUPT ERROR\n");
  uint8_t rferror = (uint8_t)RFCORE->SFR_RFERRF;
  if(rferror){
    DEBUG("cc2538rf: RF ERROR: 0x%x\n\n", rferror);
    //DEBUG("cc2538rf: Resetting RF\n");

    //Resetting Radio
    //Wait for TX to end
    while(RFCORE->XREG_FSMSTAT1 & RFCORE_XREG_FSMSTAT1_TX_ACTIVE);

    //Flush TX FIFO
    cc2538rf_flush_tx_fifo();

    //Flush RX FIFO
    cc2538rf_flush_rx_fifo();

    //Checking if Radio is already off, otherwise we get a Strobe Error
    if(RFCORE_XREG_RXENABLE != 0) {
      RFCORE_SFR_RFST = CC2538_RF_CSP_OP_ISRFOFF;
    }

    RFCORE->SFR_RFERRF = (uint8_t) 0x0;
    RFCORE_SFR_RFST = CC2538_RF_CSP_OP_ISRXON;

    //cc2538rf_init(device);
  }

}
