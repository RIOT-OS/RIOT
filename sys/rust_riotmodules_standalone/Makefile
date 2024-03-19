include $(RIOTBASE)/Makefile.base

include $(RIOTMAKE)/cargo-settings.inc.mk

APPLICATION_RUST_MODULE = rust_riotmodules_standalone

# No need to set the Cargo features enabled through the general pseudomodules:
# They're added to the crate in cargo-targets.inc.mk no matter whether it's
# building the rust_riotmodules_standalone top level crate or an application
# crate.

include $(RIOTMAKE)/cargo-targets.inc.mk
