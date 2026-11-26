<!-- WARNING: README.md is auto-generated using `cargo readme`, don't modify that file -->

# riot-sys

## Bindings for RIOT system calls

This crate contains dynamically generated Rust FFI bindings to the [RIOT
Operating System](https://riot-os.org/).

Those bindings are inherently unsafe; it is recommended that their safe
abstractions in the [riot-wrappers] crate are used in most applications.

For a newcomer's starting point, see [RIOT's documentation on using it with Rust].
This also contains installation instructions / dependencies.

[riot-wrappers]: https://crates.io/crates/riot-wrappers
[RIOT's documentation on using it with Rust]: https://doc.riot-os.org/using-rust.html

### RIOT integration

Which functions and structs are present in this crate, and sometimes their
details, inherently depends on the RIOT configuration this will be used with.
For example, RIOT's `thread_t` only has a member `name` if `DEVHELP` is
set for a build, and its `flags` member is only present if the `thread_flags`
module is in use.

All the relevant information -- including the location of the actually used
RIOT header files and flags influencing the ABI -- is conveyed to `riot-sys` by passing on the
compiler and the CFLAGS. This can either be done by passing in the path to a "compile commads"
file through the `RIOT_COMPILE_COMMANDS` environment variable (accompanied by a
`RIOT_USEMODULES`, as that part of `CFLAGS` is missing from the compile commands), or
alternatively by passing in the C compiler as `RIOT_CC` and the CFLAGS (both their
`CFLAGS_WITH_MACROS` and the `INCLUDES` part from RIOT's build system) in. When called from
within RIOT's build system, care must be taken to clear `CC` and `CFLAGS`, as these would be
interpreted by Cargo (Rust's build system) to refer to the host compiler and flags.
The flags will be interpreted by libclang based tools; care must be taken to pass in flags
suitable for clang and not for GCC.

These steps are automated in RIOT's build system.


The `RIOT_CC` and `RIOT_CFLAGS` are made available to dependent crates through
Cargo (as `DEP_RIOT_SYS_CC` etc); see [riot-wrappers]'s build.sh for an example. Similarly,
custom markers are made available based on the presence of certain defines or features in RIOT
as downstream crates require that information (typically to allow a crate to work across a
wider range of RIOT versions); see the section below for details.


### Extension

Currently, only a subset of all the RIOT headers is processed; all the relevant
header files are included in this crate's `riot-headers.h` header file. If you
need access to more RIOT APIs, more includes can be added there.


### Versioning

`riot-sys` is versioned using SemVer,
and efforts are made to not make breaking changes even while in the 0.x phase.
Note that as it passes on RIOT internals,
any of the SemVer guarantees only hold when built on the *same* RIOT --
once the underlying C code is changed, all bets are off.
Users of `riot-rs` can introspect the `DEP_RIOT_SYS_...` variables
that are available to crates that set `links = "riot-sys"`
to affect the symbols those crates use.
Typical variables to inspect are `DEP_RIOT_SYS_BINDGEN_OUTPUT_FILE`
(to determine whether a symbol is imported in the first place, eg. when RIOT renames something)
and `DEP_RIOT_SYS_CFLAGS` which includes the enabled modules.

#### Markers

**Deprecated, see below**.

Some decisions of downstream crates need to depend on whether some feature is around in
RIOT. For many things that's best checked on module level, but some minor items have no
module to mark the feature, and checking for versions by numers is not fine-grained enough,
so it's easiest to check for concrete strings in the bindgen output.

The `build.rs` of this crate contains a list of marker conditions. These lead to `MARKER_foo=1`
items emitted that are usable as `DEP_RIOT_SYS_MARKER_foo=1` by crates that explicitly `links =
"riot-sys"`. They are stable in that they'll only go away in a breaking riot-sys version;
downstream users likely stop using them earlier because they sooner or later stop supporting
old RIOT versions.

For example, in [PR #17957](https://github.com/RIOT-OS/RIOT/pull/17957), an argument to a
particular handler function changed fundamentally; no amount of `.into()` would allow writing
sensible abstractions. The marker `coap_request_ctx_t` was introduced, and is present
automatically on all RIOT versions that have that particular pull request merged. Code in
`riot-wrappers` uses conditions like `#[cfg(marker_coap_request_ctx_t)] ` to decide whether to
use the old or the new conventions.

These markers are currently checked against bindgen's output, but could query any property
that riot-sys has access to. The markers are defined in terms of some change having happened
in RIOT; the way they are tested for can change. (In particular, when riot-sys stops
supporting an older RIOT version, it can just always emit that marker).

Crates building on this should preferably not alter their own APIs depending on these,
because that would add extra (and hard-to-track) dimensions to them. If they can, they
should provide a unified view and degrade gracefully. (For example, riot-wrappers has the
unit `T` of the `phydat_unit_t` in its enum, but converts it to the generic unspecified unit
on RIOT versions that don't have the T type yet -- at least for as long as it supports
2022.01).

**Deprecation note / successor**: As of 2023-02, no new markers will be added, because
implementing this mechanism here has shown to be impracitcal: Changes need to go into riot-sys
first before they can be use (and tested in) with riot-wrappers. Instead, `BINDGEN_OUTPUT_FILE`
is exported (usable as `DEP_RIOT_SYS_BINDGEN_OUTPUT_FILE`), which points to the Rust file
generated by bindgen. Downstream crates can inspect that file, possibly using the same
string-based methods as riot-sys uses, but without any cross-crate dependencies.

Crates accessing the `BINDGEN_OUTPUT_FILE` should exercise the same caution that is recommended
above for the use of markers.

---

The types and constants of RIOT are translated in two forms:
through bindgen (to be linked together), and through C2Rust (transpiled, to be inlined).
This is necessary because neither can express the full set of useful RIOT APIs.

All bindgen types are reexported in the main module and exclusively public through there. The
C2Rust types largely reside in the [inline] module, with some pub used into the root module as
necessary or convenient.

License
-------

This crate is licensed under the same terms as of the LGPL-2.1, following the
license terms of the RIOT Operating System.

It is maintained by Christian Amsüss <chrysn@fsfe.org> as part of the etonomy
project, see <https://etonomy.org/>, and the RIOT team.
