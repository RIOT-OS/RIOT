//! Manually provided equivalents of C items that can not be expressed through bindgen

use crate::bindgen::*;

/// Equivalent of `MUTEX_INIT`, usable in const context
///
/// This mirrors the C macro (`{ .queue = { .next = NULL } }`); should mutex_t ever gain more
/// fields, this fails to build and needs to be updated.
pub const unsafe fn macro_MUTEX_INIT() -> mutex_t {
    mutex_t {
        queue: list_node_t {
            next: core::ptr::null_mut(),
        },
    }
}
