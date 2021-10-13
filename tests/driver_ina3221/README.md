# About
This is a manual test application for the INA3221 current and power
monitor driver.

# Usage
This test application will initialize the sensor with the following parameters:
- INA3221_PARAM_I2C:                I2C_DEV(0)
- INA3221_PARAM_ADDR:               INA3221_ADDR_00
- INA3221_PARAM_PIN_WRN:            GPIO_UNDEF, or given WRN input pin
- INA3221_PARAM_PIN_CRT:            GPIO_UNDEF, or given CRT input pin
- INA3221_PARAM_PIN_TC:             GPIO_UNDEF, or given TC input pin
- INA3221_PARAM_PIN_PV:             GPIO_UNDEF, or given PV input pin
- INA3221_PARAM_CONFIG:             (
                                        INA3221_ENABLE_CH1 | INA3221_ENABLE_CH2 | INA3221_ENABLE_CH3,
                                        INA3221_NUM_SAMPLES_4 |
                                        INA3221_CONV_TIME_BADC_4156US |
                                        INA3221_CONV_TIME_SADC_4156US |
                                        INA3221_MODE_CONTINUOUS_SHUNT_BUS
                                    )
- INA3221_PARAM_RSHUNT_MOHM_CH1:    100
- INA3221_PARAM_RSHUNT_MOHM_CH2:    100
- INA3221_PARAM_RSHUNT_MOHM_CH3:    100

After initialization, the application will perform basic read/write
functionality tests. If the preceding static tests succeed, the program
will enter an endless loop and check if a full conversion cycle has
completed. If new values for shunt voltage and bus voltage are available,
current and power are calculated and each value for every channel is
printed to stdio in a neat table, with available status flags.
