/*
 * SPDX-FileCopyrightText: 2024 Martine S. Lenders
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once
/**
 * @defgroup    ble_skald_bthome Skald about BTHome
 * @ingroup     ble_skald
 * @brief       Skald's BTHome abstraction
 *
 * # About
 * This Skald module supports the creation and advertisement of BTHome messages
 * (see https://bthome.io)
 *
 * # Implementation state
 * - BTHome v2 is mostly supported
 * - [Encryption](https://bthome.io/encryption/) is not supported
 * - [Trigger based devices](https://bthome.io/format/#bthome-data-format) are not supported
 *
 *
 * @{
 *
 * @file
 * @brief   BTHome interface
 *
 * @author  Martine S. Lenders <mail@martine-lenders.eu>
 */

#include "byteorder.h"

#include "net/skald.h"

#include "net/skald/bthome/defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   BTHome advertising context holding the advertising data and state
 */
typedef struct {
    skald_ctx_t skald;          /**< Skald context */
    /**
     * @brief   Pointer to service data length field
     *
     * Will point to the service data length field within skald_bthome_ctx_t::skald after calling
     * @ref skald_bthome_init().
     */
    uint8_t *svc_data_len;
} skald_bthome_ctx_t;

/**
 * @brief   Initialize the next BTHome advertisement
 *
 * @pre `ctx != NULL`
 *
 * @param[out] ctx              The BTHome context
 * @param[in] shortened_name    The shortened name for the BTHome advertisement. May be NULL, if no
 *                              shortened name should be advertised. Must end with '\0' and may not
 *                              make the length of the PDU exceed @ref NETDEV_BLE_PDU_MAXLEN
 * @param[in] complete_name     The complete name for the BTHome advertisement. May be NULL, if no
 *                              complete name should be advertised. Must end with '\0' and may not
 *                              make the length of the PDU exceed @ref NETDEV_BLE_PDU_MAXLEN
 * @param[in] dev_info          Flags for the device information. Currently, no flags are supported.
 *
 * @return The current size of the advertisement on success or an error code on failure
 * @retval -EMSGSIZE when @p shortened_name or @p complete_name cause the message to exceed
 *         @ref NETDEV_BLE_PDU_MAXLEN. With NDEBUG=0 an assertion is raised in this case.
 */
int skald_bthome_init(
    skald_bthome_ctx_t *ctx,
    const char *shortened_name, const char *complete_name,
    uint8_t dev_info
);

/**
 * @brief   Adds a measurement to the next BTHome advertisement
 * @warning This does not check if the @p obj_id was inserted in the right order (they have to be
 *          added in numerical order from low to high) or if @p data_len is the correct data length
 *          for @p obj_id. It just checks if @p data_len fits within @p NETDEV_BLE_PDU_MAXLEN of the
 *          PDU.
 *
 * @pre `ctx != NULL` and `ctx` was initialized with @ref skald_bthome_init().
 * @pre @p data fits within the advertisement in @p ctx.
 *
 * @param[in,out] ctx       The BTHome context. Must not be NULL and must be initialized with
 *                          @ref skald_bthome_init().
 * @param[in] obj_id        An object ID (see @ref skald_bthome_id_t).
 * @param[in] data          The measurement data. Must not be NULL.
 * @param[in] data_len      The length of @p data.
 *
 * @return The current size of the advertisement on success or an error code on failure
 * @retval -EMSGSIZE when @p data_len cause the message to exceed @ref NETDEV_BLE_PDU_MAXLEN.
 *         With NDEBUG=0 an assertion is raised in this case.
 */
int skald_bthome_add_measurement(
    skald_bthome_ctx_t *ctx,
    skald_bthome_id_t obj_id,
    const void *data,
    uint8_t data_len
);

/**
 * @brief   Adds a one byte unsigned measurement to the next BTHome advertisement
 * @warning This does not check if the @p obj_id was inserted in the right order (they have to be
 *          added in numerical order from low to high) or if @p data_len is the correct data length
 *          for @p obj_id. It just checks if @p data_len fits within @p NETDEV_BLE_PDU_MAXLEN of the
 *          PDU.
 *
 * @pre `ctx != NULL` and `ctx` was initialized with @ref skald_bthome_init().
 * @pre @p data fits within the advertisement in @p ctx.
 *
 * @param[in,out] ctx       The BTHome context. Must not be NULL and must be initialized with
 *                          @ref skald_bthome_init().
 * @param[in] obj_id        An object ID (see @ref skald_bthome_id_t).
 * @param[in] data          The measurement data.
 *
 * @return The current size of the advertisement on success or an error code on failure
 * @retval -EMSGSIZE when @p data cause the message to exceed @ref NETDEV_BLE_PDU_MAXLEN.
 *         With NDEBUG=0 an assertion is raised in this case.
 */
static inline int skald_bthome_add_uint8_measurement(
    skald_bthome_ctx_t *ctx,
    skald_bthome_id_t obj_id,
    uint8_t data
)
{
    return skald_bthome_add_measurement(ctx, obj_id, &data, sizeof(data));
}

/**
 * @brief   Adds a one byte signed measurement to the next BTHome advertisement
 * @warning This does not check if the @p obj_id was inserted in the right order (they have to be
 *          added in numerical order from low to high) or if @p data_len is the correct data length
 *          for @p obj_id. It just checks if @p data_len fits within @p NETDEV_BLE_PDU_MAXLEN of the
 *          PDU.
 *
 * @pre `ctx != NULL` and `ctx` was initialized with @ref skald_bthome_init().
 * @pre @p data fits within the advertisement in @p ctx.
 *
 * @param[in,out] ctx       The BTHome context. Must not be NULL and must be initialized with
 *                          @ref skald_bthome_init().
 * @param[in] obj_id        An object ID (see @ref skald_bthome_id_t).
 * @param[in] data          The measurement data.
 *
 * @return The current size of the advertisement on success or an error code on failure
 * @retval -EMSGSIZE when @p data cause the message to exceed @ref NETDEV_BLE_PDU_MAXLEN.
 *         With NDEBUG=0 an assertion is raised in this case.
 */
static inline int skald_bthome_add_int8_measurement(
    skald_bthome_ctx_t *ctx,
    skald_bthome_id_t obj_id,
    int8_t data
)
{
    return skald_bthome_add_measurement(ctx, obj_id, &data, sizeof(data));
}

/**
 * @brief   Adds a two byte unsigned measurement to the next BTHome advertisement
 * @warning This does not check if the @p obj_id was inserted in the right order (they have to be
 *          added in numerical order from low to high) or if @p data_len is the correct data length
 *          for @p obj_id. It just checks if @p data_len fits within @p NETDEV_BLE_PDU_MAXLEN of the
 *          PDU.
 *
 * @pre `ctx != NULL` and `ctx` was initialized with @ref skald_bthome_init().
 * @pre @p data fits within the advertisement in @p ctx.
 *
 * @param[in,out] ctx       The BTHome context. Must not be NULL and must be initialized with
 *                          @ref skald_bthome_init().
 * @param[in] obj_id        An object ID (see @ref skald_bthome_id_t).
 * @param[in] data          The measurement data.
 *
 * @return The current size of the advertisement on success or an error code on failure
 * @retval -EMSGSIZE when @p data cause the message to exceed @ref NETDEV_BLE_PDU_MAXLEN.
 *         With NDEBUG=0 an assertion is raised in this case.
 */
static inline int skald_bthome_add_uint16_measurement(
    skald_bthome_ctx_t *ctx,
    skald_bthome_id_t obj_id,
    uint16_t data
)
{
    uint16_t le_data = htole16(data);
    return skald_bthome_add_measurement(ctx, obj_id, &le_data, sizeof(data));
}

/**
 * @brief   Adds a two byte signed measurement to the next BTHome advertisement
 * @warning This does not check if the @p obj_id was inserted in the right order (they have to be
 *          added in numerical order from low to high) or if @p data_len is the correct data length
 *          for @p obj_id. It just checks if @p data_len fits within @p NETDEV_BLE_PDU_MAXLEN of the
 *          PDU.
 *
 * @pre `ctx != NULL` and `ctx` was initialized with @ref skald_bthome_init().
 * @pre @p data fits within the advertisement in @p ctx.
 *
 * @param[in,out] ctx       The BTHome context. Must not be NULL and must be initialized with
 *                          @ref skald_bthome_init().
 * @param[in] obj_id        An object ID (see @ref skald_bthome_id_t).
 * @param[in] data          The measurement data.
 *
 * @return The current size of the advertisement on success or an error code on failure
 * @retval -EMSGSIZE when @p data cause the message to exceed @ref NETDEV_BLE_PDU_MAXLEN.
 *         With NDEBUG=0 an assertion is raised in this case.
 */
static inline int skald_bthome_add_int16_measurement(
    skald_bthome_ctx_t *ctx,
    skald_bthome_id_t obj_id,
    int16_t data
)
{
    uint16_t le_data = htole16((uint16_t)data);
    return skald_bthome_add_measurement(ctx, obj_id, &le_data, sizeof(data));
}

/**
 * @brief   Adds a three byte unsigned measurement to the next BTHome advertisement
 * @warning This does not check if the @p obj_id was inserted in the right order (they have to be
 *          added in numerical order from low to high) or if @p data_len is the correct data length
 *          for @p obj_id. It just checks if @p data_len fits within @p NETDEV_BLE_PDU_MAXLEN of the
 *          PDU.
 *
 * @pre `ctx != NULL` and `ctx` was initialized with @ref skald_bthome_init().
 * @pre @p data fits within the advertisement in @p ctx.
 *
 * @param[in,out] ctx       The BTHome context. Must not be NULL and must be initialized with
 *                          @ref skald_bthome_init().
 * @param[in] obj_id        An object ID (see @ref skald_bthome_id_t).
 * @param[in] data          The measurement data.
 *
 * @return The current size of the advertisement on success or an error code on failure
 * @retval -EMSGSIZE when @p data cause the message to exceed @ref NETDEV_BLE_PDU_MAXLEN.
 *         With NDEBUG=0 an assertion is raised in this case.
 */
static inline int skald_bthome_add_uint24_measurement(
    skald_bthome_ctx_t *ctx,
    skald_bthome_id_t obj_id,
    uint32_t data
)
{
    uint32_t le_data = htole32(data);
    return skald_bthome_add_measurement(ctx, obj_id, &le_data, 3U);
}

/**
 * @brief   Adds a four byte unsigned measurement to the next BTHome advertisement
 * @warning This does not check if the @p obj_id was inserted in the right order (they have to be
 *          added in numerical order from low to high) or if @p data_len is the correct data length
 *          for @p obj_id. It just checks if @p data_len fits within @p NETDEV_BLE_PDU_MAXLEN of the
 *          PDU.
 *
 * @pre `ctx != NULL` and `ctx` was initialized with @ref skald_bthome_init().
 * @pre @p data fits within the advertisement in @p ctx.
 *
 * @param[in,out] ctx       The BTHome context. Must not be NULL and must be initialized with
 *                          @ref skald_bthome_init().
 * @param[in] obj_id        An object ID (see @ref skald_bthome_id_t).
 * @param[in] data          The measurement data.
 *
 * @return The current size of the advertisement on success or an error code on failure
 * @retval -EMSGSIZE when @p data cause the message to exceed @ref NETDEV_BLE_PDU_MAXLEN.
 *         With NDEBUG=0 an assertion is raised in this case.
 */
static inline int skald_bthome_add_uint32_measurement(
    skald_bthome_ctx_t *ctx,
    skald_bthome_id_t obj_id,
    uint32_t data
)
{
    uint32_t le_data = htole32(data);
    return skald_bthome_add_measurement(ctx, obj_id, &le_data, sizeof(data));
}

/**
 * @brief   Adds a four byte signed measurement to the next BTHome advertisement
 * @warning This does not check if the @p obj_id was inserted in the right order (they have to be
 *          added in numerical order from low to high) or if @p data_len is the correct data length
 *          for @p obj_id. It just checks if @p data_len fits within @p NETDEV_BLE_PDU_MAXLEN of the
 *          PDU.
 *
 * @pre `ctx != NULL` and `ctx` was initialized with @ref skald_bthome_init().
 * @pre @p data fits within the advertisement in @p ctx.
 *
 * @param[in,out] ctx       The BTHome context. Must not be NULL and must be initialized with
 *                          @ref skald_bthome_init().
 * @param[in] obj_id        An object ID (see @ref skald_bthome_id_t).
 * @param[in] data          The measurement data.
 *
 * @return The current size of the advertisement on success or an error code on failure
 * @retval -EMSGSIZE when @p data cause the message to exceed @ref NETDEV_BLE_PDU_MAXLEN.
 *         With NDEBUG=0 an assertion is raised in this case.
 */
static inline int skald_bthome_add_int32_measurement(
    skald_bthome_ctx_t *ctx,
    skald_bthome_id_t obj_id,
    int32_t data
)
{
    uint32_t le_data = htole32((uint32_t)data);
    return skald_bthome_add_measurement(ctx, obj_id, &le_data, sizeof(data));
}

/**
 * @brief   Starts periodically advertising the BTHome advertisement
 *
 * @param[in] ctx           The BTHome context. Must not be NULL and must be initialized with
 *                          @ref skald_bthome_init().
 * @param[in] adv_itvl_ms   advertising interval in milliseconds
 */
void skald_bthome_advertise(skald_bthome_ctx_t *ctx, uint32_t adv_itvl_ms);

#ifdef __cplusplus
}
#endif

/** @} */
