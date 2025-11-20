#![no_std]

use lsm303agr::{interface, mode, AccelMode, AccelOutputDataRate::Hz50, Lsm303agr};

use riot_wrappers::{cstr::cstr, i2c, mutex::Mutex, println, saul};
use saul::{registration, Phydat};

// FIXME: Is this the way we want to go? It's mimicking the C way, but we could just as well take
// the board config from some YAML.
include!(concat!(env!("BOARDDIR"), "/include/lsm303agr-config.rs"));

const NDEVICES: usize = I2C_DEVICES.len();

static DRIVER: registration::Driver<SaulLSM> = registration::Driver::new();
static DRIVER_MAG: registration::Driver<SaulLSM, MagAspect> = registration::Driver::new();

// These two being in mutexes is somewhat unnecessary (the mutexes are locked at startup and then
// never unlocked). The alternative is to unsafely access them (asserting that auto_init_lsm303agr
// / init will only ever be called once), or hiding that assertion at some preprocessor level (like
// cortex-m-rt's main does).
//
// Doing it at runtime comes at the cost of two global mutexes in memory, and some more startup
// calls.
//
// Using an Option (with .insert) rather than MaybeUninit (with .write) is another step that
// sacrifices minimal resources (could be none at all, didn't check) for readability.

// This can't go into ROM because it has a .next pointer that is altered at runtime when some other
// device is registered. (In an alternative implementation where all SAUL registries are managed by
// XFA, this would be possible; finding the point in time when they are ready to be used would be
// tricky, though.).
static REG: Mutex<[Option<registration::Registration<SaulLSM>>; NDEVICES]> =
    Mutex::new([None; NDEVICES]);
static REG_MAG: Mutex<[Option<registration::Registration<SaulLSM, MagAspect>>; NDEVICES]> =
    Mutex::new([None; NDEVICES]);
// This can't go into ROM because it contains an inner Mutex (and possibly state data from the
// Lsm303agr instance, didn't bother to check)
static LSM: Mutex<[Option<SaulLSM>; NDEVICES]> = Mutex::new([None; NDEVICES]);

#[no_mangle]
pub extern "C" fn auto_init_lsm303agr() {
    if let Err(e) = init() {
        println!("LSM303AGR init error: {}", e);
    }
}

/// Initialize the configured LSM303AGR device, returning an error string for debug if anything
/// goes wrong
fn init() -> Result<(), &'static str> {
    let lsm = LSM.try_leak().expect("LSM303AGR init is only called once");

    let reg = REG.try_leak().expect("LSM303AGR init is only called once");
    let reg_mag = REG_MAG
        .try_leak()
        .expect("LSM303AGR init is only called once");

    for (&i2cdev, (lsm, (reg, reg_mag))) in I2C_DEVICES
        .iter()
        .zip(lsm.iter_mut().zip(reg.iter_mut().zip(reg_mag.iter_mut())))
    {
        let mut device = Lsm303agr::new_with_i2c(i2c::I2CDevice::new(i2cdev));

        let mut init_clock = riot_wrappers::ztimer::Clock::msec();

        device.init().map_err(|_| "Device initialization failed")?;
        device
            .set_accel_mode_and_odr(&mut init_clock, AccelMode::Normal, Hz50)
            .map_err(|_| "Device configuration failed")?;

        let lsm = lsm.insert(SaulLSM {
            device: Mutex::new(device),
        });

        let reg = reg.insert(registration::Registration::new(
            &DRIVER,
            lsm,
            Some(cstr!("LSM303AGR accelerometer")),
        ));
        let reg_mag = reg_mag.insert(registration::Registration::new(
            &DRIVER_MAG,
            lsm,
            Some(cstr!("LSM303AGR magnetometer")),
        ));

        reg.register_static();
        reg_mag.register_static();
    }

    Ok(())
}

struct SaulLSM {
    device: Mutex<Lsm303agr<interface::I2cInterface<i2c::I2CDevice>, mode::MagOneShot>>,
}

impl registration::Drivable for &SaulLSM {
    const CLASS: saul::Class = saul::Class::Sensor(Some(saul::SensorClass::Accel));

    const HAS_READ: bool = true;

    fn read(self) -> Result<Phydat, registration::Error> {
        // SAUL doesn't guarantee exclusive access; different threads may read simultaneously.
        let mut device = self.device.try_lock().ok_or(registration::Error)?;

        let data = device.acceleration().map_err(|_| registration::Error)?;
        // Data is in the +-2g range by default, which doesn't overflow even the i16 SAUL uses
        Ok(Phydat::new(
            &[data.x_mg() as _, data.y_mg() as _, data.z_mg() as _],
            Some(saul::Unit::GForce),
            -3,
        ))
    }
}

struct MagAspect(&'static SaulLSM);

impl From<&'static SaulLSM> for MagAspect {
    fn from(input: &'static SaulLSM) -> Self {
        Self(input)
    }
}

impl registration::Drivable for MagAspect {
    const CLASS: saul::Class = saul::Class::Sensor(Some(saul::SensorClass::Mag));

    const HAS_READ: bool = true;

    fn read(self) -> Result<Phydat, registration::Error> {
        // SAUL doesn't guarantee exclusive access; different threads may read simultaneously.
        let mut device = self.0.device.try_lock().ok_or(registration::Error)?;

        let data = device.magnetic_field().map_err(|_| registration::Error)?;
        return Ok(Phydat::fit(
            &[data.x_nt(), data.y_nt(), data.z_nt()],
            Some(saul::Unit::T),
            -9,
        ));
    }
}
