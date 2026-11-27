//! Access to [RIOT's GPIO pins](http://doc.riot-os.org/group__drivers__periph__gpio.html)
//!
//! The various configured GPIO types ([InputGPIO], [OutputGPIO], [InOutGPIO]) can be used through
//! the [embedded_hal::digital] traits. As recommended for infallible types, they also
//! provide identically named direct methods, which (for input pins) also work on shared reference.

mod impl_1;

use riot_sys::{gpio_clear, gpio_mode_t, gpio_read, gpio_set, gpio_t, gpio_toggle, gpio_write};

use crate::error::NegativeErrorExt;

/// A Rust representation of RIOT's gpio_t, representing a single pin in no particular
/// configuration.
pub struct GPIO(gpio_t);

/// The subset of gpio_mode_t equivalents usable when creating an [InputGPIO]
#[non_exhaustive]
pub enum InputMode {
    In,
    InPullDown,
    InPullUp,
}

impl InputMode {
    fn to_c(self) -> gpio_mode_t {
        match self {
            InputMode::In => riot_sys::gpio_mode_t_GPIO_IN,
            InputMode::InPullDown => riot_sys::gpio_mode_t_GPIO_IN_PD,
            InputMode::InPullUp => riot_sys::gpio_mode_t_GPIO_IN_PU,
        }
    }
}

/// The subset of gpio_mode_t equivalents usable when creating an [OutputGPIO]
#[non_exhaustive]
pub enum OutputMode {
    Out,
    OpenDrain,
    OpenDrainPullUp,
}

impl OutputMode {
    fn to_c(self) -> gpio_mode_t {
        match self {
            OutputMode::Out => riot_sys::gpio_mode_t_GPIO_OUT,
            OutputMode::OpenDrain => riot_sys::gpio_mode_t_GPIO_OD,
            OutputMode::OpenDrainPullUp => riot_sys::gpio_mode_t_GPIO_OD_PU,
        }
    }
}

/// The subset of gpio_mode_t equivalents usable when creating an [InOutGPIO]
#[non_exhaustive]
pub enum InOutMode {
    OpenDrain,
    OpenDrainPullUp,
}

impl InOutMode {
    fn to_c(self) -> gpio_mode_t {
        match self {
            InOutMode::OpenDrain => riot_sys::gpio_mode_t_GPIO_OD,
            InOutMode::OpenDrainPullUp => riot_sys::gpio_mode_t_GPIO_OD_PU,
        }
    }
}

impl GPIO {
    /// Create a GPIO from a gpio_t
    ///
    /// This is as safe as any device acquisition from C is -- RIOT's drivers are (hopefully)
    /// written in such a way that concurrent writes to adjacent pins don't interfere, and those to
    /// the same device are "just" racy.
    ///
    /// (This also means that it is completely possible to have two objects for the same pin
    /// configured in different states, changing the mode while the other is around. The underlying
    /// operating system operates this, but interactions with a reconfigured pin will obviously not
    /// have the intended effect).
    pub fn from_c(gpio: gpio_t) -> Option<Self> {
        if unsafe { riot_sys::gpio_is_valid(gpio) } != 0 {
            Some(GPIO(gpio))
        } else {
            None
        }
    }

    /// Create a GPIO from its port and pin numbers
    ///
    /// ```
    /// # #![no_std]
    /// # #![no_main]
    /// # fn f() {
    /// use riot_wrappers::gpio::GPIO;
    /// let pin_c8 = GPIO::from_port_and_pin(3, 8);
    /// # }
    /// ```
    ///
    /// See [.from_c()][Self::from_c()] for safety constraints.
    pub fn from_port_and_pin(port: u32, pin: u32) -> Option<Self> {
        Self::from_c(unsafe { riot_sys::macro_GPIO_PIN(port, pin) })
    }

    pub fn configure_as_output(
        self,
        mode: OutputMode,
    ) -> Result<OutputGPIO, crate::error::NumericError> {
        unsafe { riot_sys::gpio_init(self.0, mode.to_c()) }.negative_to_error()?;
        Ok(OutputGPIO(self))
    }

    pub fn configure_as_input(
        self,
        mode: InputMode,
    ) -> Result<InputGPIO, crate::error::NumericError> {
        unsafe { riot_sys::gpio_init(self.0, mode.to_c()) }.negative_to_error()?;
        Ok(InputGPIO(self))
    }

    pub fn configure_as_inout(
        self,
        mode: InOutMode,
    ) -> Result<InOutGPIO, crate::error::NumericError> {
        unsafe { riot_sys::gpio_init(self.0, mode.to_c()) }.negative_to_error()?;
        Ok(InOutGPIO(self))
    }

    /// Get a gpio_t from a configured pin
    ///
    /// This is typically useful when populating a RIOT mechanism that works on a pre-configured
    /// pin.
    pub fn to_c(&self) -> riot_sys::gpio_t {
        self.0
    }
}

/// A [GPIO] configured and usable for output
pub struct OutputGPIO(GPIO);

impl OutputGPIO {
    /// See [GPIO::to_c]
    pub fn to_c(&self) -> riot_sys::gpio_t {
        self.0.to_c()
    }

    /// Lose information about how the pin is configured, making it configurable again
    pub fn deconfigured(self) -> GPIO {
        self.0
    }

    pub fn set_high(&mut self) {
        unsafe { gpio_set(self.to_c()) };
    }

    pub fn set_low(&mut self) {
        unsafe { gpio_clear(self.to_c()) };
    }

    pub fn set_state(&mut self, state: bool) {
        unsafe { gpio_write(self.to_c(), state as _) };
    }

    /// Toggles the pin between high and low.
    ///
    /// Unlike [`.set_high()`] and [`.set_low()`], this is not just an alias of the [embedded-hal
    /// trait method of the same
    /// name](https://docs.rs/embedded-hal/latest/embedded_hal/digital/trait.StatefulOutputPin.html#method.toggle):
    /// RIOT GPIO pins do not implement [`embedded_hal::digital::StatefulOutputPin`] because they
    /// can not read back their configured state (but *can* toggle by implementation).
    #[doc(alias = "gpio_toggle")]
    pub fn toggle(&mut self) {
        unsafe { gpio_toggle(self.to_c()) };
    }
}

/// A [GPIO] configured and usable for input
pub struct InputGPIO(GPIO);

impl InputGPIO {
    /// See [GPIO::to_c]
    pub fn to_c(&self) -> riot_sys::gpio_t {
        self.0.to_c()
    }

    /// Lose information about how the pin is configured, making it configurable again
    pub fn deconfigured(self) -> GPIO {
        self.0
    }

    pub fn is_high(&self) -> bool {
        unsafe { i32::from(gpio_read(self.to_c())) != 0 }
    }

    pub fn is_low(&self) -> bool {
        !self.is_high()
    }
}

/// A [GPIO] configured and usable for input and output
pub struct InOutGPIO(GPIO);

impl InOutGPIO {
    /// See [GPIO::to_c]
    pub fn to_c(&self) -> riot_sys::gpio_t {
        self.0.to_c()
    }

    /// Lose information about how the pin is configured, making it configurable again
    pub fn deconfigured(self) -> GPIO {
        self.0
    }

    pub fn set_high(&mut self) {
        unsafe { gpio_set(self.to_c()) };
    }

    pub fn set_low(&mut self) {
        unsafe { gpio_clear(self.to_c()) };
    }

    pub fn set_state(&mut self, state: bool) {
        unsafe { gpio_write(self.to_c(), state as _) };
    }

    pub fn is_high(&self) -> bool {
        unsafe { i32::from(gpio_read(self.to_c())) != 0 }
    }

    pub fn is_low(&self) -> bool {
        !self.is_high()
    }
}
