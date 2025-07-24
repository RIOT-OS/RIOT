// SPDX-FileCopyrightText: 2020 Christian Amsüss
// SPDX-License-Identifier: LGPL-2.1-only
#![no_std]

use riot_wrappers::{gcoap, gnrc, ztimer};
use riot_wrappers::{println, riot_main};
use core::ffi::c_void;
use riot_wrappers::riot_sys;

use coap_handler_implementations::{HandlerBuilder, ReportingHandlerBuilder};
// Fixed imports - use public API from coapcore
use coapcore::{OscoreEdhocHandler, seccfg, scope, ace, GeneralClaims, time::TimeConstraint};
use lakers_crypto_rustcrypto::Crypto;
use rand_core::{RngCore, CryptoRng, impls, Error};
use coapcore::time::TimeUnknown;
use lakers::Credential;
use ::coap_message::{MutableWritableMessage, Code};
use coapcore::CredentialErrorKind as CredentialErrorDetail;
use core::str::FromStr;
use log::{info, debug};

// Simple RIOT-compatible logger
struct RiotLogger;

impl log::Log for RiotLogger {
    fn enabled(&self, _metadata: &log::Metadata) -> bool {
        true
    }

    fn log(&self, record: &log::Record) {
        if self.enabled(record.metadata()) {
            println!("[{}] {}", record.level(), record.args());
        }
    }

    fn flush(&self) {}
}

static LOGGER: RiotLogger = RiotLogger;
extern crate rust_riotmodules;

use heapless::{String, Vec};
use p256::ecdsa::{VerifyingKey, signature::Verifier, SigningKey};

use coap_handler::Handler;

// Import OptionsExt for take_uri_path
use coap_message_utils::OptionsExt;
use coap_message::ReadableMessage;

struct LoggingWrapper<H> {
    inner: H,
}

impl<H> LoggingWrapper<H> {
    pub fn new(inner: H) -> Self {
        Self { inner }
    }
}

impl<H: Handler> Handler for LoggingWrapper<H> {
    type RequestData = H::RequestData;
    type ExtractRequestError = H::ExtractRequestError;
    type BuildResponseError<M: coap_message::MinimalWritableMessage> = H::BuildResponseError<M>;

    fn extract_request_data<M: coap_message::ReadableMessage>(
        &mut self,
        request: &M,
    ) -> Result<Self::RequestData, Self::ExtractRequestError> {
        // Log all incoming requests
        let mut uri_path = heapless::String::<64>::new();
        
        // Use take_uri_path instead of take_into for URI path processing
        request.options()
            .take_uri_path(|segment| {
                if !uri_path.is_empty() {
                    let _ = uri_path.push('/');
                }
                let _ = uri_path.push_str(segment);
            })
            .for_each(drop); // Consume the iterator without collecting
        
        // Use format! for debugging - this should work without Debug bound
        info!("CoAP: {} /{}", 
              core::any::type_name::<M::Code>(), // Alternative to Debug formatting
              uri_path);
        
        // Check if this might be an EDHOC request
        if uri_path.contains("edhoc") || uri_path.contains(".well-known") {
            info!("=== EDHOC REQUEST ===");
            info!("URI: /{}", uri_path);
            info!("Method: {}", core::any::type_name::<M::Code>());
            let payload = request.payload();
            if !payload.is_empty() {
                info!("Payload: {} bytes", payload.len());
                debug!("Payload hex: {:02x?}", &payload[..payload.len().min(16)]);
            }
        }
        
        self.inner.extract_request_data(request)
    }

    fn estimate_length(&mut self, request: &Self::RequestData) -> usize {
        self.inner.estimate_length(request)
    }

    fn build_response<M: coap_message::MutableWritableMessage>(
        &mut self,
        response: &mut M,
        request: Self::RequestData,
    ) -> Result<(), Self::BuildResponseError<M>> {
        let result = self.inner.build_response(response, request);
        
        // Log response - simplified since we can't easily get the code back
        debug!("Response built");
        
        result
    }
}

// Custom RNG implementation using RIOT's random functions
struct RiotRng;

impl RngCore for RiotRng {
    fn next_u32(&mut self) -> u32 {
        unsafe {
            riot_sys::random_uint32()
        }
    }

    fn next_u64(&mut self) -> u64 {
        impls::next_u64_via_u32(self)
    }

    fn fill_bytes(&mut self, dest: &mut [u8]) {
        unsafe {
            riot_sys::random_bytes(dest.as_mut_ptr() as *mut c_void, dest.len() as u32);
        }
    }

    fn try_fill_bytes(&mut self, dest: &mut [u8]) -> Result<(), Error> {
        self.fill_bytes(dest);
        Ok(())
    }
}

impl CryptoRng for RiotRng {}

// Fixed private key from private_key.h
const FIXED_PRIVATE_KEY: [u8; 32] = [
    0xf6, 0x3e, 0x22, 0xc8, 0xd3, 0x2d, 0xe8, 0x6b,
    0x52, 0x83, 0x72, 0xd3, 0x79, 0x4e, 0x05, 0x25,
    0xa0, 0x62, 0xfb, 0x41, 0x41, 0xed, 0x91, 0x9a,
    0x45, 0x09, 0x65, 0x19, 0xc5, 0x94, 0x8c, 0x3d
];

// Proper EDHOC Security Configuration
pub struct EdhocSecurityConfig {
    /// Authorization Server's public key for token signature verification (ES256)
    as_public_key: Option<([u8; 32], [u8; 32])>, // (x, y) coordinates
    /// Our own EDHOC credential and private key (for signing message 2)
    own_edhoc_credential: Option<(Credential, lakers::BytesP256ElemLen)>,
    /// Known EDHOC client credentials with their authorized scopes
    known_edhoc_clients: Vec<(Credential, scope::UnionScope), 4>,
    /// Unauthenticated scope (if any)
    unauthenticated_scope: Option<scope::UnionScope>,
    /// Request creation hints for unauthorized responses
    request_creation_hints: &'static [u8],
    /// Our audience identifier (for token validation)
    rs_audience: String<16>,
}

impl EdhocSecurityConfig {
    pub fn new() -> Self {
        Self {
            as_public_key: None,
            own_edhoc_credential: None,
            known_edhoc_clients: Vec::new(),
            unauthenticated_scope: None,
            request_creation_hints: b"",
            rs_audience: String::new(),
        }
    }

    pub fn with_as_public_key(mut self, x: [u8; 32], y: [u8; 32]) -> Self {
        self.as_public_key = Some((x, y));
        self
    }

    pub fn with_own_edhoc_credential(mut self, credential: Credential, key: lakers::BytesP256ElemLen) -> Self {
        self.own_edhoc_credential = Some((credential, key));
        self
    }

    pub fn with_known_edhoc_client(mut self, credential: Credential, scope: scope::UnionScope) -> Self {
        let _ = self.known_edhoc_clients.push((credential, scope));
        self
    }

    pub fn allow_unauthenticated(mut self, scope: scope::UnionScope) -> Self {
        self.unauthenticated_scope = Some(scope);
        self
    }

    pub fn with_audience(mut self, audience: &str) -> Self {
        // Fixed: Use heapless::String::from_str instead of String::from
        self.rs_audience = String::from_str(audience).unwrap_or_else(|_| String::new());
        self
    }
}

// Custom claims implementation
#[derive(Debug, Clone)]
pub struct EdhocClaims {
    pub scope: scope::UnionScope,
    pub time_constraint: TimeConstraint,
    pub is_important: bool,
}

impl GeneralClaims for EdhocClaims {
    type Scope = scope::UnionScope;

    fn scope(&self) -> &Self::Scope {
        &self.scope
    }

    fn time_constraint(&self) -> TimeConstraint {
        self.time_constraint
    }

    fn is_important(&self) -> bool {
        self.is_important
    }
}

impl seccfg::ServerSecurityConfig for EdhocSecurityConfig {
    const PARSES_TOKENS: bool = true;
    const HAS_EDHOC: bool = true;

    type GeneralClaims = EdhocClaims;

    fn decrypt_symmetric_token<'buf>(
        &self,
        _headers: &ace::HeaderMap,
        _aad: &[u8],
        _ciphertext_buffer: &'buf mut [u8],
    ) -> Result<(Self::GeneralClaims, ace::CwtClaimsSet<'buf>), coapcore::CredentialError> {
        println!("[DEBUG] decrypt_symmetric_token called");
        Err(coapcore::CredentialError::from(CredentialErrorDetail::KeyNotPresent))
    }

    fn verify_asymmetric_token<'b>(
        &self,
        headers: &ace::HeaderMap,
        signed_data: &[u8],
        signature: &[u8],
        signed_payload: &'b [u8],
    ) -> Result<(Self::GeneralClaims, ace::CwtClaimsSet<'b>), coapcore::CredentialError> {
        println!("[DEBUG] verify_asymmetric_token called");
        
        // Check algorithm - must be ES256 (COSE algorithm -7)
        if headers.alg != Some(-7) {
            println!("[DEBUG] Unsupported algorithm: {:?}", headers.alg);
            return Err(coapcore::CredentialError::from(CredentialErrorDetail::UnsupportedAlgorithm));
        }

        // Get the AS public key for signature verification
        let Some((x, y)) = self.as_public_key else {
            println!("[DEBUG] No AS public key configured");
            return Err(coapcore::CredentialError::from(CredentialErrorDetail::KeyNotPresent));
        };

        // Create verifying key from AS public key
        let as_key = VerifyingKey::from_encoded_point(
            &p256::EncodedPoint::from_affine_coordinates((&x).into(), (&y).into(), false),
        )
        .map_err(|_| {
            println!("[DEBUG] Failed to create verifying key");
            coapcore::CredentialError::from(CredentialErrorDetail::UnsupportedAlgorithm)
        })?;

        // Parse and verify signature
        let signature = p256::ecdsa::Signature::from_slice(signature)
            .map_err(|_| {
                println!("[DEBUG] Invalid signature format");
                coapcore::CredentialError::from(CredentialErrorDetail::UnsupportedAlgorithm)
            })?;

        // Verify the signature
        as_key
            .verify(signed_data, &signature)
            .map_err(|_| {
                println!("[DEBUG] AS signature verification failed");
                coapcore::CredentialError::from(CredentialErrorDetail::VerifyFailed)
            })?;

        // Parse the signed payload
        let claims: ace::CwtClaimsSet = minicbor::decode(signed_payload)
            .map_err(|_| {
                println!("[DEBUG] Failed to decode token payload");
                coapcore::CredentialError::from(CredentialErrorDetail::UnsupportedExtension)
            })?;

        // Verify audience
        if !self.rs_audience.is_empty() && claims.aud != Some(&self.rs_audience) {
            println!("[DEBUG] Token audience mismatch");
            return Err(coapcore::CredentialError::from(CredentialErrorDetail::VerifyFailed));
        }

        // Parse scope from claims
        let scope = scope::AifValue::parse(claims.scope)
            .map_err(|_| {
                println!("[DEBUG] Failed to parse scope");
                coapcore::CredentialError::from(CredentialErrorDetail::UnsupportedExtension)
            })?
            .into();

        let time_constraint = TimeConstraint::from_claims_set(&claims);

        let edhoc_claims = EdhocClaims {
            scope,
            time_constraint,
            is_important: false,
        };

        println!("[DEBUG] Token verification successful");
        Ok((edhoc_claims, claims))
    }

    fn own_edhoc_credential(&self) -> Option<(Credential, lakers::BytesP256ElemLen)> {
        println!("[DEBUG] own_edhoc_credential called");
        if let Some((cred, key)) = &self.own_edhoc_credential {
            println!("[DEBUG] Returning credential: {:?}", cred);
            println!("[DEBUG] Private key length: {}", key.len());
            // Check if credential is valid
            match cred.by_value() {
                Ok(_) => println!("[DEBUG] Credential validation: OK"),
                Err(e) => println!("[DEBUG] Credential validation: ERROR {:?}", e),
            }
        } else {
            println!("[DEBUG] No credential configured!");
        }
        self.own_edhoc_credential.clone()
    }

    fn expand_id_cred_x(
        &self,
        id_cred_x: lakers::IdCred,
    ) -> Option<(Credential, Self::GeneralClaims)> {
        println!("[DEBUG] expand_id_cred_x called");
        println!("[DEBUG] Client ID_CRED_X: {:?}", id_cred_x);

        // Check against known EDHOC clients
        for (credential, scope) in &self.known_edhoc_clients {
            if id_cred_x.reference_only() {
                println!("[DEBUG] Checking by KID reference");
                if let Ok(cred_kid) = credential.by_kid() {
                    if cred_kid == id_cred_x {
                        println!("[DEBUG] Client authenticated by KID");
                        return Some((
                            credential.clone(),
                            EdhocClaims {
                                scope: scope.clone(),
                                time_constraint: TimeConstraint::unbounded(),
                                is_important: true,
                            },
                        ));
                    }
                }
            } else {
                println!("[DEBUG] Checking by credential value");
                if let Ok(cred_value) = credential.by_value() {
                    if cred_value == id_cred_x {
                        println!("[DEBUG] Client authenticated by credential value");
                        return Some((
                            credential.clone(),
                            EdhocClaims {
                                scope: scope.clone(),
                                time_constraint: TimeConstraint::unbounded(),
                                is_important: true,
                            },
                        ));
                    }
                }
            }
        }

        // Check unauthenticated access
        if let Some(unauth_scope) = &self.unauthenticated_scope {
            println!("[DEBUG] Allowing unauthenticated access");
            if let Some(credential_by_value) = id_cred_x.get_ccs().as_ref() {
                return Some((
                    credential_by_value.clone(),
                    EdhocClaims {
                        scope: unauth_scope.clone(),
                        time_constraint: TimeConstraint::unbounded(),
                        is_important: false,
                    },
                ));
            }
        }

        println!("[DEBUG] No valid credential found");
        None
    }

    fn nosec_authorization(&self) -> Option<Self::GeneralClaims> {
        println!("[DEBUG] nosec_authorization called");
        self.unauthenticated_scope.as_ref().map(|scope| EdhocClaims {
            scope: scope.clone(),
            time_constraint: TimeConstraint::unbounded(),
            is_important: false,
        })
    }

    fn render_not_allowed<M: MutableWritableMessage>(
        &self,
        message: &mut M,
    ) -> Result<(), seccfg::NotAllowedRenderingFailed> {
        println!("[DEBUG] render_not_allowed called");
        message.set_code(Code::new(coap_numbers::code::UNAUTHORIZED).map_err(|_| {
            println!("[DEBUG] Cannot set Unauthorized code");
            seccfg::NotAllowedRenderingFailed
        })?);
        
        if !self.request_creation_hints.is_empty() {
            message
                .set_payload(self.request_creation_hints)
                .map_err(|_| {
                    println!("[DEBUG] Cannot set hints payload");
                    seccfg::NotAllowedRenderingFailed
                })?;
        }
        Ok(())
    }
}

// Function to generate public key from the fixed private key
fn compute_public_key_from_private(private_key: &[u8; 32]) -> [u8; 64] {
    use p256::{SecretKey, PublicKey};
    use p256::elliptic_curve::sec1::ToEncodedPoint;
    
    // Create secret key from our fixed private key
    let secret_key = SecretKey::from_slice(private_key)
        .expect("Private key should be valid P-256 scalar");
    
    // Fixed: Use VerifyingKey::from(&secret_key) for public key conversion
    let signing_key = SigningKey::from(&secret_key);
    let verifying_key = VerifyingKey::from(&signing_key);
    let public_key = PublicKey::from(&verifying_key);
    
    // Get the uncompressed point coordinates
    let encoded_point = public_key.to_encoded_point(false);
    
    // Fixed: Handle coordinates properly
    let mut public_key_bytes = [0u8; 64];
    if let p256::elliptic_curve::sec1::Coordinates::Uncompressed { x, y } = encoded_point.coordinates() {
        public_key_bytes[..32].copy_from_slice(x);
        public_key_bytes[32..].copy_from_slice(y);
    }
    
    public_key_bytes
}

// Modified credential generation function using fixed private key
fn generate_credpair_with_fixed_key() -> (Credential, lakers::BytesP256ElemLen) {
    println!("[DEBUG] Generating credential pair with fixed key");
    
    // Use the fixed private key
    let private_key = FIXED_PRIVATE_KEY;
    
    // Compute the corresponding public key
    let public_key_bytes = compute_public_key_from_private(&private_key);
    let (x_coord, y_coord) = public_key_bytes.split_at(32);
    println!("[DEBUG] Public key computed: {:02x?}", &public_key_bytes[..8]);
    // Create proper CBOR Credential Set (CCS) structure
    let mut cred_bytes = heapless::Vec::<u8, 128>::new();
    
    // Build CBOR structure manually
    cred_bytes.extend_from_slice(&[
        0xA1, // Map with 1 pair
        0x08, // Key: 8 (cnf)
        0xA1, // Value: Map with 1 pair  
        0x01, // Key: 1 (COSE_Key)
        0xA4, // Value: Map with 4 pairs
        0x01, 0x02, // kty: 2 (EC2)
        0x20, 0x01, // crv: 1 (P-256)  
        0x21, 0x58, 0x20, // x: bytes(32)
    ]).unwrap();
    cred_bytes.extend_from_slice(x_coord).unwrap();
    cred_bytes.extend_from_slice(&[
        0x22, 0x58, 0x20, // y: bytes(32)
    ]).unwrap();
    cred_bytes.extend_from_slice(y_coord).unwrap();
    

    println!("[DEBUG] Credential bytes length: {}", cred_bytes.len());
    println!("[DEBUG] Credential CBOR: {:02x?}", cred_bytes.as_slice());
    let credential = Credential::parse_ccs(&cred_bytes).expect("Valid credential structure");
    println!("[DEBUG] Credential generated successfully");
    
    // Fixed: Create lakers private key properly
    let mut lakers_private_key = [0u8; 32];
    lakers_private_key.copy_from_slice(&private_key);
    
    (credential, lakers_private_key)
}

// Real AS public key - replace with your actual Authorization Server's public key
fn get_as_public_key() -> ([u8; 32], [u8; 32]) {
    // These should be the actual P-256 public key coordinates of your Authorization Server
    let x = [
        0x0d, 0x8d, 0x5d, 0x2e, 0x1b, 0x02, 0x82, 0xe1,
        0x0e, 0xa0, 0x61, 0x1e, 0x6a, 0x9b, 0x49, 0xaf,
        0xbe, 0x1b, 0x0e, 0xbd, 0xcd, 0x9a, 0x89, 0x5c,
        0x7a, 0x27, 0x20, 0x25, 0xdd, 0x37, 0xf8, 0x35
    ];
    let y = [
        0x62, 0x12, 0x57, 0x45, 0x49, 0x87, 0xe1, 0xf7,
        0x23, 0x5a, 0xf9, 0xd6, 0x81, 0x33, 0x59, 0xf4,
        0x0a, 0x7a, 0xa8, 0x06, 0xf4, 0x2b, 0x75, 0x10,
        0x3a, 0x2d, 0xca, 0x61, 0x2f, 0xb1, 0xe6, 0xac
    ];
    (x, y)
}

riot_main!(main);

fn main() {
    extern "C" {
        fn do_vfs_init();
    }

    unsafe { do_vfs_init() };

    println!("[DEBUG] Starting EDHOC server...");
    log::set_logger(&LOGGER).unwrap();
    log::set_max_level(log::LevelFilter::Debug);
    info!("EDHOC server logger initialized");

    static PINGS: riot_coap_handler_demos::ping::PingPool =
        riot_coap_handler_demos::ping::PingPool::new();
    static PING_PASSIVE: riot_coap_handler_demos::ping_passive::PingHistoryMutex<
        { riot_coap_handler_demos::ping_passive::DEFAULT_SIZE },
    > = riot_coap_handler_demos::ping_passive::PingHistoryMutex::new();

    // Generate server's EDHOC credentials using the fixed private key
    let (own_credential, own_private_key) = generate_credpair_with_fixed_key();
    
    // Create RNG and crypto factory
    let rng = RiotRng;
    let crypto_factory = move || {
        let rng = RiotRng;
        Crypto::new(rng)
    };
    let time = TimeUnknown;

    println!("[DEBUG] Setting up base handler tree...");

    // Build the base handler tree
    let base_handler = coap_message_demos::full_application_tree(None)
        .below(&["ps"], riot_coap_handler_demos::ps::ps_tree())
        .below(&["led"], riot_coap_handler_demos::led::all_leds())
        .below(&["vfs"], riot_coap_handler_demos::vfs::vfs(""))
        .below(
            &["saul"],
            riot_coap_handler_demos::saul::SaulHandler::new(&["saul"]),
        )
        .below(&["netif"], riot_coap_handler_demos::netif::netif())
        .below(
            &["nib", "neigh"],
            riot_coap_handler_demos::nib::neighbor_cache(),
        )
        .below(&["ping"], riot_coap_handler_demos::ping::ping_tree(&PINGS))
        .at(
            &["pinged"],
            riot_coap_handler_demos::ping_passive::resource(&PING_PASSIVE),
        )
        .with_wkc();

    println!("[DEBUG] Creating EDHOC security configuration...");

    // Create EDHOC security configuration
    let (as_x, as_y) = get_as_public_key();
    let security_config = EdhocSecurityConfig::new()
        .with_as_public_key(as_x, as_y)
        .with_own_edhoc_credential(own_credential, own_private_key)
        .with_audience("test-rs")
        .allow_unauthenticated(scope::AllowAll.into());

    

    println!("[DEBUG] Wrapping handler with EDHOC/OSCORE security...");

    // First create the EDHOC/OSCORE handler
    let secure_handler = OscoreEdhocHandler::new(
        base_handler,
        security_config,
        crypto_factory,
        rng,
        time,
    );

    // Then wrap the entire secure handler with logging
    let logged_secure_handler = LoggingWrapper::new(secure_handler);

    // Use the logged version
    let mut handler = riot_wrappers::coap_handler::v0_2::GcoapHandler(logged_secure_handler);

    let mut listener = gcoap::SingleHandlerListener::new_catch_all(&mut handler);

    static SLOT: static_cell::StaticCell<
        gnrc::netreg::callback::Slot<riot_coap_handler_demos::ping::PingCallback>,
    > = static_cell::StaticCell::new();
    PINGS.register(SLOT.init(Default::default()));

    static PASSIVE_SLOT: static_cell::StaticCell<
        gnrc::netreg::callback::Slot<
            &'static riot_coap_handler_demos::ping_passive::PingHistoryMutex<
                { riot_coap_handler_demos::ping_passive::DEFAULT_SIZE },
            >,
        >,
    > = static_cell::StaticCell::new();
    PING_PASSIVE.register(PASSIVE_SLOT.init(Default::default()));

    gcoap::scope(|greg| {
        greg.register(&mut listener);

        println!("[DEBUG] === EDHOC Server Ready ===");
        println!("[DEBUG] EDHOC endpoint: /.well-known/edhoc");
        println!("[DEBUG] All requests will be logged");
        println!("[DEBUG] Waiting for interfaces...");

        let sectimer = ztimer::Clock::sec();
        sectimer.sleep(ztimer::Ticks(2));

        for netif in gnrc::Netif::all() {
            println!(
                "[DEBUG] Interface PID {:?} ({:?})",
                netif.pid(),
                netif.pid().get_name().unwrap_or("unnamed")
            );
            match netif.ipv6_addrs() {
                Ok(addrs) => {
                    for a in &addrs {
                        println!("[DEBUG]   Address: {:?}", a);
                    }
                }
                _ => {
                    println!("[DEBUG]   No IPv6 support");
                }
            }
        }

        println!("[DEBUG] Server running - waiting for EDHOC requests...");

        // Keep the main thread running
        loop {
            PINGS.tick();
            sectimer.sleep(ztimer::Ticks(1));
        }
    })
}
