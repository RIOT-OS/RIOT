*** Settings ***
Test Setup          Reset Application
Test Teardown       Terminate All Processes    kill=True

Resource            riot_base.keywords.txt
Resource            riot_util.keywords.txt

*** Test Cases ***
xtimer_hang
    [Documentation]     Runtime test to check if xtimer runs into a deadlock.
    ...                 It is not about clock stability nor timing accuracy.
    [Tags]  xtimer
    Run Application     timeout=15 secs
    # keyword                               # pattern               # > number
    Verify Lines Matching Pattern Greater   *Testing (???%)         90
    Result Should Contain   SUCCESS
