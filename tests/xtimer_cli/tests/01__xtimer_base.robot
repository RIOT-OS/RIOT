*** Settings ***
Documentation       Basic tests to verify functionality of the Xtimer API.

Suite Setup         Run Keywords    Reset Application
...                                 DUT Must Have API Firmware
Test Setup          Run Keywords    Reset Application
...                                 DUT Must Have API Firmware

Library             Xtimer  port=%{PORT}  baudrate=%{BAUD}  timeout=${10}
Resource            api_shell.keywords.txt
Resource            riot_base.keywords.txt

Force Tags          xtimer

*** Test Cases ***
Xtimer Now Should Succeed
    [Documentation]             Verify xtimer_now() API call.
    API Call Should Succeed     Xtimer Now

Xtimer Values Should Increase
    [Documentation]             Compare two xtimer values (t1, t2) and verify
    ...                         that they increase (t2 > t1).
    API Call Should Succeed     Xtimer Now
    ${t1}=                      API Call Get Result As Integer
    API Call Should Succeed     Xtimer Now
    ${t2}=                      API Call Get Result As Integer
    Should Be True              ${t2} > ${t1}
