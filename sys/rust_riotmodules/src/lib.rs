#![no_std]

// As we're pulling all crates in only for their side effects of having symbols (be that to place
// them in XFA symbols or to make them available for C) all these crates have to be extern-crate'd
// to be pulled in because they are not used on the laguage level.

// This list should be as auto-generated / -maintained as the one in Cargo.toml

#[cfg(feature = "riot-module-lsm303agr")]
extern crate riot_module_lsm303agr;

#[cfg(feature = "riot-module-shell-democommands")]
extern crate riot_module_shell_democommands;
