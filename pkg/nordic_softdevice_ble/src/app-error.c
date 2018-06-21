/*
 * Copyright (c) 2018, Nordic Semiconductor
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/**
 * @addtogroup cpu
 * @{
 *
 * @addtogroup nrf52832
 * @{
 *
 * @addtogroup nrf52832-ble Bluetooth Low Energy drivers
 * @{
 *
 * @file
 *         NRF5 Soft Device error handler
 * @author
 *         Pekka Nikander <pekka.nikander@iki.fi>
 */

#include "app_error.h"
#include "nrf_strerror.h"
#include "nrf_sdm.h"

#include "log.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    LOG_ERROR("=== NRF Fatal error:\n");

    assert_info_t * p_assert_info = (assert_info_t *)info;
    error_info_t  * p_error_info  = (error_info_t *)info;
    switch (id) {

#if defined(SOFTDEVICE_PRESENT) && SOFTDEVICE_PRESENT
    case NRF_FAULT_ID_SD_ASSERT:
        LOG_ERROR("=== SOFTDEVICE: ASSERTION FAILED\n");
        break;
    case NRF_FAULT_ID_APP_MEMACC:
        LOG_ERROR("=== SOFTDEVICE: INVALID MEMORY ACCESS\n");
        break;
#endif
    case NRF_FAULT_ID_SDK_ASSERT:
        LOG_ERROR("=== ASSERTION FAILED at %s:%u\n",
              p_assert_info->p_file_name,
              p_assert_info->line_num);
        break;
    case NRF_FAULT_ID_SDK_ERROR:
        LOG_ERROR("=== ERROR %u [%s] at %s:%u\r\nPC at: 0x%08x\n",
              p_error_info->err_code,
              nrf_strerror_get(p_error_info->err_code),
              p_error_info->p_file_name,
              p_error_info->line_num,
              pc);
        break;
    default:
        LOG_ERROR("=== UNKNOWN FAULT at 0x%08X\n", pc);
        break;
    }

    LOG_ERROR("=== End of error report\n");
    core_panic(PANIC_ASSERT_FAIL, "NRF Fatal error");
}
