//! Type wrappers for BLE UUIDs
//!
//! The [Uuid16], [Uuid32] and [Uuid128] types represent UUIDs of their given lengths, have
//! conversion functions to get suitable pointers for the initialization of larger structs and for
//! accessing the address in Bluetooth's serialization, and provide a convenient [core::str::FromStr]
//! implementation (ie. can be `.parse()`d) for conversion from string-formatted UUIDs.

#[derive(Debug)]
pub struct UuidParseError;

macro_rules! implementation {
    ($name:ident, $basetype:ident, $typename:ident, $bytelength:literal) => {
        /// A wrapper around ble_uuid{16,32,128}_t.
        ///
        /// The bit length is stored in this type (as opposed to only being known through the type), as
        /// that allows getting a full ble_uuid_any_t as a pointer out of a reference to a UuidX. The
        /// stored bit length is an invariant (as it's needed for the …_any_t to be usable).
        ///
        /// (Internally, this is emulated and a ble_uuid128_t-like structure is used to ease and because
        /// the author sees no reason to treat shorter numerics as scalars rather than arrays.)
        #[repr(C)]
        pub struct $name {
            u: riot_sys::ble_uuid_t,
            value: [u8; $bytelength],
        }

        // Note that this could almost be a const impl if not for feature(const_trait_impl) and the `?~
        // operator; nevertheless it is hoped that this will be evaluated at compile time for string
        // literals
        impl core::str::FromStr for $name {
            type Err = UuidParseError;

            fn from_str(input: &str) -> Result<Self, UuidParseError> {
                let u = riot_sys::ble_uuid_t {
                    type_: riot_sys::$typename,
                };
                let mut value: [u8; $bytelength] = Default::default();
                let mut write = &mut value[..];
                // There's probably a very elegant one-liner for this
                let mut chunk = input.as_bytes();
                while chunk.len() > 1 {
                    if chunk[0] == b'-' {
                        chunk = &chunk[1..];
                        continue;
                    }
                    if write.len() < 1 || chunk.len() < 2 {
                        // Output overflow or input underrun
                        return Err(UuidParseError);
                    }
                    // Writing right to left to follow the conventions
                    let (newwrite, out) = write.split_at_mut(write.len() - 1);
                    let (input, newchunk) = chunk.split_at(2);
                    write = newwrite;
                    chunk = newchunk;
                    hex::decode_to_slice(input, out).map_err(|_| UuidParseError)?;
                }
                Ok($name { u, value })
            }
        }

        impl $name {
            pub const fn value(&self) -> &[u8; $bytelength] {
                &self.value
            }
        }

        /// Useful for building values for things like `ble_gatt_svc_def` that take a pointer to a
        /// ble_uuid_t rather than to a ble_uuid_any_t, probably to simplify casting in C.
        impl<'a> Into<*const riot_sys::ble_uuid_t> for &'a $name {
            fn into(self) -> *const riot_sys::ble_uuid_t {
                &self.u as _
            }
        }
    };
}

implementation!(Uuid16, ble_uuid16_t, BLE_UUID_TYPE_16, 2);
implementation!(Uuid32, ble_uuid32_t, BLE_UUID_TYPE_32, 4);
implementation!(Uuid128, ble_uuid128_t, BLE_UUID_TYPE_128, 16);
