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

The wrapper are [documented together with riot-sys and some of the examples].

[**riot-sys**]: https://crates.io/crates/riot-sys
[**riot-wrappers**]: https://crates.io/crates/riot-wrappers
[bindgen]: https://crates.io/crates/bindgen
[C2Rust]: https://c2rust.com/
[riot_wrappers::mutex::Mutex]: https://rustdoc.etonomy.org/riot_wrappers/mutex/struct.Mutex.html
[documented together with riot-sys and some of the examples]: https://rustdoc.etonomy.org/riot_wrappers/mutex/struct.Mutex.html
[I2CDevice]: https://rustdoc.etonomy.org/riot_wrappers/i2c/struct.I2CDevice.html
[corresponding embedded-hal I2C traits]: https://rustdoc.etonomy.org/embedded_hal/blocking/i2c/index.html

Toolchain {#toolchain}
---------

To install the necessary Rust components, it is easiest use [**rustup**, installed as described on its website].

Using Rust on RIOT requires a nightly version of Rust,
because some transpiled expressions for RIOT make use of unstable features,
and because the RIOT wrappers use some unstable idioms.

Make sure you have both the nightly **toolchain**
and the core library for the CPU (**target**) of your choice available:

```
$ rustup toolchain add nightly
$ rustup target add thumbv7m-none-eabi --toolchain nightly
```

(Substitute thumbv7m-none-eavi with the value of `RUST_TARGET`
in the output of `make info-build` of an application that has your current board selected,
or just add it later whenever the Rust compiler complains about not finding the core library for a given target).


While Rust comes with its own [cargo] dependency tracker for any Rust code,
it does not attempt to install **system components**.
To avoid playing the whack-a-mole of installing components whenever an install step fails,
consider installing this list of packages on Debian
(or an equivalent list on the distribution of your choice):

```
# apt install libclang-dev llvm llvm-dev cmake libssl-dev pkg-config
```

This encompass both components needed for riot-sys and for the later installation of C2Rust.


Installing **C2Rust** is special because
it can only be built using a particular nightly version
(as explained in its [introduction post])
and needs some patches applied:

```shell
$ rustup install nightly-2019-12-05
$ rustup component add --toolchain nightly-2019-12-05 rustfmt rustc-dev
$ cargo +nightly-2019-12-05 install c2rust
$ git clone https://github.com/chrysn-pull-requests/c2rust -b for-riot
$ cd c2rust
$ cargo +nightly-2019-12-05 install --path c2rust
```

[cargo]: https://doc.rust-lang.org/cargo/
[**rustup**, installed as described on its website]: https://rustup.rs/
[introduction post]: https://immunant.com/blog/2019/08/introduction-to-c2rust/

Maintenance
-----------

The riot-sys and riot-wrappers crates are currently maintained outside of the RIOT project,
primarily for practical historical reasons
(all their CI has been set up on GitLab in the organization it was developed in).

As Rust is stricter in APIs than C is
(for example, it is a breaking change to start returning an integer where previously void has been returned).
Dealing with this,
and other aspects of maintenance of the crates,
is described in [the `rust_minimal` test's README].

[the `rust_minimal` test's README]: (https://github.com/RIOT-OS/RIOT/blob/master/tests/rust_minimal/README.md).
