Semantics of typedefs and moving their values {#typedef-and-move}
=============================================

This page describes general properties of types defined (`typedef`'d) in RIOT.
It is mostly aimed at developers who alter or extend RIOT;
API users can often ignore them if they stick to idiomatic C.

This is being documented for consistency within RIOT,
and for the benefit of users who use RIOT
from other languages (such as @rust using-rust "Rust" )
or do other advanced things like implementing coroutines.

Movability
----------

In C, generally,
any type can technically be copied or moved,
e.g. by returning it (which is possible but discouraged for larger structs),
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

Is a type movable?
------------------

Types are generally not movable, unless they are

* documented to be movable, or
* taken by value (or returned by value) in public API functions (or preprocessor macros) that use the type.

In addition,
individual functions may state that they take a reference to be a short-lived,
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
