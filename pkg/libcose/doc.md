/**
 * @defgroup pkg_libcose  libcose for RIOT
 * @ingroup  pkg
 * @brief    Constrained node COSE library
 * @see      https://github.com/bergzand/libcose
 *
 * Usage
 * -----
 *
 * Add as a package in the Makefile of your application:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * USEPKG += libcose
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The main consumers of libcose are other libraries or system components, for
 * example @ref sys_suit "SUIT".
 *
 * The use of the library itself [is described in the libcose
 * documentation](https://bergzand.github.io/libcose/), and some example code
 * can be found in
 * [`tests/pkg_libcose/`](https://github.com/RIOT-OS/RIOT/tree/master/tests/pkg_libcose).
 *
 * Backends
 * --------
 *
 * The libcose library does not implement cryptographic algorithms itself, but
 * fans that out to other libraries (which are pulled into the dependency tree
 * as packages) and exposes a consistent interface for the
 * operations they provide.
 *
 * Backends are selected by any of those pseudomodules:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * USEMODULE += libcose_crypt_hacl
 * USEMODULE += libcose_crypt_monocypher
 * USEMODULE += libcose_crypt_c25519
 * USEMODULE += libcose_crypt_tinycrypt
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The selection of pseudomodules determines the available algorithms:
 *
 * * @ref pkg_hacl "HACL" and @ref pkg_monocypher "Monocypher" both provide ChaCha20-Poly1305 and Ed25519.
 * * @ref pkg_c25519 "C25519" only provides the Ed25519 algorithm.
 * * @ref pkg_tinycrypt "TINYCRYPT" provides AES-CCM-16-64-128, AES-CCM-16-128-128
 *   ECDSA and ES256 as well as EC NIST P-256 curve support.
 *
 * Some backend may conflict, others might be complementary.
 */
