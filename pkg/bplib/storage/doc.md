@defgroup pkg_bplib_storage Storage Backends
@ingroup pkg_bplib
@brief Different storage backends for bplib.

## Usage

Using bplib requires a storage implementation to be selected, because bplib
internally calls the storage functions.

Since there are many different ways such a storage could be realized and
optimized for different things on different systems this functionality has been
removed from the bplib package and has to be provided.

The core usage of bplib *requires* the following functions to be implemented:

- `BPLib_Status_t BPLib_STOR_Init(BPLib_Instance_t* Inst)`: Constructor for the
  storage backend
- `void BPLib_STOR_Destroy(BPLib_Instance_t* Inst)`: Destructor for the storage
  backend
- `BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)`:
  Store the given bundle in the storage. Don't forget to call
  `BPLib_MEM_BundleFree()` in here once persistent. Also the AS could be
  incremented if used at all.
- `BPLib_Status_t BPLib_STOR_EgressForID(BPLib_Instance_t* Inst, uint32_t EgressID, bool LocalDelivery, size_t* NumEgressed)`:
  Retrieve bundles from the storage for the channel (when LocalDelivery = true)
  or contact (when LocalDelivery = false) at table index EgressID.
  NumEgressed should be updated to the number of egressed bundles.

Actual egress has to be done like this. Refer to unordered storage backend for
an example:

1. Find a bundle matching the EID of the target channel / contact.
2. Allocate this in bplib's memory using `BPLib_MEM_BlockAlloc()` and fill the
   saved data.
3. Use `BPLib_QM_WaitQueueTryPush()` to put the bundle in the channel / contact
   queue. If this fails deallocate with `BPLib_MEM_BundleFree()`.
4. Delete the bundle from the storage *here already*. As of right now there is
   no custody transfer and bplib has no option to tell the BPA that the bundle
   was *actually* sent out. This means this *could* lead to bundle loss in a
   power off here (graceful shutdown does handle this case though).
5. Optionally repeat from 1 to egress more than one bundle.

Additionally, functions that are also defined in the original bplib SQLite
storage, but are only called by a user, which *can* be implemented:

`BPLib_STOR_FlushPending`: Original implementation used a queue for storage and
only wrote to SQLite once this function was called, which happened regularly in
the example implementation. You probably won't need this unless you want such a
behavior.

`BPLib_STOR_GarbageCollect`: When called by the user, should delete expired as
well as egressed bundles (if the `BPLib_STOR_EgressForID` only set a egressed
flag but did not delete the bundle). In the case of unknown absolute time it is
hard to track when a bundle truly expired.

## Predefined backends

### Void Storage
`bplib_stor_void` module.

`BPLib_STOR_StoreBundle` does not store anything, and just frees the bundle.

`BPLib_STOR_EgressForID` does never find any bundle and returns.

This means, due to the architecture of bplib's router, if no channel or contact
is ready to take data directly (is in started state), the bundle will be dropped.

### VFS Storage - Unordered Egress
`bplib_stor_vfs_unordered` module.

All data is stored in the `CONFIG_BPLIB_STOR_BASE` directory
as up to 4 character hexadecimal number.

Bundles are discovered into a queue of length `CONFIG_BPLIB_EGRESS_CACHE_LEN`
for each channel and contact, breaking when the queue is full. Bundle data is
not read yet, only the EID check is made.
The order in which they are found by `vfs_readdir` is the order in which they
will be egressed, which is usually *not* ordered by anything.

Then the bundles are read from storage from the queue until it is empty.

`bplib_stor_vfs_contact_changed` and `bplib_stor_vfs_channel_changed` are used
to notify the implementation that the cache is not valid anymore, since the
reachable EIDs might have changed. This is automatically done in the default FWP
implementation.

@see pkg_bplib_fwp

### VFS Storage - Ordered Egress

`bplib_stor_vfs_ordered` module.

All data is stored in the `CONFIG_BPLIB_STOR_BASE` directory.
In here, subdirectories as `node_no/service_no` are located. Each bundle is
saved by its expiration time (creation time + lifetime).

@attention Ordering provides no benefit when running with unknown absolute DTN
           time. The ordered implementation has been shown to still be faster
           due to its directory structure.
           However, due to the way bundles are saved in this implementation, the
           unordered implementation should be preferred for the case of unknown
           time.
           Consider this implementation experimental, it may be changed in the
           future.

@note The subdirectories are currently not cleaned up when they are empty. When
      destination EID change a lot, many empty directories could be left.

Bundles are discovered into a cache of length `CONFIG_BPLIB_EGRESS_CACHE_LEN`
for each channel and contact, but iterating over ALL reachable bundles. Bundle
data is not read yet, only the EID check is made. After this cache filling, the
cache contains the bundle references ordered by urgency.

The order in which they are found by `vfs_readdir` is usually NOT the order in
which they will be egressed.

Then the bundles are read from storage from the cache, ordered by urgency, until
it is empty.

`bplib_stor_vfs_contact_changed` and `bplib_stor_vfs_channel_changed` are used
to notify the implementation that the cache is not valid anymore, since the
reachable EIDs might have changed. This is automatically done in the default FWP
implementation.

@see pkg_bplib_fwp