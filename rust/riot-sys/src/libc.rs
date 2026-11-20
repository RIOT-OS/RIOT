//! Types used around and with the exported functions
//!
//! Most of this used to be explicitly defined here until definitions from other crates were taken
//! over.

#![allow(non_camel_case_types)]

pub use core::ffi::{
    c_char,
    c_double,
    c_float,
    c_int,
    c_long,
    c_longlong,
    c_schar,
    c_short,
    c_uchar,
    c_uint,
    c_ulong,
    c_ulonglong,
    c_ushort,
    // Not even loading size_t and ssize_t as they don't fit with bindgen's mapping anyway
};

pub use core::ffi::c_void;
