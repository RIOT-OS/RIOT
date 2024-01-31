# The profile with which to build Rust usually `release` or `dev`.
#
# This needs to be known to the build scripts because the path of the produced
# binary is derived from this.
CARGO_PROFILE ?= release

# Value for CARGO_CHANNEL when using nightly
#
# As different environments have different versions of nightly installed, but
# rustup / cargo does not take "the latest installed nightly" for a toolchain,
# a good value is determined dynamically. Typical values this takes are
# `nightly` (on regular installations) and `nightly-2022-03-08` (or whichever
# date it is currently pinned to) in riotbuild.
#
# Workaround-For: https://github.com/rust-lang/rustup/issues/3015
#
# This does not get evaluated unless actually used; if rustup is not installed,
# the default value will likely not be usable but at least set the user on the
# right track.
CARGO_CHANNEL_NIGHTLY = $(shell rustup toolchain list | sed 's/ .*//' |grep nightly | tail -n1 || echo nightly)

# The Rust version to use.
#
# Examples should set this to either `stable` or `$(CARGO_CHANNEL_NIGHTLY)`.
# The default is empty, which is suitable for applications that select their
# version through a `rust-toolchain.yaml` file.
CARGO_CHANNEL ?=

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
