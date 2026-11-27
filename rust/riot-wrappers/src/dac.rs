use riot_sys::dac_t;

#[derive(Debug)]
pub struct DACLine {
    line: dac_t,
}

#[derive(Debug)]
#[non_exhaustive]
pub enum DACError {
    /// The given dac_t line did not exist
    NoLine,
    /// An unknown error did occur
    Unknown,
}

impl DACLine {
    /// Creates and initializes a new [`DACLine`].
    ///
    /// The `index` indicates which dac device from the current board should be used.
    /// For information on how many such devices are available for this board please
    /// refer to its RIOT documentation.
    ///
    /// Returns an Error if the given line does not exist
    /// on the board.
    pub fn new(index: usize) -> Result<Self, DACError> {
        let line = unsafe { riot_sys::macro_DAC_LINE(index as u32) };
        let res = unsafe { riot_sys::dac_init(line) } as i32;

        const DAC_OK: i32 = riot_sys::DAC_OK as i32;
        const DAC_NOLINE: i32 = riot_sys::DAC_NOLINE as i32;

        match res {
            DAC_OK => Ok(DACLine { line }),
            DAC_NOLINE => Err(DACError::NoLine),
            _ => Err(DACError::Unknown),
        }
    }

    /// Builds a [`DACLine`] from an already initialized [`dac_t`].
    ///
    /// Providing a not initialized [`dac_t`] results in undefined behavior.
    pub unsafe fn new_without_init(line: dac_t) -> Self {
        DACLine { line }
    }

    /// Writes the given value to this [`DACLine`]
    ///
    /// The `value` is internally scaled to the underlying
    /// dac device so that the maximum voltage output
    /// is always equivalent to [`u16::MAX`]
    pub fn set(&mut self, value: u16) {
        unsafe { riot_sys::dac_set(self.line, value) }
    }

    /// Turns the [`DACLine`] on after `DACLine::power_off`
    pub fn power_on(&mut self) {
        unsafe { riot_sys::dac_poweron(self.line) }
    }

    /// Turns the [`DACLine`] off
    pub fn power_off(&mut self) {
        unsafe { riot_sys::dac_poweroff(self.line) }
    }
}
