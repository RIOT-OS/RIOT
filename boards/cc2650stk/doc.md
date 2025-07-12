@defgroup    boards_cc2650stk CC2650STK
@ingroup     boards
@brief       Support for the SimpleLink™ CC2650 sensor tag

![CC2650STK](http://www.ti.com/diagrams/cc2650stk_cc2650stk.jpg)

The CC2650STK is an 'IoT kit' with 10 sensors, a fancy case, and a radio unit
that is capable of irradiating IEEE802.15.4 and BLE (or SMART or whatever they
call it now).

 - [Official homepage](https://www.ti.com/tool/cc2650stk)
 - [Platform](https://www.ti.com/product/CC2650) <- CPU data sheet here

Use `BOARD=cc2650stk` for building RIOT for this platform.

## Components

| MCU                       | CC2650f128                    |
|:------------------------- |:----------------------------- |
| Family                    | ARM Cortex-M3                 |
| Vendor                    | Texas Instruments             |
| RAM                       | 20KiB                         |
| Flash                     | 128KiB                        |
| Frequency - Standby       | 31.26kHz, 32kHz or 32.768kHz  |
| Frequency - Active / Idle | 48MHz                         |
| RF core                   | ARM Cortex-M0 CPU, 4KiB RAM   |
| Timers                    | 4x 32-bit                     |
| ADCs                      | 1x 12-bit (8 channels)        |
| UARTs                     | 2                             |
| SPIs                      | 2                             |
| I2C                       | 1                             |
| I2S                       | 1                             |
| Datasheet                 | [Datasheet](http://www.ti.com/lit/ds/symlink/cc2650.pdf) |
| Reference Manual          | [Reference Manual](https://www.ti.com/lit/ug/swcu117h/swcu117h.pdf) |

## Implementation Status

It's an ongoing process...

| Module         | Status        | |
|:------------- |:--------------------- |:--------------------- |
| Cortex-M3  | Partial support   | Missing: energy saving features |
| UART | OK  | |
| LEDS | OK | |
| Hardware buttons | OK | |
| Timers     | Unstable  | See [#5631](https://github.com/RIOT-OS/RIOT/issues/5361)|
| RF core | Work in progress | See [here](https://github.com/yogo1212/RIOT/tree/cc26x0_rfc). |

## Toolchains

The arm-none-eabi toolchain works fine. You can get it
[here](https://launchpad.net/gcc-arm-embedded/+download).

## Programming and Debugging

You'll need [debugging hardware](https://processors.wiki.ti.com/index.php?title=CC13xx_CC26xx_Tools_Overview#Debuggers).
So far, the [XDS110 debug probe](https://www.ti.com/tool/CC-DEVPACK-DEBUG) has
been tested. That bugger requires you to load a firmware onto it each time it
powers up. The tool is contained in the Uniflash utility or the `CodeComposer
Studio` from TI. Look for a folder called `uscif` in the installation directory,
go to the folder `xds110` therein, and follow the instructions in the
`ReadMe.txt`.

The process is relying on proprietary TI softsoftware. If you're on Windows
you can use the stuff linked to on the product websites.

On Linux, there's an application called
[Uniflash](https://www.ti.com/tool/uniflash). Sadly, you'll have to install the
whole IDE just to get the scripting interface :-[

No idea about macOS.

In order to flash the CC2650STK you need to plug the XDS110 probe through the
JTAG and so-called "DevPack" connectors. Note that the back of the SensorTag
case has a removable plastic cut-out: as a result the XDS110 can be used while
the CC2650STK is still protected in its case. For the flashing process to be
successful, the CC2650STK needs to be powered by a working battery.

Once your application code has compiled, you need to indicate the path to
your UniFlash tool. You can do it in two ways:

1. Add `export UNIFLASH_PATH = your_path` to
`boards/cc2650stk/Makefile.include` and then run `make BOARD=cc2650stk flash`

2. Use the command `make BOARD=cc2650stk UNIFLASH_PATH=your_path flash` to
flash the board.
If you're lazy you can create an alias to do it with the following command:
`alias cc2650stkmake='make BOARD=cc2650stk UNIFLASH_PATH=your_path flash'`

In both cases you can add `term` at the end of the `make` command to enter
the pyterm console directly. After the board has been flashed, it needs to be
manually reset using the reset button on the XDS110 probe.

***
WORK IN PROGRESS
***
# **Bluetooth Low Energy on the CC2650STK**
This section is meant to provide information regarding the BLE specifications
as well as their implementation on the CC2650.

BLE support two main data format: one for advertising channels packets and
the other for data channels packets. Both formats will be described in details
in the first half of the guide. The second half will provide guidance on how to
implement the most common roles of a BLE network.

References: [BlueTooth Core Specification v4.2](https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439),
[Core Specification Supplement v6](https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=302735),
[BLE Becons by TI](https://www.ti.com/lit/an/swra475a/swra475a.pdf)

## BLE packet format for advertising channels

![pdu](https://cloud.githubusercontent.com/assets/14371243/15826178/dac1490a-2c06-11e6-95fb-4d27bdb7fd8d.png)

| Field                   | Size    | Definition        | Description |
|:------------------------|:--------|:------------------|:----------------------|
| Preamble                | 1 byte  | BLE specification | Always 10101010b for advertising channel packets  |
| Access Address          | 4 bytes | BLE specification | Always 0x8E89BED6 for advertising channel packets |
| Payload Data Unit (PDU) | 2 to 257 bytes | User defined | Advertising Channel PDU |
| CRC                     | 3 bytes | BLE specification | Result of a polynomial calculated based on the PDU |

### PDU for advertising channels

The RF Core will automatically build the PDU based on the content of the
radio operation command. The following table describes the format of the PDU for
advertising channels, as well as the corresponding variables in the RIOT
command.

```
typedef struct __attribute__ ((aligned(4))) {
    radio_op_command_t ropCmd;
    uint8_t channel;
    struct {
     uint8_t init:7;
     uint8_t bOverride:1;
 } whitening;
    void *pParams; //points toward a structure of type
rfc_ble_param_advertiser_t
    void *pOutput;
} ble_rop_cmd_t;
```

| Field                 | Size    | RIOT variable | Description|
|:----------------------|:--------|:--------------|:------------------------------------------------------------|
| PDU Type              | 4 bits | `ropCmd.commandNo` | PDU Type is solely dependent on the command type. See below. |
| RFU                   | 2 bits | - | Reserved for Future Use (RFU): [You can't touch this.](https://www.youtube.com/watch?v=otCpCn0l4Wo) Assumed to be 0.|
| TxAdd                 | 1 bit  | `pParams->advConfig.deviceAddrType` | The field value is specific to the PDU type. |
| RxAdd                 | 1 bit  | - | The field value is specific to the PDU type. According to the TI documentation ([23.6.4.4](https://www.ti.com/lit/ug/swcu117h/swcu117h.pdf)), this field is not available to configure and thus assumed to be 0. |
| Length                | 6 bits | `pParams->advLen` + 6| Indicates the length of the payload field in bytes. 6 is added to account for the advertiser address. The payload length ranges from 6 to 37 bytes. |
| RFU                   | 2 bits | - | Reserved for Future Use (RFU): [You can't touch this.](https://www.youtube.com/watch?v=otCpCn0l4Wo) Assumed to be 0 |
| Advertiser address | 6 bytes | `pParams->pDeviceAddress` | First element of the payload. The different formats of address types are illustrated [here](https://cloud.githubusercontent.com/assets/14371243/15826564/4c7f5f54-2c08-11e6-8051-dc0a018f6e42.png).|
| Advertising data (AD) | 0-31 bytes | `pParams->pAdvData` | Second element of the payload. Warning: if `pParams->advLen=0` the advertiser data are disregarded.|

As mentioned in the table above, the content of some of the PDU fields are
intertwined. These relationships are established in the following tables. Please
note that the configuration of the RxAdd is not possible on the CC2650 (RxAdd=0
∀ Command No) and thus displayed for information purposes only.

| Command No | Denomination in BLE specs  | Value of PDU Type | Value of TxAdd and RxAdd | Description |
|:-----------|:---------------------------|:------------------|:---------------|:------------|
| CMD_BLE_ADV | ADV_IND | 0000b | TxAdd: advertiser's address is public (0) or random (1). RxAdd: not defined. | Connectable undirected advertising event |
| CMD_BLE_ADV_DIR | ADV_DIRECT_IND | 0001b | TxAdd: advertiser's address is public (0) or random (1). RxAdd: initiator's address is public (0) or random (1). | Connectable directed advertising event |
| CMD_BLE_ADV_NC | ADV_NONCONN_IND | 0010b | TxAdd: advertiser's address is public (0) or random (1). RxAdd: not defined. | Non-connectable undirected advertising event |
| CMD_BLE_ADV_SCAN | ADV_SCAN_IND | 0110b | TxAdd: advertiser's address is public (0) or random (1). RxAdd: not defined. | Scannable undirected advertising event|

The Advertising Data (AD) field can be populated with a set of basic data
types described in chapter 1 of the [BLE core specification supplement](https://www.bluetooth.com/specifications/adopted-specifications),
e.g. service UUID, flags or manufacturer specific data.

## BLE packet format for data channels

| Field                   | Size    | Definition        | Description |
|:------------------------|:--------|:------------------|:----------------------|
| Preamble                | 1 byte  | BLE specification | Equal to 10101010b (LSB of Access Address is 0) or 01010101b (LSB of Access Address is 1) for data channel.|
| Access Address          | 4 bytes | BLE specification | Randomly generated under constraints. |
| Payload Data Unit (PDU) | 2 to 257 bytes | User defined | Data Channel PDU|
| CRC                     | 3 bytes | BLE specification | Result of a polynomial calculated based on the PDU |

## Configuring a BLE beacon

In order to configure a BLE beacon, a radio operation command `cmd` of type
`ble_rop_cmd_t` must be sent to the RF core.

1. Define the parameters of the command by filling a structure of type
`rfc_ble_param_advertiser_t`. Minimum requirements are as follows:
 1. Set `params->endTime = 0` and `params->endTrigger.triggerType = 0` unless
you want to do something fancy timewise.
 2. Set `params->pDeviceAddress` to an `unsigned char` array containing the
[48-bit MAC address](https://en.wikipedia.org/wiki/MAC_address) of the MCU. The
MAC-48 identifier can be obtained from the function `ble_mac48_get()` defined in
`cpu/cc26x0/periph/cpuid.c`. This function returns a **public device address**,
defined in the factory config data (FCFG->MAC_BLE_n): bits 0-24 contain a serial
number unique to the MCU, while the bits 24-48 contain the Organizationally
Unique Identifier
([OUI](https://en.wikipedia.org/wiki/Organizationally_unique_identifier)) for
Texas Instrument, i.e. b0:b4:48.
 3. Set `params->pAdvData` to a byte array containing the data to be
broadcasted. The size of the array must be written as `params->advLen`. Since
the advertising packet is non-connectable, the Flag data type (described in
[Core Specification Supplement v6](https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=302735))
may be omitted from the advertising payload.
2. Configure the command itself by filling a structure of type
`ble_rop_cmd_t`. Minimum requirements are as follows:
 1. Set `cmd.ropCmd.commandNo = CMDR_CMDID_BLE_ADV_NC`. The PDU type will be
set accordingly by the RF core.
 2. Set `cmd.condition.rule = R_OP_CONDITION_RULE_NEVER` unless you plan on
executing an additional command via `cmd.pNextOp`.
 3. Set `cmd.whitening.bOverride = 0` and `cmd.whitening.init = 0` unless you
understand how to use it.
 4. Set `cmd.pParams` to the address of the `rfc_ble_param_advertiser_t`
structure defined in 1).
 5. Set the advertising channel via `cmd.channel`. There are 3 possible
channels, each identified by a `uint8_t`: 37, 38 or 39. If you want to broadcast
on all three channels you can create three ble_rop_cmd_t commands and chain them
via `cmd.pNextOp`.
3. Send the command to be executed to the RF core via the `rfc_send_cmd()`
function
