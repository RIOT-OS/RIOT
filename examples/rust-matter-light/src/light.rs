use core::fmt::Debug;

// riot modules
use riot_wrappers::println;

pub trait OnOffDevice {
    fn onoff_set(&mut self, on: bool);
    fn is_on(&self) -> bool;
}

pub trait ColorLight {
    fn set_color(&mut self, color: RgbColor);
    fn get_current_color(&self) -> Option<&RgbColor>;
}

#[derive(Debug, Clone, Default)]
pub struct RgbColor {
    red: u8, 
    green: u8, 
    blue: u8,
}

pub struct ColorTemperatureLight {
    pub on: bool,
    portpin_red: (u32, u32),
    portpin_green: (u32, u32),
    portpin_blue: (u32, u32),
    current_color: RgbColor,
}

impl ColorTemperatureLight {
    pub fn new(red_port: u32, red_pin: u32, green_port: u32, green_pin: u32, blue_port: u32, blue_pin: u32, ) -> Self {
        Self { 
            on: false,
            portpin_red: (red_port, red_pin),
            portpin_green: (green_port, green_pin),
            portpin_blue: (blue_port, blue_pin),
            current_color: RgbColor::default(),
        }
    }
}

impl Debug for ColorTemperatureLight {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        f.debug_struct("ColorTemperatureLight")
            .field("on", &self.on)
            .field("current_color", &self.current_color)
            .finish()
    }
}

impl ColorLight for ColorTemperatureLight {
    fn set_color(&mut self, color: RgbColor) {
        println!("Setting color: {:?}", &color);
        // TODO: Control GPIO's and set color
        self.current_color = color;
    }

    fn get_current_color(&self) -> Option<&RgbColor> {
        if !self.on {
            return None
        }
        Some(&self.current_color)
    }
}

mod test {
    use super::*;

    #[test]
    fn test_color_temperature_light_new() {
        let light = ColorTemperatureLight::new(0, 1, 1, 2, 2, 2);
        assert_eq!(false, light.on);
        let color = light.current_color;
        assert_eq!(0, color.red);
        assert_eq!(0, color.green);
        assert_eq!(0, color.blue);
    }
}