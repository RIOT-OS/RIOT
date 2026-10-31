@defgroup   sys_shell_commands_bplib bplib shell command
@ingroup    sys_shell_commands
@brief      bplib shell command for configuration

## Available Subcommands

The `bplib` shell command currently provides the following subcommands.

The shell command is only intended as a helper for an existing implementation,
an app with only the shell command will not be functional due to missing CLA
initialization and missing egress of received bundles.

Most setter commands are wrappers for the NC helper functions, see
@ref pkg_bplib_nc.
Not all return values are parsed back to understandable output, refer to this
documentation and `[bplib]/inc/bplib_api_types.h` to understand the return
values in such error cases.

General information:

- A \<channel\> is an int in [0, BPLIB_MAX_NUM_CHANNELS)
- A \<contact\> is an int in [0, BPLIB_MAX_NUM_CONTACTS)

### Subcommand: info

Prints an overview of the current node configuration, listing the current
channels and contacts and its EIDs and routes.

Additionally, it prints information about the storage and memory pool usage.

### Subcommand: send \<channel\> \<PAYLOAD\>

Ingresses a bundle with the given string payload over the given channel.

### Subcommand: channel \<channel\>

Allows configuration of the channel at the given index. The are the following
further subcommands:

#### Sub-Subcommand: service \<service_no\>

Allows querying or setting the service number of this channel. A service number
can be a uint64_t.

Example: `"bplib channel 0 service 123"` to set the service number of this
channel to 123.

#### Sub-Subcommand: dest \<eid\>

Allows querying or setting the destination of this channel.

bplib does not support the `dtn:` scheme at the moment, and `dtn:none` is
understood by bplib but not a valid destination.
Also, while three digit IPN values can be set, bplib might not treat them
correctly at this point. 2 digit IPN EIDs are the preferred scheme to use.

Example: `"bplib channel 0 dest ipn:500.123"` to set the service number of this
channel to IPN node 500, service number 123.

#### Sub-Subcommand: report_to \<eid\>

Allows querying or setting the report to node of this channel.

Behaves like the `dest` subcommand. Use `dtn:none` to disable reporting.

Reporting is only a suggestion, nodes do not have to send out reports (and bplib
does not generate them either at this time).

Example: `"bplib channel 0 report_to ipn:500.123"` to set the service number of this
channel to IPN node 500, service number 123.

#### Sub-Subcommand: block \<block_code\>

The \<block_code\> is one of the following:
- `pn`: Previous Node
- `ba`: Bundle Age
- `hc`: Hop Count
- `pl`: Payload
- `ct`: Custody Transfer

In another level, each of these canonical blocks can be configured in respect to:

- `include` Whether the block shall be included. Entered as bool string, as
  understood by @ref scn_bool_str().
- `crc`: CRC of the block. Can be one of "none", "CRC16" and "CRC32".
- `num`: Block number. Must be unique among all canonical blocks. 0 is reserved
  implicitly for the primary block and the payload block must have block number 1.
- `flags`: Block Processing flags. Entered as hex number.

If the final argument setting the value is not given, the current value will be
printed.

@note Setting the values is only possible if the channel is removed, i.e. a
      started channel cannot be changed. Querying the value is always possible.

@note Some settings cannot be changed as required by the BPv7 specification, e.g.
      the Bundle Age inclusion depends on the availability of the DTN time, the
      setting is ignored. Similarly, the Payload can also not be excluded.

Example: `"bplib channel 0 block pl crc CRC32"` to set the Payload Block of channel
0 to use CRC32.

Example: `"bplib channel 0 block ct include yes"` to include the Custody Transfer
Block of channel 0, thus enabling custody transfer.

#### Sub-Subcommand: max_hops \<uint8_t\>

Sets or gets the number of allowed hops.
Has to be in [1, 255].

The Hop Count block (`hc`) still has to be enabled, otherwise this setting does
nothing.

Example: `"bplib channel 0 max_hops 10"` to set the allowed number of hops to 10.

#### Sub-Subcommand: flags \<bundle_flags_hex\>

Sets or gets the additional bundle processing flags. bplib might add additional
flags (like the admin record flag for admin records). The number is input and
printed as hex.

These flags can for example be used to request status reports on certain events,
like bundle deletion. Refer to RFC 9171 4.2.3 for more information.

Example: `"bplib channel 0 flags 0"` to set the default of no additional flags.

#### Sub-Subcommand: crc \<CRC16 / CRC32\>

Sets or gets the CRC type to use for the primary block. "none" is not supported
for the primary block.

Example: `"bplib channel 0 flags CRC16"` to set the primary block to use CRC16.

#### Sub-Subcommand: lifetime \<lifetime_ms\>

Sets or gets the lifetime of the bundle in [ms]. After this time has passed, the
bundle will be deleted when it has not been delivered.

Example: `"bplib channel 0 lifetime 60000"` to set the lifetime to 60 seconds.

#### Sub-Subcommand: state [\<new_state\>]

Either prints the current state of the given channel when `new_state` is not
given, or tries to transition to the `new_state`.

The `new_state` is one of "add", "start", "stop" and "remove".

Example: `"bplib channel 0 state stop"` to stop channel 0.

### Subcommand: contact \<contact\>

Allows configuration of the contact at the given index. The are the following
further subcommands:

#### Sub-Subcommand: state [\<new_state\>]

Either prints the current state of the given contact when `new_state` is not
given, or tries to transition to the `new_state`.

The `new_state` is one of "setup", "start", "stop" and "teardown".

Example: `"bplib contact 0 state stop"` to stop contact 0.

### Future efforts / Not yet implemented

- Configuration of NC, including but not limited to:
  - Contact Configuration: e.g. which remote to send to
