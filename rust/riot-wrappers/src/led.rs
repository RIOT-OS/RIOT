//! Wrappers for the `LEDn_{ON,OFF,TOGGLE}` macros

use core::convert::Infallible;

/// The Ith LED (calling the `LED<I>_{ON,OFF,TOGGLE}` macros).
///
/// The preferred interface for turning a LED on and off is [switch_hal::OutputSwitch].
///
/// LEDs are accessible safely; any not implemented on a board are silently ignored.
#[derive(Debug)]
pub struct LED<const I: u8>(());

/// The indicated LED is not present on the current board.
#[derive(Debug)]
pub struct LedNotPresent;

impl<const I: u8> LED<I> {
    #[deprecated(
        note = "Use new_unchecked() to retain the behavior this function has always had; future versions of `.new()` will panic when used with a board that does not have that LED.",
        since = "0.9.1"
    )]
    pub const fn new() -> Self {
        Self::new_unchecked()
    }

    /// Accesses the LED numbered `I` as `LED::<I>::new_unchecked()`.
    ///
    /// It is not an error if this board does not have a LED with that number; the resulting struct
    /// will be available but its methods have no effect.
    pub const fn new_unchecked() -> Self {
        const { assert!(I < 8, "RIOT only defines LED0..7") };
        Self(())
    }

    /// Accesses the LED numbered `I` as `LED::<I>::new_checked()?`.
    ///
    /// An LED is returned if present on the board, which is known at build time.
    pub const fn new_checked() -> Result<Self, LedNotPresent> {
        if Self::is_present() {
            Ok(Self(()))
        } else {
            Err(LedNotPresent)
        }
    }

    const fn is_present() -> bool {
        unsafe {
            match I {
                0 => riot_sys::macro_LED0_IS_PRESENT() != -1,
                1 => riot_sys::macro_LED1_IS_PRESENT() != -1,
                2 => riot_sys::macro_LED2_IS_PRESENT() != -1,
                3 => riot_sys::macro_LED3_IS_PRESENT() != -1,
                4 => riot_sys::macro_LED4_IS_PRESENT() != -1,
                5 => riot_sys::macro_LED5_IS_PRESENT() != -1,
                6 => riot_sys::macro_LED6_IS_PRESENT() != -1,
                7 => riot_sys::macro_LED7_IS_PRESENT() != -1,
                _ => panic!("RIOT only defines LED0..7"),
            }
        }
    }
}

impl<const I: u8> switch_hal::OutputSwitch for LED<I> {
    type Error = Infallible;

    fn on(&mut self) -> Result<(), Self::Error> {
        // unsafe: RIOT's LED functions can be called any time (and no-op on undefined LEDs)
        unsafe {
            match I {
                0 => riot_sys::macro_LED0_ON(),
                1 => riot_sys::macro_LED1_ON(),
                2 => riot_sys::macro_LED2_ON(),
                3 => riot_sys::macro_LED3_ON(),
                4 => riot_sys::macro_LED4_ON(),
                5 => riot_sys::macro_LED5_ON(),
                6 => riot_sys::macro_LED6_ON(),
                7 => riot_sys::macro_LED7_ON(),
                _ => unreachable!(),
            }
        };
        Ok(())
    }

    fn off(&mut self) -> Result<(), Self::Error> {
        // unsafe: RIOT's LED functions can be called any time (and no-op on undefined LEDs)
        unsafe {
            match I {
                0 => riot_sys::macro_LED0_OFF(),
                1 => riot_sys::macro_LED1_OFF(),
                2 => riot_sys::macro_LED2_OFF(),
                3 => riot_sys::macro_LED3_OFF(),
                4 => riot_sys::macro_LED4_OFF(),
                5 => riot_sys::macro_LED5_OFF(),
                6 => riot_sys::macro_LED6_OFF(),
                7 => riot_sys::macro_LED7_OFF(),
                _ => unreachable!(),
            }
        };
        Ok(())
    }
}

impl<const I: u8> switch_hal::ToggleableOutputSwitch for LED<I> {
    type Error = Infallible;

    fn toggle(&mut self) -> Result<(), Self::Error> {
        // unsafe: RIOT's LED functions can be called any time (and no-op on undefined LEDs)
        unsafe {
            match I {
                0 => riot_sys::macro_LED0_TOGGLE(),
                1 => riot_sys::macro_LED1_TOGGLE(),
                2 => riot_sys::macro_LED2_TOGGLE(),
                3 => riot_sys::macro_LED3_TOGGLE(),
                4 => riot_sys::macro_LED4_TOGGLE(),
                5 => riot_sys::macro_LED5_TOGGLE(),
                6 => riot_sys::macro_LED6_TOGGLE(),
                7 => riot_sys::macro_LED7_TOGGLE(),
                _ => unreachable!(),
            }
        };
        Ok(())
    }
}
