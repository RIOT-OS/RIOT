*** Settings ***
Documentation       Data driven tests to verify writing to a valid register.

Suite Setup         Run Keywords    Reset DUT and PHILIP
...                                 DUT Must Have API Firmware
Test Setup          Run Keywords    Reset DUT and PHILIP
...                                 DUT Must Have API Firmware
...                                 I2C Acquire
Test Teardown       I2C Release
Test Template       I2C Write Bytes To Register Should Succeed

Resource            periph_i2c.keywords.txt
Variables           test_vars.py

Force Tags          periph  i2c

*** Test Cases ***          REG             DATA        LENGTH
One Byte Should Succeed     ${I2C_UREG}     ${VAL_1}    1
Two Bytes Should Succeed    ${I2C_UREG}     ${VAL_2}    2
Ten Bytes Should Succeed    ${I2C_UREG}     ${VAL_10}   10
