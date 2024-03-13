use log::info;
use riot_wrappers::saul::{ActuatorClass, Class, Phydat};
use riot_wrappers::saul::registration::{Drivable, Driver, Error};

pub const RGB_PORT: u32 = 0;
pub const RGB_RED_PIN: u32 = 7;
pub const RGB_GREEN_PIN: u32 = 6;
pub const RGB_BLUE_PIN: u32 = 5;

pub struct RgbLed {
    name: &'static str,
    portpin_red: (u32, u32),
    portpin_green: (u32, u32),
    portpin_blue: (u32, u32),
}

impl RgbLed {
    pub fn new(name: &'static str, red: (u32, u32), green: (u32, u32), blue: (u32, u32)) -> Self {
        RgbLed {
            name,
            portpin_red: red,
            portpin_green: green,
            portpin_blue: blue,
        }
    }
}

pub static RGB_LED_DRIVER: Driver<RgbLed> = Driver::new();

impl Drivable for &RgbLed {
    const CLASS: Class = Class::Actuator(Some(ActuatorClass::LedRgb));
    const HAS_READ: bool = true;
    const HAS_WRITE: bool = true;

    fn read(self) -> Result<Phydat, Error> {
        let value = Phydat::new(&[0,0,0], None, 0);
        info!("Reading from RGB LED: {:?}", value);
        Ok(value)
    }

    fn write(self, data: &Phydat) -> Result<u8, Error> {
        info!("Writing {:?} to {}", data, self.name);
        Ok(1)
    }
}