/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_adc_ng
 * @{
 *
 * @file
 * @brief       XFA initializations
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>

#include "adc_ng_internal.h"
#include "xfa.h"

/* Compilation fails when XFA_USE_CONST() and XFA_INIT_CONST() are part of the
 * same compilation unit. This is worked around by splitting out the XFA
 * initializations into this file, where we don't need to include adc_ng.h */
XFA_INIT_CONST(adc_ng_backend_t, adc_ng_backends);
XFA_INIT(adc_ng_state_t, adc_ng_states);
