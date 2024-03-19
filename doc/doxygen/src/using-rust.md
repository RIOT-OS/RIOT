Using Rust in RIOT    {#using-rust}
==================

[TOC]

On supported CPUs, Rust can be used to develop RIOT applications.
Support is indicated in the `has_rust_target` feature,
and tested for in applications using the Makefile line
`FEATURES_REQUIRED += rust_target`.

In addition to the regular RIOT build toolchain
and a recent nightly Rust toolchain for the given target,
using this also requires C2Rust with some patches applied to be installed;
see <a href="#toolchain">toolchain</a> for installation instructions.
All these are readily available in the [official RIOT docker image],
which gets used by default if `BUILD_IN_DOCKER=1` is set.

[official RIOT docker image]: https://hub.docker.com/r/riot/riotbuild

Examples
--------

Two examples are provided:

* ``rust-hello-world`` is minimal in the sense of setup and code complexity; it is the typical Hello World example.

  (Note that it is not necessarily minimal in terms of built size,
  as Rust's regular printing infrastructure is more powerful and a bit heavier than your off-the-shelf ``printf``,
  which embedded libcs already often trim down).

* ``rust-gcoap`` is a set of demo CoAP resources
  both from the [coap-message-demos] crate (containing platform and library independent examples)
  and from the [riot-module-examples] crate (containing RIOT specific examples).

There are [additional examples] available on GitLab,
maintained in coordination with the riot-wrappers crate.

[coap-message-demos]: https://gitlab.com/chrysn/coap-message-demos
[riot-module-examples]: https://gitlab.com/etonomy/riot-module-examples
[additional examples]: https://gitlab.com/etonomy/riot-examples/

IDE / editor setup
------------------

Users of Rust often take advantage of autocompletion or inline help.
To use this on RIOT projects,
some flags and environment variables have to be set,
which are listed by `make info-rust`.
These can be configured in the IDE's project setup
or exported as environment variables.

How it works
------------

The easy part of the story is that Rust code gets compiled into a static library
which is then linked together with the rest of the RIOT code;
if the main function happens to be implemented in Rust, so it is.

The **RIOT build system** contains rules and metadata to facilitate building and linking:
it calls `cargo` (Rust's own build system),
sets up paths to work well with out-of-tree builds,
configures the Rust target depending on the board's CPU,
and unpacks the static library into object files to facilitate interaction with XFA.

The [**riot-sys**] crate translates a selected subset of RIOT's header files for use in Rust;
this happens both using the [bindgen] crate (working from API information in header files)
and [C2Rust] \(translating static inline functions, and with some help from riot-sys, constant preprocessor initializers).
Functions exported by riot-sys are inherently unsafe to use (in Rust's sense of unsafe),
and may be somewhat volatile in their API due to mismatches between RIOT's / C's and Rust's API stability concepts.

The [**riot-wrappers**] crate creates safe and idiomatic wrappers around the types and functions provided by riot-sys.
Thanks to Rust's strong zero-cost abstractions, these often come at no increased runtime cost.
For example, locking a [riot_wrappers::mutex::Mutex] can rely on it having been properly initialized at creation;
furthermore, the mutex is freed when it goes out of scope.

Where practical, the wrappers are not accessed through own methods
but through established platform independent traits.
For example, the main API surface of an [I2CDevice]
is its implementation of the [corresponding embedded-hal I2C traits] for reading and writing.

The wrappers are [documented together with riot-sys and some of the examples].

[**riot-sys**]: https://crates.io/crates/riot-sys
[**riot-wrappers**]: https://crates.io/crates/riot-wrappers
[bindgen]: https://crates.io/crates/bindgen
[C2Rust]: https://c2rust.com/
[riot_wrappers::mutex::Mutex]: https://rustdoc.etonomy.org/riot_wrappers/mutex/struct.Mutex.html
[documented together with riot-sys and some of the examples]: https://rustdoc.etonomy.org/
[I2CDevice]: https://rustdoc.etonomy.org/riot_wrappers/i2c/struct.I2CDevice.html
[corresponding embedded-hal I2C traits]: https://rustdoc.etonomy.org/embedded_hal/blocking/i2c/index.html

Library components in Rust
--------------------------

It is possible to use Rust in different modules than the application itself.

Such modules are usually pseudomodules (although they may be mixed with C in regular modules as well).
They always depend on the `rust_riotmodules` module / crate,
which collects all enabled modules into a single crate by means of optional features.

If the application is not written in Rust,
that then depends on `rust_riotmodules_standalone`,
which adds a panic handler and serves as a root crate.

If the application is written in Rust,
`rust_riotmodules` needs to be added as a dependency of the application.
(This helps deduplicate between application and library code,
and also avoids symbol name clashes).
This is done by adding a dependency on the local `rust_riotmodules` crate (which is a no-op when no such modules are enabled),
and placing an `extern crate rust_riotmodules;` statement in the code.
(The latter is needed even after most `extern crate` was abolished in 2018,
because crates depended on but not used otherwise are usually not linked in).

Toolchain {#toolchain}
---------

To install the necessary Rust components, it is easiest use [**rustup**, installed as described on its website].

Using Rust on RIOT needs the latest stable version of Rust.

Make sure you have the core library for the CPU (**target**) of your choice available:

```
$ rustup target add thumbv7m-none-eabi
```

Substitute thumbv7m-none-eabi with the value of `RUST_TARGET`
in the output of `make info-build` of an application that has your current board selected
(or just add it later whenever the Rust compiler complains about not finding the core library for a given target).
Using the beta or nightly toolchains will work just as well
if they are selected through rustup's override mechanism.


While Rust comes with its own [cargo] dependency tracker for any Rust code,
it does not attempt to install **system components**.
To avoid playing the whack-a-mole of installing components whenever an install step fails,
consider installing this list of packages on Debian
(or an equivalent list on the distribution of your choice):

```
# apt install libclang-dev llvm llvm-dev cmake libssl-dev pkg-config
```

This encompass both components needed for riot-sys and for the later installation of C2Rust.


In addition to the Rust compiler you'll need to install the C2Rust transpiler;
as this is using some recent fixes, it is best installed as:

```shell
$ cargo install c2rust --git https://github.com/immunant/c2rust
```

[cargo]: https://doc.rust-lang.org/cargo/
[**rustup**, installed as described on its website]: https://rustup.rs/

Maintenance
-----------

The [riot-sys] and [riot-wrappers] crates are currently maintained as parts of the RIOT project.
While being released via crates.io on demand, usually RIOT uses a fixed version from the git repositories that are [easily updated].

The autogenerated bindings of the C API are slightly stricter than C's API,
and thus occasionally require additional work when C APIs change subtly.
The riot-sys crate takes the brunt of these changes --
it changes automatically, and no attempt is currently made to reflect these changes in a SemVer compatible manner.
The riot-wrappers crate smooths this out,
and provides an API that aims to be more stable than the C API.
It does that by generously converting types that changed,
and [introspecting generated bindings] or using [information provided by riot-sys].

The typical workflow of (C-nonbreaking, Rust-breaking) API changes is as follows:

* A PR subtly alters a type (eg. `uint8_t *` to `void *` in [#17990]).

  Consequently, builds of Rust examples break.

* A PR is opened on riot-wrappers to smooth over the change, like [aab605f4] <!-- commit reference rather than PR as that was still on GitLab back then -->.

  The PR is tested against current master in its CI (albeit not for the full set of boards).
  To test whether it also works for the changed API,
  a commit titled "REMOVEME Test with updated riot-wrappes" can be added to the original PR;
  it alters `.cargo/config.toml` to point to the changed branch,
  and removes any Cargo.lock files in the RIOT tree.

  That PR is then merged.

* The version of riot-wrappers that works both with the previous and the new code
  is pulled into the RIOT master branch by updating the Cargo.lock files.
  The PR can look like [#18181], and verifies that the new riot-wrappers works on all boards.

  That PR is then merged.

* For the next builds (up to the merging of) the original PR,
  the REMOVEME commit can be removed.

  It is good practice to rebase it onto the latest master after the update to riot-wrappers has been merged,
  as this helps keeping bisectability up.

  The PR now contains no Rust specific changes, and can be merged.

There are a few variations that can occur:

* Sometimes casting is not enough, and a type must be extracted from a signature.
  [See the phydat callback type change] for an example.

* When neither casting nor type detection is sufficient,
  a marker can be introduced through riot-sys;
  it detects a keyword's presence in the source and passes it as [information provided by riot-sys] to riot-wrappers.
  [See the coap_request_ctx_t change] for an example.

  In that case, a riot-sys PR is opened in parallel to the riot-wrappers PR.

  This method helps keeping changes backportable easily:
  riot-sys and riot-wrappers are expected to work with the latest released version of RIOT all the time,
  and avoid flag-day changes.
  (Keeping riot-sys and riot-wrappers compatible with the latest release is also important to retain the ability to backport fixes).

* When functions are moved from being static and not being static,
  their names go from `riot_sys::inline::name` to `riot_sys::name` (or vice versa).

  riot-sys [has a list] of items that are always publicly exported directly as `riot_sys::name`;
  just add the function there.

  If non-generic types are referenced in them, they go from `riot_sys::inline::my_type_t`  to `riot_sys::my_type_t`.
  The [inline_cast] function family helps making that cast a bit safer.

* Things fail around atomics.

  Until [C2Rust's support for atomics has improved],
  riot-sys requires all exported headers to use the better supported `atomic_utils.h`.

  If it is unavoidable that atomics are part of header files
  (and not actually used in any static inline functions),
  riot-sys's [atomics workarounds] can be extended as a last resort.


[riot-wrappers]: https://github.com/RIOT-OS/rust-riot-wrappers/
[riot-sys]: https://github.com/RIOT-OS/rust-riot-sys/
[easily updated]: https://github.com/RIOT-OS/RIOT/pull/17491#issuecomment-1143209437
[introspecting generated bindings]: https://github.com/RIOT-OS/rust-riot-wrappers/blob/db9d163e3eddcb7154edcf25db7207e4123964ee/src/helpers.rs#L3
[information provided by riot-sys]: https://github.com/RIOT-OS/rust-riot-sys/blob/525b2384a3541d4879a5f3845ee6241243c29a78/build.rs#L591
[#17990]: https://github.com/RIOT-OS/RIOT/pull/17990
[aab605f4]: https://github.com/RIOT-OS/rust-riot-wrappers/commit/aab605f464a279608ef0a8ad2afd5ae43179e330
[#18181]: https://github.com/RIOT-OS/RIOT/pull/18181
[See the phydat callback type change]: https://github.com/RIOT-OS/rust-riot-wrappers/pull/6/files#diff-ccb7946e3b4122ea3ce23fa9bc54eba63d75f7a6142fd4afdd9908b1bead50e0
[See the coap_request_ctx_t change]: https://github.com/RIOT-OS/rust-riot-wrappers/pull/4/files
[has a list]: https://github.com/RIOT-OS/rust-riot-sys/blob/525b2384a3541d4879a5f3845ee6241243c29a78/build.rs#L533
[inline_cast]: https://github.com/RIOT-OS/rust-riot-wrappers/blob/db9d163e3eddcb7154edcf25db7207e4123964ee/src/lib.rs#L68
[C2Rust's support for atomics has improved]: https://github.com/immunant/c2rust/issues/436
[atomics workarounds]: https://github.com/RIOT-OS/rust-riot-sys/blob/525b2384a3541d4879a5f3845ee6241243c29a78/riot-c2rust.h#L79
