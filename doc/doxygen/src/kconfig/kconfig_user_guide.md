# User's guide to configure with Kconfig                 {#kconfig-user-guide}
[TOC]

# Configure using menuconfig                     {#configure-using-menuconfig}
In order to use the graphical interface menuconfig to configure the
application, run `make menuconfig` in the application's folder. All available
configurations (based on the used modules) for the particular platform will be
presented.

Once the desired configuration is achieved save the configuration to the
default proposed path and exit. The saved configuration will be applied when
the code is compiled (`make all`).

If the current configuration will be used in the future it can be saved in the
application's folder as `user.config`, using the 'Save' option in menuconfig.
This way it will be persistent after cleaning the application directory
(`make clean`).

# Configure using '.config' files                     {#configure-using-files}
The second way to configure the application is by directly writing '.config'
files. Two files will be sources of configuration during the generation of the
final header file: `app.config` and `user.config`, which should be placed
inside the application's folder. `app.config` sets default configuration
values for the particular application, the user can override them by setting
them in `user.config`.

Let's say that the `SOCK_UTIL_SCHEME_MAXLEN` symbol in `sock_util` module needs
to be configured. The `user.config` file could look like:

```Makefile
# activate module sock_util
CONFIG_MOD_SOCK_UTIL=y
# change scheme part length
CONFIG_SOCK_UTIL_SCHEME_MAXLEN=24
```

In this case, there is no need for using menuconfig. It's enough just to call
`make all` in the application folder, as this configuration will be read and
applied. Note that if any dependency issue occurs, warnings will be generated
(e.g. trying to activate a module without activating its dependencies).

# Application configuration with Kconfig                 {#app-config-kconfig}
To expose application-specific configuration options a `Kconfig` file can
be placed in the application's folder. For an example of this you can check
the [tests/kconfig](https://github.com/RIOT-OS/RIOT/tree/master/tests/kconfig)
application.

# A note on the usage of CFLAGS
When a certain module is being configured via Kconfig the configuration macro
will not longer be overridable by means of CFLAGS (e.g. set on the
compilation command or on a Makefile). Consider this if you are getting a
'redefined warning'.

# A note on the usage of the 'clean' command
When using Kconfig as the configurator for RIOT, configuration symbols may be
used in Makefiles through the build system. For this to work properly make
sure that when cleaning an application you call `make clean && make all`,
instead of `make clean all`.
