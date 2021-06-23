# Rust's own version of the target triple / quadruple.
#
# This does not have a sane default, and needs to be set in the architecture
# files.
# CARGO_TARGET = ...

# Flags that need to be added to the RIOT_CFLAGS passed to cargo in order to
# make bindgen happy
CARGO_EXTRACFLAGS ?=

# Setting anything other than "debug" or "release" will necessitate additional
# -Z unstable-options as of 2021-03 nightlies.
CARGO_PROFILE ?= debug
# The Rust version to use.
#
# As long as C2Rust and riot-wrappers require nightly, the only alternative
# here is to pick a particular nightly when something breaks.
CARGO_CHANNEL ?= nightly

# The single Rust library to be built.
CARGO_LIB = target/$(CARGO_TARGET)/${CARGO_PROFILE}/lib$(APPLICATION_RUST_MODULE).a
