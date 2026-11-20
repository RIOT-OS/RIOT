//! Access to [RIOT's UART](https://doc.riot-os.org/group__drivers__periph__uart.html)
//!
//! Author: Kilian Barning <barning@uni-bremen.de>

use core::ptr;

use crate::error::{NegativeErrorExt, NumericError};
use crate::libc::{c_uint, c_void};
use riot_sys::*;

/// Error representing the status returned by various `UART`-functions.
#[derive(Debug)]
#[non_exhaustive]
pub enum UartDeviceError {
    InvalidDevice,
    UnsupportedConfig,
    Other,
}

impl From<NumericError> for UartDeviceError {
    fn from(n: NumericError) -> Self {
        match n {
            crate::error::ENODEV => Self::InvalidDevice,
            crate::error::ENOTSUP => Self::UnsupportedConfig,
            _ => Self::Other,
        }
    }
}

/// Number of data bits configured for a UART.
#[cfg(riot_module_periph_uart_modecfg)]
#[derive(Debug)]
#[non_exhaustive]
pub enum DataBits {
    Five,
    Six,
    Seven,
    Eight,
}

#[cfg(riot_module_periph_uart_modecfg)]
impl DataBits {
    fn to_c(self) -> uart_data_bits_t {
        match self {
            Self::Five => uart_data_bits_t_UART_DATA_BITS_5,
            Self::Six => uart_data_bits_t_UART_DATA_BITS_6,
            Self::Seven => uart_data_bits_t_UART_DATA_BITS_7,
            Self::Eight => uart_data_bits_t_UART_DATA_BITS_8,
        }
    }
}

/// Kind of parity bit configured for a UART.
#[cfg(riot_module_periph_uart_modecfg)]
#[derive(Debug)]
#[non_exhaustive]
pub enum Parity {
    None,
    Even,
    Odd,
    Mark,
    Space,
}

#[cfg(riot_module_periph_uart_modecfg)]
impl Parity {
    fn to_c(self) -> uart_parity_t {
        match self {
            Self::None => uart_parity_t_UART_PARITY_NONE,
            Self::Even => uart_parity_t_UART_PARITY_EVEN,
            Self::Odd => uart_parity_t_UART_PARITY_ODD,
            Self::Mark => uart_parity_t_UART_PARITY_MARK,
            Self::Space => uart_parity_t_UART_PARITY_SPACE,
        }
    }
}

/// Number of stop bits configured for a UART.
#[cfg(riot_module_periph_uart_modecfg)]
#[derive(Debug)]
#[non_exhaustive]
pub enum StopBits {
    One,
    Two,
}

#[cfg(riot_module_periph_uart_modecfg)]
impl StopBits {
    fn to_c(self) -> uart_stop_bits_t {
        match self {
            Self::One => uart_stop_bits_t_UART_STOP_BITS_1,
            Self::Two => uart_stop_bits_t_UART_STOP_BITS_2,
        }
    }
}

/// This struct contains the `UART` device and handles all operation regarding it.
///
/// The lifetime `'cb` indicates how long a registered callback on the device lives; for many
/// cases, that is `'static`.
#[derive(Debug)]
pub struct UartDevice<'cb> {
    dev: uart_t,
    _phantom: core::marker::PhantomData<&'cb ()>,
}

impl UartDevice<'static> {
    /// Initialize the given `UART`; as the name implies, the created `UART` device can <b>ONLY</b> send data.
    ///
    /// Returns a Result with rather `Ok<Self>` if the UART was initialized successfully or a
    /// `Err<UartDeviceStatus>` containing the error.
    ///
    /// # Arguments
    ///
    /// * `dev` – The index of the hardware device
    /// * `baud` – The used baud rate
    ///
    /// # Examples
    /// ```
    /// use riot_wrappers::uart::UartDevice;
    /// let mut uart = UartDevice::new_without_rx(0, 115200)
    ///     .unwrap_or_else(|e| panic!("Error initializing UART: {e:?}"));
    /// uart.write(b"Hello from UART");
    /// ```
    pub fn new_without_rx(index: usize, baud: u32) -> Result<Self, UartDeviceError> {
        unsafe {
            let dev = macro_UART_DEV(index as c_uint);
            uart_init(dev, baud, None, ptr::null_mut()).negative_to_error()?;
            Ok(Self {
                dev,
                _phantom: Default::default(),
            })
        }
    }

    /// Initialize the given `UART` with a static callback.
    ///
    /// Returns a Result with rather `Ok<Self>` if the UART was initialized successfully or a
    /// `Err<UartDeviceStatus>` containing the error.
    ///
    /// # Arguments
    ///
    /// * `dev` – The index of the hardware device
    /// * `baud` – The used baud rate
    /// * `user_callback` – The user defined callback that gets called from the os whenever new data is received from the `UART`
    ///
    /// # Examples
    /// ```
    /// #![feature(type_alias_impl_trait)]
    /// use riot_wrappers::uart::UartDevice;
    /// use static_cell::StaticCell;
    /// static LATEST_BYTE: StaticCell<Option<u8>> = StaticCell::new();
    /// let latest_byte = LATEST_BYTE.init(None);
    /// mod tait {
    ///     // `type_alias_impl_trait` requires uniqueness of a defining use within a module
    ///     pub type Cb = impl FnMut(u8) + Send;
    ///     pub fn build_callback(latest_byte: &'static mut Option<u8>) -> Cb {
    ///         |byte| {*latest_byte = Some(byte)}
    ///     }
    /// }
    /// static CB: StaticCell<tait::Cb> = StaticCell::new();
    /// let mut cb = CB.init(tait::build_callback(latest_byte));
    /// let mut uart = UartDevice::new_with_static_cb(0, 115200, cb)
    ///     .unwrap_or_else(|e| panic!("Error initializing UART: {e:?}"));
    /// uart.write(b"Hello from UART");
    /// ```
    pub fn new_with_static_cb<F>(
        index: usize,
        baud: u32,
        user_callback: &'static mut F,
    ) -> Result<Self, UartDeviceError>
    where
        F: FnMut(u8) + Send + 'static,
    {
        unsafe { Self::construct_uart(index, baud, user_callback) }
    }

    /// Initializes the given `UART`, and runs a `main` function while it is configured.
    ///
    /// Returns a Result with rather `Ok<RMain>` where `RMain` is the value returned by the scoped main function
    /// or a `Err<UartDeviceStatus>` containing the error.
    ///
    /// This is the scoped version of [`new_with_static_cb()`] that can be used if you want to use short-lived callbacks, such as
    /// closures or anything containing references. The `UartDevice` is deconfigured when the internal main function
    /// terminates. A common pattern around this kind of scoped functions is that `main` contains the application's
    /// main loop, and never terminates (in which case the clean-up code is eliminated during compilation).
    ///
    /// # Arguments
    ///
    /// * `dev` – The index of the hardware device
    /// * `baud` – The used baud rate
    /// * `user_callback` – The user defined callback that gets called from the os whenever new data is received from the `UART`
    /// * `main` – The main loop that is executed inside the wrapper
    ///
    /// # Examples
    /// ```
    /// use riot_wrappers::uart::UartDevice;
    /// let mut cb = |new_data| {
    ///     println!("Received {:02x}", new_data);
    /// };
    /// let mut scoped_main = |self_: &mut UartDevice| loop {
    ///    self_.write(b"Hello from UART")
    /// };
    /// let mut uart = UartDevice::new_scoped(0, 115200, &mut cb, scoped_main)
    ///    .unwrap_or_else(|e| panic!("Error initializing UART: {e:?}"));
    /// ```
    pub fn new_scoped<F, Main, RMain>(
        index: usize,
        baud: u32,
        user_callback: &mut F,
        main: Main,
    ) -> Result<RMain, UartDeviceError>
    where
        F: FnMut(u8) + Send,
        Main: for<'brand> FnOnce(&mut UartDevice<'brand>) -> RMain,
    {
        // This possibly relies on Rust code in RIOT to not unwind.
        let mut self_ = unsafe { UartDevice::construct_uart(index, baud, user_callback) }?;
        let result = (main)(&mut self_);
        drop(self_);
        Ok(result)
    }
}

impl<'cb> UartDevice<'cb> {
    /// Creates a UART with an arbitrary lifetime.
    ///
    /// # Unsafety
    ///
    /// To use this safely, the caller must ensure that the returned Self is reliably destructed before &'cb mut F becomes unavailable.
    unsafe fn construct_uart<F>(
        index: usize,
        baud: u32,
        user_callback: &'cb mut F,
    ) -> Result<Self, UartDeviceError>
    where
        F: FnMut(u8) + Send + 'cb,
    {
        let dev = macro_UART_DEV(index as c_uint);
        uart_init(
            dev,
            baud,
            Some(Self::new_data_callback::<F>),
            user_callback as *mut _ as *mut c_void,
        )
        .negative_to_error()?;
        Ok(Self {
            dev,
            _phantom: Default::default(),
        })
    }

    /// Transmits the given data via the `UART`-device.
    ///
    /// # Examples
    /// ```
    /// use riot_wrappers::uart::UartDevice;
    /// let mut uart = UartDevice::new_without_rx(0, 115200)
    ///    .unwrap_or_else(|e| panic!("Error initializing UART: {e:?}"));
    /// uart.write(b"Hello from UART\n");
    /// ```
    pub fn write(&mut self, data: &[u8]) {
        unsafe {
            uart_write(self.dev, data.as_ptr(), data.len() as size_t);
        }
    }

    /// Turns on the power from the `UART-Device`.
    pub fn power_on(&mut self) {
        unsafe { uart_poweron(self.dev) };
    }

    /// Turns off the power from the `UART-Device`.
    pub fn power_off(&mut self) {
        unsafe { uart_poweroff(self.dev) };
    }

    /// Sets the mode according to the given parameters.
    ///
    /// Should the parameters be invalid, the function returns a Err<UartDeviceStatus::UnsupportedConfig>
    ///
    /// # Arguments
    /// * `data_bits` - Number of data bits in a UART frame
    /// * `parity` - Parity mode
    /// * `stop_bits` - Number of stop bits in a UART frame
    ///
    /// # Examples
    /// ```
    /// use riot_wrappers::uart::{DataBits, Parity, StopBits, UartDevice};
    /// let mut uart = UartDevice::new_without_rx(0, 115200)
    ///     .unwrap_or_else(|e| panic!("Error initializing UART: {e:?}"));   
    /// uart.set_mode(DataBits::Eight, Parity::None, StopBits::One)   
    ///     .unwrap_or_else(|e| panic!("Error setting UART mode: {e:?}"));
    /// ```
    #[cfg(riot_module_periph_uart_modecfg)]
    pub fn set_mode(
        &mut self,
        data_bits: DataBits,
        parity: Parity,
        stop_bits: StopBits,
    ) -> Result<(), UartDeviceError> {
        unsafe {
            uart_mode(self.dev, data_bits.to_c(), parity.to_c(), stop_bits.to_c())
                .negative_to_error()?;
            Ok(())
        }
    }

    /// Undoes the effects of [.deinit_pins()][Self::deinit_pins].
    ///
    /// This function normally does not need to be called. But in some case, the pins on the `UART`
    /// might be shared with some other functionality (like `GPIO`). In this case, it is necessary
    /// to give the user the possibility to init the pins again.
    #[cfg(riot_module_periph_uart_reconfigure)]
    pub unsafe fn init_pins(&mut self) {
        uart_init_pins(self.dev);
    }

    /// Changes the pins back to plain GPIO functionality.
    #[cfg(riot_module_periph_uart_reconfigure)]
    pub unsafe fn deinit_pins(&mut self) {
        uart_deinit_pins(self.dev);
    }

    /// This is the callback that gets called directly from the kernel if new data from the `UART` is received.
    ///
    /// # Arguments
    ///
    /// * `user_callback` - The address pointing to the user defined callback
    /// * `data` - The newly received data from the `UART`  
    unsafe extern "C" fn new_data_callback<F>(user_callback: *mut c_void, data: u8)
    where
        F: FnMut(u8) + 'cb,
    {
        (*(user_callback as *mut F))(data); // We cast the void* back to the closure and call it
    }
}

impl<'cb> Drop for UartDevice<'cb> {
    /// The `drop` method resets the `UART`, removes the interrupt and tries
    /// to reset the `GPIO` pins if possible.
    fn drop(&mut self) {
        unsafe {
            uart_init(self.dev, 9600, None, ptr::null_mut());
            #[cfg(riot_module_periph_uart_reconfigure)]
            self.deinit_pins();
        }
    }
}
