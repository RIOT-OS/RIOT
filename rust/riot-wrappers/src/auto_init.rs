//! Tools for declaring a function that is run during initialization
//!
//! The [`auto_init!`](super::auto_init!) macro is this module's main product.

/// Wrapper around [riot_sys::auto_init_module_t]
///
/// Its main purpose is to make it Sync; its constructor also takes responsibility of fields that
/// may or may not be present in the struct depending on the build configuration.
#[repr(transparent)]
pub struct AutoInitModule(riot_sys::auto_init_module_t);

impl AutoInitModule {
    /// Initializer for module auto-initialization
    ///
    /// Do not call this directly: Its result must be placed in a static in a special section in
    /// memory, which is handled by the [`auto_init!`](super::auto_init!) macro.
    pub const fn new(
        init_function: extern "C" fn(),
        priority: u16,
        name: &'static core::ffi::CStr,
    ) -> Self {
        let result;
        #[cfg(marker_config_auto_init_enable_debug)]
        {
            result = Self(riot_sys::auto_init_module_t {
                init: Some(init_function),
                prio: priority,
                name: name.as_ptr() as _,
            });
        }
        #[cfg(not(marker_config_auto_init_enable_debug))]
        {
            let _ = priority;
            let _ = name;
            result = Self(riot_sys::auto_init_module_t {
                init: Some(init_function),
            });
        }
        result
    }
}

// unsafe: The items do not publicly expose anything, so just referencing them from anywhere does no
// harm. (Actual usage happens through XFA from C).
unsafe impl Sync for AutoInitModule {}

/// Run the function `$func` during auto initialization, with the priority giving the position in
/// the initialization sequence.
///
/// Note that the priority has to be a literal value. Supporting configured priorities would be
/// possible with proc macros, but their complexity would be excessive as long as this is not
/// needed.
#[macro_export]
macro_rules! auto_init {
    ( $func:ident, $priority:literal ) => {
        // Cheating our way around how we can't build identifiers like proc macros can: We're
        // referring to a function with a name; there's nothing stopping us from creating a module
        // with the same name.
        mod $func {
            extern "C" fn wrapped_as_extern_c() {
                super::$func()
            }

            #[link_section = concat!(".roxfa.auto_init_xfa.", $priority)]
            #[export_name = concat!("auto_init_xfa_", stringify!($func))] // do we need this?
            static AUTO_INIT_MODULE: $crate::auto_init::AutoInitModule =
                $crate::auto_init::AutoInitModule::new(
                    wrapped_as_extern_c,
                    $priority,
                    // unsafe: Constructing from an identifier which can't have null bytes
                    unsafe {
                        core::ffi::CStr::from_bytes_with_nul_unchecked(
                            concat!(stringify!($func), "\0").as_bytes(),
                        )
                    },
                );
        }
    };
}
