## decadriver

This test application can be used to verify the essential operations of a DW3000
UWB module using the decadriver package. The DW3000 supports UWB channels 5 and
9 and can be used for UWB ranging or data transmission.

This test allows to verify the following capabilities:

- Basic SPI communication with the module & module startup
- Sending of frames
- Reception of frames
- Automatic generation (inside the DW3000) of ACKs
- Frame filtering
- Sleeping & Wakeup

## Usage

Compile the test for the board you are using and add the `_PARAMS` definitions
for the pins used to connect to the DW3000.

## Expected results

### SPI communication & module startup

The board should recognize the DW3000 chip and show successful initialization:
```
main(): This is RIOT! (Version: [...])
[deca init] detected device id: deca0302
[deca init] DW3xxx reached IDLE_RC
[deca init] Device initialized and configured
```

### Reception of frames & Sending of frames & Auto ACK

To send and receive frames you need a second DW3000 with the test flashed on the
MCU of your choice.

Use the `dw_addr` shell command to configure the MAC16 address of the module.
In a real application both devices should have different addresses, but here it
is not really necessary. By default the address is set to `0x0001`.

#### Device 1
```
> dw_addr 1                                     # Set address to 0x0001
dw_addr 1
> dw_send 2 "Hello World!"                      # Send data to 0x0002
dw_send 2 "Hello World!"
[deca] TX sending (CRC bytes not included):
618800CADE0200010048656C6C6F20576F726C6421
[deca] TX complete
[deca] RX ok with length 5:                     # Incoming ACK
020000B8B5
```

#### Device 2
```
> dw_addr 2                                     # Set address to 0x0002
dw_addr 2
[deca] RX ok with length 23:
618800CADE0200010048656C6C6F20576F726C6421349B
[deca] TX complete                              # Outgoing ACK
```

The frames are 802.15.4 MAC frames. The frame generation is currently hard-coded
and producing a data frame requesting an ACK. The DW3000 will handle the
generation and verification of the CRC, i.e. it does not have to be calculated
on the MCU.
```
6188 00 CADE 0200 0100 48656C6C6F20576F726C6421 349B
|    |  |    |    |    |                        |
Hdr. |  PAN  |    Src  |                        CRC
     |       Dest      Payload "Hello World!"
     Seq No.
```

### Frame filtering

Frame filtering is enabled in this example. This means, frames with a
destination other than the own address will not be processed further (But they
will appear as an RX error).

When sending a frame to another destination, the other device does not show the
whole frame:
#### Device 1
```
> dw_addr 1                                     # Set address to 0x0001
dw_addr 1
> dw_send 3 "Hi!"                               # Send to 0x0003
dw_send 3 "Hi!"
[deca] TX sending (CRC bytes not included):
618801CADE03000100486921
[deca] TX complete                              # Note: No ACK
```

#### Device 2
```
> dw_addr 2                                     # Set address to 0x0002
dw_addr 2
[deca] RX error: 0x23800f07                     # 0x20000000 shows frame filter
```

### Sleeping & Wakeup

The DW3000 devices can sleep to reduce power consumption. Use the `dw_sleep`
shell command to enter DEEPSLEEP state.

Now, commands like `dw_addr` and `dw_send` on this device will likely not arrive
(unless the SPI CS line is held down long enough during a normal SPI
transaction to cause a wakeup).
Frames sent from other devices will also not arrive.

Use `dw_wakeup` to go back to the RX state, and frames can be sent and received
again.
