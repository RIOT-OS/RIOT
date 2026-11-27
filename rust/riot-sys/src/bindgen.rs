//! The bindgen generated definitions
//!
//! These are kept in a separate module (rather than including the bindings right in the lib.rs)
//! because bindgen is a bit sensitive to some of its assignments being defined differently
// Doxygen doesn't do these, and as long as bindgen doesn't translate Doxygen comments, these are
// just noise. The intra-docs links come from `@param[in] foo The Foo`.
#![allow(rustdoc::bare_urls)]
#![allow(rustdoc::invalid_rust_codeblocks)]
#![allow(rustdoc::broken_intra_doc_links)]
// The generated code is not ours to tidy up; these lints fire on every build (e.g. on bitfield
// accessors and on the C library declarations RIOT's headers pull in) and only add noise.
// `unknown_lints` keeps older compilers that lack some of these lints quiet.
#![allow(unknown_lints)]
#![allow(unused_unsafe)]
#![allow(unnecessary_transmutes)]
#![allow(suspicious_runtime_symbol_definitions)]
#![allow(clashing_extern_declarations)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
