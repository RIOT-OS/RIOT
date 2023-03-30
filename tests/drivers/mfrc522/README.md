# MFRC522 RFID Card Reader

## Overview

This test application demonstrates the usage of the MFRC522 driver interface
and can be used to test a MFRC522 RFID card reader with shell commands.

## Usage

The following shell commands are available:
* **reset** Reset the device.
* **antenna_on** Turn the antenna on.
* **antenna_off** Turn the antenna off.
* **set_antenna_gain** Set antenna gain.
* **get_antenna_gain** Get antenna gain.
* **firmware_version** Print the firmware version.
* **picc_dump** Dumps debug info about a scanned PICC.
* **scan_uid** Scan repeatedly until a PICC's UID is scanned.
* **set_uid** Set the UID of a scanned PICC.
* **self_test** Run the self test.
