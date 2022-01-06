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
    if (IS_USED(MODULE_SAUL_ADC)) {
        extern void auto_init_saul_adc(void);
        auto_init_saul_adc();
    }
    if (IS_USED(MODULE_SAUL_GPIO)) {
        extern void auto_init_gpio(void);
        auto_init_gpio();
    }
    if (IS_USED(MODULE_SAUL_PWM)) {
        extern void auto_init_saul_pwm(void);
        auto_init_saul_pwm();
    }
    if (IS_USED(MODULE_SAUL_NRF_TEMPERATURE)) {
        extern void auto_init_nrf_temperature(void);
        auto_init_nrf_temperature();
    }
    if (IS_USED(MODULE_SAUL_NRF_VDDH)) {
        extern void auto_init_nrf_vddh(void);
        auto_init_nrf_vddh();
    }
    if (IS_USED(MODULE_AD7746)) {
        extern void auto_init_ad7746(void);
        auto_init_ad7746();
    }
    if (IS_USED(MODULE_ADCXX1C)) {
        extern void auto_init_adcxx1c(void);
        auto_init_adcxx1c();
    }
    if (IS_USED(MODULE_ADS101X)) {
        extern void auto_init_ads101x(void);
        auto_init_ads101x();
    }
    if (IS_USED(MODULE_ADXL345)) {
        extern void auto_init_adxl345(void);
        auto_init_adxl345();
    }
    if (IS_USED(MODULE_APDS99XX)) {
        extern void auto_init_apds99xx(void);
        auto_init_apds99xx();
    }
    if (IS_USED(MODULE_BME680)) {
        extern void auto_init_bme680(void);
        auto_init_bme680();
    }
    if (IS_USED(MODULE_BMP180)) {
        extern void auto_init_bmp180(void);
        auto_init_bmp180();
    }
    if (IS_USED(MODULE_BMX280)) {
        extern void auto_init_bmx280(void);
        auto_init_bmx280();
    }
    if (IS_USED(MODULE_BMX055)) {
        extern void auto_init_bmx055(void);
        auto_init_bmx055();
    }
    if (IS_USED(MODULE_CCS811)) {
        extern void auto_init_ccs811(void);
        auto_init_ccs811();
    }
    if (IS_USED(MODULE_DHT)) {
        extern void auto_init_dht(void);
        auto_init_dht();
    }
    if (IS_USED(MODULE_DS18)) {
        extern void auto_init_ds18(void);
        auto_init_ds18();
    }
    if (IS_USED(MODULE_DS75LX)) {
        extern void auto_init_ds75lx(void);
        auto_init_ds75lx();
    }
    if (IS_USED(MODULE_EFM32_CORETEMP)) {
        extern void auto_init_efm32_coretemp(void);
        auto_init_efm32_coretemp();
    }
    if (IS_USED(MODULE_FXOS8700)) {
        extern void auto_init_fxos8700(void);
        auto_init_fxos8700();
    }
    if (IS_USED(MODULE_GP2Y10XX)) {
        extern void auto_init_gp2y10xx(void);
        auto_init_gp2y10xx();
    }
    if (IS_USED(MODULE_GROVE_LEDBAR)) {
        extern void auto_init_grove_ledbar(void);
        auto_init_grove_ledbar();
    }
    if (IS_USED(MODULE_HMC5883L)) {
        extern void auto_init_hmc5883l(void);
        auto_init_hmc5883l();
    }
    if (IS_USED(MODULE_HDC1000)) {
        extern void auto_init_hdc1000(void);
        auto_init_hdc1000();
    }
    if (IS_USED(MODULE_HM330X)) {
        extern void auto_init_hm330x(void);
        auto_init_hm330x();
    }
    if (IS_USED(MODULE_HSC)) {
        extern void auto_init_hsc(void);
        auto_init_hsc();
    }
    if (IS_USED(MODULE_HTS221)) {
        extern void auto_init_hts221(void);
        auto_init_hts221();
    }
    if (IS_USED(MODULE_INA2XX)) {
        extern void auto_init_ina2xx(void);
        auto_init_ina2xx();
    }
    if (IS_USED(MODULE_INA3221)) {
        extern void auto_init_ina3221(void);
        auto_init_ina3221();
    }
    if (IS_USED(MODULE_IO1_XPLAINED)) {
        extern void auto_init_io1_xplained(void);
        auto_init_io1_xplained();
    }
    if (IS_USED(MODULE_ISL29020)) {
        extern void auto_init_isl29020(void);
        auto_init_isl29020();
    }
    if (IS_USED(MODULE_ITG320X)) {
        extern void auto_init_itg320x(void);
        auto_init_itg320x();
    }
    if (IS_USED(MODULE_JC42)) {
        extern void auto_init_jc42(void);
        auto_init_jc42();
    }
    if (IS_USED(MODULE_L3G4200D)) {
        extern void auto_init_l3g4200d(void);
        auto_init_l3g4200d();
    }
    if (IS_USED(MODULE_LIS2DH12)) {
        extern void auto_init_lis2dh12(void);
        auto_init_lis2dh12();
    }
    if (IS_USED(MODULE_LIS3DH)) {
        extern void auto_init_lis3dh(void);
        auto_init_lis3dh();
    }
    if (IS_USED(MODULE_LIS3MDL)) {
        extern void auto_init_lis3mdl(void);
        auto_init_lis3mdl();
    }
    if (IS_USED(MODULE_LM75)) {
        extern void auto_init_lm75(void);
        auto_init_lm75();
    }
    if (IS_USED(MODULE_LPSXXX)) {
        extern void auto_init_lpsxxx(void);
        auto_init_lpsxxx();
    }
    if (IS_USED(MODULE_LSM303DLHC)) {
        extern void auto_init_lsm303dlhc(void);
        auto_init_lsm303dlhc();
    }
    if (IS_USED(MODULE_LSM6DSL)) {
        extern void auto_init_lsm6dsl(void);
        auto_init_lsm6dsl();
    }
    if (IS_USED(MODULE_LTC4150)) {
        extern void auto_init_ltc4150(void);
        auto_init_ltc4150();
    }
    if (IS_USED(MODULE_MAG3110)) {
        extern void auto_init_mag3110(void);
        auto_init_mag3110();
    }
    if (IS_USED(MODULE_MHZ19)) {
        extern void auto_init_mhz19(void);
        auto_init_mhz19();
    }
    if (IS_USED(MODULE_MMA7660)) {
        extern void auto_init_mma7660(void);
        auto_init_mma7660();
    }
    if (IS_USED(MODULE_MMA8X5X)) {
        extern void auto_init_mma8x5x(void);
        auto_init_mma8x5x();
    }
    if (IS_USED(MODULE_MPL3115A2)) {
        extern void auto_init_mpl3115a2(void);
        auto_init_mpl3115a2();
    }
    if (IS_USED(MODULE_MPU9X50)) {
        extern void auto_init_mpu9x50(void);
        auto_init_mpu9x50();
    }
    if (IS_USED(MODULE_OPT3001)) {
        extern void auto_init_opt3001(void);
        auto_init_opt3001();
    }
    if (IS_USED(MODULE_PCA9685)) {
        extern void auto_init_pca9685(void);
        auto_init_pca9685();
    }
    if (IS_USED(MODULE_PCF857X)) {
        extern void auto_init_pcf857x(void);
        auto_init_pcf857x();
    }
    if (IS_USED(MODULE_PH_OEM)) {
        extern void auto_init_ph_oem(void);
        auto_init_ph_oem();
    }
    if (IS_USED(MODULE_PIR)) {
        extern void auto_init_pir(void);
        auto_init_pir();
    }
    if (IS_USED(MODULE_PULSE_COUNTER)) {
        extern void auto_init_pulse_counter(void);
        auto_init_pulse_counter();
    }
    if (IS_USED(MODULE_QMC5883L)) {
        extern void auto_init_qmc5883l(void);
        auto_init_qmc5883l();
    }
    if (IS_USED(MODULE_SCD30)) {
        extern void auto_init_scd30(void);
        auto_init_scd30();
    }
    if (IS_USED(MODULE_SDP3X)) {
        extern void auto_init_sdp3x(void);
        auto_init_sdp3x();
    }
    if (IS_USED(MODULE_SDS011)) {
        extern void auto_init_sds011(void);
        auto_init_sds011();
    }
    if (IS_USED(MODULE_SEESAW_SOIL)) {
        extern void auto_init_seesaw_soil(void);
        auto_init_seesaw_soil();
    }
    if (IS_USED(MODULE_SGP30)) {
        extern void auto_init_sgp30(void);
        auto_init_sgp30();
    }
    if (IS_USED(MODULE_SHT2X)) {
        extern void auto_init_sht2x(void);
        auto_init_sht2x();
    }
    if (IS_USED(MODULE_SHT3X)) {
        extern void auto_init_sht3x(void);
        auto_init_sht3x();
    }
    if (IS_USED(MODULE_SHTC1)) {
        extern void auto_init_shtc1(void);
        auto_init_shtc1();
    }
    if (IS_USED(MODULE_SI1133)) {
        extern void auto_init_si1133(void);
        auto_init_si1133();
    }
    if (IS_USED(MODULE_SI114X)) {
        extern void auto_init_si114x(void);
        auto_init_si114x();
    }
    if (IS_USED(MODULE_SI70XX)) {
        extern void auto_init_si70xx(void);
        auto_init_si70xx();
    }
    if (IS_USED(MODULE_SM_PWM_01C)) {
        extern void auto_init_sm_pwm_01c(void);
        auto_init_sm_pwm_01c();
    }
    if (IS_USED(MODULE_SPS30)) {
        extern void auto_init_sps30(void);
        auto_init_sps30();
    }
    if (IS_USED(MODULE_TCS37727)) {
        extern void auto_init_tcs37727(void);
        auto_init_tcs37727();
    }
    if (IS_USED(MODULE_TMP006)) {
        extern void auto_init_tmp00x(void);
        auto_init_tmp00x();
    }
    if (IS_USED(MODULE_TSL2561)) {
        extern void auto_init_tsl2561(void);
        auto_init_tsl2561();
    }
    if (IS_USED(MODULE_TSL4531X)) {
        extern void auto_init_tsl4531x(void);
        auto_init_tsl4531x();
    }
    if (IS_USED(MODULE_VCNL40X0)) {
        extern void auto_init_vcnl40x0(void);
        auto_init_vcnl40x0();
    }
    if (IS_USED(MODULE_VEML6070)) {
        extern void auto_init_veml6070(void);
        auto_init_veml6070();
    }
}
