/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_adc
 * @{
 *
 * @file
 * @brief       Internal types used in the common ADC API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * Writing an ADC NG Backend
 * =========================
 *
 * Terminology
 * -----------
 *
 * - **Single Ended Input:** The ADC converts a single input voltage into
 *   an unsigned value in which zero refers to 0V and the highest value
 *   indicates that the input is equal to (or higher than) the reference
 *   voltage.
 * - **Differential Input:** The ADC use a positive and a negative input
 *   and converts the voltage difference between the two into a signed value:
 *   A value of zero indicates no voltage difference, a positive value that the
 *   positive inputs is greater than the negative, and a negative value that
 *   the negative inputs is greater than the positive. If the difference is
 *   equal to the reference voltage, the highest (or lowest) value is returned.
 *
 * Channel Mapping
 * ---------------
 *
 * Every ADC driver must at least one single ended input. All other channel
 * types are optional. The initialization code in @ref adc_ng_driver_t::init
 * must however be prepared to be called for every possible channel number and
 * must return `-ENXIO` for unsupported channels. The channel number of the
 * highest single ended channel must be given in the driver in
 * @ref adc_ng_driver_t::highest_single_ended_channel. All channel numbers
 * below this *must* also refer to single ended channels.
 *
 * Channels above the value given in
 * @ref adc_ng_driver_t::highest_single_ended_channel but below 224 are driver
 * specific and can be used to provide access to differential inputs, select
 * different gains, etc.
 *
 * Channels starting from 224 are reserved for driver independent special
 * channels. Those special channels currently are @ref ADC_NG_CHAN_FIXED_REF
 * (to select a fixed reference voltage as input to allow measuring the value
 * of other reference voltages), @ref ADC_NG_CHAN_NTC (that refers to an
 * internally connected integrated thermistor), and @ref ADC_NG_CHAN_ENTROPY
 * (to collect entropy).
 */

#ifndef ADC_NG_INTERNAL_H
#define ADC_NG_INTERNAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   This special channel must refer to an internal fixed reference
 *          voltage used as input
 *
 * ADCs not supporting this will return `-ERANGE` when this channel is
 * selected. If this is supported, it can be used to measure the correct value
 * of voltage references depending on supply voltages (including the MCUs
 * supply voltage, if selectable as reference voltage). This allows to
 * compensate for differences between nominal and actual voltage reference
 * during conversion to physical units.
 *
 * In case of boards running directly of a battery, measuring the supply
 * voltage is particularly useful to estimate the remaining battery charge.
 */
#define ADC_NG_CHAN_FIXED_REF           (UINT8_MAX)

/**
 * @brief   This special channel must refer to an internally connected
 *          thermistor
 */
#define ADC_NG_CHAN_NTC                 (UINT8_MAX - 1)

/**
 * @brief   This special channel must refer to a channel collecting entropy
 *
 * @note    When this channel is selected, a driver can (and likely should)
 *          ignore the requested resolution and reference voltage.
 *
 * When this channel is used, the @ref adc_ng_driver_t::entropy_bits least
 * significant of every sample obtained will contain a some (possibly weak)
 * entropy. The contents of the remaining bits are undefined.
 */
#define ADC_NG_CHAN_ENTROPY             (UINT8_MAX - 2)

/**
 * @brief   Use this value in @ref adc_ng_driver_t::ref_input_idx or
 *          @ref adc_ng_driver_t::ref_vcc_idx to indicate that this feature
 *          is not supported
 */
#define ADC_NG_NO_SUCH_REF              (UINT8_MAX)

/**
 * @brief   Description of an thermistor to use for temperature measurements
 */
typedef struct {
    /**
     * @brief   Contains the temperature coefficient of the NTC, or zero
     *
     * The coefficient is is given in 1/1024 mV per 0.1 °C. The resulting
     * temperature in 0.1 °C is calculated from the measured voltage using:
     *
     *    T[d°C] = (coefficient * (mV - offset)) / 1024
     */
    uint16_t coefficient;
    /**
     * @brief   The offset in mV to use for obtaining the temperature
     *
     * See @ref adc_ng_ntc_t::coefficient
     */
    uint16_t offset;
} adc_ng_ntc_t;

/**
 * @brief   Structure to capture the state of an ADC-NG backedn
 */
typedef struct {
    int16_t reference_mv;   /**< Currently selected reference voltage (in mV) */
    uint8_t resolution;     /**< Currently selected resolution (in bits) */

} adc_ng_state_t;

/**
 * @brief   Signature of @ref adc_ng_driver_t::init
 *
 * @param           handle  Handle of the ADC
 * @param[in]       chan    The ADC channel to initialize
 * @param[in]       res     The resolution to select
 * @param[in        ref     Index of the reference to use
 *                          (refers to @ref adc_ng_driver_t::refs)
 *
 * @retval  0               Success
 * @retval  -ENXIO          Invalid channel given in @p chan
 * @retval  -EALREADY       The ADC is already powered and configured
 * @retval  <0              Other error (see device driver doc)
 *
 * @post    When 0 is returned, the channel @p chan is ready take samples
 * @post    If @p res contains an unsupported resolution, an assert blows up
 * @post    If `-EALREADY` is returned, the ADC is state remains unchanged
 * @post    On other error codes the ADC is powered down
 *
 * @note    A call to @ref adc_ng_driver_t::off is needed to disable the
 *          channel again and preserve power
 */
typedef int (*adc_ng_init_t)(void *handle, uint8_t chan, uint8_t res,
                             uint8_t ref);

/**
 * @brief   Signature of @ref adc_ng_driver_t::mux
 *
 * @param           handle  Handle of the ADC
 * @param[in]       chan    The ADC channel to initialize
 *
 * @retval  0               Success
 * @retval  -ENXIO          Invalid channel given in @p chan
 * @retval  -ENOTSUP        Cannot switch to channel @p chan using the
 *                          current configuration of the ADC
 * @retval  -EIO            The ADC is not powered up and configured
 * @retval  <0              Other error (see device driver doc)
 *
 * @pre     The ADC is up and running (see @ref adc_ng_init). Otherwise
 *          `-EIO` will be returned
 * @post    Except for the ADC MUX, no ADC setting is changed by this
 * @post    On success, the ADC MUX is set to @p chan
 * @post    On failure, the ADC MUX setting is left unchanged
 */
typedef int (*adc_ng_mux_t)(void *handle, uint8_t chan);

/**
 * @brief   Signature of @ref adc_ng_driver_t::off
 *
 * @param           handle  Handle of the ADC
 */
typedef void (*adc_ng_off_t)(void *handle);

/**
 * @brief   Signature of @ref adc_ng_driver_t::single
 *
 * @param           handle  Handle of the ADC
 * @param[out]      dest    The sample will be stored here
 *
 * @return  The result of the conversion
 *
 * @pre     The ADC has been initialized, see @ref adc_ng_driver_t::init
 *
 * @post    If the ADC is not initialized, an assertion blows up
 *
 * @retval  0           Success
 * @retval  <0          Error (check device driver's doc for error codes)
 */
typedef int (*adc_ng_single_t)(void *handle, int32_t *dest);

/**
 * @brief   Internal driver interface
 */
typedef struct {
    /**
     * @brief   Initialize the given ADC channel and prepare it for sampling
     *
     * @note    A call to @ref adc_ng_driver_t::off is needed to disable the
     *          channel again and preserve power
     */
    adc_ng_init_t init;
    /**
     * @brief   Switch the ADC MUX to a different channel
     *
     * @note    This is only allowed while the ADC is up and running and to
     *          channels that are compatible with the current ADC settings
     */
    adc_ng_mux_t mux;
    /**
     * @brief   Disable the given ADC channel again and bring the ADC into a low
     *          power state
     *
     * @details If no power saving is implemented (or possible), this should be
     *          a `NULL` pointer.
     */
    adc_ng_off_t off;
    /**
     * @brief   Runs a single conversion and returns the sample
     */
    adc_ng_single_t single;
    /**
     * @brief   Bitmap containing the supported ADC resolutions
     *
     * If e.g. the resolutions 4bit, 6bit and 8bit are supported it should have
     * the value `BIT3 | BIT5 | BIT7`. Thus, currently the highest resolution
     * supported is 32 bit.
     */
    uint32_t res_supported;
    /**
     * @brief   The reference voltages supported in ascending order
     *
     * This list should be sorted in ascending order and terminated with a
     * value of zero. The reference voltage are a bitmask with the 14 least
     * significant bits containing the voltage value in mV, and the two
     * most significant bits indicate whether the reference voltage is
     * calibrated (@ref ADC_NG_REF_CALIBRATED) and if the MCUs supply voltage
     * is used as reference (@ref ADC_NG_REF_MCU_VCC).
     */
    const int16_t *refs;
    /**
     * @brief   Parameters of the internally connected thermistor or `NULL`
     */
    const adc_ng_ntc_t *ntc;
    /**
     * @brief   The index of the reference voltage that can be used as input
     *          using channel @ref ADC_NG_CHAN_FIXED_REF
     *
     * In case the fixed reference voltage can only be used as input, and not
     * set up as reference during conversion, add this reference to @ref
     * adc_ng_driver_t::refs after the last item (identified using value zero)
     * and use that index here. This will prevent that reference to be used
     * except as input.
     *
     * Use the special value @ref ADC_NG_NO_SUCH_REF if no fixed reference
     * voltage can be used as input.
     */
    uint8_t ref_input_idx;
    /**
     * @brief   The index of the VCC reference voltage
     *
     * If VCC can be chosen as reference, this should give its index in
     * @ref adc_ng_driver_t::refs. Otherwise it should have the value
     * @ref ADC_NG_NO_SUCH_REF.
     */
    uint8_t ref_vcc_idx;
    /**
     * @brief   The number of least significant bits containing entropy
     *
     * @note    This only refers to channel @ref ADC_NG_CHAN_ENTROPY
     *
     * A value of zero must be used when the ADC does not support harvesting
     * entropy. Note that even a source of weak entropy can be useful, when
     * fed into a entropy extractor. Often only taking only the least
     * significant bit when measuring a noisy source is a good choice
     * (so a value of `1`).
     */
    uint8_t entropy_bits;
    /**
     * @brief   The number of the highest single ended channel
     *
     * All channel with a number small than or equal to this value must be
     * regular single ended channels. All channel higher than this number but
     * lower than 224 are driver defined special channels (if any), e.g.
     * differential inputs. All channels equal to 224 and above are reserved
     * for driver-independent special channels like @ref ADC_NG_CHAN_FIXED_REF,
     * @ref ADC_NG_CHAN_NTC, or @ref ADC_NG_CHAN_ENTROPY.
     */
    uint8_t highest_single_ended_channel;
} adc_ng_driver_t;

/**
 * @brief   Type of the ADC-NG backend
 */
typedef struct {
    const adc_ng_driver_t *driver;  /**< Driver interface of the backend */
    void *handle;                   /**< Handle of the driver (e.g. device descriptor) */
} adc_ng_backend_t;

/**
 * @brief   Convert an ADC sample to a voltage level in mV
 *
 * @param[in]       adc         ADC that was used to take the sample
 * @param[in]       sample      The ADC sample to convert
 *
 * @return  The voltage level in mV
 *
 * @pre     The ADC identified by @p adc has not been re-initialized since
 *          taking the sample. (But it can be offline.)
 */
int16_t adc_ng_convert(uint8_t adc, int32_t sample);

/**
 * @brief   Measure the actual value of a reference voltage by selecting
 *          the constant voltage reference as input
 *
 * @param[in]       adc         ADC of which the reference voltage should be
 *                              measured
 * @param[in]       ref_idx     Index of the reference to measure
 * @param[out]      dest_mv     Measured value of the voltage reference in mV
 */
int adc_ng_measure_ref(uint8_t adc, uint8_t ref_idx, int16_t *dest_mv);

#ifdef __cplusplus
}
#endif

#endif /* ADC_NG_INTERNAL_H */
/** @} */
