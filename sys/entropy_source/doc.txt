/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup     sys_entropy_source Entropy Sources
 * @ingroup      sys
 * @brief        Collection of different Entropy Sources
 * @warning      Entropy sources need to be thoroughly evaluated before deployment!
 * @experimental This API is in an early state - expect changes.
 * @note         This API is considered as internal. Only use it if you know what
 *               you are doing and expect API changes without deprecation.
 *
 *
 * @brief        Collection of entropy sources.
 *
 *
 * This module adds support for additional entropy sources next to pure peripheral sources like
 * @ref drivers_periph_hwrng and @ref sys_puf_sram. The concepts implemented here are heavily
 * influenced by NIST SP 800-90B. Entropy sources can be used to feed more advanced entropy
 * modules for cryptographic purposes, which typically accumulate multiple sources and safely
 * maintain internal states. Alternatively, these sources can be used directly or with internal
 * conditioning enabled for non-cryptographic tasks like seed generation of general purpose PRNGs,
 * in the absence of a hardware random number generator. The API, however, is not meant to face a user.
 *
 * A common component provides optional access to health tests and conditioning
 * (@ref sys_entropy_source_config) that can be run on
 * parallel instantiations. The conditioning currently implements a von Neumann extractor to
 * unbias samples. It adds a variable runtime (dependent on the input samples) but is lightweight.
 * In future, other conditioning mechanisms such as hash based derivation functions might be
 * included.
 *
 * Entropy sources require thorough testing and evaluation for serious deployments which is out of
 * scope of this module, and we refer to <a href="https://github.com/usnistgov/
 * SP800-90B_EntropyAssessment">SP800-90B_EntropyAssessment</a> for validation. Among other metrics,
 * this tool will return an entropy estimation per sample that should be employed for every
 * deployment scenario and must be configured in software accordingly. In the specific case
 * of the ADC based entropy source, a developer needs to set @ref CONFIG_ENTROPY_SOURCE_ADC_HMIN
 * accordingly.
 *
 */
