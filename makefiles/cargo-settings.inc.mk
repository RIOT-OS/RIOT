# The profile with which to build Rust usually `release` or `dev`.
#
# This needs to be known to the build scripts because the path of the produced
# binary is derived from this.
CARGO_PROFILE ?= release

# Note that if we did not set this explicitly, CARGO_LIB would have to
# understand which value cargo uses in absence of CARGO_TARGET_DIR, which would
# be $(APPDIR)/target.
#
# For many cases, it would be beneficial to base this on BINDIRBASE rather than
# BINDIR, for that would allow different boards using the same CPU to share
# compiled code (unless they they build conditionally on environment variables,
# like riot-sys does). This is not done for two reasons:
#
# * Overriding BINDIR (like is done in Murdock) would not take effect,
#   requiring additional overrides to enable out-of-tree building.
#
# * Switching back and forth between two boards of the same CPU requires
#   riot-sys rebuilds. (On its own, this would be outweighed by the shared
#   compilation of other modules).
CARGO_TARGET_DIR = $(BINDIR)/target

# The single Rust library to be built.
#
# The dev->debug and bench->release substitutions represent a historical
# peculiarity in cargo: "For historical reasons, the `dev` and `test` profiles
# are stored in the `debug` directory, and the `release` and `bench` profiles
# are stored in the `release` directory. User-defined profiles are stored in a
# directory with the same name as the profile".
CARGO_LIB = $(CARGO_TARGET_DIR)/$(RUST_TARGET)/$(patsubst test,debug,$(patsubst dev,debug,$(patsubst bench,release,${CARGO_PROFILE})))/lib$(APPLICATION_RUST_MODULE).a

# Options passed into all Cargo commands, in particular to the build command.
#
# Most of these are populated by RIOT modules that are backed by Rust. Popular
# options added by the user are `-Zbuild-std=core` (only available on nightly)
# to apply LTO and profile configuration to the core library.
CARGO_OPTIONS ?=
