//! RIOT's system random number generator

/// The global random number generator available in RIOT
///
/// Actual functionality is available through its implementation of [rand_core_06::RngCore]. It
/// implements [rand_core_06::CryptoRng] if the RIOT module `prng_shaxprng` is present. (If more
/// CSPRNGs are implemented in RIOT, the list around this implementation needs to be extended).
///
/// Note that this *is* copy (unlike what RngCore recommends) -- because the state is not in here
/// but global (as is their own OsRng).
#[derive(Copy, Clone, Debug)]
pub struct Random(());

impl Random {
    /// Access the system random number generator
    #[cfg(riot_module_auto_init_random)]
    pub fn new() -> Self {
        Random(())
    }

    /// Seed and start the random number generator
    ///
    /// While technically not unsound, this is marked unsafe as it may overwrite existing good RNG
    /// state.
    ///
    /// This is not going through the the [rand_core_06::SeedableRng] trait because that would
    /// require per-RNG seedability.
    pub unsafe fn new_with_seed(seed: u32) -> Self {
        riot_sys::random_init(seed);
        Random(())
    }
}

#[cfg(riot_module_auto_init_random)]
impl Default for Random {
    fn default() -> Self {
        Self::new()
    }
}

impl rand_core_06::RngCore for Random {
    fn next_u32(&mut self) -> u32 {
        // unsafe: C API makes no requirements, and the type ensures it's seeded
        unsafe { riot_sys::random_uint32() }
    }

    fn next_u64(&mut self) -> u64 {
        let mut result = core::mem::MaybeUninit::uninit();
        // unsafe: C API makes no requirements, and the type ensures it's seeded
        unsafe { riot_sys::random_bytes(result.as_mut_ptr() as _, 8) };
        // unsafe: We had it written, and every state is inhabited
        unsafe { result.assume_init() }
    }

    fn fill_bytes(&mut self, dest: &mut [u8]) {
        // unsafe: C API makes no requirements, and the type ensures it's seeded
        unsafe { riot_sys::random_bytes(dest.as_mut_ptr() as _, dest.len() as _) };
    }

    fn try_fill_bytes(&mut self, dest: &mut [u8]) -> Result<(), rand_core_06::Error> {
        Ok(self.fill_bytes(dest))
    }
}

#[cfg(riot_module_prng_shaxprng)]
impl rand_core_06::CryptoRng for Random {}
