use core::ffi::CStr;
use riot_wrappers::println;

// saul module
use riot_wrappers::saul::{
    registration::{Drivable, Driver, Error, register_and_then},
    Class, ActuatorClass, Phydat, RegistryEntry
};
// shell module
use riot_wrappers::shell::{self, CommandList};
use crate::gpio::{RGB_BLUE_PIN, RGB_GREEN_PIN, RGB_PORT, RGB_RED_PIN, set_output};

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

struct CurrentColor {
    red: i16,
    green: i16,
    blue: i16,
}

static mut RGB_CURRENT: CurrentColor = CurrentColor {
    red: 0,
    green: 0,
    blue: 0
};

pub static RGB_LED_DRIVER: Driver<RgbLed> = Driver::new();

impl Drivable for &RgbLed {
    const CLASS: Class = Class::Actuator(Some(ActuatorClass::LedRgb));
    const HAS_READ: bool = true;
    const HAS_WRITE: bool = true;

    fn read(self) -> Result<Phydat, Error> {
        let value = Phydat::new(&[
            unsafe { RGB_CURRENT.red },
            unsafe { RGB_CURRENT.green },
            unsafe { RGB_CURRENT.blue }],
            None,
            0
        );
        println!("Reading from RGB LED: {:?}", value);
        Ok(value)
    }

    fn write(self, data: &Phydat) -> Result<u8, Error> {
        println!("Writing {:?} to {}", data, self.name);
        let values = data.value();

        set_output(self.portpin_red.0, self.portpin_red.1, values[0] > 0);
        set_output(self.portpin_green.0, self.portpin_green.1, values[1] > 0);
        set_output(self.portpin_blue.0, self.portpin_blue.1, values[2] > 0);

/*         // obtain GPIO outputs for RGB LED's and set values
        let mut red = get_output(self.portpin_red.0, self.portpin_red.1);
        red.set_state(values[0] > 0);
        let mut green = get_output(self.portpin_green.0, self.portpin_green.1);
        green.set_state(values[1] > 0);
        let mut blue = get_output(self.portpin_blue.0, self.portpin_blue.1);
        blue.set_state(values[2] > 0); */

        // save current RGB values
        unsafe {
            RGB_CURRENT.red = values[0];
            RGB_CURRENT.green = values[1];
            RGB_CURRENT.blue = values[2];
        }

        Ok(1)
    }
}

pub fn find_actuator(actuator_type: ActuatorClass) -> Option<RegistryEntry> {
    let mut rgb_led: Option<RegistryEntry> = None;
    // Find RGB LED at SAUL registry and write values to it
    RegistryEntry::all().for_each(|entry| {
            if let Some(Class::Actuator(Some(actuator_type))) = entry.type_() {
                rgb_led = Some(entry);
            }
        }
    );

    rgb_led
}

pub fn run_shell_with_saul() -> ! {
    let rgb_led = RgbLed::new(
        "Color Temperature Light",
        (RGB_PORT, RGB_RED_PIN),
        (RGB_PORT, RGB_GREEN_PIN),
        (RGB_PORT, RGB_BLUE_PIN),
    );

    register_and_then(
        &RGB_LED_DRIVER,
        &rgb_led,
        Some(CStr::from_bytes_with_nul(b"Color Temperature Light\0").unwrap()),
        || {
            println!("RGB LED registered as SAUL actuator");
            shell::new().run_forever_providing_buf()
        },
    );
}