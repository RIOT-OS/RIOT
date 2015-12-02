/**
 * @ingroup   driver_cc112x
 * @{
 *
 * @file
 * @brief     cc112x default settings override
 *
 * By setting either CC112X_DEFAULT_PATABLE or CC110X_DEFAULT_FREQ in board.h,
 * it is possible to override the default pa table or base frequency registers
 * on a per-device basis.
 *
 * @author    Kubaszek Mateusz <mathir.km.riot@gmail.com>
 */
#ifndef CC110X_DEFAULTSETTINGS_H
#define CC110X_DEFAULTSETTINGS_H

#include "cc112x-defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CC112X_CHANNEL0_FREQ	863125000UL
#define CC112X_CHANNEL_WIDTH	200000UL
#define CC112X_XTAL_FREQ		32000000UL


typedef struct
{
  uint16_t  addr;
  uint8_t   data;
}registerSetting_t;

/**
* @brief   PATABLE with available output powers
* @note    If changed in size, adjust MAX_OUTPUT_POWER definition
*          in CC110x interface
*/

const char cc112x_default_pa_table[8] = {
   0x00,   /*< -52 dBm */
   0x0D,   /*< -20 dBm */
   0x34,   /*< -10 dBm */
   0x57,   /*< - 5 dBm */
   0x8E,   /*<   0 dBm */
   0x85,   /*< + 5 dBm */
   0xCC,   /*< + 7 dBm */
   0xC3    /*< +10 dBm */
};

/**
* @brief cc112x default settings
* @note	XTAL freq = 32MHz
* 			carrier freq = 868.1MHz
* 			symbol rate = 50kbps
* 			Rx filter BW = 100kHz
* 			modulation format = 2-GFSK
* 			deviation = 25kHz
* 			TX power = 0dBm
*/

static const registerSetting_t preferredSettings[]=
{
// bilbioteka Okasa
//		  {CC112X_IOCFG3,            0xB0},
//		  {CC112X_IOCFG2,            0x14},
//		  {CC112X_IOCFG1,            0xB0},
//		  {CC112X_IOCFG0,            0x40},
//		  {CC112X_SYNC2,             0x90},
//		  {CC112X_SYNC1,             0x4E},
//		  {CC112X_SYNC0,             0x10},
//		  {CC112X_SYNC_CFG1,         0x08},
//		  {CC112X_SYNC_CFG0,         0x13},
//		  {CC112X_DEVIATION_M,       0x99},
//		  {CC112X_MODCFG_DEV_E,      0x0D},
//		  {CC112X_DCFILT_CFG,        0x15},
//		  {CC112X_PREAMBLE_CFG1,     0x19},
//		  {CC112X_FREQ_IF_CFG,       0x3A},
//		  {CC112X_IQIC,              0x00},
//		  {CC112X_CHAN_BW,           0x02},
//		  {CC112X_MDMCFG0,           0x05},
//		  {CC112X_SYMBOL_RATE2,      0x99},
//		  {CC112X_SYMBOL_RATE1,      0x99},
//		  {CC112X_SYMBOL_RATE0,      0x99},
//		  {CC112X_AGC_REF,           0x3C},
//		  {CC112X_AGC_CS_THR,        0x03},
//		  {CC112X_AGC_CFG1,          0xA9},
//		  {CC112X_AGC_CFG0,          0xC0},
//		  {CC112X_FIFO_CFG,          0x00},
//		  {CC112X_FS_CFG,            0x12},
//		  {CC112X_PKT_CFG2,          0x0C},
//		  {CC112X_PKT_CFG0,          0x20},
//		  {CC112X_PA_CFG0,           0x79},
//		  {CC112X_PKT_LEN,           0xFF},
//		  {CC112X_IF_MIX_CFG,        0x00},
//		  {CC112X_TOC_CFG,           0x0A},
//		  {CC112X_FREQ2,             0x6C},
//		  {CC112X_FREQ1,             0xBB},
//		  {CC112X_FREQ0,             0x33},
//		  {CC112X_FS_DIG1,           0x00},
//		  {CC112X_FS_DIG0,           0x5F},
//		  {CC112X_FS_CAL1,           0x40},
//		  {CC112X_FS_CAL0,           0x0E},
//		  {CC112X_FS_DIVTWO,         0x03},
//		  {CC112X_FS_DSM0,           0x33},
//		  {CC112X_FS_DVC0,           0x17},
//		  {CC112X_FS_PFD,            0x50},
//		  {CC112X_FS_PRE,            0x6E},
//		  {CC112X_FS_REG_DIV_CML,    0x14},
//		  {CC112X_FS_SPARE,          0xAC},
//		  {CC112X_FS_VCO0,           0xB4},
//		  {CC112X_XOSC5,             0x0E},
//		  {CC112X_XOSC1,             0x03}

  {CC112X_IOCFG3,              0x1A},	/* HIZ */
  {CC112X_IOCFG2,              0x06},	/* Packet TX RX */
  {CC112X_IOCFG1,              0x30},	/* HIZ */
  {CC112X_IOCFG0,              0x30},	/* HIZ */
  {CC112X_SYNC3,               0x93},
  {CC112X_SYNC2,               0x0B},
  {CC112X_SYNC1,               0x51},
  {CC112X_SYNC0,               0xDE},	/* Sync word = 0x930b51de (default) */
  {CC112X_SYNC_CFG1,           0x10},	/* Sync threshold value (actually high - poor quality signals will be accepted)*/
  {CC112X_SYNC_CFG0,           0x17},	/* Bite error qualifier disabled in sync word; sync word 32bit*/
  {CC112X_DEVIATION_M,         0x9A},	/* Deviation */
  {CC112X_MODCFG_DEV_E,        0x0D},	/* 2-GFSK */
  {CC112X_DCFILT_CFG,          0x1C},	/* DC filter 256 samples, DCFILT_BW=100 */
  {CC112X_PREAMBLE_CFG1,       0x14},
  {CC112X_PREAMBLE_CFG0,       0x2A},	/* default */
  {CC112X_FREQ_IF_CFG,         0x40},
  {CC112X_IQIC,                0x46},
  {CC112X_CHAN_BW,             0x02},
  {CC112X_MDMCFG1,             0x46},
  {CC112X_MDMCFG0,             0x05},
  {CC112X_SYMBOL_RATE2,        0x99},
  {CC112X_SYMBOL_RATE1,        0x99},
  {CC112X_SYMBOL_RATE0,        0x9A},
  {CC112X_AGC_REF,             0x20},
  {CC112X_AGC_CS_THR,          0x19},
  {CC112X_AGC_GAIN_ADJUST,     0x00},
  {CC112X_AGC_CFG3,            0x91},
  {CC112X_AGC_CFG2,            0x20},
  {CC112X_AGC_CFG1,            0xA9},
  {CC112X_AGC_CFG0,            0xCF},
  {CC112X_FIFO_CFG,            0x00},
  {CC112X_DEV_ADDR,            0x00},
  {CC112X_SETTLING_CFG,        0x0B},
  {CC112X_FS_CFG,              0x12},
  {CC112X_WOR_CFG1,            0x08},
  {CC112X_WOR_CFG0,            0x21},
  {CC112X_WOR_EVENT0_MSB,      0x00},
  {CC112X_WOR_EVENT0_LSB,      0x00},
  {CC112X_PKT_CFG2,            0x00},	//Always indicate channel as clear
  {CC112X_PKT_CFG1,            0x00},	//CRC, DATA_WHITTENING, ADDRESS_CHCK,
  {CC112X_PKT_CFG0,            0x20},
  {CC112X_RFEND_CFG1,          0x0F},
  {CC112X_RFEND_CFG0,          0x00},
  {CC112X_PA_CFG2,             0x7F},
  {CC112X_PA_CFG1,             0x56},
  {CC112X_PA_CFG0,             0x7B},
  {CC112X_PKT_LEN,             0xFF},
  {CC112X_IF_MIX_CFG,          0x00},
  {CC112X_FREQOFF_CFG,         0x22},
  {CC112X_TOC_CFG,             0x0B},
  {CC112X_MARC_SPARE,          0x00},
  {CC112X_ECG_CFG,             0x00},
  {CC112X_CFM_DATA_CFG,        0x00},
  {CC112X_EXT_CTRL,            0x01},
  {CC112X_RCCAL_FINE,          0x00},
  {CC112X_RCCAL_COARSE,        0x00},
  {CC112X_RCCAL_OFFSET,        0x00},
  {CC112X_FREQOFF1,            0x00},
  {CC112X_FREQOFF0,            0x00},
  {CC112X_FREQ2,               0x6C},
  {CC112X_FREQ1,               0x83},
  {CC112X_FREQ0,               0x33},
  {CC112X_IF_ADC2,             0x02},
  {CC112X_IF_ADC1,             0xA6},
  {CC112X_IF_ADC0,             0x04},
  {CC112X_FS_DIG1,             0x00},
  {CC112X_FS_DIG0,             0x50},
  {CC112X_FS_CAL3,             0x00},
  {CC112X_FS_CAL2,             0x20},
  {CC112X_FS_CAL1,             0x40},
  {CC112X_FS_CAL0,             0x0E},
  {CC112X_FS_CHP,              0x28},
  {CC112X_FS_DIVTWO,           0x03},
  {CC112X_FS_DSM1,             0x02},
  {CC112X_FS_DSM0,             0x33},
  {CC112X_FS_DVC1,             0xF3},
  {CC112X_FS_DVC0,             0x13},
  {CC112X_FS_PFD,              0x50},
  {CC112X_FS_PRE,              0x6E},
  {CC112X_FS_REG_DIV_CML,      0x14},
  {CC112X_FS_SPARE,            0xAC},
  {CC112X_FS_VCO4,             0x14},
  {CC112X_FS_VCO3,             0x00},
  {CC112X_FS_VCO2,             0x00},
  {CC112X_FS_VCO1,             0x00},
  {CC112X_FS_VCO0,             0xB8},
  {CC112X_GBIAS6,              0x00},
  {CC112X_GBIAS5,              0x02},
  {CC112X_GBIAS4,              0x00},
  {CC112X_GBIAS3,              0x00},
  {CC112X_GBIAS2,              0x10},
  {CC112X_GBIAS1,              0x00},
  {CC112X_GBIAS0,              0x00},
  {CC112X_IFAMP,               0x01},
  {CC112X_LNA,                 0x01},
  {CC112X_RXMIX,               0x01},
  {CC112X_XOSC5,               0x0E},
  {CC112X_XOSC4,               0xA0},
  {CC112X_XOSC3,               0xC7},
  {CC112X_XOSC2,               0x04},
  {CC112X_XOSC1,               0x01},
  {CC112X_XOSC0,               0x00},
  {CC112X_ANALOG_SPARE,        0x00},
  {CC112X_PA_CFG3,             0x00},
  {CC112X_BIST,                0x00},
  {CC112X_DCFILTOFFSET_I1,     0x00},
  {CC112X_DCFILTOFFSET_I0,     0x00},
  {CC112X_DCFILTOFFSET_Q1,     0x00},
  {CC112X_DCFILTOFFSET_Q0,     0x00},
  {CC112X_IQIE_I1,             0x00},
  {CC112X_IQIE_I0,             0x00},
  {CC112X_IQIE_Q1,             0x00},
  {CC112X_IQIE_Q0,             0x00},
  {CC112X_DEM_STATUS,          0x00},
  {CC112X_AGC_GAIN2,           0xD1},
  {CC112X_AGC_GAIN1,           0x00},
  {CC112X_AGC_GAIN0,           0x3F},
  {CC112X_CFM_TX_DATA_IN,      0x00},
  {CC112X_RNDGEN,              0x7F},
  {CC112X_ANG1,                0x00},
  {CC112X_ANG0,                0x00},
  {CC112X_FSCAL_CTRL,          0x01},
  {CC112X_SERIAL_STATUS,       0x00},
};

const uint16_t registerSettingsCount = sizeof(preferredSettings)/sizeof(registerSetting_t);

#ifdef __cplusplus
}
#endif

#endif /* CC110X_DEFAULTSETTINGS_H */
/** @} */
