/**
 * Auto initialization for SAUL devices
 *
 * Copyright (C) 2020 Freie Universität Berlin
 *               2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_auto_init
 * @{
 * @file
 * @brief   initializes any used SAUL device that has a trivial init function
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include "kernel_defines.h"

/**
 * @brief   Initializes sensors and actuators for SAUL
 */
void saul_init_devs(void)
{
    if (MODULE_SAUL_ADC) {
        extern void auto_init_saul_adc(void);
        auto_init_saul_adc();
    }
    if (MODULE_SAUL_BAT_VOLTAGE) {
        extern void auto_init_saul_bat_voltage(void);
        auto_init_saul_bat_voltage();
    }
    if (MODULE_SAUL_GPIO) {
        extern void auto_init_gpio(void);
        auto_init_gpio();
    }
    if (MODULE_SAUL_PWM) {
        extern void auto_init_saul_pwm(void);
        auto_init_saul_pwm();
    }
    if (MODULE_SAUL_NRF_TEMPERATURE) {
        extern void auto_init_nrf_temperature(void);
        auto_init_nrf_temperature();
    }
    if (MODULE_SAUL_NRF_VDDH) {
        extern void auto_init_nrf_vddh(void);
        auto_init_nrf_vddh();
    }
    if (MODULE_ABP2) {
        extern void auto_init_abp2(void);
        auto_init_abp2();
    }
    if (MODULE_AD7746) {
        extern void auto_init_ad7746(void);
        auto_init_ad7746();
    }
    if (MODULE_ADCXX1C) {
        extern void auto_init_adcxx1c(void);
        auto_init_adcxx1c();
    }
    if (MODULE_ADS1X1X) {
        extern void auto_init_ads1x1x(void);
        auto_init_ads1x1x();
    }
    if (MODULE_ADXL345) {
        extern void auto_init_adxl345(void);
        auto_init_adxl345();
    }
    if (MODULE_AMG88XX) {
        extern void auto_init_amg88xx(void);
        auto_init_amg88xx();
    }
    if (MODULE_APDS99XX) {
        extern void auto_init_apds99xx(void);
        auto_init_apds99xx();
    }
    if (MODULE_BME680) {
        extern void auto_init_bme680(void);
        auto_init_bme680();
    }
    if (MODULE_BMP180) {
        extern void auto_init_bmp180(void);
        auto_init_bmp180();
    }
    if (MODULE_BMX280) {
        extern void auto_init_bmx280(void);
        auto_init_bmx280();
    }
    if (MODULE_BMX055) {
        extern void auto_init_bmx055(void);
        auto_init_bmx055();
    }
    if (MODULE_CCS811) {
        extern void auto_init_ccs811(void);
        auto_init_ccs811();
    }
    if (MODULE_DHT) {
        extern void auto_init_dht(void);
        auto_init_dht();
    }
    if (MODULE_DS18) {
        extern void auto_init_ds18(void);
        auto_init_ds18();
    }
    if (MODULE_DS75LX) {
        extern void auto_init_ds75lx(void);
        auto_init_ds75lx();
    }
    if (MODULE_EFM32_CORETEMP) {
        extern void auto_init_efm32_coretemp(void);
        auto_init_efm32_coretemp();
    }
    if (MODULE_FXOS8700) {
        extern void auto_init_fxos8700(void);
        auto_init_fxos8700();
    }
    if (MODULE_GP2Y10XX) {
        extern void auto_init_gp2y10xx(void);
        auto_init_gp2y10xx();
    }
    if (MODULE_GP8XXX) {
        extern void auto_init_gp8xxx(void);
        auto_init_gp8xxx();
    }
    if (MODULE_GROVE_LEDBAR) {
        extern void auto_init_grove_ledbar(void);
        auto_init_grove_ledbar();
    }
    if (MODULE_HMC5883L) {
        extern void auto_init_hmc5883l(void);
        auto_init_hmc5883l();
    }
    if (MODULE_HDC1000) {
        extern void auto_init_hdc1000(void);
        auto_init_hdc1000();
    }
    if (MODULE_HM330X) {
        extern void auto_init_hm330x(void);
        auto_init_hm330x();
    }
    if (MODULE_HSC) {
        extern void auto_init_hsc(void);
        auto_init_hsc();
    }
    if (MODULE_HTS221) {
        extern void auto_init_hts221(void);
        auto_init_hts221();
    }
    if (MODULE_INA2XX) {
        extern void auto_init_ina2xx(void);
        auto_init_ina2xx();
    }
    if (IS_ACTIVE(MODUE_INA3221)) {
        extern void auto_init_ina3221(void);
        auto_init_ina3221();
    }
    if (MODULE_IO1_XPLAINED) {
        extern void auto_init_io1_xplained(void);
        auto_init_io1_xplained();
    }
    if (MODULE_ISL29020) {
        extern void auto_init_isl29020(void);
        auto_init_isl29020();
    }
    if (MODULE_ITG320X) {
        extern void auto_init_itg320x(void);
        auto_init_itg320x();
    }
    if (MODULE_JC42) {
        extern void auto_init_jc42(void);
        auto_init_jc42();
    }
    if (MODULE_L3G4200D) {
        extern void auto_init_l3g4200d(void);
        auto_init_l3g4200d();
    }
    if (MODULE_L3GXXXX) {
        extern void auto_init_l3gxxxx(void);
        auto_init_l3gxxxx();
    }
    if (MODULE_LIS2DH12) {
        extern void auto_init_lis2dh12(void);
        auto_init_lis2dh12();
    }
    if (MODULE_LIS3DH) {
        extern void auto_init_lis3dh(void);
        auto_init_lis3dh();
    }
    if (MODULE_LIS3MDL) {
        extern void auto_init_lis3mdl(void);
        auto_init_lis3mdl();
    }
    if (MODULE_LM75) {
        extern void auto_init_lm75(void);
        auto_init_lm75();
    }
    if (MODULE_LPSXXX) {
        extern void auto_init_lpsxxx(void);
        auto_init_lpsxxx();
    }
    if (MODULE_LSM303AGR) {
        extern void auto_init_lsm303agr(void);
        auto_init_lsm303agr();
    }
    if (MODULE_LSM303DLHC) {
        extern void auto_init_lsm303dlhc(void);
        auto_init_lsm303dlhc();
    }
    if (MODULE_LSM6DSXX) {
        extern void auto_init_lsm6dsxx(void);
        auto_init_lsm6dsxx();
    }
    if (MODULE_LTC4150) {
        extern void auto_init_ltc4150(void);
        auto_init_ltc4150();
    }
    if (MODULE_MAG3110) {
        extern void auto_init_mag3110(void);
        auto_init_mag3110();
    }
    if (MODULE_MAX31855) {
        extern void auto_init_max31855(void);
        auto_init_max31855();
    }
    if (MODULE_MAX31865) {
        extern void auto_init_max31865(void);
        auto_init_max31865();
    }
    if (MODULE_MCP23X17) {
        extern void auto_init_mcp23x17(void);
        auto_init_mcp23x17();
    }
    if (MODULE_MCP47XX) {
        extern void auto_init_mcp47xx(void);
        auto_init_mcp47xx();
    }
    if (MODULE_MHZ19) {
        extern void auto_init_mhz19(void);
        auto_init_mhz19();
    }
    if (MODULE_MMA7660) {
        extern void auto_init_mma7660(void);
        auto_init_mma7660();
    }
    if (MODULE_MMA8X5X) {
        extern void auto_init_mma8x5x(void);
        auto_init_mma8x5x();
    }
    if (MODULE_MPL3115A2) {
        extern void auto_init_mpl3115a2(void);
        auto_init_mpl3115a2();
    }
    if (MODULE_MPU9X50) {
        extern void auto_init_mpu9x50(void);
        auto_init_mpu9x50();
    }
    if (MODULE_OPT3001) {
        extern void auto_init_opt3001(void);
        auto_init_opt3001();
    }
    if (MODULE_PAA5100JE) {
        extern void auto_init_paa5100je(void);
        auto_init_paa5100je();
    }
    if (MODULE_PCA9685) {
        extern void auto_init_pca9685(void);
        auto_init_pca9685();
    }
    if (MODULE_PCF857X) {
        extern void auto_init_pcf857x(void);
        auto_init_pcf857x();
    }
    if (MODULE_PH_OEM) {
        extern void auto_init_ph_oem(void);
        auto_init_ph_oem();
    }
    if (MODULE_PIR) {
        extern void auto_init_pir(void);
        auto_init_pir();
    }
    if (MODULE_PULSE_COUNTER) {
        extern void auto_init_pulse_counter(void);
        auto_init_pulse_counter();
    }
    if (MODULE_QMA6100P) {
        extern void auto_init_qma6100p(void);
        auto_init_qma6100p();
    }
    if (MODULE_QMC5883L) {
        extern void auto_init_qmc5883l(void);
        auto_init_qmc5883l();
    }
    if (MODULE_QMI8658) {
        extern void auto_init_qmi8658(void);
        auto_init_qmi8658();
    }
    if (MODULE_SCD30) {
        extern void auto_init_scd30(void);
        auto_init_scd30();
    }
    if (MODULE_SDP3X) {
        extern void auto_init_sdp3x(void);
        auto_init_sdp3x();
    }
    if (MODULE_SDS011) {
        extern void auto_init_sds011(void);
        auto_init_sds011();
    }
    if (MODULE_SEESAW_SOIL) {
        extern void auto_init_seesaw_soil(void);
        auto_init_seesaw_soil();
    }
    if (MODULE_SEN5X) {
        extern void auto_init_sen5x(void);
        auto_init_sen5x();
    }
    if (MODULE_SGP30) {
        extern void auto_init_sgp30(void);
        auto_init_sgp30();
    }
    if (MODULE_SHT2X) {
        extern void auto_init_sht2x(void);
        auto_init_sht2x();
    }
    if (MODULE_SHT3X) {
        extern void auto_init_sht3x(void);
        auto_init_sht3x();
    }
    if (MODULE_SHTCX) {
        extern void auto_init_shtcx(void);
        auto_init_shtcx();
    }
    if (MODULE_SI1133) {
        extern void auto_init_si1133(void);
        auto_init_si1133();
    }
    if (MODULE_SI114X) {
        extern void auto_init_si114x(void);
        auto_init_si114x();
    }
    if (MODULE_SI70XX) {
        extern void auto_init_si70xx(void);
        auto_init_si70xx();
    }
    if (MODULE_SM_PWM_01C) {
        extern void auto_init_sm_pwm_01c(void);
        auto_init_sm_pwm_01c();
    }
    if (MODULE_SPS30) {
        extern void auto_init_sps30(void);
        auto_init_sps30();
    }
    if (MODULE_TCS37727) {
        extern void auto_init_tcs37727(void);
        auto_init_tcs37727();
    }
    if (MODULE_TMP006) {
        extern void auto_init_tmp00x(void);
        auto_init_tmp00x();
    }
    if (MODULE_TSL2561) {
        extern void auto_init_tsl2561(void);
        auto_init_tsl2561();
    }
    if (MODULE_TSL4531X) {
        extern void auto_init_tsl4531x(void);
        auto_init_tsl4531x();
    }
    if (MODULE_VCNL40X0) {
        extern void auto_init_vcnl40x0(void);
        auto_init_vcnl40x0();
    }
    if (MODULE_VEML6070) {
        extern void auto_init_veml6070(void);
        auto_init_veml6070();
    }
    if (MODULE_VL6180X) {
        extern void auto_init_vl6180x(void);
        auto_init_vl6180x();
    }
    if (MODULE_SERVO) {
        extern void auto_init_servo(void);
        auto_init_servo();
    }
    if (MODULE_WS281X) {
        extern void auto_init_ws281x(void);
        auto_init_ws281x();
    }
}
