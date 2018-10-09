*** Settings ***
Documentation       Verify the size of thread_t, first by comparing the output
...                 string, and second by parsing output and calculate the sum.

Test Setup          Reset Application
Test Teardown       Terminate All Processes     kill=True

Resource            riot_base.keywords.txt
Resource            riot_util.keywords.txt

Variables           test_vars.py

*** Test Cases ***
Thread_t Fields Should be Equal
    [Documentation]     Verify size of thread_t by string comparison of its fields.
    [Tags]              core
    Run Application     timeout=3 secs
    ${tcb_size}=        Get First Line Matching Regexp Group  ${REGEXP_TCB_TOTAL}
    # use the correct list depending on platfrom variant
    ${tcb_parts}=       Set Variable If  ${tcb_size} > 30  ${TCB_PARTS_32}  ${TCB_PARTS_16}
    : FOR  ${elem}  IN  @{tcb_parts}
    \                   Result Should Contain  ${elem}
    Result Should Contain  SUCCESS

Calculate Thread_t Size
    [Documentation]     Verify size of thread_t by summing up size of its fields.
    [Tags]              core
    Run Application     timeout=3 secs
    ${tcb_size}=        Get First Line Matching Regexp Group  ${REGEXP_TCB_TOTAL}
    @{tcb_parts}=       Get All Lines Matching Regexp Group   ${REGEXP_TCB_PARTS}
    ${tcb_sum}=         Get Sum from List   @{tcb_parts}
    Should Be Equal As Integers         ${tcb_size}     ${tcb_sum}
    Result Should Contain  SUCCESS
