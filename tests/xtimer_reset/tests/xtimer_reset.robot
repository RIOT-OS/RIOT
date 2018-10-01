*** Settings ***
Test Setup          Reset Application
Test Teardown       Terminate All Processes    kill=True

Resource            riot_base.keywords.txt
Resource            riot_util.keywords.txt

*** Test Cases ***
xtimer_reset
    [Documentation]     Try to reset an already active timer.
    [Tags]              xtimer
    Run Application     timeout=3 secs
    # keyword                               # pattern               # == number
    Verify Lines Matching Pattern Equal     *now=[0123456789]*      3
    Result Should Contain   Test completed!
