This crate contains wrappers around the [RIOT Operating
System](https://riot-os.org/)'s C API exposed by riot-sys and
makes an attempt to provide idiomatic Rust wrappers (eg. implementing
embedded-hal for peripherals, implementing fmt::Write for stdio) around those.

The [crate documentation](https://doc.riot-os.org/rustdoc/latest/riot_wrappers/) outlines which
modules are available, and which other crates' traits they implement.

For a newcomer's starting point, see [RIOT's documentation on using it with Rust].
For basic code examples see [RIOT's examples](https://github.com/RIOT-OS/RIOT/tree/master/examples/lang_support/official)
(those with "rust" in their name), and the
[additional examples](https://gitlab.com/etonomy/riot-examples/)
which showcase more of the wrapped APIs.

[RIOT's documentation on using it with Rust]: https://doc.riot-os.org/using-rust.html

Library and run-time components
-------------------------------

The riot-wrappers crate tries to stay out of the way by default to enable
various types of applications (ie. not only "Rust application running atop
RIOT", but also "RIOT module / driver implemented in Rust" or others).

To facilitate what is currently the best explored use case ("Rust application
running atop RIOT"), applications can use the ``main!`` macro to wrap a regular
Rust function like ``fn main() -> ()`` into a function that's exported with
proper name and signature to serve as ``main`` function in RIOT.

When that is used, it also makes sense to enable the ``set_panic_handler``
feature. It implements a panic handler that outputs the panic message to RIOT's
standard output, and puts the affected thread to sleep permanently.  (There is
no unwinding or similar; threads in RIOT are not really expected to terminate
and be restarted).

With such a main function and panic handler, a Rust crate can be built as a
static library and linked as a part of the RIOT build process without the need
for application specific C code. The RIOT build system automates that linking,
and examples of the setup required in Cargo.toml and Makefile are available as
part of RIOT's example directory.

Supported RIOT & Rust versions
------------------------------

Currently, this crate targets the latest development version of RIOT.
Support for the latest release is maintained on a best-effort basis.

This crate has no MSRV, it may start depending on the latest stable as soon as RIOT's build infrastructure has it.

When a released version of RIOT is used with anything but the riot-sys / riot-wrappers / nightly-compiler combination it was released with,
it is likely that all these must be upgraded together.

In terms of public API,
riot-wrappers aims to uphold SemVer guarantees
(with little exceptions explicitly documented with the relevant items,
such as reserving the right to replace a type with a `pub use` from the standard library once a feature is stabilized).
Unlike that of riot-sys,
this API is stable across releases of RIOT.

On item presence and modules
----------------------------

This crate makes some of its modules' presence conditional on whether the
corresponding RIOT module is active in the build configuration; that
information is obtained by inspecting the `riotbuild.h file`. For example,
`riot_wrappers::saul` is only present if `USEMODULE += saul` is (directly or
indirectly) set in the Makefile.

This makes things very auto-magical, and I'm not yet sure whether that's the
best way for things to be. The Cargo way would be that the crate using
riot-wrappers actively enables some features in riot-wrappers -- but the crate
can not act on RIOT's module selection, as by the time it is called, RIOT is
already configured. The RIOT way would be to enable the modules the application
needs in the Makefile (possibly with dependencies pulling others in), but the
crate not being a module makes that hard.

This automagic way is convenient now; later iterations might be more explicit
and profit from better integration.

Code conventions
----------------

There is a [contributing guide](CONTRIBUTING.md) for this repository.

In older pieces of code (predating the use of C2Rust), static inline RIOT functions
or expanded macros are used. To keep track of them, comments in the shape of
``EXPANDED ${FILE}:${LINE}`` are set (referring to line numbers in RIOT commit 6b96f69b).

As these are being replaced by using C2Rust idioms, conflicts between C2Rust's
and bindgen's versions of structs arise instead, typically around pointers. When these
are cast away, they're fed through `inline_cast` & co to perform some checks,
or commented with ``INLINE TRANSMUTE`` for the very hard cases.

License
-------

This crate is dual-licensed under the same terms of the MIT license or the
Apache 2.0 license, as is commonplace in the embedded Rust ecosystem.

Note that it crate depends on `riot-sys`, which is licensed under RIOT's LGPL
2.1 to reflect that it uses code transpiled from RIOT.

The crate is maintained by Christian Amsüss <chrysn@fsfe.org> as part of the etonomy
project, see <https://etonomy.org/>, and the RIOT team.
