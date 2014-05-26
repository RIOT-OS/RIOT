# RIOT native wireshark dissector
This allows wireshark to parse packets send over TAP by RIOT's native.

## Installation
Just copy the script ``riot.lua`` to ``$HOME/.wireshark/plugins`` and restart
Wireshark.

## Usage
### Debian/Ubuntu
Ensure that dumpcat is available for you as non-superusers:

```bash
sudo dpkg-reconfigure wireshark-common
sudo usermod -a -G wireshark $USER
```

## Configuration
Depending on what you want to send over the native TAP you might want to change
the next header dissector. Currently we have included the dissectors for
IEEE 802.15.4 and 6LoWPAN into the script file, but anything is thinkable.
Refer to the wireshark manual to get the protocol names, e.g.:

```lua
    local next_dis = Dissector.get("ipv6")
```
