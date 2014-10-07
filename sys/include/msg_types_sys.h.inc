#ifdef MODULE_TRANSCEIVER
    MT_TRANSCEIVER_RCV_PKT_CC1020,      /**< packet was received by CC1020 transceiver */
    MT_TRANSCEIVER_RCV_PKT_CC1100,      /**< packet was received by CC1100 transceiver */
    MT_TRANSCEIVER_RCV_PKT_CC2420,      /**< packet was received by CC2420 transceiver */
    MT_TRANSCEIVER_RCV_PKT_MC1322X,     /**< packet was received by mc1322x transceiver */
    MT_TRANSCEIVER_RCV_PKT_NATIVE,      /**< packet was received by native transceiver */
    MT_TRANSCEIVER_RCV_PKT_AT86RF231,   /**< packet was received by AT86RF231 transceiver */

    /* Message types for transceiver <-> upper layer communication */
    MT_TRANSCEIVER_PKT_PENDING,         /**< packet pending in transceiver buffer */
    MT_TRANSCEIVER_SND_PKT,             /**< request for sending a packet */
    MT_TRANSCEIVER_SND_ACK,             /**< request for sending an acknowledgement */
    MT_TRANSCEIVER_SWITCH_RX,           /**< switch transceiver to RX sate */
    MT_TRANSCEIVER_POWERDOWN,           /**< power down transceiver */
    MT_TRANSCEIVER_GET_CHANNEL,         /**< Get current channel */
    MT_TRANSCEIVER_SET_CHANNEL,         /**< Set a new channel */
    MT_TRANSCEIVER_GET_ADDRESS,         /**< Get the radio address */
    MT_TRANSCEIVER_SET_ADDRESS,         /**< Set the radio address */
    MT_TRANSCEIVER_GET_LONG_ADDR,       /**< Get the long radio address, if existing */
    MT_TRANSCEIVER_SET_LONG_ADDR,       /**< Set the long radio address, if supported by hardware */
    MT_TRANSCEIVER_SET_MONITOR,         /**< Set transceiver to monitor mode (disable address checking) */
    MT_TRANSCEIVER_GET_PAN,             /**< Get current pan */
    MT_TRANSCEIVER_SET_PAN,             /**< Set a new pan */

    /* debug message types */
    MT_TRANSCEIVER_DBG_IGN,             /**< add a physical address to the ignore list */

    /* Error messages */
    MT_TRANSCEIVER_ENOBUFFER,           /**< No buffer left */

    /* reserve message types for higher layer notifications */
    MT_TRANSCEIVER_UPPER_LAYER_1,       /**< reserved */
    MT_TRANSCEIVER_UPPER_LAYER_2,       /**< reserved */
    MT_TRANSCEIVER_UPPER_LAYER_3,       /**< reserved */
    MT_TRANSCEIVER_UPPER_LAYER_4,       /**< reserved */
    MT_TRANSCEIVER_UPPER_LAYER_5,       /**< reserved */
#endif
