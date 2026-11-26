//! Registration and use of [SAUL], the Sensor Actuator Uber Layer
//!
//! This modules falls largely into two parts:
//!
//! * For creating and registering SAUL devices, see the [registration] submodule.
//!
//! * [`RegistryEntry`] with its various constructors finds sensors or actuators in SAUL,
//!   and allows interacting with them.
//!
//! In mapping SAUL semantics to Rust, some parts are not aligned in full:
//!
//! * The [`Phydat`] type used here *always* has a length -- as opposed to `phydat_t` which contains
//!   up to PHYDAT_DIM values, and transports the number of used items on the side -- but not
//!   always.
//!
//!   This affects sensor data writing, and is documented with the respective calls.
//!
//! [SAUL]: https://doc.riot-os.org/group__drivers__saul.html

use crate::error;
use crate::helpers::PointerToCStr;
use error::{NegativeErrorExt, NumericError, EINVAL};

pub mod registration;

/// A discovered SAUL registry entry
pub struct RegistryEntry(*mut riot_sys::saul_reg);

/// Public result type of [`RegistryEntry::all()`].
///
/// Do not rely on the precise type here -- this is meant only to be used as a means to explicitly
/// give associated types derived from this. The only reliable properties of this are that it is
/// `impl Iterator<Item = RegistryEntry>`, and that it is the return type of `all()`.
pub type AllRegistryEntries = core::iter::MapWhile<
    core::iter::Map<core::ops::RangeFrom<usize>, fn(usize) -> Option<RegistryEntry>>,
    fn(Option<RegistryEntry>) -> Option<RegistryEntry>,
>;

impl RegistryEntry {
    /// Find a registry entry by its index
    ///
    /// Wrapper around `saul_reg_find_nth`.
    pub fn nth(pos: usize) -> Option<Self> {
        // unsafe: all positions are valid, and if it's not null, it's a static pointer as SAUL
        // registrations can't really be removed.
        (unsafe { riot_sys::saul_reg_find_nth(pos as _).as_mut() }).map(|r| RegistryEntry(r))
    }

    /// All registered entries.
    ///
    /// Do not expect more from its return type than being `impl Iterator<Item = Self>`; this will
    /// change back to that once `type_alias_impl_trait` is stable.
    pub fn all() -> AllRegistryEntries {
        // Could alternatively be implemented by hopping through the list's next pointer -- more
        // efficient, but relies on internals that are not part of the API
        //
        // (The shell command in sc_saul_reg also jumps through next, but that's in-tree.)
        (0..).map(Self::nth as _).map_while(|p| p)
    }

    pub fn type_(&self) -> Option<Class> {
        // unsafe: Registrations are stable
        let type_ = unsafe { (*(*self.0).driver).type_ };
        Class::from_c(type_)
    }

    pub fn name(&self) -> Option<&'static str> {
        // unsafe: Registrations are stable, and point to null-terminated strings or are NULL.
        unsafe { Some((*self.0).name.to_lifetimed_cstr()?.to_str().ok()?) }
    }

    /// Read a value from the SAUL device
    pub fn read(&self) -> Result<Phydat, NumericError> {
        // Could work with MaybeUninit here, but probably not worth it.
        let mut result: Phydat = Default::default();
        let length = (unsafe { riot_sys::saul_reg_read(self.0 as *mut _, &mut result.values) })
            .negative_to_error()?;
        result.length = length as _;
        Ok(result)
    }

    /// Write a value to the SAUL device
    ///
    /// Note that the saul_reg_write call does not really pass on the initialized length of the
    /// values to the device, but the device returns the used length. If the lengths do not match,
    /// the returned length is expressed as an error.
    pub fn write(&self, value: Phydat) -> Result<(), NumericError> {
        // Value copied as we can't really be sure that no SAUL device will ever write here
        let length =
            unsafe { riot_sys::saul_reg_write(self.0, &value.values as *const _ as *mut _) }
                .negative_to_error()?;
        if length != value.length.into() {
            // It's not pretty to synthesize an error here, but neither would be expressing the
            // partial write in the context of lengthful phydat items.
            Err(EINVAL)
        } else {
            Ok(())
        }
    }
}

/// A wrapper around phydat_t that keeps the values and the number of valid values in one place.
#[derive(Default, Copy, Clone)]
pub struct Phydat {
    values: riot_sys::phydat_t,
    // Always <= PHYDAT_DIM
    length: u8,
}

impl Phydat {
    /// Create a new phydat data value.
    ///
    /// # Panics
    ///
    /// This function panics if data length exceeds PHYDAT_DIM (3).
    pub fn new(data: &[i16], unit: Option<Unit>, scale: i8) -> Self {
        // Working with MaybeUninit might be legal here but is probably not worth it.
        let mut val = [0; riot_sys::PHYDAT_DIM as _];
        val[..data.len()].copy_from_slice(data);
        Phydat {
            values: riot_sys::phydat_t {
                val,
                unit: Unit::to_c(unit),
                scale: scale,
            },
            length: data.len() as _,
        }
    }

    /// Create a new pydat data value, from data adjusted using the scale to fit in the i16 phydat
    /// uses internally
    ///
    /// See [phydat_fit](https://doc.riot-os.org/group__sys__phydat.html#gafafe8717882db85c250f203b020f8863)
    /// for trade-offs.
    ///
    /// # Panics
    ///
    /// like `new()`
    #[doc(alias = "phydat_fit")]
    pub fn fit(data: &[i32], unit: Option<Unit>, scale: i8) -> Self {
        let mut phydat = Phydat {
            values: riot_sys::phydat_t {
                val: Default::default(),
                unit: Unit::to_c(unit),
                scale: scale,
            },
            length: data.len() as _,
        };
        unsafe { riot_sys::phydat_fit(&mut phydat.values, data.as_ptr(), data.len() as _) };
        phydat
    }

    pub fn value(&self) -> &[i16] {
        &self.values.val[..self.length as _]
    }

    pub fn unit(&self) -> Option<Unit> {
        Unit::from_c(self.values.unit)
    }

    pub fn scale(&self) -> i8 {
        self.values.scale
    }
}

impl core::fmt::Debug for Phydat {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> Result<(), core::fmt::Error> {
        write!(
            f,
            "Phydat {{ {:?} x 10^{} {:?} }}",
            self.value(),
            self.scale(),
            self.unit()
        )
    }
}

impl core::fmt::Display for Phydat {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> Result<(), core::fmt::Error> {
        if self.length == 1 {
            write!(f, "{}", self.values.val[0])?;
        } else {
            write!(f, "{:?}", &self.values.val[..self.length as _])?;
        }
        if self.values.scale != 0 {
            write!(f, "×10^{}", self.values.scale)?;
        }
        match Unit::from_c(self.values.unit).map(|u| (u, u.name_owned::<16>())) {
            Some((_, Some(s))) => write!(f, " {}", s)?,
            Some((u, _)) => write!(f, " in units of {:?}", u)?,
            None => (),
        }
        Ok(())
    }
}

/// Device class
///
/// Both for the class in general and for its details, Option is used to represent undefined /
/// unknown values, which are used as a wildcard in queries and as an indicator of unknown /
/// unsupported types in introspection.
#[derive(Copy, Clone, Debug)]
pub enum Class {
    Actuator(Option<ActuatorClass>),
    Sensor(Option<SensorClass>),
}

impl Class {
    fn from_c(input: u8) -> Option<Class> {
        use ActuatorClass::*;
        use Class::*;
        use SensorClass::*;

        match input as _ {
            riot_sys::SAUL_ACT_ANY => Some(Actuator(None)),
            riot_sys::SAUL_ACT_LED_RGB => Some(Actuator(Some(LedRgb))),
            riot_sys::SAUL_ACT_SERVO => Some(Actuator(Some(Servo))),
            riot_sys::SAUL_ACT_MOTOR => Some(Actuator(Some(Motor))),
            riot_sys::SAUL_ACT_SWITCH => Some(Actuator(Some(Switch))),
            riot_sys::SAUL_ACT_DIMMER => Some(Actuator(Some(Dimmer))),
            riot_sys::SAUL_SENSE_ANY => Some(Sensor(None)),
            riot_sys::SAUL_SENSE_BTN => Some(Sensor(Some(Btn))),
            riot_sys::SAUL_SENSE_TEMP => Some(Sensor(Some(Temp))),
            riot_sys::SAUL_SENSE_HUM => Some(Sensor(Some(Hum))),
            riot_sys::SAUL_SENSE_LIGHT => Some(Sensor(Some(Light))),
            riot_sys::SAUL_SENSE_ACCEL => Some(Sensor(Some(Accel))),
            riot_sys::SAUL_SENSE_MAG => Some(Sensor(Some(Mag))),
            riot_sys::SAUL_SENSE_GYRO => Some(Sensor(Some(Gyro))),
            riot_sys::SAUL_SENSE_COLOR => Some(Sensor(Some(Color))),
            riot_sys::SAUL_SENSE_PRESS => Some(Sensor(Some(Press))),
            riot_sys::SAUL_SENSE_ANALOG => Some(Sensor(Some(Analog))),
            riot_sys::SAUL_SENSE_UV => Some(Sensor(Some(Uv))),
            riot_sys::SAUL_SENSE_OBJTEMP => Some(Sensor(Some(Objtemp))),
            riot_sys::SAUL_SENSE_COUNT => Some(Sensor(Some(Count))),
            riot_sys::SAUL_SENSE_DISTANCE => Some(Sensor(Some(Distance))),
            riot_sys::SAUL_SENSE_CO2 => Some(Sensor(Some(Co2))),
            riot_sys::SAUL_SENSE_TVOC => Some(Sensor(Some(Tvoc))),
            riot_sys::SAUL_SENSE_GAS => Some(Sensor(Some(Gas))),
            riot_sys::SAUL_SENSE_OCCUP => Some(Sensor(Some(Occup))),
            riot_sys::SAUL_SENSE_PROXIMITY => Some(Sensor(Some(Proximity))),
            riot_sys::SAUL_SENSE_RSSI => Some(Sensor(Some(Rssi))),
            riot_sys::SAUL_SENSE_CHARGE => Some(Sensor(Some(Charge))),
            riot_sys::SAUL_SENSE_CURRENT => Some(Sensor(Some(Current))),
            riot_sys::SAUL_SENSE_PM => Some(Sensor(Some(Pm))),
            riot_sys::SAUL_SENSE_CAPACITANCE => Some(Sensor(Some(Capacitance))),
            riot_sys::SAUL_SENSE_VOLTAGE => Some(Sensor(Some(Voltage))),
            riot_sys::SAUL_SENSE_PH => Some(Sensor(Some(Ph))),
            riot_sys::SAUL_SENSE_POWER => Some(Sensor(Some(Power))),
            riot_sys::SAUL_SENSE_SIZE => Some(Sensor(Some(Size))),
            x if x & riot_sys::SAUL_CAT_MASK == riot_sys::SAUL_CAT_ACT => Some(Actuator(None)),
            x if x & riot_sys::SAUL_CAT_MASK == riot_sys::SAUL_CAT_SENSE => Some(Sensor(None)),
            _ => None,
        }
    }

    const fn to_c(self) -> u8 {
        use ActuatorClass::*;
        use Class::*;
        use SensorClass::*;

        (match self {
            Actuator(None) => riot_sys::SAUL_ACT_ANY,
            Actuator(Some(LedRgb)) => riot_sys::SAUL_ACT_LED_RGB,
            Actuator(Some(Servo)) => riot_sys::SAUL_ACT_SERVO,
            Actuator(Some(Motor)) => riot_sys::SAUL_ACT_MOTOR,
            Actuator(Some(Switch)) => riot_sys::SAUL_ACT_SWITCH,
            Actuator(Some(Dimmer)) => riot_sys::SAUL_ACT_DIMMER,
            Sensor(None) => riot_sys::SAUL_SENSE_ANY,
            Sensor(Some(Btn)) => riot_sys::SAUL_SENSE_BTN,
            Sensor(Some(Temp)) => riot_sys::SAUL_SENSE_TEMP,
            Sensor(Some(Hum)) => riot_sys::SAUL_SENSE_HUM,
            Sensor(Some(Light)) => riot_sys::SAUL_SENSE_LIGHT,
            Sensor(Some(Accel)) => riot_sys::SAUL_SENSE_ACCEL,
            Sensor(Some(Mag)) => riot_sys::SAUL_SENSE_MAG,
            Sensor(Some(Gyro)) => riot_sys::SAUL_SENSE_GYRO,
            Sensor(Some(Color)) => riot_sys::SAUL_SENSE_COLOR,
            Sensor(Some(Press)) => riot_sys::SAUL_SENSE_PRESS,
            Sensor(Some(Analog)) => riot_sys::SAUL_SENSE_ANALOG,
            Sensor(Some(Uv)) => riot_sys::SAUL_SENSE_UV,
            Sensor(Some(Objtemp)) => riot_sys::SAUL_SENSE_OBJTEMP,
            Sensor(Some(Count)) => riot_sys::SAUL_SENSE_COUNT,
            Sensor(Some(Distance)) => riot_sys::SAUL_SENSE_DISTANCE,
            Sensor(Some(Co2)) => riot_sys::SAUL_SENSE_CO2,
            Sensor(Some(Tvoc)) => riot_sys::SAUL_SENSE_TVOC,
            Sensor(Some(Gas)) => riot_sys::SAUL_SENSE_GAS,
            Sensor(Some(Occup)) => riot_sys::SAUL_SENSE_OCCUP,
            Sensor(Some(Proximity)) => riot_sys::SAUL_SENSE_PROXIMITY,
            Sensor(Some(Rssi)) => riot_sys::SAUL_SENSE_RSSI,
            Sensor(Some(Charge)) => riot_sys::SAUL_SENSE_CHARGE,
            Sensor(Some(Current)) => riot_sys::SAUL_SENSE_CURRENT,
            Sensor(Some(Pm)) => riot_sys::SAUL_SENSE_PM,
            Sensor(Some(Capacitance)) => riot_sys::SAUL_SENSE_CAPACITANCE,
            Sensor(Some(Voltage)) => riot_sys::SAUL_SENSE_VOLTAGE,
            Sensor(Some(Ph)) => riot_sys::SAUL_SENSE_PH,
            Sensor(Some(Power)) => riot_sys::SAUL_SENSE_POWER,
            Sensor(Some(Size)) => riot_sys::SAUL_SENSE_SIZE,
        }) as _
    }

    /// Human-readable name of the class
    pub fn name(self) -> Option<&'static str> {
        unsafe { riot_sys::saul_class_to_str(self.to_c()).to_lifetimed_cstr()? }
            .to_str()
            .ok()
    }
}

#[derive(Copy, Clone, Debug)]
#[non_exhaustive]
/// Classes of actuators; typically used as details on a [Class]
pub enum ActuatorClass {
    LedRgb,
    Servo,
    Motor,
    Switch,
    Dimmer,
}

#[derive(Copy, Clone, Debug)]
#[non_exhaustive]
/// Classes of sensors; typically used as details on a [Class]
pub enum SensorClass {
    Btn,
    Temp,
    Hum,
    Light,
    Accel,
    Mag,
    Gyro,
    Color,
    Press,
    Analog,
    Uv,
    Objtemp,
    Count,
    Distance,
    Co2,
    Tvoc,
    Gas,
    Occup,
    Proximity,
    Rssi,
    Charge,
    Current,
    Pm,
    Capacitance,
    Voltage,
    Ph,
    Power,
    Size,
}

#[derive(Copy, Clone, Debug)]
/// Unit of measurement required to interpret numeric values in a [Phydat] exchanged with a SAUL
/// device
#[non_exhaustive]
pub enum Unit {
    /// Note that this means "data has no physical unit", and is distinct from "No unit given",
    /// which is `Option::<Unit>::None` as opposed to `Some(Unit::None)`.
    None,
    TempC,
    TempF,
    TempK,
    Lux,
    M,
    M2,
    M3,
    GForce,
    Dps,
    Gram,
    A,
    V,
    W,
    Gauss,
    T,
    Dbm,
    Coulomb,
    F,
    Ohm,
    Ph,
    Bar,
    Pa,
    Cd,
    Bool,
    Cts,
    Percent,
    Permill,
    Ppm,
    Ppb,
    Time,
    Date,
    Gpm3,
    Cpm3,
}

impl Unit {
    fn from_c(input: u8) -> Option<Self> {
        match input as _ {
            riot_sys::UNIT_NONE => Some(Unit::None),
            riot_sys::UNIT_TEMP_C => Some(Unit::TempC),
            riot_sys::UNIT_TEMP_F => Some(Unit::TempF),
            riot_sys::UNIT_TEMP_K => Some(Unit::TempK),
            riot_sys::UNIT_LUX => Some(Unit::Lux),
            riot_sys::UNIT_M => Some(Unit::M),
            riot_sys::UNIT_M2 => Some(Unit::M2),
            riot_sys::UNIT_M3 => Some(Unit::M3),
            riot_sys::UNIT_G_FORCE => Some(Unit::GForce),
            riot_sys::UNIT_DPS => Some(Unit::Dps),
            riot_sys::UNIT_GRAM => Some(Unit::Gram),
            riot_sys::UNIT_A => Some(Unit::A),
            riot_sys::UNIT_V => Some(Unit::V),
            riot_sys::UNIT_W => Some(Unit::W),
            riot_sys::UNIT_GAUSS => Some(Unit::Gauss),
            riot_sys::UNIT_T => Some(Unit::T),
            riot_sys::UNIT_DBM => Some(Unit::Dbm),
            riot_sys::UNIT_COULOMB => Some(Unit::Coulomb),
            riot_sys::UNIT_F => Some(Unit::F),
            riot_sys::UNIT_OHM => Some(Unit::Ohm),
            riot_sys::UNIT_PH => Some(Unit::Ph),
            riot_sys::UNIT_BAR => Some(Unit::Bar),
            riot_sys::UNIT_PA => Some(Unit::Pa),
            riot_sys::UNIT_CD => Some(Unit::Cd),
            riot_sys::UNIT_BOOL => Some(Unit::Bool),
            riot_sys::UNIT_CTS => Some(Unit::Cts),
            riot_sys::UNIT_PERCENT => Some(Unit::Percent),
            riot_sys::UNIT_PERMILL => Some(Unit::Permill),
            riot_sys::UNIT_PPM => Some(Unit::Ppm),
            riot_sys::UNIT_PPB => Some(Unit::Ppb),
            riot_sys::UNIT_TIME => Some(Unit::Time),
            riot_sys::UNIT_DATE => Some(Unit::Date),
            riot_sys::UNIT_GPM3 => Some(Unit::Gpm3),
            riot_sys::UNIT_CPM3 => Some(Unit::Cpm3),
            _ => None,
        }
    }

    fn to_c(input: Option<Self>) -> u8 {
        (match input {
            Some(Unit::None) => riot_sys::UNIT_NONE,
            Some(Unit::TempC) => riot_sys::UNIT_TEMP_C,
            Some(Unit::TempF) => riot_sys::UNIT_TEMP_F,
            Some(Unit::TempK) => riot_sys::UNIT_TEMP_K,
            Some(Unit::Lux) => riot_sys::UNIT_LUX,
            Some(Unit::M) => riot_sys::UNIT_M,
            Some(Unit::M2) => riot_sys::UNIT_M2,
            Some(Unit::M3) => riot_sys::UNIT_M3,
            Some(Unit::GForce) => riot_sys::UNIT_G_FORCE,
            Some(Unit::Dps) => riot_sys::UNIT_DPS,
            Some(Unit::Gram) => riot_sys::UNIT_GRAM,
            Some(Unit::A) => riot_sys::UNIT_A,
            Some(Unit::V) => riot_sys::UNIT_V,
            Some(Unit::W) => riot_sys::UNIT_W,
            Some(Unit::Gauss) => riot_sys::UNIT_GAUSS,
            Some(Unit::T) => riot_sys::UNIT_T,
            Some(Unit::Dbm) => riot_sys::UNIT_DBM,
            Some(Unit::Coulomb) => riot_sys::UNIT_COULOMB,
            Some(Unit::F) => riot_sys::UNIT_F,
            Some(Unit::Ohm) => riot_sys::UNIT_OHM,
            Some(Unit::Ph) => riot_sys::UNIT_PH,
            Some(Unit::Bar) => riot_sys::UNIT_BAR,
            Some(Unit::Pa) => riot_sys::UNIT_PA,
            Some(Unit::Cd) => riot_sys::UNIT_CD,
            Some(Unit::Bool) => riot_sys::UNIT_BOOL,
            Some(Unit::Cts) => riot_sys::UNIT_CTS,
            Some(Unit::Percent) => riot_sys::UNIT_PERCENT,
            Some(Unit::Permill) => riot_sys::UNIT_PERMILL,
            Some(Unit::Ppm) => riot_sys::UNIT_PPM,
            Some(Unit::Ppb) => riot_sys::UNIT_PPB,
            Some(Unit::Time) => riot_sys::UNIT_TIME,
            Some(Unit::Date) => riot_sys::UNIT_DATE,
            Some(Unit::Gpm3) => riot_sys::UNIT_GPM3,
            Some(Unit::Cpm3) => riot_sys::UNIT_CPM3,
            None => riot_sys::UNIT_UNDEF,
        }) as _
    }

    /// String representation of a given unit (e.g. `V`, `m`, `none` or `time`)
    #[doc(alias = "phydat_unit_write")]
    pub fn name_owned<const S: usize>(self) -> Option<heapless::String<S>> {
        let mut result = heapless::String::new();
        // SAFETY: The C API will only write UTF-8 bytes.
        let bytes = unsafe { result.as_mut_vec() };
        // SAFETY: C API promises to write only up to S bytes, will not write NULL byte.
        let len = unsafe {
            riot_sys::phydat_unit_write(
                // Casting away signedness
                bytes.as_mut_ptr() as *mut _,
                // size_t is not always usize
                S as _,
                Self::to_c(Some(self)),
            )
        }
        .negative_to_error()
        .ok()?;
        // SAFETY: We just wrote those bytes.
        unsafe { bytes.set_len(len as _) };
        Some(result)
    }
}
