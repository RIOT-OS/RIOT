@defgroup    sys_registry    RIOT Registry
@ingroup     sys
@brief       RIOT Registry module for handling runtime configurations

@warning This implementation is not complete and not yet thoroughly tested.
         Please do not use this module in production, as it may introduce security issues.

## Abstract

This module provides a system level runtime configuration system for RIOT.

A runtime configuration system is in charge of providing a mechanism to set and get the values of `Configuration Parameters` that are used during the execution of the firmware, as well as a way to persist these values. Runtime configurations are deployment-specific and can be changed on a per node basis.
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
- Common interface for modules to expose their runtime Configuration Parameters and handle them
- Common interface for storing Configuration Parameters in non-volatile storage devices

## Design

### Architecture

The proposed architecture, as shown below, is formed by one or more Applications or Configuration Managers and the RIOT Registry.
The RIOT Registry acts as a common interface to access Runtime Configurations and store them in non-volatile storage.
All runtime configurations can be accessed either from the RIOT application using the provided RIOT Registry interfaces or through the interfaces exposed by the Configuration Managers.
A RIOT Application may interact with a Configuration Manager in order to modify access control rules or enable different exposed interfaces.

#### Path Based Configuration Managers (Needs `int_path or string_path` extension)

These Configuration Managers are a simple representation of the default configuration structure of the RIOT Registry.
They use either the `int_path` or the `string_path` Registry extension module to expose the parameters using a path.

#### Custom Schema Based Configuration Managers

These Configuration Managers have their own configuration structure (custom predefined object models etc.) and can not automatically be mapped to / from the RIOT Registry itself.
To make them work, a custom mapping module needs to be implemented, which maps each Configuration Parameter from the registry to the correct format of the Configuration Manager.

![design_architecture](https://github.com/RIOT-OS/RIOT/assets/10547444/26830a26-882a-422c-ba20-abfa4e07ce8c)

### Namespaces and Storages

> [!NOTE]
> Storages are part of a follow-up PR

The RIOT Registry interacts with RIOT modules via `Configuration Schemas`, and with non-volatile storages via `Storages`.
This way the functionality of the RIOT Registry is independent of the functionality of a `module` or `storage` implementation.
It is possible to get or set the values of `Configuration Parameters`.
It is also possible to transactionally apply configurations or export their values to a buffer or print them.
To persist Configuration Values, it is possible to store them in non-volatile storages.

Any mechanism of security (`access control`, `encryption` of configurations) is `not` directly in the scope of the Registry but in the Configuration Managers and the specific implementations of the `Configuration Schemas` and `Storages`.

The graphic below shows an example of two `Configuration Namespaces` (SYS and APP).
The `APP` namespace contains a application specific `My app` Configuration Schema and the `SYS` namespace specifies a `WLAN` and a `LED Strip` Configuration Schema.
The application `My app` uses the custom `My app` Configuration Schema to expose custom Configuration Parameters to the RIOT Registry and the drivers `WS2812`, `SK6812` and `UCS1903` contain instances of
the `LED Strip` Configuration Schema to expose common LED Strip Configuration Parameters.

Also, there are two Storages available: `MTD` and `VFS`.
The `MTD` Storage internally uses the RIOT `MTD` driver and the `VFS` Storage internally uses the RIOT `VFS` module.

![design_namespaces_and_storages](https://github.com/user-attachments/assets/49b066bc-5419-454a-9797-c0972b3bc83b)

### Components

The RIOT Registry is split into multiple components as can be seen in the graphic below:

#### Registry Core

This component holds the most basic functionality of the RIOT Registry.
It allows to `set` and `get` Configuration Values, transactionally `apply` them to make the changes come into effect and `export` all Configuration Parameters that exist in a given `Configuration Namespace`, `Configuration Schema` or `Configuration Group`.

Furthermore it is possible to `add` `Configuration Namespaces` or `Configuration Schema Instances`.

#### Registry Namespace

The `Configuration Namespaces` such as `SYS` or `APP` and their respective `Configuration Schemas` are not part of the Registry itself.
It is possible to `add` custom `Configuration Namespaces` depending on the given needs.

![design_registry](https://github.com/user-attachments/assets/473cef24-39b8-4539-b754-53146a497521)

## API

The graphic below shows the API of the RIOT Registry.
The top shows the Core API to manage Configuration Parameters.
On the right-hand side are functions to `set` and `get` Configuration Parameters, transactionally `apply` them and `export` them to a buffer or terminal.
On the left-hand side are setup functions to `add` `Configuration Namespaces` and `Configuration Schema Instances` to the Registry.

The functionality of these functions is explained in the following paragraphs.

![api_structure](https://github.com/user-attachments/assets/588cb389-250a-4797-a78e-a76c4efe18fb)

### Usage

#### Add Namespaces to the Registry

To be able to use `Configuration Schemas` and their `Parameters` etc. it is necessary to add a `Configuration Namespace` that holds the required `Configuration Schemas` to the Registry.

This is possible using the `REGISTRY_ADD_NAMESPACE` macro, providing the name of the `Configuration Namespace` and a pointer to a `registry_namespace_t` object as arguments.

```c
#define REGISTRY_ADD_NAMESPACE(_name, _namespace)
```

#### Add Configuration Schema Instances to the Registry

To implement runtime configuration functionality into a module, it is necessary to add a `Configuration Schema Instance` of the needed `Configuration Schema` to the Registry.

This is possible using the `registry_add_schema_instance` function, providing the `Configuration Schema` and the `Configuration Schema Instance` as arguments.

```c
int registry_add_schema_instance(
    const registry_schema_t *schema,
    const registry_schema_instance_t *instance,
);
```

#### Get configurations

A Configuration Value can be retrieved using the `registry_get` function.
The function takes the `registry_node_t` and a `registry_value_t` pointer (to return the value) as its arguments.

```c
int registry_get(
    const registry_node_t *node,
    registry_value_t *value
);
```

#### Set configurations

A Configuration Value can be set using the `registry_set` function.
The function takes the `registry_node_t`, a `void*` buffer and the buffer size as its arguments.

The buffer must contain the value in its correct c-type.
If the Registry expects a `u8`, but a `u16` is provided, the operation will fail.
Furthermore the registry can specify constraints like `minimum` and `maximum` values and an array of `allowed` or `forbidden` values.
If these constraints are not fulfilled, then the operation will fail as well.

```c
int registry_set(
    const registry_node_t *node,
    const void *buf,
    const size_t buf_len,
);
```

#### Apply configurations

Once the value(s) of one or multiple `Configuration Parameter(s)` are changed by the `registry_set` function, they still need to be applied, so that the new values are taken into effect.

Configuration Parameter(s) can be applied using the `registry_apply` function.
This function applies every `Configuration Parameter` currently available in the Registry that is a direct or indirect child of the specified location within the configuration tree, specified by the `registry_node_t` argument.

When a whole `Schema Instance`, or a single `Configuration Parameter` is applied, it will be passed on to the `apply_cb` handler of the `Configuration Schema Instance`, provided by the module that needs runtime configuration.
This way the module gets notified, when the `Configuration Parameter` has been applied and can apply the changes accordingly.

```c
int registry_apply(const registry_node_t *node);
```

#### Export configurations

Some times it is convenient to have a way to see what `Configuration Namespaces`, `Configuration Schemas`, `Configuration Schema Instances`, `Configuration Groups` or `Configuration Parameters` are available within our current RIOT Registry deployment.
To get this information there is the `registry_export` function.

This function exports every `Configuration Object` currently available in the Registry within the scope of the provided `registry_node_t` argument.

When a node in the schema is exported, it will be passed on to the `export_cb` handler provided as an argument of each `registry_export*` function.

```c
int registry_export(
    const registry_node_t *node,
    const registry_export_cb_t export_cb,
    const uint8_t tree_traversal_depth,
    const void *context
);
```
