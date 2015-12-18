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

    typedef struct {
        uint16_t addr;
        uint8_t data;
    } registerSetting_t;

    /**
     * @brief   PATABLE with available output powers
     * @note    If changed in size, adjust MAX_OUTPUT_POWER definition
     *          in CC110x interface
     */

    const char cc112x_default_pa_table[8] = {0x00, /*< -52 dBm */
    0x0D, /*< -20 dBm */
    0x34, /*< -10 dBm */
    0x57, /*< - 5 dBm */
    0x8E, /*<   0 dBm */
    0x85, /*< + 5 dBm */
    0xCC, /*< + 7 dBm */
    0xC3 /*< +10 dBm */
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

    static const registerSetting_t preferredSettings[] = {
            {CC112X_IOCFG3, 0x30},
            {CC112X_IOCFG2, 0x06},
            {CC112X_IOCFG0, 0x30},
            {CC112X_SYNC_CFG1, 0x8},
            {CC112X_DEVIATION_M, 0x99},
            {CC112X_MODCFG_DEV_E, 0x0D},
            {CC112X_DCFILT_CFG, 0x15},
            {CC112X_FREQ_IF_CFG, 0x39},
            {CC112X_IQIC, 0x00},
            {CC112X_CHAN_BW, 0x02},
            {CC112X_MDMCFG0, 0x05},
            {CC112X_SYMBOL_RATE2, 0x99},
            {CC112X_SYMBOL_RATE1, 0x99},
            {CC112X_SYMBOL_RATE0, 0x9A},
            {CC112X_AGC_REF, 0x3C},
            {CC112X_AGC_CS_THR, 0x03},
            {CC112X_AGC_CFG1, 0xA9},
            {CC112X_AGC_CFG0, 0xC0},
            {CC112X_FIFO_CFG, 0x40},
            {CC112X_FS_CFG, 0x12},
            {CC112X_PKT_CFG2, 0x0C},
            {CC112X_PKT_CFG1, 0x45},
            {CC112X_PKT_CFG0, 0x20},
            {CC112X_PA_CFG0, 0x7B},
            {CC112X_PKT_LEN, 0xFF},
            {CC112X_IF_MIX_CFG, 0x00},
            {CC112X_TOC_CFG, 0x0A},
            {CC112X_FREQ2, 0x6C},
            {CC112X_FREQ1, 0xBB},
            {CC112X_FREQ0, 0x33},
            {CC112X_FS_DIG1, 0x00},
            {CC112X_FS_DIG0, 0x5F},
            {CC112X_FS_CAL0, 0x0E},
            {CC112X_FS_DIVTWO, 0x03},
            {CC112X_FS_DSM0, 0x33},
            {CC112X_FS_DVC0, 0x17},
            {CC112X_FS_PFD, 0x50},
            {CC112X_FS_PRE, 0x6E},
            {CC112X_FS_REG_DIV_CML, 0x14},
            {CC112X_FS_SPARE, 0xAC},
            {CC112X_XOSC5, 0x0E},
            {CC112X_XOSC3, 0xC7},
            {CC112X_XOSC1,0x07}
    };

    const uint16_t registerSettingsCount = sizeof(preferredSettings)
            / sizeof(registerSetting_t);

#ifdef __cplusplus
}
#endif

#endif /* CC110X_DEFAULTSETTINGS_H */
/** @} */
