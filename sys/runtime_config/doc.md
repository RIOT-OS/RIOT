@defgroup    sys_runtime_config    Runtime config
@ingroup     sys
@brief       Runtime config module for handling runtime configurations

@warning This implementation is not complete and not yet thoroughly tested.
         Please do not use this module in production, as it may introduce security issues.
         Missing features are:
         - A code generator that transpiles YAML schemas to C code
         - Persistent storage extension
         - Int path extension
         - String path extension

This module provides a system level runtime configuration system for RIOT.

A runtime configuration system is in charge of providing a mechanism to set and get the values of configuration parameters that are used during the execution of the firmware, as well as a way to persist these values. Runtime configurations are deployment-specific and can be changed on a per node basis.
Appropriate management tools could also enable the configuration of nodes.

Examples of runtime configurations are:

- Transmission duty cycles
- Sensor thresholds
- Security credentials
- System state variables
- RGB-LED colors

These parameters might have constraints, like a specific order to be applied (due to interdependencies) or value boundaries.

The main advantages of having such a system are:

- Easy to apply per-node configuration during deployment
- No need to implement a special mechanism for per-node configurations during firmware updates (only in the case of migration), as the parameters persist.
- Common interface for modules to expose their runtime configuration parameters and handle them
- Common interface for storing configuration parameters in non-volatile storage devices

## Design

### Architecture

The proposed architecture, as shown below, is formed by one or more applications or configuration managers and the runtime config API.
The runtime config API acts as a common interface to access runtime configurations and store them in non-volatile storage.
All runtime configurations can be accessed either from the RIOT application using the provided runtime config interfaces or through the interfaces exposed by the configuration managers.
A RIOT application may interact with a configuration manager in order to modify access control rules or enable different exposed interfaces.

#### Path Based Configuration Managers (Needs `int_path or string_path` extension)

These configuration managers mirror the internal structure of the RIOT runtime
configuration tree.
They use either the `int_path` or the `string_path` extension module to expose the parameters via a path of either strings or integers.

#### Custom Schema Based Configuration Managers

These configuration managers have their own configuration structure (custom predefined object models etc.) and can not automatically be mapped to / from the runtime config schemas.
To make them work, a custom mapping module needs to be implemented per configuration manager, which maps each configuration parameter from the RIOT runtime config module to the correct format of the configuration manager.

![design_architecture](https://github.com/RIOT-OS/RIOT/assets/10547444/26830a26-882a-422c-ba20-abfa4e07ce8c)

### Configuration Structure

The runtime config system interacts with RIOT modules via `Configuration Schemas`, and with non-volatile storages via `Storages`.
This way the functionality of the Runtime config is independent of the functionality of a `module` or `storage` implementation.
It is possible to get or set the values of `Configuration Parameters`.
It is also possible to apply configurations, export their values to a buffer or print them.
To persist Configuration Values, it is possible to store them in non-volatile storages.

Any mechanism of security (`access control`, `encryption` of configurations) is `not` directly in the scope of the runtime config module but in the Configuration Managers and the specific implementations of the `Configuration Schemas` and `Storages`.

The graphic below shows an example of two `Configuration Namespaces` (SYS and APP).
The `APP` namespace contains a application specific `My app` Configuration Schema and the `SYS` namespace specifies a `WLAN` and a `LED Strip` Configuration Schema.
The application `My app` uses the custom `My app` Configuration Schema to expose custom Configuration Parameters to the Runtime config and the drivers `WS2812`, `SK6812` and `UCS1903` contain instances of
the `LED Strip` Configuration Schema to expose common LED Strip Configuration Parameters.

Also, there are two Storages available: `MTD` and `VFS`.
The `MTD` Storage internally uses the RIOT `MTD` driver and the `VFS` Storage internally uses the RIOT `VFS` module.

![design_namespaces_and_storages](https://github.com/user-attachments/assets/49b066bc-5419-454a-9797-c0972b3bc83b)

### Components

The Runtime config is split into multiple components as can be seen in the graphic below:

#### Runtime Config Core

```makefile
USEMODULE += runtime_config
```

This component holds the most basic functionality of the Runtime config.
It allows to `set` and `get` Configuration Values, transactionally `apply` them to make the changes come into effect and `export` all Configuration Parameters that exist in a given `Configuration Namespace`, `Configuration Schema` or `Configuration Group`.

Furthermore it is possible to `add` `Configuration Namespaces` or `Configuration Schema Instances`.

#### Runtime Config Namespace

The `Configuration Namespaces` such as `SYS` or `APP` and their respective `Configuration Schemas` are not part of the runtime config module itself.
It is possible to `add` custom `Configuration Namespaces` depending on the given needs.

![design_runtime_config](https://github.com/user-attachments/assets/473cef24-39b8-4539-b754-53146a497521)

## API

The graphic below shows the API of the Runtime config.
The top shows the Core API to manage Configuration Parameters.
On the right-hand side are functions to `set` and `get` Configuration Parameters, transactionally `apply` them and `export` them to a buffer or terminal.
On the left-hand side are setup functions to `add` `Configuration Namespaces` and `Configuration Schema Instances`.

The functionality of these functions is explained in the following paragraphs.

![api_structure](https://github.com/user-attachments/assets/588cb389-250a-4797-a78e-a76c4efe18fb)

### Usage

#### Add Namespaces

To be able to use `Configuration Schemas` and their `Parameters` etc. it is necessary to add a `Configuration Namespace` that contains the required `Configuration Schemas`.

This is possible using the `RUNTIME_CONFIG_ADD_NAMESPACE` macro, providing the name of the `Configuration Namespace` and a pointer to a `runtime_config_namespace_t` object as arguments.

```c
#define RUNTIME_CONFIG_ADD_NAMESPACE(_name, _namespace)
```

#### Add Configuration Schema Instances

To expose runtime configurations, it is necessary to add a `Configuration Schema Instance` of the needed `Configuration Schema`.

This is possible using the `runtime_config_add_schema_instance` function, providing the `Configuration Schema` and the `Configuration Schema Instance` as arguments.

```c
#include "runtime_config.h"
#include "runtime_config/namespace/sys.h"
#include "runtime_config/namespace/sys/board_led.h"

static runtime_config_error_t board_led_instance_apply_cb(
    const runtime_config_group_or_parameter_id_t *group_or_parameter_id,
    const runtime_config_schema_instance_t *instance)
{
    /* Handle configuration changes */

    return 0;
}

static runtime_config_sys_board_led_instance_t board_led_instance_data = {
    .enabled = 0,
};

static runtime_config_schema_instance_t board_led_instance = {
    .data = &board_led_instance_data,
    .apply_cb = &board_led_instance_apply_cb,
};

runtime_config_add_schema_instance(&runtime_config_sys_board_led, &board_led_instance);
```

#### Get configurations

A Configuration Value can be retrieved using the `runtime_config_get` function.
The function takes the `runtime_config_node_t` and a `runtime_config_value_t` pointer (to return the value) as its arguments.

```c
int runtime_config_get(
    const runtime_config_node_t *node,
    runtime_config_value_t *value
);
```

#### Set configurations

A Configuration Value can be set using the `runtime_config_set` function.
The function takes the `runtime_config_node_t`, a `void*` buffer and the buffer size as its arguments.

The buffer must contain the value in its correct c-type.
If the config schema expects a `u8`, but a `u16` is provided, the operation will fail.

```c
int runtime_config_set(
    const runtime_config_node_t *node,
    const void *buf,
    const size_t buf_len,
);
```

#### Apply configurations

Once the value(s) of one or multiple `Configuration Parameter(s)` are changed by the `runtime_config_set` function, they still need to be applied, so that the new values are taken into effect.

Configuration Parameter(s) can be applied using the `runtime_config_apply` function.
This function applies every `Configuration Parameter` currently available in the runtime configuration tree that is a direct or indirect child of the specified location within the configuration tree, specified by the `runtime_config_node_t` argument.

When a whole `Schema Instance`, or a single `Configuration Parameter` is applied, it will be passed on to the `apply_cb` handler of the `Configuration Schema Instance`, provided by the module that needs runtime configuration.
This way the module gets notified, when the `Configuration Parameter` has been applied and can apply the changes accordingly.

```c
int runtime_config_apply(const runtime_config_node_t *node);
```

#### Export configurations

Some times it is convenient to have a way to see what `Configuration Namespaces`, `Configuration Schemas`, `Configuration Schema Instances`, `Configuration Groups` or `Configuration Parameters` are available within our current Runtime config deployment.
To get this information there is the `runtime_config_export` function.

This function exports every `Configuration Object` currently available in the runtime configuration tree within the scope of the provided `runtime_config_node_t` argument.

When a node in the schema is exported, it will be passed on to the `export_cb` handler provided as an argument of each `runtime_config_export*` function.

```c
int runtime_config_export(
    const runtime_config_node_t *node,
    const runtime_config_export_cb_t export_cb,
    const uint8_t tree_traversal_depth,
    const void *context
);
```
