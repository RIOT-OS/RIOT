Expected result
===============
The test will initialize all basic networking functionality including the
minimal NRF51822 BLE radio driver and run the shell providing netif shell
commands (**blesnd** and **ifconfig**). The transmitted BLE packets can
be captured and verified using SDR hardware like Ubertooth One or
HackRF.

Background
==========
Use the shell commands to test the link layer functionality of the minimal
NRF51822 BLE radio driver (nrfmin). Further information about the BLE protocol
standard and excpected packet data formats can be found within the official
**BLUETOOTH SPECIFICATION Version 4.2 [Vol 6, Part B]**

Usage
=====
The **blesnd** shell command is used to send BLE packets via the NRF51822
BLE radio driver. The following list of BLE Advertising Channel packet types
are supported:
- ADV_IND
- ADV_DIRECT_IND
- ADV_NONCONN_IND
- SCAN_REQ
- SCAN_RSP
- ADV_SCAN_IND_TYPE

The **ifconfig** shell command is used to set the requested BLE Advertising
Address, BLE Channel number and BLE Access Address (optional).

Examples
========
(Transmitted BLE packets were captured using Ubertooth One)

Shell commnad: `ifconfig 5 set adv_addr c2:5e:72:69:6f:74`
```
  success: set BLE advertising address on interface 5 to c2:5e:72:69:6f:74
```
Shell command: `blesnd 5 0`
```
40 06 74 6f 69 72 5e c2 85 0d da 
Advertising / AA 8e89bed6 (valid)/  6 bytes
    Channel Index: 37
    Type:  ADV_IND
    AdvA:  c2:5e:72:69:6f:74 (random)

    Data:  74 6f 69 72 5e c2
    CRC:   85 0d da
```
Shell command: `blesnd 5 0 08:09:52:49:4f:54:2d:4f:53:02:01:06:02:0a:00:03:03:20:22:07:ff:00:00:64:61:74:61`
```
40 21 74 6f 69 72 5e c2 08 09 52 49 4f 54 2d 4f 53 02 01 06 02 0a 00 03 03 20 22 07 ff 00 00 64 61 74 61 86 53 d7 
Advertising / AA 8e89bed6 (valid)/ 33 bytes
    Channel Index: 37
    Type:  ADV_IND
    AdvA:  c2:5e:72:69:6f:74 (random)
    AdvData: 08 09 52 49 4f 54 2d 4f 53 02 01 06 02 0a 00 03 03 20 22 07 ff 00 00 64 61 74 61
        Type 09 (Complete Local Name)
           RIOT-OS
        Type 01 (Flags)
           00000110
        Type 0a (Tx Power Level)
           0 dBm
        Type 03
            20 22
        Type ff
            00 00 64 61 74 61

    Data:  74 6f 69 72 5e c2 08 09 52 49 4f 54 2d 4f 53 02 01 06 02 0a 00 03 03 20 22 07 ff 00 00 64 61 74 61
    CRC:   86 53 d7
```
Shell command: `blesnd 5 1 02:5f:31:6c:a7:26`
```
41 0c 74 6f 69 72 5e c2 02 5f 31 6c a7 26 b5 d2 bf 
Advertising / AA 8e89bed6 (valid)/ 12 bytes
    Channel Index: 37
    Type:  ADV_DIRECT_IND

    Data:  74 6f 69 72 5e c2 02 5f 31 6c a7 26
    CRC:   b5 d2 bf
```
Shell command: `blesnd 5 2`
```
42 06 74 6f 69 72 5e c2 b8 3d f8 
Advertising / AA 8e89bed6 (valid)/  6 bytes
    Channel Index: 37
    Type:  ADV_NONCONN_IND

    Data:  74 6f 69 72 5e c2
    CRC:   b8 3d f8
```
Shell command: `blesnd 5 3 02:5f:31:6c:a7:26`
```
43 0c 74 6f 69 72 5e c2 02 5f 31 6c a7 26 36 73 99 
Advertising / AA 8e89bed6 (valid)/ 12 bytes
    Channel Index: 37
    Type:  SCAN_REQ
    ScanA: c2:5e:72:69:6f:74 (random)
    AdvA:  26:a7:6c:31:5f:02 (public)

    Data:  74 6f 69 72 5e c2 02 5f 31 6c a7 26
    CRC:   36 73 99
```
Shell command: `blesnd 5 4`
```
44 06 74 6f 69 72 5e c2 ff 6d 9e 
Advertising / AA 8e89bed6 (valid)/  6 bytes
    Channel Index: 37
    Type:  SCAN_RSP
    AdvA:  c2:5e:72:69:6f:74 (random)
    ScanRspData:

    Data:  74 6f 69 72 5e c2
    CRC:   ff 6d 9e
```
