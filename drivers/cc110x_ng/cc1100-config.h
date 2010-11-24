#ifndef CC1100_CONFIG_H
#define CC1100_CONFIG_H

/** CC1100 register configuration */
typedef struct {
	uint8_t IOCFG2;
	uint8_t IOCFG1;
	uint8_t IOCFG0;
	uint8_t FIFOTHR;
	uint8_t SYNC1;
	uint8_t SYNC0;
	uint8_t PKTLEN;
	uint8_t PKTCTRL1;
	uint8_t PKTCTRL0;
	uint8_t ADDR;
	uint8_t CHANNR;
	uint8_t FSCTRL1;
	uint8_t FSCTRL0;
	uint8_t FREQ2;
	uint8_t FREQ1;
	uint8_t FREQ0;
	uint8_t MDMCFG4;
	uint8_t MDMCFG3;
	uint8_t MDMCFG2;
	uint8_t MDMCFG1;
	uint8_t MDMCFG0;
	uint8_t DEVIATN;
	uint8_t MCSM2;
	uint8_t MCSM1;
	uint8_t MCSM0;
	uint8_t FOCCFG;
	uint8_t BSCFG;
	uint8_t AGCCTRL2;
	uint8_t AGCCTRL1;
	uint8_t AGCCTRL0;
	uint8_t WOREVT1;
	uint8_t WOREVT0;
	uint8_t WORCTRL;
	uint8_t FREND1;
	uint8_t FREND0;
	uint8_t FSCAL3;
	uint8_t FSCAL2;
	uint8_t FSCAL1;
	uint8_t FSCAL0;
} cc1100_reg_t;

/** CC1100 radio configuration */
typedef struct {
	cc1100_reg_t reg_cfg;	 	///< CC1100 register configuration
	uint8_t pa_power;			///< Output power setting
} cc1100_cfg_t;

/**
 * @brief	Radio Control Flags
 */
typedef struct
{
	uint32_t TOF;				///< Time of flight of the last packet and last ACK
	uint32_t TCP;				///< Time to compute packet
	unsigned RPS		: 16;	///< Raw packets sent to transmit last packet
	unsigned RTC		: 8;	///< Retransmission count of last send packet
	unsigned RSSI	 	: 8;	///< The RSSI value of last received packet
	unsigned RSSI_SEND 	: 8; 	///< The RSSI value of the last send unicast packet of this node
	unsigned LQI		: 8;	///< The LQI value of the last received packet
	unsigned LL_ACK  	: 1;	///< Is set if Link-Level ACK is received, otherwise 0 (reset on new burst)
	unsigned CAA     	: 1;	///< The status of the air (1 = air free, 0 = air not free)
	unsigned CRC     	: 1;	///< The CRC status of last received packet (1 = OK, 0 = not OK)
	unsigned SEQ	 	: 1;	///< Sequence number (toggles between 0 and 1)
	unsigned MAN_WOR 	: 1; 	///< Manual WOR set (for randomized WOR times => no synch)
	unsigned KT_RES_ERR : 1;	///< A hwtimer resource error has occurred (no free timers available)
	unsigned TX         : 1;	///< State machine TX lock, only ACKs will be received
	unsigned WOR_RST 	: 1;	///< Reset CC1100 real time clock (WOR) on next WOR strobe
} cc1100_flags;

/**
 * @brief	Statistic interface for debugging
 */
typedef struct cc1100_statistic {
	uint32_t	packets_in;
	uint32_t	packets_in_crc_fail;
	uint32_t	packets_in_while_tx;
	uint32_t	packets_in_dups;
	uint32_t	packets_in_up;
	uint32_t	packets_out;
	uint32_t	packets_out_acked;
	uint32_t	packets_out_broadcast;
	uint32_t	raw_packets_out;
	uint32_t	raw_packets_out_acked;
	uint32_t	acks_send;
	uint32_t	rx_buffer_max;
	uint32_t	watch_dog_resets;
} cc1100_statistic_t;

#endif
