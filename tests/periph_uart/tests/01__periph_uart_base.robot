*** Settings ***
Suite Setup         Run Keywords    Reset DUT and PHILIP
...                                 DUT Must Have Periph UART Application
Test Setup          Run Keywords    Reset DUT and PHILIP
...                                 DUT Must Have Periph UART Application

Resource            periph_uart.keywords.txt
Resource            api_shell.keywords.txt

Variables           test_vars.py

Force Tags          periph  uart

*** Test Cases ***
Echo
    API Call Should Succeed     Uart Init
    API Call Should Succeed     PHILIP.Setup Uart
    API Call Should Succeed     Uart Send String   ${SHORT_TEST_STRING}
    Should Be Equal             ${RESULT['data'][0]}  ${SHORT_TEST_STRING}

Long Echo
    API Call Should Succeed     Uart Init
    API Call Should Succeed     PHILIP.Setup Uart
    API Call Should Succeed     Uart Send String   ${LONG_TEST_STRING}
    Should Be Equal             ${RESULT['data'][0]}  ${LONG_TEST_STRING}

Extended Echo
    API Call Should Succeed     Uart Init
    API Call Should Succeed     PHILIP.Setup Uart     mode=1
    API Call Should Succeed     Uart Send String      ${SHORT_TEST_STRING}
    Should Be Equal             ${RESULT['data'][0]}  ${SHORT_TEST_STRING_INC}

Extended Long Echo
    API Call Should Succeed     Uart Init
    API Call Should Succeed     PHILIP.Setup Uart     mode=1
    API Call Should Succeed     Uart Send String      ${LONG_TEST_STRING}
    Should Be Equal             ${RESULT['data'][0]}  ${LONG_TEST_STRING_INC}

Register Access
    API Call Should Succeed     Uart Init
    API Call Should Succeed     PHILIP.Setup Uart     mode=2
    API Call Should Succeed     Uart Send String      ${REG_152_READ}
    Should Be Equal             ${RESULT['data'][0]}  ${REG_152_READ_DATA}

Should Not Access Invalid Register
    API Call Should Succeed     Uart Init
    API Call Should Succeed     PHILIP.Setup Uart     mode=2
    API Call Should Succeed     Uart Send String      ${REG_WRONG_READ}
    Should Be Equal             ${RESULT['data']}  ${REG_WRONG_READ_DATA}

Baud Test
    API Call Should Succeed     Uart Init
    API Call Should Succeed     PHILIP.Setup Uart     mode=1
    API Call Should Succeed     Uart Send String      ${SHORT_TEST_STRING}
    API Call Should Succeed     Uart Init             baud=${38400}
    API Call Should Timeout     Uart Send String      ${SHORT_TEST_STRING}
