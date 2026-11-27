/*
 * SPDX-FileCopyrightText: 2026 UGA
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_tmp117
 * @{
 *
 * @file
 * @brief       TMP117 driver implementation
 *
 * @author      léo cordier <leo.cordier@univ-grenoble-alpes.fr>
 *
 * @}
 */

#include <assert.h>

#include "ztimer.h"

#include "tmp117.h"
#include "tmp117_params.h"
#include "tmp117_regs.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define I2C_GENERAL_CALL_ADDRESS   0x00

#define DEFAULT_CONFIG_VALUE    (dev->params.conv_mode << \
                                 TMP117_CONV_MODE_SHIFT | dev->params.conv_cycle << \
                                 TMP117_CONV_CYCLE_SHIFT | dev->params.avg << TMP117_AVG_SHIFT)

const uint16_t tmp117_avg_value[8] = {1, 8, 32, 64};


/*
 * Local functions prototypes
 */
static int _write_16_reg(tmp117_t *dev, uint8_t reg_addr, uint16_t value);
static int _read_16_reg(tmp117_t *dev, uint8_t reg_addr, uint16_t *value);
static void _general_call_reset(i2c_t dev);
static int _unlock_EEPROM(tmp117_t *dev);
static int _is_EEPROM_busy(tmp117_t *dev);


int tmp117_init(tmp117_t *dev, const tmp117_params_t *params)
{
    int status;
    int res = TMP117_NOI2C;

    assert(dev && params);
    dev->params = *params;
    dev->is_initialized = 0;

    i2c_acquire(dev->params.i2c);
    uint16_t reg = 0;

    status = _read_16_reg(dev, TMP117_REG_DEVICE_ID, &reg);

    /* checking if sensor is a tmp117 */
    if (status < 0 || reg != TMP117_REG_DEVICE_ID_DEFAULT_VAL) {
        DEBUG("[tmp117] init - error: unable to communicate with the device "
              "(reg=%x)\n", reg);
        res = TMP117_NOI2C;
        goto release;
    }

    /* checking sensor configuration */

    res = tmp117_check_configuration(dev,(tmp117_params_t*)params);
    if(res == TMP117_NODATA){
    	goto release;
    }
    if(res == TMP117_BADCFG){
    	DEBUG("[%s] EEPROM config don't match wanted parameters: writing new config in EEPROM\n",__FUNCTION__);
        /* persist configuration in EEPROM */
    	_unlock_EEPROM(dev);
        if (_write_16_reg(dev, TMP117_REG_CONFIG, DEFAULT_CONFIG_VALUE)) {
            DEBUG("[tmp117] init - error: unable to write configuration in EEPROM\n");
            res = TMP117_NODATA;
            goto release;
        }

        /* wait for the EEPROM write to finish */
        ztimer_sleep(ZTIMER_MSEC,7);
        while(_is_EEPROM_busy(dev)){
        	ztimer_sleep(ZTIMER_MSEC,1);
        }

        /* reboot the device to get the new configuration */
        _general_call_reset(dev->params.i2c);
    }else{
    	DEBUG("[%s] EEPROM config match wanted parameters !\n",__FUNCTION__);
    }

    dev->is_initialized = 1;
    res = TMP117_OK;

release:
    i2c_release(dev->params.i2c);
    return res;
}

int tmp117_check_configuration(tmp117_t *dev, tmp117_params_t *check_params){
	if(check_params->conv_mode == TMP117_CONV_OS){
		check_params->conv_mode = TMP117_CONV_SD;
	}

	uint16_t check_params_reg =  (check_params->conv_mode << TMP117_CONV_MODE_SHIFT \
								| check_params->conv_cycle << TMP117_CONV_CYCLE_SHIFT \
								| check_params->avg << TMP117_AVG_SHIFT);
	uint16_t check_params_mask = TMP117_CONV_MODE_MASK | TMP117_CONV_CYCLE_MASK | TMP117_AVG_MASK;
	uint16_t config_reg;

	int status;

	status = _read_16_reg(dev, TMP117_REG_CONFIG, &config_reg);
	if(status != TMP117_OK){
		DEBUG("[tmp117] error: unable to read configuration register\n");
		return TMP117_NODATA;
	}

	DEBUG("[%s]tmp117 config: %4x   wanted config: %4x",__FUNCTION__,config_reg & check_params_mask,check_params_reg);

	if( (config_reg & check_params_mask) != check_params_reg){
		return TMP117_BADCFG;
	}

	return TMP117_OK;
}

int tmp117_read_temperature(tmp117_t *dev, int16_t *temperature)
{
    int res = TMP117_NOI2C;

    int16_t raw_temp;

    i2c_acquire(dev->params.i2c);

    if (_read_16_reg(dev, TMP117_REG_TEMP_RESULT, (uint16_t *)&raw_temp)) {
        DEBUG("[tmp117] read - error: unable to read temperature \n");
        goto release;
    }

    *temperature = (((int32_t)raw_temp) * 100)  >> 7;  /* converting raw value to centi-degrees */

    res = TMP117_OK;

release:
    i2c_release(dev->params.i2c);
    return res;
}

int tmp117_set_conversion_mode(tmp117_t *dev, tmp117_conv_mode_t mode)
{
    int res = TMP117_NOI2C;
    uint16_t conf_reg;

    i2c_acquire(dev->params.i2c);

    if (_read_16_reg(dev, TMP117_REG_CONFIG, &conf_reg)) {
        DEBUG("[tmp117] set conv mode - error: unable to read conf reg \n");
        goto release;
    }

    conf_reg &= ~TMP117_CONV_MODE_MASK;
    conf_reg |= mode << TMP117_CONV_MODE_SHIFT;

    /* writing configuration register */
    if (_write_16_reg(dev, TMP117_REG_CONFIG, conf_reg)) {
        DEBUG("[tmp117] set conv mode - error: unable to set configuration \n");
        goto release;
    }

    dev->params.conv_mode = mode;
    res = TMP117_OK;

release:
    i2c_release(dev->params.i2c);
    return res;
}

int tmp117_set_conversion_cycle(tmp117_t *dev, tmp117_conv_cycle_t cycle)
{
    int res = TMP117_NOI2C;
    uint16_t conf_reg;

    i2c_acquire(dev->params.i2c);

    if (_read_16_reg(dev, TMP117_REG_CONFIG, &conf_reg)) {
        DEBUG("[tmp117] set conv cycle - error: unable to read conf reg \n");
        goto release;
    }

    conf_reg &= ~TMP117_CONV_CYCLE_MASK;
    conf_reg |= cycle << TMP117_CONV_CYCLE_SHIFT;

    /* writing configuration register */
    if (_write_16_reg(dev, TMP117_REG_CONFIG, conf_reg)) {
        DEBUG("[tmp117] set conv cycle - error: unable to set configuration \n");
        goto release;
    }
    dev->params.conv_cycle = cycle;
    res = TMP117_OK;

release:
    i2c_release(dev->params.i2c);
    return res;
}

int tmp117_set_averaging(tmp117_t *dev, tmp117_avg_t avg)
{
    int res = TMP117_NOI2C;
    uint16_t conf_reg;

    i2c_acquire(dev->params.i2c);

    if (_read_16_reg(dev, TMP117_REG_CONFIG, &conf_reg)) {
        DEBUG("[tmp117] set averaging - error: unable to read conf reg \n");
        goto release;
    }

    conf_reg &= ~TMP117_AVG_MASK;
    conf_reg |= avg << TMP117_AVG_SHIFT;

    /* writing configuration register */
    if (_write_16_reg(dev, TMP117_REG_CONFIG, conf_reg)) {
        DEBUG("[tmp117] set averaging - error: unable to set configuration \n");
        goto release;
    }
    dev->params.avg = avg;
    res = TMP117_OK;

release:
    i2c_release(dev->params.i2c);
    return res;
}

int tmp117_is_data_ready(tmp117_t *dev)
{
    int res = TMP117_NOI2C;
    uint16_t conf_reg;

    i2c_acquire(dev->params.i2c);

    if (_read_16_reg(dev, TMP117_REG_CONFIG, &conf_reg)) {
        DEBUG("[tmp117] is data ready - error: unable to read conf reg \n");
        goto release;
    }

    if ((conf_reg & TMP117_DATA_READY_MASK) != 0) {
        res = TMP117_DATAREADY;
    }
    else {
        res = TMP117_OK;
    }

release:
    i2c_release(dev->params.i2c);
    return res;
}

/*
 * Local functions
 */

static int _write_16_reg(tmp117_t *dev, uint8_t reg_addr, uint16_t value)
{
    uint8_t tmp_buf[2];

    /* reorganising value */
    tmp_buf[0] = value >> 8;
    tmp_buf[1] = value & 0xFF;

    if (i2c_write_regs(dev->params.i2c, dev->params.addr, reg_addr, tmp_buf, 2, 0)) {
        DEBUG("[tmp117] unable to write reg %02X\n", reg_addr);
        return TMP117_NOI2C;
    }

    return TMP117_OK;
}

static int _read_16_reg(tmp117_t *dev, uint8_t reg_addr, uint16_t *value)
{
    uint8_t tmp_buf[2];

    if (i2c_read_regs(dev->params.i2c, dev->params.addr, reg_addr, tmp_buf, 2, 0)) {
        DEBUG("[tmp117] unable to read reg %02X\n", reg_addr);
        return TMP117_NOI2C;
    }

    /* reorganising value */
    *value = tmp_buf[0] << 8  | tmp_buf[1];

    return TMP117_OK;
}

static int _unlock_EEPROM(tmp117_t *dev){
    if (_write_16_reg(dev,TMP117_REG_EEPROM_UNLOCK, (1 << TMP117_EUN_SHIFT))) {
        DEBUG("[%s] error: unable to write EEPROM unlock register \n",__FUNCTION__);
        return TMP117_NODATA;
    }

    return TMP117_OK;
}

static int _is_EEPROM_busy(tmp117_t *dev){
	uint16_t eeprom_reg;

    if (_read_16_reg(dev,TMP117_REG_EEPROM_UNLOCK, &eeprom_reg) != TMP117_OK) {
        DEBUG("[%s] error: unable to read EEPROM unlock register \n",__FUNCTION__);
        return TMP117_NODATA;
    }

    if( (eeprom_reg & TMP117_EEPROM_Busy_MASK) != 0 ){
    	return TMP117_BUSY;
    }

    return TMP117_OK;
}

static void _general_call_reset(i2c_t dev){
	i2c_write_byte(dev, I2C_GENERAL_CALL_ADDRESS, 0x06, 0);
}

