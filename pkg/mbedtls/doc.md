/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup   pkg_mbedtls ARM Mbed TLS
 * @ingroup    pkg
 * @brief      SSL crypto library
 * @see        https://github.com/ARMmbed/mbedtls.git
 * @warning    Entropy sources need to be thoroughly evaluated before deployment.
 * @experimental This port is in an early state - expect changes.
 *
 *
 * # Overview
 *
 * This port includes the ARM Mbed TLS library as a package. A more comprehensive
 * library documentation can be found in the [knowledge base](https://tls.mbed.org/kb) and the
 * [source code documentation](https://tls.mbed.org/api/).  Mbed TLS is highly
 * configurable and it allows inclusion of alternative hardware or OS specific implementations
 * to be used by the library.
 * A porting guide can be found [here](https://tls.mbed.org/kb/how-to/how-do-i-port-mbed-tls-to-a-new-environment-OS).
 * The configuration and enablement of alternative implementations has to be defined in
 * the [configuration file](include/riot_mbedtls_config.h) of this package.
 *
 *
 * # Tested Submodules
 *
 * ## Entropy
 * The `entropy` and `entropy_poll` components of the Mbed TLS library
 * have been tested in RIOT. The module uses a SHA-256 or a SHA-512 based
 * accumulator. This port enables the SHA-256 accumulator and it adapts the RIOT SHA-256
 * implementation to be used as a backend. Our reconfiguration disables the internal
 * Mbed TLS implementation. A future
 * compile time configuration in RIOT with transparent hardware crypto acceleration
 * support will be used by the package seamlessly.
 *
 * The entropy module is initialized with one or more entropy
 * sources which are polled iteratively and accumulated. Sources for entropy are
 * registered with a function pointer and an assumption about their strength (strong/weak).
 * The module is designed for cryptographic purposes, thus, an entropy poll will
 * fail if there is not a single strong source. Strong sources are typically true
 * (hardware) random number generators with conditioning which will provide full entropy
 * per sample. In contrast, weak sources do not necessarily provide full entropy per
 * sample and act as a complement, even if they only contain few random bits in a sample,
 * which is typically the case for sampled noise sources.
 *
 *
 * ### Entropy Sources
 *
 * RIOT provides default sources to feed the entropy module. Yet, only
 * @ref drivers_periph_hwrng and @ref sys_entropy_source_adc are enabled in this package. It is planned to
 * extend this list. In addition, a user can connect a personal source to the entropy
 * poll by implementing [`mbedtls_entropy_add_source`](https://tls.mbed.org/api/entropy_8h.html).
 *
 * It is noteworthy that an entropy module might be required during OS bootstrapping,
 * hence, a module that is added in `main()` will not be incorporated during `auto_init()`.
 *
 * Mbed TLS entropy sources are expressed as pseudomodules.
 *
 * ```
 * PSEUDOMODULES += mbedtls_entropy_source_hwrng
 * PSEUDOMODULES += mbedtls_entropy_source_adc
 * ```
 *
 * Clearly, these sources require hardware capabilities which are indicated by a platform.
 *
 * ```
 * FEATURES_PROVIDED += periph_hwrng
 * FEATURES_PROVIDED += periph_adc
 * ```
 *
 * All sources will be enabled by default, if available. Furthermore, a single source
 * can be manually excluded from the entropy poll by disabling it in the applications Makefile.
 *
 * ```
 * DISABLE_MODULE += mbedtls_entropy_source_hwrng
 * ```
 *
 * ### Usage
 * The RIOT API @ref pkg_mbedtls_entropy
 * can be used which wraps around the Mbed TLS API and handles one entropy context internally.
 * Thereby, @ref entropy_mbedtls_riot_get initializes the module, retrieves entropy
 * values and uninitializes the context afterwards.
 * Alternatively, the entropy module can be used directly by calling the Mbed TLS API which requires
 * the entropy context to be allocated from application code. The adaptation in this
 * package will then only utilize the first configured entropy source
 * during initialization.
 *
 * # Testing
 * Many Mbed TLS implementations provide self tests within the boundaries of
 * a module and the [test folder](../../tests/pkg_mbedtls) acts as a place to execute
 * these tests in RIOT context. It is noteworthy, that built-in Mbed TLS entropy source tests
 * only execute on the source that is implemented in `mbedtls_hardware_poll`.
 * The additional sources that were added using `mbedtls_entropy_add_source` are ignored in the test.
 *
 */
