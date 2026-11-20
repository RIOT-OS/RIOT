//! The bindgen generated definitions
//!
//! These are kept in a separate module (rather than including the bindings right in the lib.rs)
//! because bindgen is a bit sensitive to some of its assignments being defined differently
// Doxygen doesn't do these, and as long as C2Rust doesn't translate (and transpile??) Doxygen
// comments, these are just noise. The intra-docs links come from `@param[in] foo The Foo`.
#![allow(rustdoc::bare_urls)]
#![allow(rustdoc::invalid_rust_codeblocks)]
#![allow(rustdoc::broken_intra_doc_links)]

use core::ffi as libc;

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
