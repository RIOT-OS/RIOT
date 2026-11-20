//! Tools for registering a Rust device in SAUL
//!
//! A SAUL sensor or actuator is expressed as an implementation of [Drivable]. Once built, that
//! drivable is registered at SAUL, either through [register_and_then], or through building a
//! [Registration] for a `'static` place and calling [Registration::register_static].
//!
//! As SAUL decouples the per-type parts of a sensor from the per-instance parts, there is a
//! [Driver] struct that manages the per-type aspects. This driver also manages the dynamic
//! dispatch by being generic over the [Drivable] and exposing untyped function pointers. (In a
//! sense, SAUL ships its own version of Rust's `dyn`, and Driver manages that).

use crate::libc;
use core::ffi::CStr;

use super::{Class, Phydat};
use crate::error::NegativeErrorExt;

/// The single error read and write operations may produce; corresponds to an `-ECANCELED`.
/// (-ENOTSUP is expressed by not having support for the operation in the first place, indicated by
/// the `HAS_{READ,WRITE}` consts).
pub struct Error;

/// API through which SAUL operations are done
///
/// This is typically implemented on a `&T` (where T is what the Driver and Registration is for),
/// but can be alternatively implemented on a newtype around such pointers to drive various aspects
/// of a device.
pub trait Drivable: Sized {
    /// Sensor class (type)
    const CLASS: Class;

    /// Set to true if `read` is implemented.
    ///
    /// Doing this on the type level (rather than having read and write return a more
    /// differentiated error) allows the driver to point to the shared [riot_sys::saul_read_notsup]
    /// / [riot_sys::saul_write_notsup] handler rather than to monomorphize a custom erring handler
    /// for each device.
    const HAS_READ: bool = false;
    /// Set to true if `write` is implemented.
    const HAS_WRITE: bool = false;

    /// Read the current state
    fn read(self) -> Result<Phydat, Error> {
        // This function's presence in generated code should already show that something is
        // configured badly; could consider making that a linker error (but riot-wrappers is not in
        // the habit of doing that).
        unimplemented!("Sensor reading not implemented; HAS_READ should not have been set.")
    }

    /// Set the state of an actuator, or reconfigure a sensor
    ///
    /// A &self is passed in on write because there could be concurrent access from multiple SAUL
    /// users. One option of handling this is to implement Drivable for `Mutex<T>`.
    ///
    /// Note that due to the way SAUL is structured, the drivable can not know the number of
    /// entries which the user intended to set. The Drivable trait always builds the Rust Phydat
    /// (which contains a length) with the maximum available length (some of which may contain
    /// uninitialized data, which is OK as i16 has no uninhabited values), and the writer needs to
    /// return how many of the entries it actually used.
    fn write(self, _data: &Phydat) -> Result<u8, Error> {
        // See also comment in read()
        unimplemented!("Sensor writing not implemented; HAS_READ should not have been set.")
    }
}

/// A typed saul_driver_t, created from a Drivable's build_driver() static method, and used as
/// statically lived references registrations.
///
/// `DEV` indicates the type of the item pointed to in the registration's field, which is usually the
/// Drivable itself, but may be specialized by AsRef into a particular drivable, eg. when a device
/// is used by two drivers representing different aspects of the device.
pub struct Driver<DEV, DRIV = &'static DEV>
where
    DEV: Sized + Sync + 'static,
    &'static DEV: Into<DRIV>,
    DRIV: Drivable + 'static,
{
    driver: riot_sys::saul_driver_t,
    _phantom: core::marker::PhantomData<(DEV, DRIV)>,
}

// While the old supported RIOT version has a `saul_notsup` and the new has
// `saul_{read,write}_notsup`, it's easier to just implement our own. After the 2022.07 release
// they can go away again, and users go with riot_sys::saul_[...]_notsup
extern "C" fn saul_read_notsup(_dev: *const libc::c_void, _dat: *mut riot_sys::phydat_t) -> i32 {
    -(riot_sys::ENOTSUP as i32)
}
extern "C" fn saul_write_notsup(_dev: *const libc::c_void, _dat: *const riot_sys::phydat_t) -> i32 {
    -(riot_sys::ENOTSUP as i32)
}

impl<DEV, DRIV> Driver<DEV, DRIV>
where
    DEV: Sized + Sync + 'static,
    &'static DEV: Into<DRIV>,
    DRIV: Drivable + 'static,
{
    pub const fn new() -> Self {
        Driver {
            driver: riot_sys::saul_driver_t {
                read: if DRIV::HAS_READ {
                    Some(Self::read_raw)
                } else {
                    Some(saul_read_notsup)
                },
                write: if DRIV::HAS_WRITE {
                    Some(Self::write_raw)
                } else {
                    Some(saul_write_notsup)
                },
                type_: DRIV::CLASS.to_c(),
            },
            _phantom: core::marker::PhantomData,
        }
    }

    unsafe extern "C" fn read_raw(dev: *const libc::c_void, res: *mut riot_sys::phydat_t) -> i32 {
        let device = &*(dev as *const DEV);
        let device = device.into();
        match device.read() {
            Ok(d) => {
                res.write(d.values);
                d.length.into()
            }
            // The only legal device error -- ENOTSUP would mean there's no handler at all
            Err(_) => -(riot_sys::ECANCELED as i32),
        }
    }

    unsafe extern "C" fn write_raw(
        dev: *const libc::c_void,
        data: *const riot_sys::phydat_t,
    ) -> i32 {
        let device = &*(dev as *const DEV);
        let device = device.into();
        let data = *data;
        // PHYDAT_DIM: See write documentation
        let data = Phydat {
            values: data,
            length: riot_sys::PHYDAT_DIM as _,
        };
        match device.write(&data) {
            Ok(n) => n as _,
            // The only legal device error -- ENOTSUP would mean there's no handler at all
            Err(_) => -(riot_sys::ECANCELED as i32),
        }
    }
}

// unsafe: All the content we have in the inner struct is Send (being just plain functions), so is
// the whole. (We don't store the DEV pointer here, so we don't need DEV Sync, but Driver needs it)
unsafe impl<DEV, DRIV> Send for Driver<DEV, DRIV>
where
    DEV: Sized + Sync + 'static,
    &'static DEV: Into<DRIV>,
    DRIV: Drivable + 'static,
{
}

pub struct Registration<DEV, DRIV = &'static DEV>
where
    DEV: Sized + Sync + 'static,
    &'static DEV: Into<DRIV>,
    DRIV: Drivable + 'static,
{
    reg: riot_sys::saul_reg_t,
    _phantom: core::marker::PhantomData<(&'static DEV, DRIV)>,
}

// unsafe: The registration is as Send as the pointer to the DEV it contains (and for DEV Sync is
// required)
unsafe impl<DEV, DRIV> Send for Registration<DEV, DRIV>
where
    DEV: Sized + Sync + 'static,
    &'static DEV: Into<DRIV>,
    DRIV: Drivable + 'static,
{
}

impl<DEV, DRIV> Registration<DEV, DRIV>
where
    DEV: Sized + Sync + 'static,
    &'static DEV: Into<DRIV>,
    DRIV: Drivable + 'static,
{
    // Unlike in the old implementation, no attempt is made to build it short-lived and then
    // upgrade -- not for lifetime reasons, but because for `register_static` all is already
    // static anyway, and `build_with` can just as well be called with the components, claim their
    // lifetime first and then go through here.
    pub fn new(
        driver: &'static Driver<DEV, DRIV>,
        device: &'static DEV,
        name: Option<&'static CStr>,
    ) -> Self {
        Registration {
            reg: riot_sys::saul_reg_t {
                next: core::ptr::null_mut(),
                dev: device as *const _ as *mut _,
                name: name.map(|n| n.as_ptr() as _).unwrap_or(core::ptr::null()),
                driver: &driver.driver as *const _,
            },
            _phantom: core::marker::PhantomData,
        }
    }

    /// Hook the registration in with the global SAUL list
    ///
    /// If you can not obtain a &'static, you may consider [`register_and_then()`].
    pub fn register_static(&'static mut self) {
        (unsafe { riot_sys::saul_reg_add(&mut self.reg) })
            .negative_to_error()
            .expect("Constructed registries are always valid");
    }
}

/// Hook the registration in with the global SAUL list
///
/// Compared to [`Registration::register_static()`], this is convenient for threads that run
/// forever and which just need a reference to move into an infinitely executing closure to get the
/// same guarantees as from a static reference.
// It would be nice to have a helper function that proves the infinite lifetime independently
// of the registration, but none such is known.
//
// If unwinding is ever added in RIOT, this will need a guard similar to the one in the
// `replace_with` crate.
pub fn register_and_then<DEV, DRIV>(
    driver: &Driver<DEV, DRIV>,
    device: &DEV,
    name: Option<&CStr>,
    f: impl FnOnce() -> crate::never::Never,
) -> !
where
    DEV: Sized + Sync + 'static,
    &'static DEV: Into<DRIV>,
    DRIV: Drivable + 'static,
{
    // Reborrow for 'static lifetime.
    //
    // This is safe because we never terminate, and thus keep these references forever.
    // (It could be done prettier, but see above on preferring to have a proper library for
    // this anyway).
    let (driver, device, name) = unsafe { core::mem::transmute((driver, device, name)) };

    let mut registration = Registration::<DEV, DRIV>::new(driver, device, name);

    (unsafe { riot_sys::saul_reg_add(&mut registration.reg) })
        .negative_to_error()
        .expect("Constructed registries are always valid");
    f()
}
