#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "net/gnrc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#include "net/netdev/lora.h"
#include "net/lora.h"

#include "net/ieee802154/radio.h"

#include "event/thread.h"
#include "xtimer.h"
#include "thread.h"
#include "periph/timer.h"

#include "kernel_defines.h"
#include "event.h"

const uint8_t llcc68_max_sf = LORA_SF11;
const uint8_t sx126x_max_sf = LORA_SF12;

#include "sx126x.h"
#include "sx126x_params.h"
#include "sx126x_internal.h"

#include "checksum/ucrc16.h"


#define MAC_TIMER_CHAN_ACK  (0U)    /**< MAC timer channel for transmitting an ACK frame */
#define MAC_TIMER_CHAN_IFS  (1U)    /**< MAC timer channel for handling IFS logic */


static uint8_t sx126x_short_addr[IEEE802154_SHORT_ADDRESS_LEN];
static uint8_t sx126x_long_addr[IEEE802154_LONG_ADDRESS_LEN];
static uint16_t sx126x_pan_id;


static struct {
    bool ifs        : 1;    /**< if true, the device is currently inside the IFS period */
    bool cca_send   : 1;    /**< whether the next transmission uses CCA or not */
    bool ack_filter : 1;    /**< whether the ACK filter is activated or not */
    bool promisc    : 1;    /**< whether the device is in promiscuous mode or not */
   bool pending    : 1;    /**< whether there pending bit should be set in the ACK frame or not */
} cfg = {
    .cca_send   = true,
    .ack_filter = true,
};

static const ieee802154_radio_ops_t sx126x_ops;
static ieee802154_dev_t *_sx126x_hal_dev;


typedef enum {
    STATE_IDLE,
    STATE_TX,
    STATE_ACK,
    STATE_RX,
    STATE_CCA_CLEAR,
    STATE_CCA_BUSY,
} sx126x_state_t;

static volatile uint8_t _state;
static int _set_state(sx126x_t *dev, sx126x_state_t state);
static int _get_state(sx126x_t *dev, void* val);
#if IS_USED(MODULE_SX126X_STM32WL)



void _sx126x_handler(event_t *event)
{
    (void) event;
    ieee802154_dev_t *hal = _sx126x_hal_dev;
    sx126x_hal_task_handler(hal);
}

event_t sx126x_ev = {.handler = _sx126x_handler};

void isr_subghz_radio(void)
{
    /* Disable NVIC to avoid ISR conflict in CPU. */
    NVIC_DisableIRQ(SUBGHZ_Radio_IRQn);
    NVIC_ClearPendingIRQ(SUBGHZ_Radio_IRQn);
    event_post(EVENT_PRIO_HIGHEST, &sx126x_ev);
    cortexm_isr_end();
}
#endif 




 

void sx126x_hal_setup(sx126x_t *dev, ieee802154_dev_t *hal)
{
    hal->driver = &sx126x_ops;
    hal->priv = dev;

    _sx126x_hal_dev = hal;

}

void sx126x_setup(sx126x_t *dev, uint8_t index)
{
    (void)dev;
    (void)index;
    /* reset buffer */
}



static int _set_state(sx126x_t *dev, sx126x_state_t state)
{
    switch (state) {
    case STATE_IDLE:
        DEBUG("[sx126x] netdev: set STATE_STANDBY\n");
        sx126x_set_standby(dev, SX126X_CHIP_MODE_STBY_XOSC);
        break;

    case STATE_RX:
        DEBUG("[sx126x] netdev: set STATE_RX\n");
#if IS_USED(MODULE_SX126X_RF_SWITCH)
        /* Refer Section 4.2 RF Switch in Application Note (AN5406) */
        if (dev->params->set_rf_mode) {
            dev->params->set_rf_mode(dev, SX126X_RF_MODE_RX);
        }
#endif
        sx126x_cfg_rx_boosted(dev, true);
        int _timeout = (sx126x_symbol_to_msec(dev, dev->rx_timeout));
        if (_timeout != 0) {
            sx126x_set_rx(dev, _timeout);
        }
        else {
            sx126x_set_rx(dev, SX126X_RX_SINGLE_MODE);
        }
        break;

    case STATE_TX:
        DEBUG("[sx126x] netdev: set STATE_TX\n");
#if IS_USED(MODULE_SX126X_RF_SWITCH)
        if (dev->params->set_rf_mode) {
            dev->params->set_rf_mode(dev, dev->params->tx_pa_mode);
        }
#endif
        sx126x_set_tx(dev, 0);
        break;

    default:
        return -ENOTSUP;
    }
    return sizeof(sx126x_state_t);
}

static int _get_state(sx126x_t *dev, void *val)
{
    sx126x_chip_status_t radio_status;

    sx126x_get_status(dev, &radio_status);
    sx126x_state_t state = STATE_IDLE;

    switch (radio_status.chip_mode) {
    case SX126X_CHIP_MODE_RFU:
    case SX126X_CHIP_MODE_STBY_RC:
    case SX126X_CHIP_MODE_STBY_XOSC:
        state = STATE_IDLE;
        break;

    case SX126X_CHIP_MODE_TX:
        state = STATE_TX;
        break;

    case SX126X_CHIP_MODE_RX:
        state = STATE_RX;
        break;

    default:
        break;
    }
    memcpy(val, &state, sizeof(sx126x_state_t));
    return sizeof(sx126x_state_t);
}

void sx126x_hal_task_handler(ieee802154_dev_t *hal)
{
    sx126x_t *dev = hal->priv;
    assert(dev);
    uint8_t val;
    (void)_get_state(dev, &val);
    sx126x_irq_mask_t irq_mask;

    sx126x_get_and_clear_irq_status(dev, &irq_mask);

    if (sx126x_is_stm32wl(dev)) {
    #if IS_USED(MODULE_SX126X_STM32WL)
        NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);
    #endif  
    if (irq_mask & SX126X_IRQ_TX_DONE) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_TX_DONE\n");
        hal->cb(hal, IEEE802154_RADIO_CONFIRM_TX_DONE);
        }
    else if (irq_mask & SX126X_IRQ_RX_DONE) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_RX_DONE\n");
        hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_DONE);   
    }
    else if (irq_mask & SX126X_IRQ_PREAMBLE_DETECTED) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_PREAMBLE_DETECTED\n");
    }
    else if (irq_mask & SX126X_IRQ_SYNC_WORD_VALID) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_SYNC_WORD_VALID\n");
        
    }
    else if (irq_mask & SX126X_IRQ_HEADER_VALID) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_HEADER_VALID\n");
        hal->cb(hal, IEEE802154_RADIO_INDICATION_RX_START);
    }
    else if (irq_mask & SX126X_IRQ_HEADER_ERROR) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_HEADER_ERROR\n");
    }
    else if (irq_mask & SX126X_IRQ_CRC_ERROR) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_CRC_ERROR\n");
        hal->cb(hal, IEEE802154_RADIO_INDICATION_CRC_ERROR);
    }
    else if (irq_mask & SX126X_IRQ_CAD_DONE) {
        if (irq_mask & SX126X_IRQ_CAD_DETECTED){
            DEBUG("[sx126x] netdev: SX126X_IRQ_CAD_DETECTED \n");
            dev->cad_detected = true;
        }
        DEBUG("[sx126x] netdev: SX126X_IRQ_CAD_DONE\n");
        hal->cb(hal, IEEE802154_RADIO_CONFIRM_CCA);
        _set_state(dev, STATE_RX);
        
    }
    else if (irq_mask & SX126X_IRQ_TIMEOUT) {
        DEBUG("[sx126x] netdev: SX126X_IRQ_TIMEOUT\n");
    }
    else {
        DEBUG("[sx126x] netdev: SX126X_IRQ_NONE\n");
    }


}
}

                        /*RADIO HAL FUNCTIONS*/

static int _write(ieee802154_dev_t *hal, const iolist_t *iolist){

    sx126x_t *dev = hal->priv;
    (void)dev;
    size_t pos = 0;
    /* Full buffer used for Tx */
    sx126x_set_buffer_base_address(dev, 0x00, 0x00);
    /* Write payload buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (iol->iol_len > 0) {
            sx126x_write_buffer(dev, pos, iol->iol_base, iol->iol_len);
            DEBUG("[sx126x] netdev: send: wrote data to payload buffer.\n");
            pos += iol->iol_len;
        }
    }

    sx126x_set_lora_payload_length(dev, pos);
    DEBUG("[sx126x] writing (size: %d).\n", (pos));
    return 0;
}

static int _request_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx){
    sx126x_t *dev = hal->priv;
    (void)dev;
    (void)ctx;
    switch (op) {
        case IEEE802154_HAL_OP_TRANSMIT:

        _set_state(dev, STATE_TX);

        break;
        case IEEE802154_HAL_OP_SET_RX:

        _set_state(dev, STATE_RX);

        break;

        case IEEE802154_HAL_OP_SET_IDLE:

        _set_state(dev, STATE_IDLE);

        break;

        case IEEE802154_HAL_OP_CCA:
            DEBUG("[sx126x] netdev: HAL_OP_CCA (CAD Detection state)\n");
            dev->cad_detected = 0;
            _set_state(dev, STATE_IDLE);
            sx126x_set_cad(dev);

        break;

        default:
        assert(false);
        break;

    }
    return 0;
}

static int _confirm_op(ieee802154_dev_t *hal, ieee802154_hal_op_t op, void *ctx){
    sx126x_t *dev = hal->priv;
    (void)op;
    (void)ctx;
    ieee802154_tx_info_t *info = ctx;
    bool eagain = false;
    sx126x_state_t state;
    _get_state(dev, &state);

    
switch (op){
    case IEEE802154_HAL_OP_TRANSMIT:
       if (info) {
            info->status = (dev->cad_detected) ? TX_STATUS_MEDIUM_BUSY : TX_STATUS_SUCCESS;
        }
    while(state == STATE_TX)
        _get_state(dev, &state);
        
    break;

    case IEEE802154_HAL_OP_SET_RX:
        if(state!=STATE_RX) 
            eagain = true;
    break;

    case IEEE802154_HAL_OP_SET_IDLE:
        eagain = (state != STATE_IDLE);
        break;

    case IEEE802154_HAL_OP_CCA:
            *((bool*) ctx) = !dev->cad_detected;
    break;

    default:
        eagain = false;
        assert(false);
        break;
    
}

_state = state;

    if (eagain) {
        DEBUG("EAGAIN");
        return -EAGAIN;
    }

    return 0;
}

static int _len(ieee802154_dev_t *hal){
    (void)hal;
    sx126x_t *dev = hal->priv;
    sx126x_rx_buffer_status_t rx_buffer_status;
    sx126x_get_rx_buffer_status(dev, &rx_buffer_status);
    return rx_buffer_status.pld_len_in_bytes;
}

static int _read(ieee802154_dev_t *hal, void *buf, size_t max_size, ieee802154_rx_info_t *info)
{
    (void)hal;
    (void)buf;
    (void)info;

    sx126x_t* dev = hal->priv;
    
    uint8_t size = 0;


        /* Getting information about last received packet */
    netdev_lora_rx_info_t *packet_info = (void*)info;
    sx126x_rx_buffer_status_t rx_buffer_status;
    sx126x_pkt_status_lora_t pkt_status;
    sx126x_get_rx_buffer_status(dev, &rx_buffer_status);
    size = rx_buffer_status.pld_len_in_bytes;
    sx126x_get_lora_pkt_status(dev, &pkt_status);
    if (packet_info) {
        packet_info->snr = pkt_status.snr_pkt_in_db;
        packet_info->rssi = pkt_status.rssi_pkt_in_dbm;
    }

      /* Put PSDU to the output buffer */
    
    
    if (buf == NULL) {
        return size;
    }

    if (size > max_size) {
        return -ENOBUFS;
    }

    if (size < 3) {
        return -EBADMSG;
    }
    sx126x_read_buffer(dev, rx_buffer_status.buffer_start_pointer, (uint8_t*)buf, size);
    DEBUG("[sx126x] first 3 bytes of received packet: %d %d %d\n", *(uint8_t*)buf, *((uint8_t*)buf+1), *((uint8_t*)buf+2));
    return size;
}

static int _set_cca_threshold(ieee802154_dev_t *hal, int8_t threshold)
{
    (void)hal;
    (void)threshold;

    return 0;
}

static int _config_phy(ieee802154_dev_t *hal, const ieee802154_phy_conf_t *conf){
    (void)hal;
    (void)conf;
    sx126x_t *dev = hal->priv;
    uint8_t channel = conf->channel;
    int8_t pow = conf->pow;
    assert(channel >= 11 && channel <= 26);

    if (channel == 26) {
        sx126x_set_channel(dev, 869525000LU);
    }
    else {
        sx126x_set_channel(dev, (channel-11)*200000LU + 865500000LU);
    }

    assert(pow >= -14 && pow <= 22);

    DEBUG("FREQ = %ld, POWER = %d \n", (channel-11)*200000LU + 865500000LU, pow);
    sx126x_set_tx_params(dev, pow, SX126X_RAMP_10_US);
    return 0;
}

static int _off(ieee802154_dev_t *hal)
{
    (void)hal;
    sx126x_t *dev = hal->priv;
    sx126x_set_sleep(dev, SX126X_SLEEP_CFG_COLD_START);
    return 0;
}

static int _config_addr_filter(ieee802154_dev_t *hal, ieee802154_af_cmd_t cmd, const void *value)
{
    (void)hal;
    (void)cmd;
    (void)value;
    const uint16_t *pan_id = value;
    switch(cmd) {
        case IEEE802154_AF_SHORT_ADDR:
            memcpy(sx126x_short_addr, value, IEEE802154_SHORT_ADDRESS_LEN);
            break;
        case IEEE802154_AF_EXT_ADDR:
            memcpy(sx126x_long_addr, value, IEEE802154_LONG_ADDRESS_LEN);
            break;
        case IEEE802154_AF_PANID:
            sx126x_pan_id = *pan_id;
            break;
        case IEEE802154_AF_PAN_COORD:
            return -ENOTSUP;
    }
    return 0;
}

static int _request_on(ieee802154_dev_t *hal)
{
    (void)hal;
    sx126x_t *dev = hal->priv;
    _set_state(dev, STATE_IDLE);
    return 0;
}

static int _confirm_on(ieee802154_dev_t *hal)
{
    (void)hal;

    return -ENOTSUP;
}

static int _set_cca_mode(ieee802154_dev_t *hal, ieee802154_cca_mode_t mode)
{
    (void)hal;
    sx126x_t* dev = hal->priv;
    DEBUG("[sx126x] netdev: set_cca_mode \n");
        dev->cad_params.cad_exit_mode = SX126X_CAD_ONLY,
        dev->cad_params.cad_detect_min = 10,
        dev->cad_params.cad_detect_peak = 22,
        dev->cad_params.cad_symb_nb = SX126X_CAD_02_SYMB,
        dev->cad_params.cad_timeout = 0x000F00;
    (void)mode;

    return 0;
}

static int _config_src_addr_match(ieee802154_dev_t *hal, ieee802154_src_match_t cmd, const void *value)
{
    (void)hal;
    (void)cmd;
    (void)value;

    return -ENOTSUP;
}

static int _set_frame_filter_mode(ieee802154_dev_t *hal, ieee802154_filter_mode_t mode)
{
    (void)hal;

    bool ackf = true;
    bool _promisc = false;

    switch (mode) {
        case IEEE802154_FILTER_ACCEPT:
            DEBUG("Filter_accept_all\n");
            break;
        case IEEE802154_FILTER_PROMISC:
            _promisc = true;
            break;
        case IEEE802154_FILTER_ACK_ONLY:
            ackf = false;
            DEBUG("Filter_ack_only\n");
            break;
        default:
            return -ENOTSUP;
    }

    cfg.ack_filter = ackf;
    cfg.promisc = _promisc;

    return 0;
}

static int _set_csma_params(ieee802154_dev_t *hal, const ieee802154_csma_be_t *bd, int8_t retries)
{
    (void)hal;
    (void)bd;
    (void)retries;

    return -ENOTSUP;
}


static const ieee802154_radio_ops_t sx126x_ops = {
    .caps =  IEEE802154_CAP_24_GHZ
          | IEEE802154_CAP_IRQ_CRC_ERROR
          | IEEE802154_CAP_IRQ_RX_START
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_IRQ_CCA_DONE
          //| IEEE802154_CAP_IRQ_ACK_TIMEOUT
          | IEEE802154_CAP_PHY_OQPSK,
    .write = _write,
    .read = _read,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .len = _len,
    .off = _off,
    .request_op = _request_op,
    .confirm_op = _confirm_op,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_csma_params = _set_csma_params,
    .config_addr_filter = _config_addr_filter,
    .config_src_addr_match = _config_src_addr_match,
    .set_frame_filter_mode = _set_frame_filter_mode,
};
