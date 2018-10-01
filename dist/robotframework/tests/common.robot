*** Settings ***
Documentation       A collection of common tests and logging procedures that run
...                 before any other test suite. Its purpose is to verify and
...                 log the system environment under which subsequent tests run.

Default Tags        common  warn-if-failed

Suite Teardown      Terminate All Processes     kill=True

Library             Process
Library             OperatingSystem

*** Test Cases ***

Log Toolchain Versions
    [Documentation]     Write installed toolchain and library versions to the log.
    ${result}=          Run Process  %{RIOTTOOLS}/ci/print_toolchain_versions.sh  shell=True
    Log                 ${result.stdout}

Log Commit Hash
    [Documentation]     Write the commit hash of the curren git HEAD to the log.
    ${result}=          Run Process  git  rev-parse  HEAD  shell=True
    Log                 ${result.stdout}
