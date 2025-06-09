// Copyright (C) 2020 Christian Amsüss
//
// This file is subject to the terms and conditions of the GNU Lesser
// General Public License v2.1. See the file LICENSE in the top level
// directory for more details.
#![no_std]

use riot_wrappers::{gcoap, gnrc, ztimer};
use riot_wrappers::{println, riot_main};
use core::ffi::c_void;
use riot_wrappers::riot_sys;

use coap_handler_implementations::{HandlerBuilder, ReportingHandlerBuilder};
use coapcore::{OscoreEdhocHandler, seccfg, scope, ace, error::CredentialError, error::CredentialErrorDetail, generalclaims::GeneralClaims, time::TimeConstraint};
use lakers_crypto_rustcrypto::Crypto;
use rand_core::{RngCore, CryptoRng, impls, Error};
use coapcore::time::TimeUnknown;
use lakers::{CryptoTrait, Credential};
extern crate rust_riotmodules;

use heapless::{String, Vec};
use ccm::{KeyInit, aead::AeadInPlace};
use p256::ecdsa::{VerifyingKey, signature::Verifier};

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

// Custom Security Configuration
pub struct CustomSecurityConfig {
    /// Symmetric key for AES-CCM-16-128-256 (COSE algorithm 31)
    as_symmetric_key: Option<[u8; 32]>,
    /// Asymmetric key for ES256 (COSE algorithm -7) - (x, y, audience)
    as_asymmetric_key: Option<([u8; 32], [u8; 32], String<8>)>,
    /// Our own EDHOC credential and private key
    own_edhoc_credential: Option<(Credential, lakers::BytesP256ElemLen)>,
    /// Known EDHOC client credentials with their scopes
    known_edhoc_clients: Vec<(Credential, scope::UnionScope), 4>,
    /// Unauthenticated scope
    unauthenticated_scope: Option<scope::UnionScope>,
    /// Request creation hints for unauthorized responses
    request_creation_hints: &'static [u8],
}

impl CustomSecurityConfig {
    pub fn new() -> Self {
        Self {
            as_symmetric_key: None,
            as_asymmetric_key: None,
            own_edhoc_credential: None,
            known_edhoc_clients: Vec::new(),
            unauthenticated_scope: None,
            request_creation_hints: b"",
        }
    }

    pub fn with_symmetric_as_key(mut self, key: [u8; 32]) -> Self {
        self.as_symmetric_key = Some(key);
        self
    }

    pub fn with_asymmetric_as_key(mut self, x: [u8; 32], y: [u8; 32], audience: String<8>) -> Self {
        self.as_asymmetric_key = Some((x, y, audience));
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
}

// Custom claims implementation
#[derive(Debug, Clone)]
pub struct CustomClaims {
    pub scope: scope::UnionScope,
    pub time_constraint: TimeConstraint,
    pub is_important: bool,
}

impl GeneralClaims for CustomClaims {
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

impl seccfg::ServerSecurityConfig for CustomSecurityConfig {
    const PARSES_TOKENS: bool = true;
    const HAS_EDHOC: bool = true;

    type GeneralClaims = CustomClaims;

    fn decrypt_symmetric_token<'buf>(
        &self,
        headers: &ace::HeaderMap,
        aad: &[u8],
        ciphertext_buffer: &'buf mut [u8],
    ) -> Result<(Self::GeneralClaims, ace::CwtClaimsSet<'buf>), CredentialError> {
        type Aes256Ccm = ccm::Ccm<aes::Aes256, ccm::consts::U16, ccm::consts::U13>;
        const TAG_SIZE: usize = 16;
        const NONCE_SIZE: usize = 13;

        // Get the symmetric key
        let key = self.as_symmetric_key.ok_or_else(|| {
            println!("Error: Symmetrically encrypted token received, but no symmetric key configured.");
            CredentialErrorDetail::KeyNotPresent
        })?;

        let cipher = Aes256Ccm::new((&key).into());

        // Extract nonce from headers
        let nonce: &[u8; NONCE_SIZE] = headers
            .iv
            .ok_or_else(|| {
                println!("Error: IV missing from token.");
                CredentialErrorDetail::InconsistentDetails
            })?
            .try_into()
            .map_err(|_| {
                println!("Error: Token's IV length mismatches algorithm.");
                CredentialErrorDetail::InconsistentDetails
            })?;

        // Split ciphertext and tag
        let ciphertext_len = ciphertext_buffer
            .len()
            .checked_sub(TAG_SIZE)
            .ok_or_else(|| {
                println!("Error: Token's ciphertext too short for the algorithm's tag.");
                CredentialErrorDetail::InconsistentDetails
            })?;

        let (ciphertext, tag) = ciphertext_buffer.split_at_mut(ciphertext_len);

        // Decrypt
        cipher
            .decrypt_in_place_detached(nonce.into(), aad, ciphertext, ccm::Tag::from_slice(tag))
            .map_err(|_| {
                println!("Error: Token decryption failed.");
                CredentialErrorDetail::VerifyFailed
            })?;

        // Parse claims
        let claims: ace::CwtClaimsSet = minicbor::decode(ciphertext)
            .map_err(|_| {
                println!("Error: Failed to decode CWT claims set.");
                CredentialErrorDetail::UnsupportedExtension
            })?;

        // Parse scope
        let scope = scope::AifValue::parse(claims.scope)
            .map_err(|_| {
                println!("Error: Failed to parse scope from token.");
                CredentialErrorDetail::UnsupportedExtension
            })?
            .into();

        let time_constraint = TimeConstraint::from_claims_set(&claims);

        let custom_claims = CustomClaims {
            scope,
            time_constraint,
            is_important: false,
        };

        println!("Successfully decrypted and validated symmetric token.");
        Ok((custom_claims, claims))
    }

    fn verify_asymmetric_token<'b>(
        &self,
        headers: &ace::HeaderMap,
        signed_data: &[u8],
        signature: &[u8],
        signed_payload: &'b [u8],
    ) -> Result<(Self::GeneralClaims, ace::CwtClaimsSet<'b>), CredentialError> {
        // Check algorithm
        if headers.alg != Some(-7) {
            println!("Error: Unsupported algorithm for asymmetric token.");
            return Err(CredentialErrorDetail::UnsupportedAlgorithm.into());
        }

        // Get the asymmetric key
        let Some((x, y, rs_audience)) = self.as_asymmetric_key.as_ref() else {
            println!("Error: Asymmetrically signed token received, but no asymmetric key configured.");
            return Err(CredentialErrorDetail::KeyNotPresent.into());
        };

        // Create verifying key
        let as_key = VerifyingKey::from_encoded_point(
            &p256::EncodedPoint::from_affine_coordinates(x.into(), y.into(), false),
        )
        .map_err(|_| {
            println!("Error: Failed to create verifying key from configured coordinates.");
            CredentialErrorDetail::InconsistentDetails
        })?;

        // Parse signature
        let signature = p256::ecdsa::Signature::from_slice(signature)
            .map_err(|_| {
                println!("Error: Invalid signature format.");
                CredentialErrorDetail::InconsistentDetails
            })?;

        // Verify signature
        as_key
            .verify(signed_data, &signature)
            .map_err(|_| {
                println!("Error: Signature verification failed.");
                CredentialErrorDetail::VerifyFailed
            })?;

        // Parse claims
        let claims: ace::CwtClaimsSet = minicbor::decode(signed_payload)
            .map_err(|_| {
                println!("Error: Failed to decode signed payload.");
                CredentialErrorDetail::UnsupportedExtension
            })?;

        // Check audience
        if claims.aud != Some(rs_audience) {
            println!("Error: Token audience doesn't match our audience.");
            return Err(CredentialErrorDetail::VerifyFailed.into());
        }

        // Parse scope
        let scope = scope::AifValue::parse(claims.scope)
            .map_err(|_| {
                println!("Error: Failed to parse scope from signed token.");
                CredentialErrorDetail::UnsupportedExtension
            })?
            .into();

        let time_constraint = TimeConstraint::from_claims_set(&claims);

        let custom_claims = CustomClaims {
            scope,
            time_constraint,
            is_important: false,
        };

        println!("Successfully verified and validated asymmetric token.");
        Ok((custom_claims, claims))
    }

    fn own_edhoc_credential(&self) -> Option<(Credential, lakers::BytesP256ElemLen)> {
        self.own_edhoc_credential.clone()
    }

    fn expand_id_cred_x(
        &self,
        id_cred_x: lakers::IdCred,
    ) -> Option<(Credential, Self::GeneralClaims)> {
        println!("Evaluating peer's credential...");

        // Check against known EDHOC clients
        for (credential, scope) in &self.known_edhoc_clients {
            if id_cred_x.reference_only() {
                // Check by KID
                if let Ok(cred_kid) = credential.by_kid() {
                    if cred_kid.as_ref() == Ok(&id_cred_x) {
                        println!("Peer indicated use of preconfigured key by KID.");
                        return Some((
                            credential.clone(),
                            CustomClaims {
                                scope: scope.clone(),
                                time_constraint: TimeConstraint::unbounded(),
                                is_important: true,
                            },
                        ));
                    }
                }
            } else {
                // Check by value
                if let Ok(cred_value) = credential.by_value() {
                    if cred_value.as_ref() == Ok(&id_cred_x) {
                        println!("Peer indicated use of preconfigured credential by value.");
                        return Some((
                            credential.clone(),
                            CustomClaims {
                                scope: scope.clone(),
                                time_constraint: TimeConstraint::unbounded(),
                                is_important: true,
                            },
                        ));
                    }
                }
            }
        }

        // Check if unauthenticated clients are allowed
        if let Some(unauth_scope) = &self.unauthenticated_scope {
            println!("Unauthenticated clients are generally accepted, evaluating credential.");
            if let Some(credential_by_value) = id_cred_x.get_ccs().as_ref() {
                println!("The unauthorized client provided a usable credential by value.");
                return Some((
                    credential_by_value.clone(),
                    CustomClaims {
                        scope: unauth_scope.clone(),
                        time_constraint: TimeConstraint::unbounded(),
                        is_important: false,
                    },
                ));
            }
        }

        println!("No matching credential found for peer.");
        None
    }

    fn nosec_authorization(&self) -> Option<Self::GeneralClaims> {
        self.unauthenticated_scope.as_ref().map(|scope| CustomClaims {
            scope: scope.clone(),
            time_constraint: TimeConstraint::unbounded(),
            is_important: false,
        })
    }

    fn render_not_allowed<M: coap_message::MutableWritableMessage>(
        &self,
        message: &mut M,
    ) -> Result<(), seccfg::NotAllowedRenderingFailed> {
        use coap_message::Code;
        message.set_code(M::Code::new(coap_numbers::code::UNAUTHORIZED).map_err(|_| {
            println!("Error: CoAP stack can not represent Unauthorized responses.");
            seccfg::NotAllowedRenderingFailed
        })?);
        message
            .set_payload(self.request_creation_hints)
            .map_err(|_| {
                println!("Error: Request creation hints do not fit in error message.");
                seccfg::NotAllowedRenderingFailed
            })?;
        Ok(())
    }
}

// Generate credential pair function - simplified version without cbo! macro
fn generate_credpair() -> (Credential, lakers::BytesP256ElemLen) {
    let mut crypto = lakers_crypto_rustcrypto::Crypto::new(RiotRng);
    let (private, public) = crypto.p256_generate_key_pair();
    
    // Create a simple credential with the public key
    // This is a minimal CCS (CBOR Credential Set) structure
    let mut cred_bytes = heapless::Vec::<u8, 128>::new();
    
    // Simple CBOR structure: {8: {1: {1: 2, 2: h'', -1: 1, -2: public_key}}}
    // This represents: {cnf: {COSE_Key: {kty: EC2, kid: empty, crv: P-256, x: public_key}}}
    cred_bytes.extend_from_slice(&[
        0xA1, 0x08, 0xA1, 0x01, 0xA4, 0x01, 0x02, 0x02, 0x40, 0x20, 0x01, 0x21, 0x58, 0x20
    ]).unwrap();
    cred_bytes.extend_from_slice(&public).unwrap();
    
    let credential = Credential::parse_ccs(&cred_bytes).expect("Valid credential");
    println!("Generated private/public key pair.");
    
    (credential, private)
}

// Dummy keys for testing - replace with real keys later
fn get_dummy_as_symmetric_key() -> [u8; 32] {
    [0x42; 32] // Dummy symmetric key
}

fn get_dummy_as_asymmetric_key() -> ([u8; 32], [u8; 32]) {
    // Dummy ES256 public key coordinates
    let x = [0x01; 32];
    let y = [0x02; 32];
    (x, y)
}

riot_main!(main);

fn main() {
    extern "C" {
        fn do_vfs_init();
    }

    unsafe { do_vfs_init() };

    static PINGS: riot_coap_handler_demos::ping::PingPool =
        riot_coap_handler_demos::ping::PingPool::new();
    static PING_PASSIVE: riot_coap_handler_demos::ping_passive::PingHistoryMutex<
        { riot_coap_handler_demos::ping_passive::DEFAULT_SIZE },
    > = riot_coap_handler_demos::ping_passive::PingHistoryMutex::new();

    // Generate server credentials
    let (own_credential, own_private_key) = generate_credpair();
    
    // Create RNG and crypto factory
    let rng = RiotRng;
    let crypto_factory = move || {
        let rng = RiotRng;
        Crypto::new(rng)
    };
    let time = TimeUnknown;

    // Build the base handler tree first
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

    // Create custom security configuration
    let security_config = CustomSecurityConfig::new()
        .with_symmetric_as_key(get_dummy_as_symmetric_key())
        .with_asymmetric_as_key(
            get_dummy_as_asymmetric_key().0,
            get_dummy_as_asymmetric_key().1,
            String::from("test-rs").unwrap()
        )
        .with_own_edhoc_credential(own_credential, own_private_key)
        .allow_unauthenticated(scope::AllowAll.into());

    // Wrap the base handler with EDHOC/OSCORE security
    let secure_handler = OscoreEdhocHandler::new(
        base_handler,
        security_config,
        crypto_factory,
        rng,
        time,
    );

    let mut handler = riot_wrappers::coap_handler::v0_2::GcoapHandler(secure_handler);

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

        println!(
            "CoAP server ready with EDHOC endpoint at /.well-known/edhoc; waiting for interfaces to settle before reporting addresses..."
        );

        let sectimer = ztimer::Clock::sec();
        sectimer.sleep(ztimer::Ticks(2));

        for netif in gnrc::Netif::all() {
            println!(
                "Active interface from PID {:?} ({:?})",
                netif.pid(),
                netif.pid().get_name().unwrap_or("unnamed")
            );
            match netif.ipv6_addrs() {
                Ok(addrs) => {
                    for a in &addrs {
                        println!("    Address {:?}", a);
                    }
                }
                _ => {
                    println!("    Does not support IPv6.");
                }
            }
        }

        // Sending main thread to sleep; can't return or the Gcoap handler would need to be
        // deregistered (which it can't).
        loop {
            PINGS.tick();
            sectimer.sleep(ztimer::Ticks(1));
        }
    })
}
