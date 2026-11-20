use super::*;

use core::convert::Infallible;
use embedded_hal::digital::{ErrorType, InputPin, OutputPin, PinState};

impl ErrorType for InputGPIO {
    type Error = Infallible;
}

impl InputPin for InputGPIO {
    fn is_high(&mut self) -> Result<bool, Infallible> {
        Ok(InputGPIO::is_high(self))
    }

    fn is_low(&mut self) -> Result<bool, Infallible> {
        Ok(InputGPIO::is_low(self))
    }
}

impl ErrorType for OutputGPIO {
    type Error = Infallible;
}

impl OutputPin for OutputGPIO {
    fn set_high(&mut self) -> Result<(), Infallible> {
        Ok(OutputGPIO::set_high(self))
    }

    fn set_low(&mut self) -> Result<(), Infallible> {
        Ok(OutputGPIO::set_low(self))
    }

    fn set_state(&mut self, state: PinState) -> Result<(), Infallible> {
        Ok(OutputGPIO::set_state(self, state.into()))
    }
}

impl ErrorType for InOutGPIO {
    type Error = Infallible;
}

impl InputPin for InOutGPIO {
    fn is_high(&mut self) -> Result<bool, Infallible> {
        Ok(InOutGPIO::is_high(self))
    }

    fn is_low(&mut self) -> Result<bool, Infallible> {
        Ok(InOutGPIO::is_low(self))
    }
}

impl OutputPin for InOutGPIO {
    fn set_high(&mut self) -> Result<(), Infallible> {
        Ok(InOutGPIO::set_high(self))
    }

    fn set_low(&mut self) -> Result<(), Infallible> {
        Ok(InOutGPIO::set_low(self))
    }

    fn set_state(&mut self, state: PinState) -> Result<(), Infallible> {
        Ok(InOutGPIO::set_state(self, state.into()))
    }
}
