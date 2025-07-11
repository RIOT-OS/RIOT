#!/usr/bin/env python3

import asyncio
import logging
import cbor2
import hashlib
import hmac
import secrets
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
from cryptography.hazmat.backends import default_backend
import aiocoap
from aiocoap import Message, Code
from aiocoap.oscore import SimpleGroupContext, FilesystemSecurityContext
import struct
import json
import os

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class EdhocClient:
    """EDHOC Client implementation for aiocoap"""
    
    def __init__(self, server_host="[::1]", server_port=5683):
        self.server_host = server_host
        self.server_port = server_port
        self.context = None
        
        # EDHOC parameters
        self.method = 3  # static-static
        self.cipher_suite = 2  # AES-CCM-16-64-128, SHA-256, 8, X25519, Ed25519, AES-CCM-16-64-128, SHA-256
        
        # Client credentials (these would normally be provisioned)
        self.client_private_key = None
        self.client_public_key = None
        self.client_credential = None
        self.client_kid = b"client_001"  # Key identifier
        
        # Server's expected public key (in a real scenario, this would be pre-configured or obtained securely)
        # Using the same key from your server code for testing
        self.server_public_key_x = bytes.fromhex("0d8d5d2e1b0282e10ea0611e6a9b49afbe1b0ebdcd9a895c7a272025dd37f835")
        self.server_public_key_y = bytes.fromhex("621257454987e1f7235af9d6813359f40a7aa806f42b75103a2dca612fb1e6ac")
        
        # Access token (would be obtained from Authorization Server)
        self.access_token = None
        
        # EDHOC session state
        self.ephemeral_key = None
        self.session_transcript = []
        self.shared_secret = None
        self.oscore_context = None
        
    def generate_client_credentials(self):
        """Generate client's EDHOC credentials"""
        # Generate EC P-256 key pair for client
        self.client_private_key = ec.generate_private_key(ec.SECP256R1(), default_backend())
        self.client_public_key = self.client_private_key.public_key()
        
        # Get public key coordinates
        public_numbers = self.client_public_key.public_numbers()
        x_bytes = public_numbers.x.to_bytes(32, 'big')
        y_bytes = public_numbers.y.to_bytes(32, 'big')
        
        # Create CBOR credential (CCS format)
        # Structure: {8: {1: {1: 2, -1: 1, -2: x_coord, -3: y_coord, 4: [kid]}}}
        cose_key = {
            1: 2,      # kty: EC2
            -1: 1,     # crv: P-256
            -2: x_bytes,  # x coordinate
            -3: y_bytes,  # y coordinate
            4: [self.client_kid]  # kid
        }
        
        self.client_credential = {
            8: {1: cose_key}  # cnf: {COSE_Key: ...}
        }
        
        logger.info(f"Generated client credentials with KID: {self.client_kid.hex()}")
        logger.info(f"Client public key x: {x_bytes.hex()}")
        logger.info(f"Client public key y: {y_bytes.hex()}")
    
    def generate_access_token(self):
        """Generate a mock access token (normally obtained from AS)"""
        # In a real implementation, this would be obtained from the Authorization Server
        # For testing, we'll create a mock token
        token_payload = {
            1: "test-rs",  # aud: audience (resource server)
            2: "client_001",  # sub: subject (client identifier)
            3: int(asyncio.get_event_loop().time()) + 3600,  # exp: expiration
            4: int(asyncio.get_event_loop().time()),  # iat: issued at
            9: {  # scope in AIF format
                "/led": 5,  # GET and PUT permissions
                "/ping": 1,  # GET permission
                "/ps": 1,   # GET permission
            }
        }
        
        # Encode as CBOR
        token_cbor = cbor2.dumps(token_payload)
        
        # For simplicity, we'll use an unprotected token
        # In practice, this should be signed by the AS
        self.access_token = token_cbor
        logger.info("Generated mock access token")
    
    async def send_edhoc_message1(self):
        """Send EDHOC Message 1"""
        logger.info("Sending EDHOC Message 1...")
        
        # Generate ephemeral key
        self.ephemeral_key = ec.generate_private_key(ec.SECP256R1(), default_backend())
        ephemeral_public = self.ephemeral_key.public_key()
        
        # Get ephemeral public key coordinates
        eph_numbers = ephemeral_public.public_numbers()
        g_x = eph_numbers.x.to_bytes(32, 'big') + eph_numbers.y.to_bytes(32, 'big')
        
        # Message 1 structure: (METHOD, SUITES_I, G_X, C_I, EAD_1)
        c_i = secrets.token_bytes(2)  # Connection identifier
        
        message1_cbor = cbor2.dumps([
            self.method,
            self.cipher_suite,
            g_x,
            c_i,
            #{}  # EAD_1 (empty for now)
        ])[1:] # quick and dirty array-to-cbor-stream

        message1_cbor = cbor2.dumps(True) + message1_cbor
        
        # Store transcript
        self.session_transcript.append(message1_cbor)
        
        # Send to server
        context = await aiocoap.Context.create_client_context()
        self.context = context
        
        request = Message(
            code=Code.POST,
            uri=f"coap://{self.server_host}:{self.server_port}/.well-known/edhoc",
            payload=message1_cbor,
            content_format=62  # application/cid-edhoc+cbor (more widely expected)
        )
        
        response = await context.request(request).response
        
        if response.code.is_successful():
            logger.info("EDHOC Message 1 sent successfully")
            return response.payload
        else:
            raise Exception(f"EDHOC Message 1 failed: {response.code}")
    
    async def process_edhoc_message2(self, message2_data):
        """Process EDHOC Message 2 and send Message 3"""
        logger.info("Processing EDHOC Message 2...")
        
        # Decode Message 2: (G_Y, C_R, {ID_CRED_R, Signature_R, EAD_2})
        try:
            message2 = cbor2.loads(message2_data)
            g_y = message2[0]
            c_r = message2[1]
            encrypted_part = message2[2]
            
            logger.info(f"Received G_Y length: {len(g_y)}")
            logger.info(f"Received C_R: {c_r.hex() if isinstance(c_r, bytes) else c_r}")
            
        except Exception as e:
            logger.error(f"Failed to decode Message 2: {e}")
            raise
        
        # Store Message 2 in transcript
        self.session_transcript.append(message2_data)
        
        # Extract server's ephemeral public key
        if len(g_y) == 64:
            server_x = int.from_bytes(g_y[:32], 'big')
            server_y = int.from_bytes(g_y[32:], 'big')
        else:
            raise ValueError("Invalid G_Y format")
        
        # Compute shared secret using ECDH
        server_ephemeral_public = ec.EllipticCurvePublicNumbers(
            server_x, server_y, ec.SECP256R1()
        ).public_key(default_backend())
        
        shared_key = self.ephemeral_key.exchange(ec.ECDH(), server_ephemeral_public)
        
        # Derive keys using EDHOC key derivation
        # This is simplified - real EDHOC uses more complex KDF
        prk = hashes.Hash(hashes.SHA256(), backend=default_backend())
        prk.update(shared_key)
        prk_digest = prk.finalize()
        
        # Create Message 3
        logger.info("Creating EDHOC Message 3...")
        
        # Client's ID_CRED_I (credential identifier)
        id_cred_i = {4: self.client_kid}  # by kid
        
        # Create signature input (simplified)
        signature_input = b"EDHOC-Message-3-Signature"
        signature_input += cbor2.dumps(id_cred_i)
        
        # Sign with client's private key
        signature = self.client_private_key.sign(
            signature_input,
            ec.ECDSA(hashes.SHA256())
        )
        
        # Message 3 payload
        message3_payload = [
            id_cred_i,
            signature,
            {}  # EAD_3
        ]
        
        # Add access token to EAD_3 if available
        if self.access_token:
            message3_payload[2] = {
                1: self.access_token  # Access token in EAD
            }
        
        message3_cbor = cbor2.dumps(message3_payload)
        
        # Encrypt Message 3 (simplified encryption)
        # In real EDHOC, this uses proper AEAD encryption
        aes_key = prk_digest[:16]  # Use first 16 bytes as AES key
        aesgcm = AESGCM(aes_key)
        nonce = secrets.token_bytes(12)
        
        encrypted_message3 = aesgcm.encrypt(nonce, message3_cbor, b"")
        
        final_message3 = cbor2.dumps([nonce + encrypted_message3])
        
        # Store in transcript
        self.session_transcript.append(final_message3)
        
        # Send Message 3
        request = Message(
            code=Code.POST,
            uri=f"coap://{self.server_host}:{self.server_port}/.well-known/edhoc",
            payload=final_message3,
            content_format=62
        )
        
        response = await self.context.request(request).response
        
        if response.code.is_successful():
            logger.info("EDHOC Message 3 sent successfully")
            
            # Derive OSCORE context
            self.derive_oscore_context(prk_digest)
            return True
        else:
            logger.error(f"EDHOC Message 3 failed: {response.code}")
            return False
    
    def derive_oscore_context(self, prk):
        """Derive OSCORE security context from EDHOC"""
        logger.info("Deriving OSCORE security context...")
        
        # Derive OSCORE keys (simplified)
        # Real EDHOC uses proper HKDF with specific labels
        
        # Master Secret
        master_secret = prk[:16]
        
        # Master Salt  
        master_salt = prk[16:24] if len(prk) >= 24 else b"\x00" * 8
        
        # Sender/Recipient IDs (simplified)
        sender_id = b"\x01"
        recipient_id = b"\x02"
        
        self.oscore_context = {
            'master_secret': master_secret,
            'master_salt': master_salt,
            'sender_id': sender_id,
            'recipient_id': recipient_id,
            'algorithm': 10,  # AES-CCM-16-64-128
        }
        
        logger.info("OSCORE context derived successfully")
        logger.info(f"Master Secret: {master_secret.hex()}")
        logger.info(f"Master Salt: {master_salt.hex()}")
    
    async def test_protected_request(self):
        """Test a protected CoAP request using the established OSCORE context"""
        if not self.oscore_context:
            logger.error("No OSCORE context available")
            return
        
        logger.info("Testing protected CoAP request...")
        
        try:
            # Create OSCORE-protected request to /ping
            # Note: This is a simplified example. Real OSCORE implementation
            # requires proper message encryption and replay protection
            
            request = Message(
                code=Code.GET,
                uri=f"coap://{self.server_host}:{self.server_port}/ping",
            )
            
            # Add OSCORE option (simplified)
            # In a real implementation, you'd use aiocoap's OSCORE support
            request.opt.oscore = b""  # Empty OSCORE option for first message
            
            response = await self.context.request(request).response
            
            if response.code.is_successful():
                logger.info(f"Protected request successful: {response.payload.decode()}")
                return True
            else:
                logger.info(f"Protected request returned: {response.code}")
                return False
                
        except Exception as e:
            logger.error(f"Protected request failed: {e}")
            return False
    
    async def run_edhoc_handshake(self):
        """Run the complete EDHOC handshake"""
        try:
            logger.info("Starting EDHOC handshake...")
            
            # Generate client credentials
            self.generate_client_credentials()
            
            # Generate access token
            self.generate_access_token()
            
            # Step 1: Send EDHOC Message 1
            message2_data = await self.send_edhoc_message1()
            
            # Step 2: Process Message 2 and send Message 3
            success = await self.process_edhoc_message2(message2_data)
            
            if success:
                logger.info("EDHOC handshake completed successfully!")
                
                # Test the established security context
                await self.test_protected_request()
                
                return True
            else:
                logger.error("EDHOC handshake failed")
                return False
                
        except Exception as e:
            logger.error(f"EDHOC handshake error: {e}")
            return False
        finally:
            if self.context:
                await self.context.shutdown()

async def main():
    """Main function"""
    # Configure server address (adjust as needed)
    server_host = "[2001:db8::5c0d:88b4:66d4:1824]"  # IPv6 localhost, change to your server's address
    server_port = 5683
    
    # You can also use IPv4
    # server_host = "127.0.0.1"
    
    client = EdhocClient(server_host, server_port)
    
    logger.info(f"Connecting to EDHOC server at {server_host}:{server_port}")
    
    success = await client.run_edhoc_handshake()
    
    if success:
        logger.info("EDHOC client completed successfully!")
        return 0
    else:
        logger.error("EDHOC client failed!")
        return 1

if __name__ == "__main__":
    import sys
    sys.exit(asyncio.run(main()))
