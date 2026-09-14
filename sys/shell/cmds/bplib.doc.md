@defgroup   sys_shell_commands_bplib bplib shell command
@ingroup    sys_shell_commands
@brief      bplib shell command for configuration

## Available Subcommands

The `bplib` shell command currently provides the following subcommands.

The shell command is only intended as a helper for an existing implementation,
an app with only the shell command will not be functional due to missing CLA
initialization and missing egress of received bundles.

General information:

- A \<channel\> is an int in [0, BPLIB_MAX_NUM_CHANNELS)
- A \<contact\> is an int in [0, BPLIB_MAX_NUM_CONTACTS)

### Subcommand: send \<channel\> \<PAYLOAD\>

Ingresses a bundle with the given string payload over the given channel.

### Subcommand: channel \<channel\> [\<new_state\>]

Either prints the current state of the given channel when `new_state` is not
given, or tries to transition to the `new_state`.

The `new_state` is one of "setup", "start", "stop" and "teardown".

### Subcommand: contact \<contact\> [\<new_state\>]

Either prints the current state of the given contact when `new_state` is not
given, or tries to transition to the `new_state`.

The `new_state` is one of "add", "start", "stop" and "remove".

### Future efforts / Not yet implemented

- Configuration of NC, including but not limited to:
  - Channel Configuration: e.g. which blocks to include, which CRC to use
  - Contact Configuration: e.g. which remote to send to
- Configuration of general values, many of which are currently compile time
  constants. That would include for example the local EID.
