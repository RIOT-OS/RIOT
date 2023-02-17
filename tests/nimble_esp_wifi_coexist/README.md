# About

This application tests the coexistence of Bluetooth and WiFi interface.
For that purpose it uses the `NimBLE` BLE stack as a scanner and the
ESP32x WiFi interface simultaneously.

# Usage

Comile and flash the application with command
```
CFLAGS='-DWIFI_SSID=\"myssid\" -DWIFI_PASS=\"mypass\"'
BOARD=esp32-wroom-32 make -C tests/nimble_esp_wifi_coexist flash term
```
Once the test application is flashed and the WiFi connection is established,
ping the node from the host with command
```
sudo ping6 fe80::xxxx:xxxx:xxxx:xxxx%eth0 -i 0.01 -s 1280
```
to produce network load and execute the command `scan` on the node.
The scan should still work.
