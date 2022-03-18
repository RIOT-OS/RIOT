Semantics of typedefs and moving their values {#typedef-and-move}
=============================================

This page describes general properties of types defined (`typedef`'d) in RIOT.
It is mostly aimed at developers who alter or extend RIOT;
API users can often ignore them if they stick to idiomatic C.

Movability
----------

In C, generally,
any type can technically be copied or moved,
e.g. by returning it,
by assigning it to other variables
or by using `memcpy` / `memmove`.

Once a value has been passed through an initializer that takes it by reference,
any further copy or move are generally *not* allowed any more,
for the initializer might have done anything self-referential inside the value,
or (more commonly) have inserted it into some linked list.

Some types are described to be "movable".
If a type is movable, its values may be moved to a different memory address
even after it has been passed into a function by reference.
Consequently, functions taking a reference
may use the reference to access the value for the duration of the function,
but not store the pointer anywhere.

Why do we care?
---------------

In C we avoid passing large items by value,
and consequently pass pointers around to initialize and finalize values;
consequently, a stable address of a value is often around and ready to be used.

In @rust using-rust "Rust" there are no restrictions on the size of return values,
and logically moving values makes it easy to uphold [RAII] guarantees.
(The Rust compiler will often use its knowledge of the program to not make values actually move,
but neither is that guaranteed nor can it be reliably observed).

[RAII]: https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization

As most of RIOT's C APIs are being used by Rust through wrappers,
movability of a type needs to be specified
in order to no force Rust to assume *may never move* on *all* types.
Types that may not move but are allocated by Rust
require more [verbose and complicated] application code.

[verbose and complicated]: https://doc.rust-lang.org/core/pin/index.html

In addition to the benefits for the Rust wrappers,
the documenting movability helps keeping RIOT internally consistent,
and assists in other advanced use cases like coroutines.

Is a type movable?
------------------

Types are generally not movable, unless they are

* documented to be movable, or
* taken by value (or returned by value) in public API functions (or preprocessor macros) that use the type.

In addition,
individual functions may state that they take a reference to be short-lived,
and make no requirements on the pointer's validity beyond the function's execution.
(The @ref ztimer_is_set function might make such a statement).

Examples
--------

The @ref ipv6_addr_t type is movable (because it is (TBD -- will be) documented to be):
while it is frequently passed by referenced due to its sheer size,
an application may just `memcpy` an address out of an obtained pointer and store it for later use
(maybe along with a cache entry).

In contrast, a @ref ztimer_t is not movable:
a value may be created locally and populated (in theory even be moved around),
but once it is passed to @ref ztimer_set by reference,
that pointer needs to stay valid until further notice.
Unlike with other types, this is documented in very explicit words on the function.

More subtly, @ref vfs_DIR is also not movable (because that is the default for any type):
Once passed into @ref vfs_opendir, its memory location needs to stay stable
until @ref vfs_closedir releases it.
Moving it is less likely to have ill-effects (because only the @ref pkg_littlefs2 uses the property),
but users still need to treat it as not movable.

In contrast, the file descriptors of the VFS are movable
because they are returned by value in the @ref vfs_open function and passed into @ref vfs_close the same way.
(Also, file descriptors are `int`, and as primitive types are not covered by this page).

API stability
-------------

Removing the note from a function that the type is movable should be considered a breaking change.
The change note on such an API change may say that
"this change is unlikely to break idiomatic C applications".
