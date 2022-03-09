# Rust's own version of the target triple / quadruple.
#
# This does not have a sane default, and needs to be set in the architecture
# files.
# RUST_TARGET = ...

# Flags that need to be added to the RIOT_CFLAGS passed to cargo in order to
# make bindgen happy
CARGO_EXTRACFLAGS ?=

# Setting anything other than "debug" or "release" will necessitate additional
# -Z unstable-options as of 2021-03 nightlies.
CARGO_PROFILE ?= release

# The Rust version to use.
#
# As long as C2Rust and riot-wrappers require nightly, the only alternative
# here is to pick a particular nightly when something breaks.
#
# (Default is empty, because the riotbuild container picks a particular nightly
# and sets it as a default; users without a nightly default need to either
# override this here or in rustup)
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
CARGO_LIB = $(CARGO_TARGET_DIR)/$(RUST_TARGET)/${CARGO_PROFILE}/lib$(APPLICATION_RUST_MODULE).a
