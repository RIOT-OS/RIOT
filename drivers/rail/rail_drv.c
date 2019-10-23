/*
 * Copyright (C) 2018 Hochschule RheinMain
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>
#include <inttypes.h>
#include <assert.h>

#include "byteorder.h"

#include "rail_radio.h"

/* gecko sdk rail lib includes*/
#include "em_core.h"
#include "rail.h"
#include "rail_chip_specific.h"
#include "pa_conversions_efr32.h"
#include "rail_assert_error_codes.h"
#include "ieee802154/rail_ieee802154.h"

/* riot os rail driver includes*/
#include "rail_drv.h"
#include "rail_netdev.h"
#include "rail_802154_config.h"

#include "net/ieee802154.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "log.h"

/*  RADIO CONFIGURATION */
/* channel config for sub GHz radio
 */
#if (RAIL_RADIO_BAND == 868) || (RAIL_RADIO_BAND == 915)

#if (RAIL_RADIO_BAND == 868)
static RAIL_ChannelConfigEntryAttr_t radio_channel_entry_868;
#elif (RAIL_RADIO_BAND == 915)
static RAIL_ChannelConfigEntryAttr_t radio_channel_entry_915;
#endif

static const RAIL_ChannelConfigEntry_t radio_channel_entry[] = {
#if (RAIL_RADIO_BAND == 868)
    { .phyConfigDeltaAdd = NULL, /* Add this to default config for this entry */
      .baseFrequency = 868300000U,
      .channelSpacing = 600000U,
      .physicalChannelOffset = 0,
      .channelNumberStart = 0,
      .channelNumberEnd = 0,
      .maxPower = RAIL_TX_POWER_MAX,
      .attr = &radio_channel_entry_868 },
#elif (RAIL_RADIO_BAND == 915)
    { .phyConfigDeltaAdd = NULL, /* Add this to default config for this entry */
      .baseFrequency = 906000000U,
      .channelSpacing = 2000000U,
      .physicalChannelOffset = 1,
      .channelNumberStart = 1,
      .channelNumberEnd = 10,
      .maxPower = RAIL_TX_POWER_MAX,
      .attr = &radio_channel_entry_915 }
#endif
};

#endif

/* IEEE 802.15.4 config for 868MHz and 912MHz */
#if (RAIL_RADIO_BAND == 868)

static const uint32_t ieee802154_config_863[] = RAIL_IEEE802154_CONFIG_868MHZ;
static const uint32_t ieee802154_config_863_min[] = {
    0xFFFFFFFFUL,
};

static const RAIL_ChannelConfig_t _rail_radio_channel_config = {
    .phyConfigBase = ieee802154_config_863,
    .phyConfigDeltaSubtract = ieee802154_config_863_min,
    .configs = &radio_channel_entry[0],
    .length = 1
};
#elif (RAIL_RADIO_BAND == 915)

static const uint32_t ieee802154_config_863[] = RAIL_IEEE802154_CONFIG_915MHZ;
static const uint32_t ieee802154_config_915_min[] = {
    0xFFFFFFFFUL,
};

static const RAIL_ChannelConfig_t _rail_radio_channel_config = {
    .phyConfigBase = ieee802154_config_915,
    .phyConfigDeltaSubtract = ieee802154_config_915_min,
    .configs = &radio_channel_entry[0],
    .length = 1
};
#endif

static const RAIL_IEEE802154_Config_t _rail_ieee802154_config = {
    .addresses = NULL,
    .ackConfig = {
        .enable = true,                     /* Turn on auto ACK for IEEE 802.15.4 */
        .ackTimeout = 864,                 /*  54 symbols * 16 us/symbol = 864 us */
        .rxTransitions = {
            .success = RAIL_RF_STATE_RX,    /* after rx -> state rx */
            .error = RAIL_RF_STATE_RX       /* ignored */
        },
        .txTransitions = {
            .success = RAIL_RF_STATE_RX,    /* after tx -> state rx */
            .error = RAIL_RF_STATE_RX       /* ignored */
        }
    },
    .timings = { .idleToRx = 100,
                 /* Make txToRx slightly lower than desired to make sure we get to
                    RX in time
                  */
                 .txToRx = 192 - 10,
                 .idleToTx = 100,
                 .rxToTx = 192,
                 .rxSearchTimeout = 0,
                 .txToRxSearchTimeout = 0 },
    .framesMask = RAIL_IEEE802154_ACCEPT_STANDARD_FRAMES,
    .promiscuousMode = false,
    .isPanCoordinator = false
};

static const RAIL_CsmaConfig_t _rail_csma_default_config = RAIL_CSMA_CONFIG_802_15_4_2003_2p4_GHz_OQPSK_CSMA;

/* TODO use define to restrict to supported freq */
RAIL_DECLARE_TX_POWER_VBAT_CURVES(piecewiseSegments, curvesSg, curves24Hp, curves24Lp);

/********************* BUFFER MANAGEMENT ***************************/

/* tx buffer */
static uint8_t _transmit_buffer[IEEE802154_FRAME_LEN_MAX + 1];

/********************* LOKAL VARIABLES ******************************/

/* ref to rail_t/ netdev_t struct for this driver
   TODO howto distinguish between multible netdevs?
   necessary for multiprotocol support, multible instances of the driver etc.
   Possible solution: map with an rhandle as key and netdev as value?
 */
static rail_t *_rail_dev = NULL;

/************************ private functions *********************************/

/* callback handler for RAIL driver blob, get called to handle events.
   The hw irqs are allready handled when called
 */
static void _rail_radio_event_handler(RAIL_Handle_t rhandle, RAIL_Events_t event);

void rail_setup(rail_t *dev, const rail_params_t *params)
{

    netdev_t *netdev = (netdev_t *)dev;

    /* register driver (defined in rail_netdev) */
    netdev->driver = &rail_driver;

    /* init dev with params */
    memcpy(&dev->params, params, sizeof(rail_params_t));

    DEBUG("rail->setup called\n");

    /* default, no promiscuous  mode */
    dev->promiscuousMode = false;

    dev->state = RAIL_TRANSCEIVER_STATE_UNINITIALIZED;

    /* TODO config for 868/912MHz different? */
    dev->csma_config = _rail_csma_default_config;

#ifdef RAIL_RADIO_HAS_SUBGHZ
    /* set default channel page, only relevant if sub ghz channel is set */
    dev->channel_page = RAIL_SUBGHZ_DEFAULT_PAGE;
#endif

    dev->event_count = 0;
}

/* init Packet Trace (PTI) functionality -> usefull for debugging */
#if (RAIL_PTI_ENABLED == 1)
int _rail_PTI_init(rail_t *dev)
{

    /* init gpio for trace output */

    RAIL_PtiConfig_t pti_config = RAIL_PTI_CONFIG;

    RAIL_ConfigPti(dev->rhandle, &pti_config);

    RAIL_EnablePti(dev->rhandle, true);

    DEBUG("RADIO_PTI_Init done\n");

    return 0;
}
#endif

/* initialisation of the transceivers power amplifier
   have to be called in init
   TODO what about deep sleep?
 */
int _rail_PA_init(rail_t *dev)
{

    /* Initialize the RAIL Tx power curves for all PAs on this chip */
    RAIL_TxPowerCurvesConfig_t tx_power_curves_config = {
        curves24Hp,
        curvesSg,
        curves24Lp,
        piecewiseSegments
    };

    RAIL_Status_t ret = RAIL_InitTxPowerCurves(&tx_power_curves_config);

    if (ret != RAIL_STATUS_NO_ERROR) {
        LOG_ERROR("Error init PA  (Tx power curves) for rail - error msg: %s\n", rail_error2str(ret));
        return -1;
    }

    /* Power config, depends on chip etc ... */
    /* TODO
         - multi freq, mult protocol
         - there are SoCs with high and low power PAs,
            how to determain which version has this SoC?
            atm only 2.4GHZ HighPower is supported
     */
    RAIL_TxPowerConfig_t tx_power_config = {
#if RAIL_RADIO_BAND == 2400
        RAIL_TX_POWER_MODE_2P4_HP,
#elif (RAIL_RADIO_BAND == 868) || (RAIL_RADIO_BAND == 915)
        RAIL_TX_POWER_MODE_SUBGIG,
#endif
        RAIL_RADIO_PA_VOLTAGE, /* voltage vPA for the DCDC connection */
        10
    };

    ret = RAIL_ConfigTxPower(dev->rhandle, &tx_power_config);

    if (ret != RAIL_STATUS_NO_ERROR) {
        LOG_ERROR("Error init PA  (config Tx power) for rail - error msg: %s\n", rail_error2str(ret));
        return -1;
    }

    ret = RAIL_SetTxPowerDbm(dev->rhandle, ((RAIL_TxPower_t)RAIL_DEFAULT_TXPOWER) * 10);

    if (ret != RAIL_STATUS_NO_ERROR) {
        LOG_ERROR("Error init PA  (set tx power) for rail");
        return -1;
    }

    DEBUG("RADIO_PA_Init done\n");

    return 0;
}

int rail_init(rail_t *dev)
{

    RAIL_Status_t ret;
    int r = 0;

    netdev_ieee802154_t *netdev = (netdev_ieee802154_t *)dev;

    /* save ref for this driver, for global access (esp in rail event handler) */
    /* TODO multible instances? */
    _rail_dev = dev;

    DEBUG("rail_init called\n");

    dev->state = RAIL_TRANSCEIVER_STATE_UNINITIALIZED;

    /* init the queue for the rail events */
    rail_event_queue_init(&(dev->event_queue));

    /* start with long addr mode. */
    netdev->flags |= NETDEV_IEEE802154_SRC_MODE_LONG;

    /* get informations about the used raillib */
    /* TODO check if driver is compatible? */
    RAIL_Version_t rail_version;
    RAIL_GetVersion(&rail_version, true);

    DEBUG("Using Silicon Labs RAIL Lib. Version %u.%u Rev: %u build: %u multiprotocol: %s \n",
          rail_version.major, rail_version.minor, rail_version.rev, rail_version.build,
          rail_version.multiprotocol ? "YES" : "NO");

    /* init rail blob config
       set to zero, because API manual request it
     */
    memset(&(dev->rconfig), 0, sizeof(RAIL_Config_t));

    dev->rconfig.eventsCallback = &_rail_radio_event_handler;
    dev->rconfig.protocol = NULL;
    dev->rconfig.scheduler = NULL;

    /* Init rail driver blob instance */
    dev->rhandle = RAIL_Init(&(dev->rconfig), NULL);

    if (dev->rhandle == NULL) {
        LOG_ERROR("Can not init rail blob driver\n");
        return -1;
    }

    /* config data management, easier version with packets */

    static const RAIL_DataConfig_t rail_data_config = {
        TX_PACKET_DATA,
        RX_PACKET_DATA,
        PACKET_MODE,
        PACKET_MODE,
    };

    ret = RAIL_ConfigData(dev->rhandle, &rail_data_config);

    if (ret != RAIL_STATUS_NO_ERROR) {
        LOG_ERROR("Can not init rail data config - error msg: %s\n", rail_error2str(ret));
        return -1;
    }

    /* config transceiver calibration (freq depends on temperature etc) */

    ret = RAIL_ConfigCal(dev->rhandle, RAIL_CAL_ALL);

    if (ret != RAIL_STATUS_NO_ERROR) {
        LOG_ERROR("Can not init rail calibration - error msg: %s\n", rail_error2str(ret));
        return -1;
    }

    /* configure the channels for 802.15.4 */

#if RAIL_RADIO_BAND == 2400
    /* for 2.4 GHz the RAIL API provides a std conform default config */
    DEBUG("using 2.4GHz radio band\n");
    ret = RAIL_IEEE802154_Config2p4GHzRadio(dev->rhandle);

#elif (RAIL_RADIO_BAND == 868) || (RAIL_RADIO_BAND == 915)
    /* from gecko sdk 2.4 there is a official config api for sub ghz radio
       but it does not support channel 0

     */
#if (RAIL_RADIO_BAND == 868)
    DEBUG("using 868MHz radio band\n");
    /*ret =     RAIL_IEEE802154_ConfigGB863MHzRadio(dev->rhandle); */
    ret = RAIL_ConfigChannels(dev->rhandle, &_rail_radio_channel_config, NULL);
#elif (RAIL_RADIO_BAND == 915)
    DEBUG("using 915MHz radio band\n");
    /*ret =     RAIL_IEEE802154_ConfigGB915MHzRadio(dev->rhandle);*/
    ret = RAIL_ConfigChannels(dev->rhandle, &_rail_radio_channel_config, NULL);
#endif

#endif /*  (RAIL_RADIO_BAND == 868) || (RAIL_RADIO_BAND == 915) */

    if (ret != RAIL_STATUS_NO_ERROR) {
        LOG_ERROR("Can not init rail radio channels - error msg: %s\n", rail_error2str(ret));
        return -1;
    }

    /* init IEEE802.15.4 acceleration features  */

    ret = RAIL_IEEE802154_Init(dev->rhandle, &_rail_ieee802154_config);
    if (ret != RAIL_STATUS_NO_ERROR) {
        LOG_ERROR("Can not init rail ieee 802.15.4 support - error msg: %s\n", rail_error2str(ret));
        return -1;
    }

    /* activate auto ack in the layer above */
    static const netopt_enable_t enable = NETOPT_ENABLE;
    netdev_ieee802154_set(&dev->netdev, NETOPT_ACK_REQ,
                          &enable, sizeof(enable));

    /* init radio PA */
    r = _rail_PA_init(dev);

    if (r != 0) {
        return r;
    }

    /* setup transmitt buffer */
    uint16_t buf_size = RAIL_SetTxFifo(dev->rhandle, _transmit_buffer, 0, sizeof(_transmit_buffer));
    if (buf_size == 0) {
        LOG_ERROR("Can not set TxFifo - error msg: %s\n", rail_error2str(ret));
        return -1;
    }

    /* setup PTI */
#if (RAIL_PTI_ENABLED == 1)
    r = _rail_PTI_init(dev);
    assert(r == 0);
#endif

    /* TODO
         - how to figure out if this device is the PAN coord?
     */

    /* get mac addr from SoC */
    eui64_t eui = rail_helper_get_hw_EUI();

    DEBUG("Node EUI: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
          eui.uint8[0],
          eui.uint8[1],
          eui.uint8[2],
          eui.uint8[3],
          eui.uint8[4],
          eui.uint8[5],
          eui.uint8[6],
          eui.uint8[7]);

    /* TODO provide default PANID / NID by params? */
    DEBUG("Set PanID to 0x%04x\n", RAIL_DEFAULT_PANID);

    netdev->pan = RAIL_DEFAULT_PANID;

    ret = RAIL_IEEE802154_SetPanId(dev->rhandle, RAIL_DEFAULT_PANID, 0);
    if (ret != RAIL_STATUS_NO_ERROR) {
        DEBUG("Can not set PAN ID %d - error msg: %s\n", RAIL_DEFAULT_PANID, rail_error2str(ret));
    }

    /* set short addr */
    DEBUG("Set ShortAddr 0x%04x\n", ntohs(eui.uint16[3].u16));

    /* yeah riot want it in big endian*/
    memcpy(netdev->short_addr, &eui.uint16[3].u16, 2);

    /* rail want it in little endian ...*/
    ret = RAIL_IEEE802154_SetShortAddress(dev->rhandle, byteorder_ntohs(eui.uint16[3]), 0);
    if (ret != RAIL_STATUS_NO_ERROR) {
        DEBUG("Can not set short addr - error msg: %s", rail_error2str(ret));
    }

    /* set long addr aka EUI */
    DEBUG("Set LongAddr 0x%08lx%08lx\n", ntohl(eui.uint64.u32[0]), ntohl(eui.uint64.u32[1]));

    memcpy(netdev->long_addr, &eui.uint8, IEEE802154_LONG_ADDRESS_LEN);

    /* and for the long address, it have to be little endian aka reversed order */
    uint64_t addr_rev = byteorder_ntohll(eui.uint64);

    ret = RAIL_IEEE802154_SetLongAddress(dev->rhandle, (uint8_t *)&addr_rev, 0);
    if (ret != RAIL_STATUS_NO_ERROR) {
        DEBUG("Can not set long addr - error msg: %s", rail_error2str(ret));
    }

    /* get transmitt power, TODO only for debug mode */

    RAIL_TxPower_t power_tx_ddBm = RAIL_GetTxPowerDbm(dev->rhandle);

    DEBUG("TX Power set to %hd deci dBm\n", power_tx_ddBm);

    /* configure the RAIL driver blob events we want the receive
       it is possible to get all events with RAIL_EVENTS_ALL
     */

    ret = RAIL_ConfigEvents(dev->rhandle,
                            RAIL_EVENTS_ALL,    /* mask of events, which should be modified, here all */
                                                /* events to subscribe */
                            RAIL_EVENT_RX_ACK_TIMEOUT |
                            RAIL_EVENT_RX_FRAME_ERROR |
                            RAIL_EVENT_RX_ADDRESS_FILTERED |
                            RAIL_EVENT_RX_PACKET_RECEIVED |
                            RAIL_EVENT_TX_PACKET_SENT |
                            RAIL_EVENT_TX_CHANNEL_BUSY |
                            RAIL_EVENT_TX_ABORTED |
                            RAIL_EVENT_TX_BLOCKED |
                            RAIL_EVENT_TX_UNDERFLOW |
                            RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND |
                            RAIL_EVENT_CAL_NEEDED);

    if (ret != RAIL_STATUS_NO_ERROR) {
        LOG_ERROR("Can not subsripe to rail events - error msg: %s", rail_error2str(ret));
        return -1;
    }

    /* TODO use / set some state? */

    DEBUG("rail_init done\n");
    LOG_INFO("rail radio driver initialised\n");

    return 0;
}

/* TODO
    - docu
    - rename to transmit_packet, frames are done a layer above
 */
int rail_transmit_frame(rail_t *dev, uint8_t *data_ptr, size_t data_length)
{
    DEBUG("[rail] rail_transmit_frame called\n");

    /* force radio state to idle, aboard running ops, so we can transmitt
       otherwise the transceiver might be receiving/transmitting and the new
       transmit op fails.
       TODO ensure there are no other running ops
     */
    RAIL_Idle(dev->rhandle, RAIL_IDLE_ABORT, true);

    /* write packet payload in the buffer of the rail driver blob*/
    RAIL_WriteTxFifo(dev->rhandle, data_ptr, data_length, true);

    /* config tx options, here just the defaults*/
    RAIL_TxOptions_t tx_option = RAIL_TX_OPTIONS_DEFAULT;

    /* TODO internal state */
    dev->state = RAIL_TRANSCEIVER_STATE_TX;

    /* check if ack req is requested */

    if (dev->netdev.flags & NETDEV_IEEE802154_ACK_REQ) {
        tx_option |= RAIL_TX_OPTION_WAIT_FOR_ACK;
        DEBUG("tx option auto ack\n");
        /* TODO wait for ack, necessary or done by layer above? */
    }

    DEBUG("[rail] transmit - radio state: %s\n", rail_radioState2str(RAIL_GetRadioState(dev->rhandle)));

    /* start tx with settings in csma_config
     */
    RAIL_Status_t ret = RAIL_StartCcaCsmaTx(dev->rhandle,
                                            dev->netdev.chan,
                                            tx_option,
                                            &dev->csma_config,
                                            NULL);

    if (ret != RAIL_STATUS_NO_ERROR) {
        LOG_ERROR("Can't start transmit - current state %s - error msg: %s \n",
                  rail_radioState2str(RAIL_GetRadioState(dev->rhandle)),
                  rail_error2str(ret));
        rail_start_rx(dev);
        return -1;
    }
    DEBUG("Started transmit\n");

    dev->netdev.netdev.event_callback((netdev_t *)&dev->netdev, NETDEV_EVENT_TX_STARTED);

    /* TODO
       - if this should be asymmetric blocking call, we have to wait for the
         tx done event by the callback
        - or use while (RAIL_GetRadioState(dev->rhandle) & RAIL_RF_STATE_TX );
     */
    while (RAIL_GetRadioState(dev->rhandle) & RAIL_RF_STATE_TX) {}

    rail_start_rx(dev);
    return 0;
}

int rail_start_rx(rail_t *dev)
{

    /*
       TODO process:
        check state
        if uninit/init etc/ -> error;
        if calibrate || error
            -> tx error
        if idle and rx
            if waiting for ack
                no timeout? error
            timeout?, stats++, cont
     */
    /* check if set? or just a global setting? */
    if (dev->promiscuousMode == true) {
        RAIL_IEEE802154_SetPromiscuousMode(dev->rhandle, true);
    }
    else {
        RAIL_IEEE802154_SetPromiscuousMode(dev->rhandle, false);
    }

    /* set channel to listen to */
    RAIL_StartRx(dev->rhandle, dev->netdev.chan, NULL);
    dev->state = RAIL_TRANSCEIVER_STATE_RX;

    DEBUG("[rail] recv - radio state: %s\n", rail_radioState2str(RAIL_GetRadioState(dev->rhandle)));
    return 0;
}

/* RAIL blob event handler */
/* moved everything possible to netdev->isr()
 */
static void _rail_radio_event_handler(RAIL_Handle_t rhandle, RAIL_Events_t event)
{

    /* TODO get the right netdev struct */
    rail_t *dev = _rail_dev;

    /* init event msg struct */
    rail_event_msg_t event_msg = { .event = RAIL_EVENTS_NONE, .rx_packet = RAIL_RX_PACKET_HANDLE_INVALID };

    /* debug/stat purpose, store event count */
    dev->event_count++;

    event_msg.event_count = dev->event_count;

    /* rail events are a bitmask, therefore multible events within this call
       are possible -> TODO
     */

    /* Indicates a Data Request is being received when using IEEE 802.15.4
       functionality. */
    if (event & RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND) {
        /* TODO what is source match? and why might it be necessary to filter
           here the packet?
         */
        DEBUG("Rail event ieee 802.15.4 data request command\n");
        RAIL_IEEE802154_SetFramePending(rhandle);

        return;
    }

    /* Occurs when the application needs to run a calibration.*/
    if (event & RAIL_EVENT_CAL_NEEDED) {

        DEBUG("Rail event calibration needed \n");
        LOG_INFO("Rail radio transceiver needs a calibration: executed\n");

        RAIL_Status_t ret;

        ret = RAIL_Calibrate(rhandle, NULL, RAIL_CAL_ALL_PENDING);

        DEBUG("calibration done, ret: %d \n", ret);

        assert(ret == RAIL_STATUS_NO_ERROR);

        return;
    }

    /* store the rail event */
    event_msg.event = event;

    /*	Occurs whenever a packet is received
        Can not moved to netdev->isr(), because packet is only accessable in
        this handler
     */
    if (event & RAIL_EVENT_RX_PACKET_RECEIVED) {
        DEBUG("Rail event rx packet received\n");

        /* check if packet is ok */

        RAIL_RxPacketHandle_t rx_handle;
        rx_handle = RAIL_GetRxPacketInfo(rhandle,
                                         RAIL_RX_PACKET_HANDLE_NEWEST,
                                         &(event_msg.rx_packet_info));

        DEBUG("[rail] rx packet event - len p 0x%02x - len2 0x%02x\n",
              event_msg.rx_packet_info.firstPortionData[0],
              event_msg.rx_packet_info.packetBytes);

        if (event_msg.rx_packet_info.packetStatus != RAIL_RX_PACKET_READY_SUCCESS) {
            /* error */

            DEBUG("Got an packet with an error - packet status msg: %s \n",
                  rail_packetStatus2str(event_msg.rx_packet_info.packetStatus));

            /* overwrite type, because we handle it as a frame error */
            event_msg.event = RAIL_EVENT_RX_FRAME_ERROR;
            /* if the packet is broken, we can release the memory */
            RAIL_ReleaseRxPacket(rhandle, rx_handle);
        }
        else {
            DEBUG("Rail event rx packet good packet \n");

            /* hold packet so it can be received from netdev thread context */
            RAIL_HoldRxPacket(rhandle);
            /* save the rx packet handle in the rail event msg */
            event_msg.rx_packet = rx_handle;
            /* save the size of the packet */
            event_msg.rx_packet_size = event_msg.rx_packet_info.packetBytes;
        }
    }

    /* debug events */

    if (event & RAIL_EVENT_TX_START_CCA) {

        DEBUG("RAIL_EVENT_TX_START_CCA\n");
    }
    if (event & RAIL_EVENT_TX_CCA_RETRY) {

        DEBUG("RAIL_EVENT_TX_CCA_RETRY\n");
    }
    if (event & RAIL_EVENT_TX_CHANNEL_BUSY) {

        DEBUG("RAIL_EVENT_TX_CHANNEL_BUSY\n");
    }
    if (event & RAIL_EVENT_TX_CHANNEL_CLEAR) {

        DEBUG("RAIL_EVENT_TX_CHANNEL_CLEAR\n");
    }
    if (event & RAIL_EVENT_TXACK_UNDERFLOW) {

        DEBUG("RAIL_EVENT_TXACK_UNDERFLOW\n");
    }
    if (event & RAIL_EVENT_TX_UNDERFLOW) {

        DEBUG("RAIL_EVENT_TX_UNDERFLOW\n");
    }
    if (event & RAIL_EVENT_TXACK_BLOCKED) {

        DEBUG("RAIL_EVENT_TXACK_BLOCKED\n");
    }
    if (event & RAIL_EVENT_TX_BLOCKED) {

        DEBUG("RAIL_EVENT_TX_BLOCKED\n");
    }
    if (event & RAIL_EVENT_TXACK_ABORTED) {

        DEBUG("RAIL_EVENT_TXACK_ABORTED\n");
    }

    if (event & RAIL_EVENT_TXACK_PACKET_SENT) {

        DEBUG("RAIL_EVENT_TXACK_PACKET_SENT\n");
    }
    if (event & RAIL_EVENT_TX_PACKET_SENT) {

        DEBUG("RAIL_EVENT_TX_PACKET_SENT\n");
    }
    if (event & RAIL_EVENT_TX_FIFO_ALMOST_EMPTY) {

        DEBUG("RAIL_EVENT_TX_FIFO_ALMOST_EMPTY\n");
    }
    if (event & RAIL_EVENT_RX_TIMING_DETECT) {

        DEBUG("RAIL_EVENT_RX_TIMING_DETECT\n");
    }
    if (event & RAIL_EVENT_RX_TIMING_LOST) {

        DEBUG("RAIL_EVENT_RX_TIMING_LOST\n");
    }
    if (event & RAIL_EVENT_RX_FILTER_PASSED) {

        DEBUG("RAIL_EVENT_RX_FILTER_PASSED\n");
    }
    if (event & RAIL_EVENT_RX_PACKET_ABORTED) {

        DEBUG("RAIL_EVENT_RX_PACKET_ABORTED\n");
    }
    if (event & RAIL_EVENT_RX_SCHEDULED_RX_END) {

        DEBUG("RAIL_EVENT_RX_SCHEDULED_RX_END\n");
    }
    if (event & RAIL_EVENT_RX_TIMEOUT) {

        DEBUG("RAIL_EVENT_RX_TIMEOUT\n");
    }
    if (event & RAIL_EVENT_RX_ADDRESS_FILTERED) {

        DEBUG("RAIL_EVENT_RX_ADDRESS_FILTERED\n");
    }
    if (event & RAIL_EVENT_RX_FRAME_ERROR) {

        DEBUG("RAIL_EVENT_RX_FRAME_ERROR\n");
    }
    if (event & RAIL_EVENT_RX_SYNC2_DETECT) {

        DEBUG("RAIL_EVENT_RX_SYNC2_DETECT\n");
    }
    if (event & RAIL_EVENT_RX_SYNC1_DETECT) {

        DEBUG("RAIL_EVENT_RX_SYNC1_DETECT\n");
    }
    if (event & RAIL_EVENT_RX_PREAMBLE_DETECT) {

        DEBUG("RAIL_EVENT_RX_PREAMBLE_DETECT\n");
    }
    if (event & RAIL_EVENT_RX_PREAMBLE_LOST) {

        DEBUG("RAIL_EVENT_RX_PREAMBLE_LOST\n");
    }

    /* add event to queue, for netdev to process */
    rail_event_queue_add(&(dev->event_queue), &event_msg);

    /* let the netdev->isr() handle the rest */
    dev->netdev.netdev.event_callback((netdev_t *)&dev->netdev, NETDEV_EVENT_ISR);
}
