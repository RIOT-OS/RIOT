# Basic Kconfig test
This test aims to test Kconfig on RIOT, and to show how symbols will be handled
during the migration phase.

## Configuration options
The application exposes the following configuration options:
- `APP_MSG_1_TEXT`: Text for the first message
- `APP_MSG_2`: Enable/disable printing the second message
- `KCONFIG_APP_MSG_1`: Enable/disable configuring the first message via Kconfig

All these options can be used by the application via macros, adding the
`CONFIG_` prefix to the symbol name (e.g. `CONFIG_APP_MSG_1_TEXT`).

## Optionally using Kconfig
`KCONFIG_APP_MSG_1` option exists for compatibility with the current way of
configuring RIOT. The `app.h` header contains the default value for the first
message, which is the one used if `KCONFIG_APP_MSG_1` is not set by the user.
Note that, in this case, the default text for message 1 in the header file
differs from the one in the Kconfig file, this is just to make more evident
which the applied value was.

## Changing configuration
The user can configure the values via the menuconfig interface
(`make menuconfig`), or by hand, either editing the `app.config` file or adding
an `user.config` file. Note that once the user has used the menuconfig interface
(i.e. has edited the `merge.config` file directly) a `make clean` has to be
issued in order to edit the value by hand.
