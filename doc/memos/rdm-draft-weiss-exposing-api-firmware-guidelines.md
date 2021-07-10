- rdm-draft-weiss-exposing-api-firmware-guidelines.md
- Title: Guidelines for Write Firmware to Expose RIOT APIs
- Author: Kevin Weiss
- Status: In-progress
- Type: Process
- Date: December 2018

## Abstract

In order to improve the testing and interface tools for RIOT a standard way of
exposing API calls to a higher level should be defined. This document describes
a set of guidelines indicating semantics and basic design on the firmware level
that allow interfaces and tools to be build upon.

## Status

This document is currently under open discussion. Its content is licensed with
a Creative Commons CC-BY-SA license.

## Terminology
This memo uses the RFC2119 terminology and the following acronyms:
API: Application Programming Interface
cmd: Command
msg: Message

# 1. Introduction
The guidelines are used indicate the semantics and design when writing
firmware to expose APIs through the shell.

_NOTE: It may not be applicable for every API call_

# 2. Test Firmware Design

The following guidelines are used when writing the C program to be flashed to
the board.

## 2.1. Test Firmware Declarations

- Tests should interact through a shell
- Commands should be synchronous
- Tests should have a `get_metadata` command
- The `get_metadata` command should contain at least board name, and
application name
- Tests should have a `echo <random 8 hex digits>` command
- Test commands should provide similar API/function calls
- Tests that require `test_<name of test command>` command only if interaction
with the shell causes inaccuracies
- Commands that use `test_<name of test command>` should give as much data
and information as possible to reproduce command and allow higher level
functionality
- Asynchronous actions should be hidden with the `test_<name of test command>`
format
- There should be a standard interface for every command sent
[cmd, data, msg, result]
- [cmd] should only be provided once for each command
- [cmd] should allow the user to reproduce the step, this may be the
API/function call with the variables or a descriptive name for a set of calls
- [msg] should be optional
- [msg] if used, may be provided several times
- [msg] should not be actionable and only contain debug/log
information
- [msg] should be anything that cannot be parsed or recognized
- [data] should be optional
- [data] if used, may be provided several times
- [data] should be actionable and adhere to standard JSON formatting
- [result] must be provided exactly once for each command
- [result] value should only be "SUCCESS" or "ERROR"
- [result] should be the last value in the command response.

## 2.2.1 Expected Response Format From Commands In Firmware

- [cmd] -> `printf("{\"cmd\": \"api_call(%i, %i, &input_data)\"}\n", arg1, arg2)`
- [msg] -> `printf("This is useful debug info, anything that has no
structure will be saved as a [msg]\n")`
- [data] -> `printf("{\"data\": %i}\n", data)`
- [data] -> `printf("{\"data\": [%i, %i]}\n", data[0], data[1])`
- [data] -> `printf("{\"data\": \"%s\"}\n", actionable_string)`
- [result] -> `printf("{\"result\": \"SUCCESS\"}\n")` or
`printf("{\"result\": \"ERROR\"}\n")`

## 2.2.2 Expected Response Format Grammar
```
command := "{\"cmd\":" STRING_WITH_QUOTES "}" EOL
message := STRING EOL
data := "{\"data\":" <json_compatible_data> "}" EOL
result := "{\"result\":" RESULT_VALUE "}" EOL

EOL := \"\n\"
RESULT_VALUE := ("SUCCESS") | ("ERROR")
STRING_WITH_QUOTES := /"(?:[^"\\]|\\.)*"/
```

## 2.3.  Basic Real Example
This example show how an real api call should be exposed.
```c
int cmd_i2c_read_reg(int argc, char **argv)
{
    // Do parsing
    ...
    printf("{\"cmd\": \"i2c_read_reg(%i, 0x%02X, %02X, &data, 0x%02X)\"}\n",
           dev, addr, reg, flags);
    ret = i2c_read_reg(dev, addr, reg, &data, flags);
    if (ret == I2C_ACK) {
        printf("Successfully read from register\n");
        printf("{\"data\": 0x%02X}\n", data);
        printf("{\"result\": \"SUCCESS\"}\n");
    }
    else {
        printf("Errorcode: %s\n", _get_error_code_string(ret));
        printf("{\"data\": %i}\n", ret);
        printf("{\"result\": \"ERROR\"}\n");
    }
    return ret;
}
```

## 2.4. Multiple Data Example

This example shows how large amounts of data can be sent with one call.
It shows multiple data responses.
It also shows a [cmd] field that doesn't exactly match the api call but
gives enough information to be able to reproduce.

 ```c
 int cmd_get_lots_of_data(int argc, char **argv)
 {
     // Do parsing
     ...
     printf("{\"cmd\": \"get_data_api_command(get_data_arg=[0 to %i])\"}\n",
            amount_of_api_calls)
     printf("Getting several values for the api command\n")
     for (i = 0; i < amount_of_api_calls; i++) {
         data = get_data_api_command(i);
         prqintf("{\"data\": 0x%X}\n", data);
     }
     printf("{\"result\": \"SUCCESS\"}\n");
 }
 ```

## Acknowledgements

Thanks to Gaëtan Harter, Thomas C. Schmitt, Juan Carrano for their
comments and suggestions.

## References

- Internet Engineering Task Force ([IETF](http://ietf.org/))

## Revision

- Rev0: initial document

## Contact
The author of this memo can be contacted via email at
kevin.weiss@haw-hamburg.de
