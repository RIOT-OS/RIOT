GNRC LoRaWAN application
=============================

This application is a showcase for testing GNRC LoRaWAN stack. You should be
able to send and receive LoRaWAN packets and perform basic LoRaWAN commands
(Link Check).

The MAC layers still doesn't implement any duty cycle restriction mechanism.
However, it respects the retransmission procedure.

Only Class A in EU868 and IN865 regions are supported so far.

Usage
=====

It's necessary to join the LoRaWAN network either via OTAA or ABP.
All keys, addresses and EUIs are in network endian (big endian).
Region need to be set in the Makefile.

## OTAA

Join by OTAA is set by default.
Set the Application Key, Device EUI and Application EUI using ifconfig. Assuming
the interface pid is 3:

```
ifconfig 3 set deveui AAAAAAAAAAAAAAAA
ifconfig 3 set appeui BBBBBBBBBBBBBBBB
ifconfig 3 set appkey CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
ifconfig 3 up
```
If Chirpstack is being used, the AppEUI is ignored.

Wait for 5-6 seconds. Type `ifconfig`. The link status should be `up`:

```
Iface  3  HWaddr: 26:01:27:2F  Frequency: 868500000Hz  BW: 125kHz  SF: 7
          CR: 4/5  Link: up
          TX-Power: 14dBm  State: SLEEP  Demod margin.: 0  Num gateways.: 0
          IQ_INVERT
          RX_SINGLE OTAA

```

## ABP

Deactivate OTAA using ifconfig and set the AppSKey, NwkSKey and DevAddr;

```
ifconfig 3 -otaa
ifconfig 3 set appskey DDDDDDDDDDDDDDDD
ifconfig 3 set nwkskey EEEEEEEEEEEEEEEE
ifconfig 3 set addr FFFFFFFF
ifconfig 3 up
```

The join by ABP occurs immediately.

Alternatively all keys can be set using CFLAGS so it's only required to
select join mode and type `ifconfig <if_pid> up`.

E.g in the application Makefile:

```
CFLAGS += -DCONFIG_LORAMAC_DEV_EUI_DEFAULT=\"AAAAAAAAAAAAAAAA\"
CFLAGS += -DCONFIG_LORAMAC_APP_EUI_DEFAULT=\"BBBBBBBBBBBBBBBB\"
CFLAGS += -DCONFIG_LORAMAC_APP_KEY_DEFAULT=\"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC\"
CFLAGS += -DCONFIG_LORAMAC_APP_SKEY_DEFAULT=\"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\"
CFLAGS += -DCONFIG_LORAMAC_NWK_SKEY_DEFAULT=\"EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\"
CFLAGS += -DCONFIG_LORAMAC_DEV_ADDR_DEFAULT=\"FFFFFFFF\"
```

## Send data

After join, send data using `txtsnd` command:
```
txtsnd <if> <hex_port> <data>
```

E.g to send "Hello RIOT!" to LoRaWAN port 123 (hex 0x7B) via interface 3:

```
txtsnd 3 7B "Hello RIOT!"
```

## Changing datarate of transmission
Use `ifconfig` to change the datarate of the transmission. E.g to set the DR to
2:

```
ifconfig 3 set dr 2
```

## Perform a Link Check

Use `ifconfig` to request a Link Check on the next transmission:

```
ifconfig 3 link_check
```

Send some data. The result of the Link Check request can be seen with
`ifconfig`.

```
ifconfig 3 link_check
txtsnd 3 01 "Join the RIOT!"
```

Check demodulation margin and number of gateways using `ifconfig`

```
ifconfig
Iface  3  HWaddr: 26:01:2C:EA  Frequency: 867500000Hz  BW: 125kHz  SF: 7
          CR: 4/5  Link: up
          TX-Power: 14dBm  State: SLEEP  Demod margin.: 14  Num gateways.: 2
          IQ_INVERT
          RX_SINGLE OTAA

```

## Confirmable and unconfirmable messages

Use `ifconfig` to set the `ack_req` flag. With this flag on, messages are
confirmable.

E.g send confirmable messages:

```
ifconfig 3 ack_req
txtsnd 3 01 "My confirmable message"
```

And unconfirmable messages:

```
ifconfig 3 -ack_req
txtsnd 3 01 "My unconfirmable message"
```

## Receiving data

Schedule a downlink for the LoRaWAN node in the Application Server. If using
TTN, this can be done under `Applications > <APP> > Devices > <DEV> > Overview`
and then check the `Downlink` section.

After sending data, the LoRaWAN Network Server will reply with the downlink
data. For simplicity, this application is configured to dump downlink data to
[GNRC pktdump](https://api.riot-os.org/pktdump_8h.html).

E.g:
```
PKTDUMP: data received:
~~ SNIP  0 - size:   2 byte, type: NETTYPE_UNDEF (0)
00000000  AA  AA
~~ SNIP  1 - size:   9 byte, type: NETTYPE_NETIF (-1)
if_pid: 3  rssi: -32768  lqi: 0
flags: 0x0
src_l2addr: (nil)
dst_l2addr: 01
~~ PKT    -  2 snips, total size:  11 byte
```

This downlink was sent to port 1 (check `dst_l2addr` field)

Current state and future plans
==============================

The current GNRC LoRaWAN stack is still in an experimental state. It's still
not compliant with the LoRaWAN specification because some features like duty
cycle restrictions and some FOps are missing. Work in progress.

Next steps:
- Add other regions (US915, etc)
- Add Adaptive Data Rate
- Add Duty Cycle restrictions
- Add support for RTC
