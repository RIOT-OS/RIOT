#![no_std]

// As we're pulling all crates in only for their side effects of having symbols (they are required
// on the Rust side like riot_wrappers' panic_handler, and rust_riotmodules does that on its own
// too) all these crates have to be extern-crate'd to be pulled in because they are not used on the
// language level.

extern crate riot_wrappers;
extern crate rust_riotmodules;
