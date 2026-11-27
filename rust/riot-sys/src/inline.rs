//! Former home of the C2Rust transpiled `static inline` functions
//!
//! `static inline` functions (and the `macro_` helpers) are now exposed through bindgen's static
//! function wrappers directly in the crate root, sharing the types of all other bindings. This
//! module only re-exports the crate root for compatibility with code that still uses
//! `riot_sys::inline::...` paths.
pub use super::*;
