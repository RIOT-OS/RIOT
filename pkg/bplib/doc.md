@defgroup pkg_bplib bplib DTN stack
@ingroup  pkg
@brief    RIOT port of the bplib DTN BPv7 stack.

This package includes the RIOT port of NASA's bplib, a delay tolerant network
stack implementing the bundle protocol (BPv7).

In the ported version (7.0.2) of bplib the following features of the bundle protocol are NOT possible:
- Fragmentation
- Status report generation
- BIBE extension
  - This means also no custody transfer
- BPSec extension
- dtn EID Scheme. Only ipn is supported.

## Usage

The package can be used with `USEPKG += bplib`.

To use any function of the core bplib include `"bplib.h"`, do not include bplib's submodules
directly. `bplib.h` includes the others and has the extern c wrapping.

Requires a storage implementation (see below).

By default bplib's TIME module saves a file to track past time references over
different boot cycles, so `vfs` is used.
This can be disable with the `bplib_no_vfs` module.

Also a CLA implementation (see below) should be used as well, otherwise only local message deliveries are possible.

### Non automated things
To ensure the bundle storage does not fill up infinitely, you want to also call `BPLib_STOR_GarbageCollect()` more or less regularly,
or when the storage becomes filled to a certain level.

In order to maintain information about monotonic times across boot cycles `BPLib_TIME_MaintenanceActivities()` has to be called.
This should thus be called at least when a reboot is scheduled or bplib is terminated, or, to handle unforseen power-offs, regularly.

## Lists of sub-modules
### List of generic modules
Module name   | Description   | Requirements
------------- | ------------- | -----------
`bplib_init`  | Initialization | A FWP and a NC module
`bplib_nc`  | NC (Node Config) implementation | -
`bplib_fwp`  | FWP (Framework Proxy) implementation | `ztimer64` (msec)
`bplib_barebones` | By default the three modules above are enabled, to allow easy configuration. If you want to change one or all of these modules use `bplib_barebones` to disable all of these.| -
`bplib_walltime_available`  | Use module to tell `bplib_default_fwp` that walltime is available and set to the current timestamp. | `walltime`

### List of storage implementations
Module name   | Description   | Requirements
------------- | ------------- | -----------
`bplib_stor_vfs_ordered` | Stores bundles on the filesystem, in folders `node_id/service_id/`.<br> This path will be prefixed by `CONFIG_BPLIB_STOR_BASE`, which defaults to `/nvm0/bp`.<br> Bundles will be egressed ordered by urgency, which does not work when DTN time is not known.  | `vfs` and a filesystem
`bplib_stor_vfs_unordered` | Stores bundles on the filesystem in `CONFIG_BPLIB_STOR_BASE`, which defaults to defaults to `/nvm0/bp`.<br> Bundles will be retrieved in the order they are discovered by `vfs_readdir`.<br> This should be faster than the ordered option, unless **many** contacts / channels are used. | `vfs` and a filesystem
`bplib_stor_void` | Voids all bundles which cannot be delivered to a contact / channel immediately.<br> This kind of destroys the purpose of the bundle protocol but might be useful for leaf nodes and testing. | -

### List of CLA implementations
Module name   | Description   | Requirements
------------- | ------------- | -----------
`bplib_cla_udp`  | Basic UDPCL implementation, that sends bundles without any other wrapping as UDP payload. | `gnrc_sock_udp`
`bplib_cla_ble`  | Basic BLE L2CAP CLA. | `nimble`

### List of optimization modules
Module name   | Description   | Benefit
------------- | ------------- | -----------
`bplib_include_nc_telemetry` | By default the telemetry tables in bplib's NC have been patched out. Should you want to use telemetry, use this module. | 3.3 KB by *not* using this
`bplib_include_as` | By default the Admin Statistics (AS) tables tave been patched out. Should you want to use statistics about how many bundles were forwarded and so forth, use this module. | 7.5 KB by *not* using this
`bplib_no_vfs` | Prevents the use of `vfs` in bplib's time module.<br> bplib saves some information about the previous DTN times of previous boot cycles on vfs. This is used to track the expiry times across different boot cycles, however if `bplib_stor_void` is used, no bundles get stored, so no bundles can expire either. | No `vfs` in combination with `bplib_stor_void`.

### List of configuration options
Macro Name   | Description   | Requirements
------------- | ------------- | -----------
CONFIG_BPLIB_TIME_FILE_NAME | file path of the time information file. Make sure that this is *not* in a subfolder which is iterated by the storage implementation, or it might think this file is a bundle. | -
CONFIG_BPLIB_CLA_BLE_TIMEOUT | Timeout [ms] after which the BLE CLA will terminate after `bplib_cla_ble_stop()` is called. | `bplib_cla_ble`
CONFIG_BPLIB_CLA_BLE_MTU | MTU of the L2CAP channel. This has to be slightly larger than the bundle payload you intend to send, due to the other bundle blocks. Make sure the nimble MSYS pool is sufficiently large. | `bplib_cla_ble`
CONFIG_BPLIB_CLA_BLE_CID | CID of the L2CAP channel | `bplib_cla_ble`
CONFIG_BPLIB_CLA_UDP_BUFLEN | MTU over UDP. Should probably be around 1400 max. | `bplib_cla_udp`
CONFIG_BPLIB_CLA_UDP_TIMEOUT | Timeout [ms] after which the UDP CLA will terminate after `bplib_cla_udp_stop()` is called | `bplib_cla_udp`
CONFIG_BPLIB_MEMPOOL_LEN | Mempool size available for bplib bundles in send / delivery queues. Increasing this means more bundles can be queued, since bundles ingressed when to mempool is full, are dropped. Note: Each bundle is at least `sizeof BPLib_BBlocks_t` (around 700) + the blocks allocated for the actual payload. | `bplib_init`
CONFIG_BPLIB_GEN_WORKER_TIMEOUT | Timeout [ms] after which the generic worker times out after bplib termination | `bplib_init`
CONFIG_BPLIB_GENERIC_STACK_SIZE | Stack size of the generic worker. | `bplib_init`
CONFIG_BPLIB_MAX_SEQ_NUM | Maximum sequence number, after which it will wrap back to 0 | `bplib_nc`
CONFIG_BPLIB_STOR_BASE | File path prefix of the folder where the bundles will be stored. | `bplib_stor_vfs_*`
CONFIG_BPLIB_EGRESS_CACHE_LEN | Number of bundle reference stored in the caches / queues per channel and contact. Larger means the storage is searched less, at the price of more used memory. | `bplib_stor_vfs_*`
CONFIG_BPLIB_STOR_MAX_DUPLICATE_CHECKS | Used to upper bound the linear seach for a filename which is not yet used. This applies only for the ordered storage, so when DTN time is known there should not be many bundles with the same expiration timestamp. Max 255. | `bplib_stor_vfs_ordered`

Also the generic bplib config options from [bplib]/inc/bplib_cfg.h. These are currently not prefixed by CONFIG_.

The defaults, i.e. not using `bplib_include_nc_telemetry` and
`bplib_include_as` save more than 20KB of application size.

@see https://datatracker.ietf.org/doc/html/rfc9171

@see https://github.com/nasa/bplib

