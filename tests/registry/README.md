## RIOT Registry test application
This application tests the handling of runtime configuration parameters using
the RIOT Registry module. Through shell commands the configuration parameters
can be listed, set, get and loaded from / saved to a storage facility, and the
content of the storage can be dumped.

3 configuration parameters are defined:

| Group | Name | Type |
| ----- | ---- | ---- |
| app | opt1 | int64 |
| app | opt2 | int8 |
| app | bytes | bytes |

### Storage facility
By default the `dummy` (RAM) 'storage' is used to test the registry, to change
to another of the implemented storages (e.g. EEPROM) modify the Makefile.