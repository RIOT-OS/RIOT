/***************************************************************************//**
* \file cy_cryptolite.h
* \version 2.80
*
* \brief
*  This file provides interface header
*  for the Cryptolite PDL driver.
*
********************************************************************************
* \copyright
* Copyright (c) (2020-2022), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/
/**
* \addtogroup group_cryptolite
* \{
* \note IP Supported: CRYPTOLITE
* \note Availability of Cryptolite Algorithms will be chip specific, Refer to individual API for availability.
* \note Device Categories: CAT1B. Please refer <a href="usergroup1.html">Device Catalog</a>.
*
* The PDL Cryptolite driver provides a public API to perform hardware accelerated cryptographic calculations.
*
* The functions and other declarations used in this driver are in cy_cryptolite.h.
* You can also include cy_pdl.h to get access to all functions and declarations in the PDL.
*
* The Cryptolite driver supports AES (128bits), SHA-256, HMAC-SHA256, TRNG, RSA, ECDSA, EDDSA.
*
*
* \note For EDDSA, user application needs to implement SHA-512 functions. The prototypes of SHA functions are available in \ref cy_stc_cryptolite_ed25519_sha512_t.
* There is also separate EDDSA \ref Cy_Cryptolite_ED25519_Init and \ref Cy_Cryptolite_ED25519_Free functions needs to be called.
*
*
* \section group_cryptolite_configuration_considerations Configuration Considerations
*
* User can enable/disable cryptographic functionality based on the project
* requirements. To do so, create a configuration header file to configure cryptographic
* functionalities and define a macro CY_CRYPTOLITE_USER_CONFIG_FILE with configuration
* header file name and add to project environment. If CY_CRYPTOLITE_USER_CONFIG_FILE macro
* is not defined in project environment, firmware will enable all available
* cryptographic functionalities.
*
*Firmware sets up a cryptographic operation by passing in the required data as parameters in the function calls.
*
* All Cryptolite function require a context. A context is a data
* structure that the driver uses for its operations. Firmware declares a
* context (allocates memory) but does not write or read the values in that
* context. In effect, the context is a scratch pad you provide to the driver.
* The driver uses the context to store and manipulate data during cryptographic
* operations. The Cryptolite driver header file declare all the required structures
* for context.
*
*
* \section group_cryptolite_sha256_definitions Definitions
*
* <table class="doxtable">
*   <tr>
*     <th>Term</th>
*     <th>Definition</th>
*   </tr>
*
*   <tr>
*     <td>Secure Hash Algorithm (SHA)</td>
*     <td>A cryptographic hash function.
*     This function takes a message of an arbitrary length and reduces it to a
*     fixed-length residue or message digest after performing a series of
*     mathematically defined operations that practically guarantee that any
*     change in the message will change the hash value. It is used for message
*     authentication by transmitting a message with a hash value appended to it
*     and recalculating the message hash value using the same algorithm at the
*     recipient's end. If the hashes differ, then the message is corrupted.
*     For more information see [Secure Hash standard description]
*     (https://csrc.nist.gov/csrc/media/publications/fips/180/2/archive/2002-08-01/documents/fips180-2.pdf).
*     </td>
*   </tr>
*   <tr>
*     <td>Plaintext</td>
*     <td>An unencrypted message</td>
*   </tr>
*
*   <tr>
*     <td>Ciphertext</td>
*     <td>An encrypted message</td>
*   </tr>
*
*   <tr>
*     <td>Block Cipher</td>
*     <td>An encryption function for fixed-size blocks of data.
*     This function takes a fixed-size key and a block of plaintext data from
*     the message and encrypts it to generate ciphertext. Block ciphers are
*     reversible. The function performed on a block of encrypted data will
*     decrypt that data.</td>
*   </tr>
*
*   <tr>
*     <td>Block Cipher Mode</td>
*     <td>A mode of encrypting a message using block ciphers for messages of an
*     arbitrary length. The message is padded so that its length is an integer
*     multiple of the block size. ECB (Electronic Code Book), CBC (Cipher Block
*     Chaining), and CFB (Cipher Feedback) are all modes of using block ciphers
*     to create an encrypted message of an arbitrary length.
*     </td>
*   </tr>
*   <tr>
*     <td>Advanced Encryption Standard (AES)</td>
*     <td>The [AES standard] (https://nvlpubs.nist.gov/nistpubs/fips/nist.fips.197.pdf)
*     specifies the Rijndael algorithm, a symmetric block
*     cipher that can process 128-bit data blocks, using cipher keys with
*     128-, 192-, and 256-bit lengths. Rijndael was designed to handle
*     additional block sizes and key lengths. However, they are not adopted in
*     this standard. AES is also used for message authentication.
*     </td>
*   </tr>
*   <tr>
*     <td>Message Authentication Code (MAC)</td>
*     <td>MACs are used to verify that a received message has not been altered.
*     This is done by first computing a MAC value at the sender's end and
*     appending it to the transmitted message. When the message is received,
*     the MAC is computed again and checked against the MAC value transmitted
*     with the message. If they do not match, the message has been altered.
*     Either a Hash algorithm (such as SHA) or a block cipher (such as AES) can
*     be used to produce the MAC value. Keyed MAC schemes use a Secret Key
*     along with the message, thus the Key value must be known to be able to
*     compute the MAC value.</td>
*   </tr>
*
*   <tr>
*     <td>Hash Message Authentication Code (HMAC)</td>
*     <td>A specific type of message authentication code (MAC) that involves a
*     cryptographic hash function and a secret cryptographic key.
*     It computes the MAC value using a Hash algorithm.
*     For more information see [The Keyed-Hash Message Authentication Code standard]
*     (https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.198-1.pdf)
*     </td>
*   </tr>
*   <tr>
*     <td>True Random Number Generator (TRNG)</td>
*     <td>A block that generates a number that is statistically random and based
*     on some physical random variation. The number cannot be duplicated by
*     running the process again.</td>
*   </tr>
*
*   <tr>
*     <td>Symmetric Key Cryptography</td>
*     <td>Uses a common, known key to encrypt and decrypt messages (a shared
*     secret between sender and receiver). An efficient method used for
*     encrypting and decrypting messages after the authenticity of the other
*     party has been established. AES is well-known symmetric cryptography methods.</td>
*   </tr>
*
*   <tr>
*     <td>Asymmetric Key Cryptography</td>
*     <td>Also referred to as Public Key encryption. To receive a message,
*     you publish a very large public key (up to 4096 bits currently). The
*     public key is described by the pair (n, e) where n is a product of
*     two randomly chosen primes p and q. The exponent e is a random integer
*     1 < e < Q where Q = (p-1) (q-1). The private key d is uniquely defined
*     by the integer 1 < d < Q so that ed congruent with 1 (mod Q ). To send
*     a message to the publisher of the public key, you encrypt the message
*     with the public key. This message can now be decrypted only with the
*     private key. The message is now sent over any channel to the recipient
*     who can decrypt it with the private (secret) key. The same process is
*     used to send messages to the sender of the original message. The
*     asymmetric cryptography relies on the mathematical impracticality
*     (usually related to the processing power available at any given time)
*     of factoring the keys.
*     </td>
*   </tr>
*
* </table>
*
* \section group_cryptolite_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td >2.80</td>
*     <td>
*      <ul>
*      <li> Added new API's \ref Cy_Cryptolite_ED25519_Init, \ref Cy_Cryptolite_ED25519_Free, \ref Cy_Cryptolite_ED25519_Sign, \ref Cy_Cryptolite_ED25519_PointMultiplication, \ref Cy_Cryptolite_ED25519_PointDecode,
*          \ref Cy_Cryptolite_ED25519_Verify, \ref Cy_Cryptolite_ED25519_MakePublicKey, \ref Cy_Cryptolite_EC25519_MakePublicKey, \ref Cy_Cryptolite_EC25519_PointMultiplication.</li>
*      <li> Added new function pointers \ref cy_func_get_random_data_t, \ref cy_cryptolite_ed25519_sha512_init_t, \ref cy_cryptolite_ed25519_sha512_update_t,
*          \ref cy_cryptolite_ed25519_sha512_start_t, \ref cy_cryptolite_ed25519_sha512_finish_t, \ref cy_cryptolite_ed25519_sha512_free_t.</li>
*      <li> Added new enum \ref cy_en_cryptolite_eddsa_sig_type_t. Added new structures \ref cy_stc_cryptolite_ec25519_dp_type, \ref cy_stc_cryptolite_ed25519_sha512_t.</li>
*      <li> Added new macros </li>
*      <li> updated API's \ref Cy_Cryptolite_Cmac_Update, \ref Cy_Cryptolite_Aes_Ecb_Update, \ref Cy_Cryptolite_Aes_Cbc_Update.</li>
*      <li> updated enum \ref cy_en_cryptolite_ecc_curve_id_t. updated structure \ref cy_stc_cryptolite_context_ecdsa_t.</li>

*     </ul>
*     <td>Added support for amazon sidewalk crypto requirement.</td>
*   </tr>
*   <tr>
*     <td>2.70</td>
*     <td>Updated structure \ref cy_stc_cryptolite_context_sha256_t. </td>
*     <td>Bug fixes</td>
*   </tr>
*   <tr>
*     <td>2.60</td>
*     <td>
*         <ul>
*         <li>Added new API \ref Cy_Cryptolite_ECC_SharedSecret.</li>
*         <li>Updated APIs \ref Cy_Cryptolite_Sha256_Start, \ref Cy_Cryptolite_Sha256_Update, \ref Cy_Cryptolite_Aes_Ccm_Finish</li>
*         </ul>
*     </td>
*     <td>Added ECDH support for CAT1B devices and Bug fixes</td>
*   </tr>
*   <tr>
*     <td>2.50</td>
*     <td>
*         <ul>
*         <li>Support for AES-CCM, AES-CMAC, AES-CBC-MAC & AES multistage for modes ECB, CBC, CFB, CTR added.</li>
*         <li>Support for HKDF added.</li>
*         <li>Support for ECC key pair generation & ECDSA Sign added .</li>
*         </ul>
*     </td>
*     <td></td>
*   </tr>
*   <tr>
*     <td>2.40</td>
*     <td>Updated value of CY_CRYPTOLITE_DEF_TRNG_GARO macro.</td>
*     <td>Usability enhancement.</td>
*   </tr>
*   <tr>
*     <td>2.30</td>
*     <td>Updated APIs \ref Cy_Cryptolite_ECC_VerifyHash and \ref Cy_Cryptolite_Aes_Ctr.</td>
*     <td>Support for ECDSA verify for HASHZERO message added and MISRA 2012 violation fix</td>
*   </tr>
*   <tr>
*     <td>2.20</td>
*     <td>
*         <ul>
*         <li>Updated TRNG default config.</li>
*         </ul>
*     </td>
*     <td></td>
*   </tr>
*   <tr>
*     <td>2.10</td>
*     <td>
*         <ul>
*         <li>Added HMAC-SHA256 & AES(128 bit) support.</li>
*         <li>Added TRNG support.</li>
*         <li>Added RSA signature verification support upto 4096 bit</li>
*         <li>Added ECDSA signature verification support for SECP256R1 & SECP384R1</li>
*         </ul>
*     </td>
*     <td></td>
*   </tr>
*   <tr>
*     <td>2.0</td>
*     <td>Renamed the sha256 context structure from  cy_stc_cryptolite_context_sha_t to cy_stc_cryptolite_context_sha256_t </td>
*     <td></td>
*   </tr>
*   <tr>
*     <td>1.0</td>
*     <td>Initial Version</td>
*     <td></td>
*   </tr>
* </table>
*   \defgroup group_cryptolite_lld_sha Hash operations (SHA)
*   \{
*     \defgroup group_cryptolite_lld_sha_functions Functions
*   \}
*   \defgroup group_cryptolite_lld_mac Message Authentication Code (HMAC, CMAC)
*   \{
*     \defgroup group_cryptolite_lld_mac_functions Functions
*   \}
*   \defgroup group_cryptolite_lld_aes Symmetric Key Algorithm (AES)
*   \{
*     \defgroup group_cryptolite_lld_aes_functions Functions
*   \}
*   \defgroup group_cryptolite_lld_kdf Key Derivative Function (HKDF)
*   \{
*     \defgroup group_cryptolite_lld_kdf_functions Functions
*   \}
*   \defgroup group_cryptolite_lld_asymmetric Asymmetric Key Algorithm (RSA,ECP,ECDSA,EDDSA)
*   \{
*     \defgroup group_cryptolite_lld_asymmetric_functions Functions
*   \}
*   \defgroup group_cryptolite_lld_keygen  Key Generation (ECP)
*   \{
*     \defgroup group_cryptolite_lld_keygen_functions Functions
*   \}
*   \defgroup group_cryptolite_lld_random_number Random Number Generation(TRNG)
*   \{
*     \defgroup group_cryptolite_lld_rng_functions Functions
*   \}
*   \defgroup group_cryptolite_lld_vu Vector Unit (VU)
*   \{
*     \defgroup group_cryptolite_lld_vu_functions Functions
*   \}
* \defgroup group_cryptolite_data_structures Common Data Structures
* \defgroup group_cryptolite_enums Common Enumerated Types
* \defgroup group_cryptolite_macros Macros
*/

#if !defined (CY_CRYPTOLITE_H)
#define CY_CRYPTOLITE_H

#include "cy_device.h"

#if defined (CY_IP_MXCRYPTOLITE)

#include "cy_cryptolite_sha256.h"
#include "cy_cryptolite_hmac.h"
#include "cy_cryptolite_ecdsa.h"
#include "cy_cryptolite_rsa.h"
#include "cy_cryptolite_trng.h"
#include "cy_cryptolite_aes.h"
#include "cy_cryptolite_aes_ccm.h"
#include "cy_cryptolite_ecc_key_gen.h"
#include "cy_cryptolite_hkdf.h"
#include "cy_cryptolite_cmac.h"


#endif /* CY_IP_MXCRYPTOLITE */

#endif /* #if !defined (CY_CRYPTOLITE_H) */
/** \} group_cryptolite */

/* [] END OF FILE */
