use core::convert::Infallible;

pub struct ADCLine(riot_sys::adc_t);

impl ADCLine {
    /// Initialize an ADC line and get it as a handle. This is declared as unsafe as it may only
    /// be called once. (A safe abstraction would need to check which RIOT devices have been
    /// initialized already).
    ///
    /// This being unsafe is inconsistent with other subsystem wrappers that chose to not declare
    /// this unsafe; that inconsistency is tracked in
    /// <https://github.com/RIOT-OS/rust-riot-wrappers/issues/59> and so far unresolved.
    pub unsafe fn init(line: riot_sys::adc_t) -> Result<Self, i32> {
        let success = riot_sys::adc_init(line);
        match success {
            0 => Ok(ADCLine(line)),
            e => Err(e),
        }
    }

    /// Initialize an ADC line identified by the line number it is assigned on the board
    ///
    /// Safety: See [.init()][Self::init()]
    pub unsafe fn from_number(line: u32) -> Result<Self, i32> {
        let line = riot_sys::macro_ADC_LINE(line);
        Self::init(line)
    }
}

/// A configured representation of the single operating-system level ADC that RIOT exposes via its
/// ADC API. The individual ADC lines are addressed as ADCLine structs and can be used uniformly
/// with the (any) ADC struct. The differences between the hardware ADCs are as hidden to the
/// embedded_hal API as they are hidden to RIOT applications.
pub struct ADC {
    pub resolution: riot_sys::adc_res_t,
}

impl embedded_hal_0_2::adc::Channel<ADC> for ADCLine {
    type ID = riot_sys::adc_t;
    fn channel() -> Self::ID {
        unimplemented!("See https://github.com/rust-embedded/embedded-hal/issues/110")
    }
}

impl embedded_hal_0_2::adc::OneShot<ADC, i32, ADCLine> for ADC {
    type Error = Infallible;

    fn read(&mut self, pin: &mut ADCLine) -> nb::Result<i32, Infallible> {
        // Sorry, blocking still
        Ok(unsafe { riot_sys::adc_sample(pin.0, self.resolution) })
    }
}
