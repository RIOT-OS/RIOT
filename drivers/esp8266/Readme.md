# This is the RIOT OS driver for the ESP8266 low cost 802.11b/g/n module
We're running the standard [AT Command Firmware](https://github.com/espressif/ESP8266_AT) on the ESPs. [Command Set Quick Reference](https://room-15.github.io/blog/2015/03/26/esp8266-at-command-reference/)
## Communication Handling
The Communication happens in seuqential executed requests and responses. As long as response is pending, no new request can be performed. The requesting function awaits the response. The receiver callback function compares the end of the receive buffer with all the possible terminations and messages the requesting function if one is found. Then the response can be processed.
