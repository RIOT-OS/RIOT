/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dfplayer
 * @{
 *
 * @file
 * @brief       Types used in the DFPlayer Mini Device Driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef DFPLAYER_TYPES_H
#define DFPLAYER_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "mutex.h"
#include "periph/gpio.h"
#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Enumeration of the RX states of the DFPlayer driver
 */
typedef enum {
    DFPLAYER_RX_STATE_START,    /**< Waiting for start symbol (`0x7e`) */
    DFPLAYER_RX_STATE_VERSION,  /**< Waiting for version (`0xff`) */
    DFPLAYER_RX_STATE_LENGTH,   /**< Waiting for length (`0x06`) */
    DFPLAYER_RX_STATE_DATA,     /**< Receiving data */
    DFPLAYER_RX_STATE_NUMOF     /**< Number of RX states */
} dfplayer_rx_state_t;

/**
 * @brief   Enumeration of the equalizer settings supported by the DFPlayer
 */
typedef enum {
    DFPLAYER_EQ_NORMAL,         /**< The "Normal" equalizer setting */
    DFPLAYER_EQ_POP,            /**< The "Pop" equalizer setting */
    DFPLAYER_EQ_ROCK,           /**< The "Rock" equalizer setting */
    DFPLAYER_EQ_JAZZ,           /**< The "Jazz" equalizer setting */
    DFPLAYER_EQ_CLASSIC,        /**< The "Classic" equalizer setting */
    DFPLAYER_EQ_BASE,           /**< The "Base" equalizer setting */
    DFPLAYER_EQ_NUMOF           /**< Number of supported equalizer settings */
} dfplayer_eq_t;

/**
 * @brief   Enumeration of the playback modes supported by the DFPlayer
 */
typedef enum {
    DFPLAYER_MODE_UNKOWN,       /**< No idea, the data sheet is vague */
    DFPLAYER_MODE_REPEAT_DIR,   /**< Repeating a directory */
    DFPLAYER_MODE_REPEAT,       /**< Repeating a single file */
    DFPLAYER_MODE_RANDOM,       /**< Playing all files in random order */
    DFPLAYER_MODE_NORMAL,       /**< Normal playback */
    DFPLAYER_MODE_NUMOF         /**< Number of supported playback modes */
} dfplayer_mode_t;

/**
 * @brief   Enumeration of the different sources for playback supported
 */
typedef enum {
    DFPLAYER_SOURCE_USB,        /**< Read files from USB storage */
    DFPLAYER_SOURCE_SDCARD,     /**< Read files from SD card */
    DFPLAYER_SOURCE_AUX,        /**< No idea, the data sheet never mentions AUX again */
    DFPLAYER_SOURCE_SLEEP,      /**< No idea, the data sheet is extremely vague on this */
    DFPLAYER_SOURCE_FLASH,      /**< Read files from NOR flash */
    DFPLAYER_SOURCE_NUMOF       /**< Number of supported playback modes */
} dfplayer_source_t;

/**
 * @brief   Enumeration of the detectable states of the DFPlayer
 */
typedef enum {
    DFPLAYER_STATE_PLAYING,     /**< Currently playing a file */
    DFPLAYER_STATE_PAUSED,      /**< Playback is paused, can be resumed */
    DFPLAYER_STATE_STOPPED,     /**< No file playing */
    DFPLAYER_STATE_NUMOF,       /**< Number of DFPlayer states supported by the driver */
} dfplayer_state_t;

/**
 * @brief   Enumeration of the different naming schemes
 */
typedef enum {
    DFPLAYER_SCHEME_FOLDER_FILE,/**< Naming scheme `<folder>/<file>` */
    DFPLAYER_SCHEME_MP3_FILE,   /**< Naming scheme `MP3/<number>` */
    DFPLAYER_SCHEME_NUMOF,      /**< Number of naming schemes supported */
} dfplayer_scheme_t;

/**
 * @brief   Set of DFPlayer playback sources
 */
typedef uint8_t dfplayer_source_set_t;

/**
 * @brief   A DFPlayer Mini device descriptor
 */
typedef struct dfplayer dfplayer_t;

/**
 * @brief   Signature of the function called when a playback of track completed
 *
 * @param   dev     Device descriptor of the DFPlayer triggering the event
 * @param   src     Source medium the track was played from
 * @param   track   The number of the track that was played
 *
 * @warning This function is called from interrupt context
 */
typedef void (*dfplayer_cb_done_t)(dfplayer_t *dev, dfplayer_source_t src,
                                   uint16_t track);

/**
 * @brief   Signature of the function called when a medium was inserted/ejected
 *
 * @param   dev     Device descriptor of the DFPlayer triggering the event
 * @param   srcs    Set of sources currently available for playback
 *
 * @warning This function is called from interrupt context
 */
typedef void (*dfplayer_cb_src_t)(dfplayer_t *dev, dfplayer_source_set_t srcs);

/**
 * @brief   Initialization parameters of a DFPlayer Mini device descriptor
 */
typedef struct {
    uart_t uart;                /**< UART interface connected to the DFPlayer */
    gpio_t busy_pin;            /**< GPIO connected to the DFPlayer's busy pin */
    uint8_t volume;             /**< Initial volume */
} dfplayer_params_t;

/**
 * @brief   Data structure representing a file on the DFPlayer
 *
 * If @ref dfplayer_file_t::scheme is DFPLAYER_SCHEME_MP3_FILE,
 * @ref dfplayer_file_t::number holds the number of the represented file.
 * If @ref dfplayer_file_t::scheme is DFPLAYER_SCHEME_FOLDER_FILE,
 * @ref dfplayer_file_t::folder holds the number of the folder containing
 * the represented file and @ref dfplayer_file_t::file the number of the file.
 *
 * E.g. file `32/123.mp3` would be represented as:
 *
 * ```C
 * dfplayer_file_t file = {
 *    .scheme = DFPLAYER_SCHEME_FOLDER_FILE,
 *    .folder = 32,
 *    .file = 123
 * }
 * ```
 *
 * and `MP3/0042.mp3` as:
 *
 * ```C
 * dfplayer_file_t file = {
 *    .scheme = DFPLAYER_SCHEME_MP3_FILE,
 *    .number = 42
 * }
 * ```
 */
typedef struct {
    union {
        uint16_t number;        /**< Number of the file (naming scheme "MP3/1337.mp3") */
        struct {
            uint8_t folder;     /**< Folder of the file (naming scheme "42/123.mp3") */
            uint8_t file;       /**< Name of the file (naming scheme "42/123.mp3") */
        };
    };
    dfplayer_scheme_t scheme;   /**< Used naming scheme */
} dfplayer_file_t;

/**
 * @brief   A DFPlayer Mini device descriptor
 */
struct dfplayer {
    dfplayer_cb_done_t cb_done; /**< Function to call when playing a track completed */
    dfplayer_cb_src_t cb_src;   /**< Function to call when set of available playback sources changes */
    uint32_t last_event_us;     /**< Time stamp of the last event in µs */
    uint8_t buf[6];             /**< Data buffer for response from DFPlayer */
    dfplayer_file_t file;       /**< Currently played song */
    uint8_t len;                /**< Length of the frame in the buffer */
    uint8_t flags;              /**< Flags storing info about the driver state*/
    mutex_t mutex;              /**< Used to mutual exclusive access */
    uart_t uart;                /**< UART interface connected to the DFPlayer */
    gpio_t busy_pin;            /**< GPIO connected to the DFPlayer's busy pin */
    mutex_t sync;               /**< Used to wait on ISR */
    dfplayer_source_set_t srcs; /**< Set of available playback sources */
    dfplayer_rx_state_t state;  /**< Current state of the DFPlayer */
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* DFPLAYER_TYPES_H */
