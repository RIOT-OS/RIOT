Wireshark Dissector for LwMAC protocol
======================================

Usually dissectors are stored in `/usr/share/wireshark`, so copy `lwmac.lua` and append

```lua
dofile(DATA_DIR.."lwmac.lua")
```

to `init.lua`. Make sure that Lua scripts are not disabled: `disable_lua = false`
