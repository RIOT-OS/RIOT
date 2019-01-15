*** Settings ***
Documentation       Basic tests to verify functionality of the periph I2C API.

Suite Setup         Run Keywords    Reset DUT and PHILIP
...                                 DUT Must Have API Firmware
Test Setup          Run Keywords    Reset DUT and PHILIP
...                                 DUT Must Have API Firmware

Resource            periph_i2c.keywords.txt
Resource            api_shell.keywords.txt

Force Tags          periph  i2c

*** Test Cases ***
Acquire and Release Should Succeed
    [Documentation]             Verify I2C acquire and release API calls.
    API Call Should Succeed     I2C Acquire
    API Call Should Succeed     I2C Release

Acquire after Release Should Succeed
    [Documentation]             Verify acquiring an I2C bus after release.
    API Call Should Succeed     I2C Acquire
    API Call Should Succeed     I2C Release
    API Call Should Succeed     I2C Acquire

Double Acquire Should Timeout
    [Documentation]             Verify that acquiring a locked I2C bus blocks the thread.
    API Call Should Succeed     I2C Acquire
    API Call Should Timeout     I2C Acquire

Unacquired Read Register Should Succeed
    [Documentation]             Verfiy reading a register without acquiring the I2C bus.
    API Call Should Succeed     I2C Read Reg

Read Register After Release Should Error
    [Documentation]             Verfiy reading a register doesn't work after releasing the I2C bus.
    API Call Should Succeed     I2C Acquire
    API Call Should Succeed     I2C Release
    API Call Should Error       I2C Read Reg

Read Register After NACK Should Succeed
    [Documentation]             Verify recovery of I2C bus after NACK.
    API Call Should Error       I2C Read Reg  addr=42
    API Call Should Succeed     I2C Read Reg
