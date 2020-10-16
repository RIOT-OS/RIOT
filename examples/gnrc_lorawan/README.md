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
The application listens to downlinks on Port 2 by default.
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
CFLAGS += -DLORAMAC_DEV_EUI_DEFAULT=\{0xAA\,0xAA\,0xAA\,0xAA\,0xAA\,0xAA\,0xAA\,0xAA\}
CFLAGS += -DLORAMAC_APP_EUI_DEFAULT=\{0xBB\,0xBB\,0xBB\,0xBB\,0xBB\,0xBB\,0xBB\,0xBB\}
CFLAGS += -DLORAMAC_APP_KEY_DEFAULT=\{0xCC\,0xCC\,0xCC\,0xCC\,0xCC\,0xCC\,0xCC\,0xCC\,0xCC\,0xCC\,0xCC\,0xCC\,0xCC\,0xCC\,0xCC\,0xCC\}
CFLAGS += -DLORAMAC_APP_SKEY_DEFAULT=\{0xDD\,0xDD\,0xDD\,0xDD\,0xDD\,0xDD\,0xDD\,0xDD\,0xDD\,0xDD\,0xDD\,0xDD\,0xDD\,0xDD\,0xDD\,0xDD\}
CFLAGS += -DLORAMAC_NWK_SKEY_DEFAULT=\{0xEE\,0xEE\,0xEE\,0xEE\,0xEE\,0xEE\,0xEE\,0xEE\,0xEE\,0xEE\,0xEE\,0xEE\,0xEE\,0xEE\,0xEE\,0xEE\}
CFLAGS += -DLORAMAC_DEV_ADDR_DEFAULT=\{0xFF\,0xFF\,0xFF\,0xFF\}
```

## Send data

After join, send data using `send` command. E.g to send "Hello RIOT!" to port 2:

```
send 3 "Hello RIOT!" 2
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
send 3 "Join the RIOT!"
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
send 3 "My confirmable message"
```

And unconfirmable messages:

```
ifconfig 3 -ack_req
send 3 "My unconfirmable message"
```

Current state and future plans
============

The current GNRC LoRaWAN stack is still in an experimental state. It's still
not compliant with the LoRaWAN specification because some features like duty
cycle restrictions and some FOps are missing. Work in progress.

Next steps:
- Add other regions (US915, etc)
- Add Adaptive Data Rate
- Add Duty Cycle restrictions
- Add support for RTC
