#ifndef NRFX_NFCT_T2T_EMULATOR_H
#define NRFX_NFCT_T2T_EMULATOR_H

#include "kernel_defines.h"
#include "event.h"
#include "thread.h"

#if !IS_USED(MODULE_NRFX_NFCT)
#error Please use the nrfx_nfct module to enable \
    the functionality on this device
#endif

#include "nrfx_nfct.h"
#include "net/nfc/t2t/t2t.h"

typedef struct {
    event_t super;
    uint32_t size;
} nrfx_t2t_receive_event_t;

typedef struct {
    event_t super;
    uint32_t size;
} nrfx_t2t_transmit_event_t;

#ifndef DEFAULT_T2T_EMULATOR_DEV
#define DEFAULT_T2T_EMULATOR_DEV (nfct_t2t_emulator_t) {    \
    .init = initialize_t2t,                                 \
    .enable = enable_t2t,                                   \
    .disable = disable_t2t,                                 \
    .uninit = uninitialize_t2t                              \
}
#endif

/**
 * @brief Initializes the Type 2 Tag emulator
 *
 * @note The tag will be modified and read after passing it to the emulator. To
 * guarantee that the emulator works correctly, the tag after should not be
 * accessed after initialization.
 * The tag can be enabled after initialization.
 *
 * @param[in] tag Tag to use for the emulator
 */
void initialize_t2t(nfc_t2t_t* tag);

/**
 * @brief Unititializes the Type 2 Tag emulator
 *
 * @note This only works on a tag that is not enabled.
 */
void uninitialize_t2t(void);

/**
 * @brief Enables an initialized Type 2 Tag emulator
 *
 * @note The emulator has to be initialized first.
 */
void enable_t2t(void);

/**
 * @brief Disables an enabled Type 2 Tag emulator
 */
void disable_t2t(void);

#endif