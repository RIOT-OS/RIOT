//! The bindgen generated definitions
//!
//! These are kept in a separate module (rather than including the bindings right in the lib.rs)
//! because bindgen is a bit sensitive to some of its assignments being defined differently
// Doxygen doesn't do these, and as long as C2Rust doesn't translate (and transpile??) Doxygen
// comments, these are just noise. The intra-docs links come from `@param[in] foo The Foo`.
#![allow(rustdoc::bare_urls)]
#![allow(rustdoc::invalid_rust_codeblocks)]
#![allow(rustdoc::broken_intra_doc_links)]
// The generated code is not ours to tidy up; these lints fire on every build (e.g. on bitfield
// accessors and on the C library declarations RIOT's headers pull in) and only add noise.
// `unknown_lints` keeps older compilers that lack some of these lints quiet.
#![allow(unknown_lints)]
#![allow(unnecessary_transmutes)]
// 'expect' allows us to see when the necessity goes way -- but some lints don't trigger in all
// situations. If any of those show up as "this lint expectation is unfulfilled", first try
// removing it at all and run a few more examples. If it triggers for some and not for others, move
// it to "allow"; if removing it causes no further warnings: concratulations, our code got better
// (or the linter now produces fewer false negatives).
#![expect(unused_unsafe)]
#![expect(suspicious_runtime_symbol_definitions)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
