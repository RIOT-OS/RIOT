*** Settings ***
Documentation       A suite of static code tests
...
...                 These tests are typically run before any other steps, like
...                 compiling, linking, and running functional tests.

Suite Teardown      Terminate All Processes    kill=True

Resource            riot_base.keywords.txt

*** Test Cases ***
toolchains
    [Documentation]     Print installed toolchains and library versions.
    [Tags]  core  warn-if-failed
    Run Static Test     ${RIOTTOOLS}/ci/print_toolchain_versions.sh

coccinelle
    [Documentation]     Run coccinelle static code analysis.
    [Tags]  core
    Run Static Test

cppcheck
    [Documentation]     Run CPPcheck static code analysis.
    [Tags]  core
    Run Static Test

externc
    [Documentation]     Check externc declaration in header files.
    [Tags]  core
    Run Static Test

flake8
    [Documentation]     Run flake8 checks on python scripts.
    [Tags]  core  warn-if-failed
    Run Static Test

headerguards
    [Documentation]     Check headerguards in header files.
    [Tags]  core
    Run Static Test

whitespacecheck
    [Documentation]     Check all files for trailing whitespaces.
    [Tags]  core
    Run Static Test
