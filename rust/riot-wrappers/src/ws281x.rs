//! Tools for using the [WS2812/SK6812 RGB LED
//! (NeoPixel)](https://doc.riot-os.org/group__drivers__ws281x.html) drivers

use core::convert::TryInto;
use core::mem::MaybeUninit;

use crate::error::NegativeErrorExt;

/// A WS281x chain backed by an owned buffer of compatible LEDs
pub struct BufferedWs281x<C: ChannelType, const N: usize> {
    pub buffer: [C; N],
    // The device's linked buffer is left blank -- we don't use the setter functions anyway, and
    // the internal reference would require an internal reference.
    dev: riot_sys::ws281x_t,
}

// unsafe: It does contain a dev, but its pointers (by construction) go nowhere
unsafe impl<C: ChannelType + Send, const N: usize> Send for BufferedWs281x<C, N> {}

// The Copy requirement can later be dropped when we Default::default() works for generic arrays
// with the default initializer
impl<C: ChannelType + Default + Copy, const N: usize> BufferedWs281x<C, N> {
    /// Initialize a device and associated buffer.
    ///
    /// This requires having an explicit pin at hand (which is notoriously hard to construct with
    /// riot-sys as GPIO_PIN is a macro); an alternative could be starting one from a ws281x_params
    /// obtained from the board.
    pub fn init(pin: crate::gpio::GPIO) -> Self {
        let mut dev = MaybeUninit::uninit();

        let params = riot_sys::ws281x_params_t {
            buf: core::ptr::null::<u8>() as _,
            numof: 0,
            pin: pin.to_c(),
        };

        unsafe { riot_sys::ws281x_init(dev.as_mut_ptr(), &params) }
            .negative_to_error()
            .expect("Init failed");

        Self {
            buffer: [Default::default(); N],
            dev: unsafe { dev.assume_init() },
        }
    }
}

impl<C: ChannelType, const N: usize> BufferedWs281x<C, N> {
    pub fn write(&mut self) {
        unsafe {
            riot_sys::ws281x_prepare_transmission(crate::inline_cast_mut(&mut self.dev as *mut _));
            riot_sys::ws281x_write_buffer(
                &mut self.dev,
                &self.buffer as *const _ as *const core::ffi::c_void,
                (N * core::mem::size_of::<C>())
                    .try_into()
                    .expect("Buffer exceeds experssible range"),
            );
            riot_sys::ws281x_end_transmission(crate::inline_cast_mut(&mut self.dev as *mut _));
        }
    }
}

/// Value representing a single LED on a WS281x strip
///
/// The memory representation of implementations needs to be suitable to be sent right on as bytes.
/// This does not make the trait unsafe to implement: ws281x_write_buffer takes data as void
/// pointer reinterpreted as u8 pointer, and any memory content is valid in this view.
pub trait ChannelType: Sized {}

#[derive(Default, Copy, Clone)]
#[repr(transparent)]
pub struct GRBW([u8; 4]);

impl ChannelType for GRBW {}

impl GRBW {
    pub fn rgbw(&self) -> (u8, u8, u8, u8) {
        (self.0[0], self.0[1], self.0[2], self.0[3])
    }

    pub fn set_rgbw(&mut self, r: u8, g: u8, b: u8, w: u8) {
        self.0 = [g, r, b, w];
    }
}

#[derive(Default, Copy, Clone)]
#[repr(transparent)]
pub struct GRB([u8; 3]);

impl ChannelType for GRB {}

impl GRB {
    pub fn rgb(&self) -> (u8, u8, u8) {
        (self.0[0], self.0[1], self.0[2])
    }

    pub fn set_rgb(&mut self, r: u8, g: u8, b: u8) {
        self.0 = [g, r, b];
    }
}
