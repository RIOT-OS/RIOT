#![no_std]

// Crates in here are pub used in case they make anything accessible to Rust applications directly;
// they can then access the crates as `rust_riotmodules::lsm303agr` or similar.
//
// (Also, if they were not pub used, they'd need to be extern crate'd).

// This list should be as auto-generated / -maintained as the one in Cargo.toml

#[cfg(feature = "riot-module-lsm303agr")]
pub use riot_module_lsm303agr as lsm303agr;

#[cfg(feature = "riot-module-shell-democommands")]
pub use riot_module_shell_democommands as democommands;
