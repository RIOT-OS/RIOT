#ifndef AZ_RIOT_PNP_IFACE_H
#define AZ_RIOT_PNP_IFACE_H

#include <az_riot_config.h>

/**
 * @file az_riot_pnp_iface.h
 * @brief Azure IoT PnP Interface Header File
 */

/**
 * @brief Structure representing the Azure Riot Context
 */
struct AzRiotContext;
typedef struct AzRiotContext AzRiotContext;

/**
 * @brief Enumeration defining Azure Riot Client Types
 */
typedef enum {
    DPS = 0,    ///< Device Provisioning Service
    HUB         ///< IoT Hub
} azRiotClientType;

/**
 * @brief Creates an instance of AzRiotContext
 *
 * @param mqtts_writebuf Buffer for MQTT write operations
 * @param mqtts_writebuf_len Length of the MQTT write buffer
 * @param mqtts_readbuf Buffer for MQTT read operations
 * @param mqtts_readbuf_len Length of the MQTT read buffer
 * @return AzRiotContext instance
 */
AzRiotContext *create_AzRiotContext(unsigned char *mqtts_writebuf, int mqtts_writebuf_len,
                                    unsigned char *mqtts_readbuf, int mqtts_readbuf_len);

/**
 * @brief Initializes DPS client
 *
 * @param azContext Azure Riot Context
 * @return 0 on success, negative value on failure
 */
int init_dps_client(AzRiotContext *azContext);

/**
 * @brief Initializes IoT Hub client
 *
 * @param azContext Azure Riot Context
 * @param hostAddress IoT Hub host address
 * @param deviceId ID of the IoT device
 * @return 0 on success, negative value on failure
 */
int init_iot_hub_client(AzRiotContext *azContext,  char *hostAddress,  char *deviceId);

/**
 * @brief Sends telemetry message to IoT Hub
 *
 * @param azContext Azure Riot Context
 * @param telemetry_message_payload Telemetry message payload
 * @return 0 on success, negative value on failure
 */
int send_telemetry_message_to_iot_hub(AzRiotContext *azContext,
                                      unsigned char *telemetry_message_payload);

/**
 * @brief Registers device with provisioning service
 *
 * @param azContext Azure Riot Context
 * @return 0 on success, negative value on failure
 */
int register_device_with_provisioning_service(AzRiotContext *azContext);

/**
 * @brief Connects Azure client (either DPS or Hub)
 *
 * @param azContext Azure Riot Context
 * @param clientType Type of Azure client (DPS or Hub)
 * @return 0 on success, negative value on failure
 */
int connect_azure_client(AzRiotContext *azContext, azRiotClientType clientType);

/**
 * @brief Subscribes to Azure client topics (either DPS or Hub)
 *
 * @param azContext Azure Riot Context
 * @param clientType Type of Azure client (DPS or Hub)
 * @return 0 on success, negative value on failure
 */
int subscribe_to_azure_client_topics(AzRiotContext *azContext, azRiotClientType clientType);

/**
 * @brief Disconnects Azure client
 *
 * @param azContext Azure Riot Context
 * @return 0 on success, negative value on failure
 */
int disconnect_azure_client(AzRiotContext *azContext);

/**
 * @brief Sends operation query message to DPS
 *
 * @param context Azure Riot Context
 * @param operation_id Operation ID
 * @return 0 on success, negative value on failure
 */
int send_operation_query_message_to_dps(AzRiotContext *context, char *operation_id);

#endif
