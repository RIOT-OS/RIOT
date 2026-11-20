/// Workaround to get name for Never type for the few places it is actually needed (eg. as return
/// type for callback signatures)
///
/// From <https://github.com/rust-lang/rust/issues/43301#issuecomment-912390203>, adjusted for
/// usability with pub interfaces by using a pub trait in a private module (sealing).
///
/// # Deprecation
///
/// While on its own this does no harm, using the Never type to implement a trait on it has long
/// been known to be problematic with the 1.100 release.
///
/// In this crate, this is worked around by inspecting the rustc version, which is a crude hack,
/// and therefore the MSRV is to be bumped to 1.100 soon.
///
/// Once that happens, the feature and this module will be removed.
#[cfg(not(feature = "actual_never_type"))]
use crate::helpers::*;

#[cfg(not(feature = "actual_never_type"))]
pub(crate) type Never = <fn() -> ! as ReturnTypeExtractor>::ReturnType;
#[cfg(feature = "actual_never_type")]
pub(crate) type Never = !;
