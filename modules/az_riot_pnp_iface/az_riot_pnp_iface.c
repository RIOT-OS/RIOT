
#include "az_riot_pnp_iface.h"
#include <stdio.h>
#include "mqtts_riot_iface.h"

#include <azure/az_core.h>
#include <azure/az_iot.h>
#include <azure/core/az_span.h>

#ifndef DEFAULT_CERT_DIR_CA
#define DEFAULT_CERT_DIR_CA NULL
#endif
#ifndef DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN
#define DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN NULL
#endif
#ifndef DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY
#define DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY NULL
#endif

#define MQTTS_DEFAULT_KEEP_ALIVE_INTERVAL_SEC AZ_IOT_DEFAULT_MQTT_CONNECT_KEEPALIVE_SECONDS
#define AzureQOS 1 // Azure supports maximum of QOS 1

#define QUERY_TOPIC_BUFFER_LENGTH 256
#define REGISTER_TOPIC_BUFFER_LENGTH 128
#define PROVISIONING_ENDPOINT_BUFFER_LENGTH 256
#define MQTT_PAYLOAD_BUFFER_LENGTH 256

struct AzRiotContext {
    MQTTSContext *mqtts_context;
    az_iot_hub_client *iot_hub_client;
    az_iot_provisioning_client *dps_client;
};

static AzRiotContext azureContext;
static az_iot_hub_client iot_hub_client;
static az_iot_provisioning_client dps_client;

void default_test_precondition_failed_callback(void)
{
    // This is the default callback implementation for precondition failed errors
    // You can customize the behavior here, such as logging an error message
    printf("Precondition failed error occurred\n");
}

int handle_provisioning_message(char *topic, int topic_len, char *payload, int payload_len);
int handle_iot_message(char *topic, int topic_len, char *payload, int payload_len);

static void _on_azure_msg_arival(char *topic, int topic_len, char *payload, int payload_len)
{
    int ret = -1;

    if (azureContext.dps_client != NULL) {
        ret = handle_provisioning_message(topic, topic_len, payload, payload_len);
    }
    if (ret < 0 && azureContext.iot_hub_client != NULL) { // if not dps then iot hub
        handle_iot_message(topic, topic_len, payload, payload_len);
    }
}

int init_mqtts_layer(AzRiotContext *azContext, unsigned char *mqtts_writebuf,
                     int mqtts_writebuf_len, unsigned char *mqtts_readbuf, int mqtts_readbuf_len)
{
    azContext->mqtts_context = mqtts_create_context(NULL);

    int ret = -1;

    if (azContext->mqtts_context == NULL) {
        printf("[Azure]: mqtts context failed\n");
        return ret;
    }
    // azContext->mqtts_context->keepAliveInterval = AZ_IOT_DEFAULT_MQTT_CONNECT_KEEPALIVE_SECONDS;
    mqtts_set_message_arrival_callback(_on_azure_msg_arival);
    char *ca_cert_path = DEFAULT_CERT_DIR_CA;
    char *cert_chain_path = DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN;
    char *private_key_path = DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY;

    if ((ret =
             mqtts_set_certificate(azContext->mqtts_context, ca_cert_path, cert_chain_path,
                                   private_key_path)) < 0) {
        printf("[Azure]: mqtts_set_certificate failed\n");
        return ret;
    }
    ret = mqtts_init(azContext->mqtts_context, mqtts_writebuf, mqtts_writebuf_len, mqtts_readbuf,
                     mqtts_readbuf_len);
    if (ret < 0) {
        printf("[Azure]: mqtts init failed\n");
        return ret;
    }
    return 0;
}

AzRiotContext *create_AzRiotContext(unsigned char *mqtts_writebuf, int mqtts_writebuf_len,
                                    unsigned char *mqtts_readbuf, int mqtts_readbuf_len)
{

    az_precondition_failed_set_callback(default_test_precondition_failed_callback);

    int res = init_mqtts_layer(&azureContext, mqtts_writebuf, mqtts_writebuf_len, mqtts_readbuf,
                               mqtts_readbuf_len);

    if (res < 0) {
        printf("[Azure]: Failed init_mqtts_layer\n");
        return NULL;
    }
    return &azureContext;
}

int init_iot_hub_client(AzRiotContext *azContext, char *hostName, char *deviceId)
{
    if (azContext->iot_hub_client != NULL) {
        printf("[Azure]: iot_hub_client is already initialized\n");
        return 0;
    }
    azContext->iot_hub_client = &iot_hub_client;
    az_iot_hub_client_options azureOpt = az_iot_hub_client_options_default();
    az_span iotHubHostName = az_span_create((uint8_t *)hostName, strlen(hostName));
    az_span iotHubDeviceId = az_span_create((uint8_t *)deviceId, strlen(deviceId));

    if (az_result_failed(az_iot_hub_client_init(azContext->iot_hub_client, iotHubHostName,
                                                iotHubDeviceId, &azureOpt))) {
        printf("[Azure]: az_iot_hub_client_init ERROR.\n");
        return -1;
    }
    printf("[Azure]: az_iot_hub_client_init OK!\n");
    return 0;
}

int init_dps_client(AzRiotContext *az_context)
{
    if (az_context->dps_client != NULL) {
        printf("[Azure]: az dps client is already initialized\n");
        return 0;
    }
    az_context->dps_client = &dps_client;

    az_iot_provisioning_client_options azureOpt = az_iot_provisioning_client_options_default();

    az_span dpsGloablHostName = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_DPS_GLOBAL_HOST);
    az_span dpsRegistrationID = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_DPS_REGISTRATION_ID);
    az_span dpsIdScope = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_DPS_IDSCOPE);

    az_result azureRes = az_iot_provisioning_client_init(az_context->dps_client, dpsGloablHostName,
                                                         dpsIdScope, dpsRegistrationID, &azureOpt);

    if (az_result_failed(azureRes)) {
        printf("[Azure]: az_iot_dps_client_init ERROR!\n");
        return -1;
    }
    printf("[Azure]: az_iot_dps_client_init OK.\n");
    return 0;
}

int connect_dps_client(AzRiotContext *azContext)
{
    if ((azContext == NULL) || (azContext->dps_client == NULL)) {
        printf("[Azure]: Invalid azure context");
        return -1;
    }

    char client_id_buf[PROVISIONING_ENDPOINT_BUFFER_LENGTH];

    if (az_result_failed(az_iot_provisioning_client_get_client_id(azContext->dps_client,
                                                                  client_id_buf,
                                                                  PROVISIONING_ENDPOINT_BUFFER_LENGTH,
                                                                  NULL))) {
        printf("[Azure]: az_iot_provisioning_client_get_client_id failed\n");
        return -1;
    }

    char user_name_buf[PROVISIONING_ENDPOINT_BUFFER_LENGTH];

    if (az_result_failed(az_iot_provisioning_client_get_user_name(azContext->dps_client,
                                                                  user_name_buf,
                                                                  PROVISIONING_ENDPOINT_BUFFER_LENGTH,
                                                                  NULL))) {
        printf("[Azure]: az_iot_provisioning_client_get_user_name failed\n");
        return -1;
    }

    int res = mqtts_connect(azContext->mqtts_context, AZ_RIOT_DPS_GLOBAL_HOST, client_id_buf,
                            user_name_buf, NULL);

    if (res != 0) {
        printf("[Azure]: mqtts_connect failed to connect to DPS client with error code %d\n", res);
    }
    return res;
}

int connect_iot_hub_client(AzRiotContext *azContext)
{
    if ((azContext == NULL) || (azContext->iot_hub_client == NULL)) {
        printf("[Azure]: Invalid azure context");
        return -1;
    }
    char client_id_buf[PROVISIONING_ENDPOINT_BUFFER_LENGTH];

    if (az_result_failed(az_iot_hub_client_get_client_id(azContext->iot_hub_client, client_id_buf,
                                                         PROVISIONING_ENDPOINT_BUFFER_LENGTH,
                                                         NULL))) {
        printf("[Azure]: az_iot_hub_client_get_client_id failed\n");
        return -1;
    }
    char user_name_buf[PROVISIONING_ENDPOINT_BUFFER_LENGTH];

    if (az_result_failed(az_iot_hub_client_get_user_name(azContext->iot_hub_client, user_name_buf,
                                                         PROVISIONING_ENDPOINT_BUFFER_LENGTH,
                                                         NULL))) {
        printf("[Azure]: az_iot_hub_client_get_user_name failed\n");
        return -1;
    }

    int res = mqtts_connect(azContext->mqtts_context, AZ_RIOT_HUB_HOST, client_id_buf,
                            user_name_buf, NULL);

    if (res != 0) {
        printf("[Azure]: mqtts_connect failed to connect to IoT Hub client with error code %d\n",
               res);
    }
    return res;
}

int connect_azure_client(AzRiotContext *azContext, azRiotClientType clientType)
{
    switch (clientType) {
    case HUB:
        return connect_iot_hub_client(azContext);
    case DPS:
        return connect_dps_client(azContext);
    default:
        break;
    }
    return -1;
}

int register_device_with_provisioning_service(AzRiotContext *azContext)
{
    char register_topic_buffer[REGISTER_TOPIC_BUFFER_LENGTH];
    int rc = az_iot_provisioning_client_register_get_publish_topic(
        azContext->dps_client, register_topic_buffer, sizeof(register_topic_buffer), NULL);

    if (az_result_failed(rc)) {
        printf("[Azure]: Failed to get the Register topic: az_result return code 0x%08x.\n", rc);
        return -1;
    }
    // Devices registering a ModelId while using Device Provisioning Service must specify
    // their ModelId in an MQTT payload sent during registration.

    char mqtt_payload[MQTT_PAYLOAD_BUFFER_LENGTH];

    snprintf(mqtt_payload, MQTT_PAYLOAD_BUFFER_LENGTH, "{\"registrationId\":\"%s\"}",
             AZ_RIOT_DPS_REGISTRATION_ID);
    az_iot_provisioning_client_payload_options options =
        az_iot_provisioning_client_payload_options_default();

    printf("**[Azure]: Trying device registration with Payload=> %.*s\n", sizeof(mqtt_payload),
           mqtt_payload);
    return mqtts_publish(azContext->mqtts_context, register_topic_buffer,
                         (unsigned char *)mqtt_payload, AzureQOS, 0);
}

int subscribe_mqtt_client_to_provisioning_service_topics(AzRiotContext *azContext)
{
    int ret = mqtts_subscribe(azContext->mqtts_context,
                              AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC, AzureQOS);

    if (ret < 0) {
        printf("dps_example: Unable to subscribe to %s (%d)\n",
               AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC, ret);
    }
    else {
        printf("dps_example: Now subscribed to %s, QOS %d\n",
               AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC, AzureQOS);
    }
    return ret;
}

// subscribe_mqtt_client_to_iot_hub_topics subscribes to well-known MQTT topics that Azure IoT Hub
// uses to signal incoming commands to the device and notify device of properties.
int subscribe_mqtt_client_to_iot_hub_topics(AzRiotContext *azContext)
{
    // Subscribe to incoming commands
    int ret = mqtts_subscribe(azContext->mqtts_context, AZ_IOT_HUB_CLIENT_COMMANDS_SUBSCRIBE_TOPIC,
                              AzureQOS);

    if (ret < 0) {
        printf("hub_example: Unable to subscribe to %s (%d)\n",
               AZ_IOT_HUB_CLIENT_COMMANDS_SUBSCRIBE_TOPIC, ret);
        return ret;
    }
    else {
        printf("hub_example: Now subscribed to %s, QOS %d\n",
               AZ_IOT_HUB_CLIENT_COMMANDS_SUBSCRIBE_TOPIC, AzureQOS);
    }

    // Subscribe to property update notifications.  Messages will be sent to this topic when
    // writable properties are updated by the service.
    ret = mqtts_subscribe(azContext->mqtts_context,
                          AZ_IOT_HUB_CLIENT_PROPERTIES_WRITABLE_UPDATES_SUBSCRIBE_TOPIC, AzureQOS);
    if (ret < 0) {
        printf("hub_example: Unable to subscribe to %s (%d)\n",
               AZ_IOT_HUB_CLIENT_PROPERTIES_WRITABLE_UPDATES_SUBSCRIBE_TOPIC, ret);
        return ret;
    }
    else {
        printf("hub_example: Now subscribed to %s, QOS %d\n",
               AZ_IOT_HUB_CLIENT_PROPERTIES_WRITABLE_UPDATES_SUBSCRIBE_TOPIC, AzureQOS);
    }

    // Subscribe to the properties message topic.  When the device invokes a PUBLISH to get
    // all properties (both reported from device and reported - see request_all_properties() below)
    // the property payload will be sent to this topic.
    ret = mqtts_subscribe(azContext->mqtts_context,
                          AZ_IOT_HUB_CLIENT_PROPERTIES_MESSAGE_SUBSCRIBE_TOPIC, AzureQOS);
    if (ret < 0) {
        printf("hub_example: Unable to subscribe to %s (%d)\n",
               AZ_IOT_HUB_CLIENT_PROPERTIES_MESSAGE_SUBSCRIBE_TOPIC, ret);
        return ret;
    }
    else {
        printf("hub_example: Now subscribed to %s, QOS %d\n",
               AZ_IOT_HUB_CLIENT_PROPERTIES_MESSAGE_SUBSCRIBE_TOPIC, AzureQOS);
    }
    // Messages received on the C2D topic will be cloud-to-device messages.
    ret =
        mqtts_subscribe(azContext->mqtts_context, AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC, AzureQOS);
    if (ret < 0) {
        printf("hub_example: Unable to subscribe to %s (%d)\n",
               AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC, ret);
        return ret;
    }
    else {
        printf("hub_example: Now subscribed to %s, QOS %d\n",
               AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC, AzureQOS);
    }
    return ret;
}

int subscribe_to_azure_client_topics(AzRiotContext *azContext, azRiotClientType clientType)
{
    switch (clientType) {
    case HUB:
        return subscribe_mqtt_client_to_iot_hub_topics(azContext);
    case DPS:
        return subscribe_mqtt_client_to_provisioning_service_topics(azContext);
    default:
        return -1;
    }
}

int send_telemetry_message_to_iot_hub(AzRiotContext *azContext,
                                      unsigned char *telemetry_message_payload)
{
    // Get the Telemetry topic to publish the telemetry messages.
    char telemetry_topic_buffer[128];
    int rc;

    rc = az_iot_hub_client_telemetry_get_publish_topic(
        azContext->iot_hub_client, NULL, telemetry_topic_buffer, sizeof(telemetry_topic_buffer),
        NULL);
    if (az_result_failed(rc)) {
        printf("[Azure]: Failed to get the Telemetry topic: az_result return code 0x%08x.\n", rc);
        return -1;
    }
    return mqtts_publish(azContext->mqtts_context, telemetry_topic_buffer,
                         telemetry_message_payload, AzureQOS, 0);
}

// send_operation_query_message PUBLISHes a message to query
// the status of the registration request.
int send_operation_query_message_to_dps(AzRiotContext *context, char *operation_id)
{
    az_span operation_id_span = az_span_create((uint8_t *)operation_id, strlen(operation_id));

    char query_topic_buffer[QUERY_TOPIC_BUFFER_LENGTH];

    if (az_result_failed(az_iot_provisioning_client_query_status_get_publish_topic(context->
                                                                                   dps_client,
                                                                                   operation_id_span,
                                                                                   query_topic_buffer,
                                                                                   sizeof(
                                                                                       query_topic_buffer),
                                                                                   NULL))) {
        printf("[Azure]: send_operation_query_message_to_dps failed\n");
        return -1;
    }
    // IMPORTANT: Wait the recommended retry-after number of seconds before query.
    return mqtts_publish(context->mqtts_context, query_topic_buffer, (unsigned char *)"", 0, 0);
}
int handle_provisioning_message(char *topic, int topic_len, char *payload, int payload_len)
{
    az_iot_provisioning_client_register_response register_response;
    // Initialize the incoming topic to a span
    az_span const topic_span = az_span_create((uint8_t *)topic, topic_len);
    az_span const message_span = az_span_create((uint8_t *)payload, payload_len);

    if (az_result_failed(az_iot_provisioning_client_parse_received_topic_and_payload(azureContext.
                                                                                     dps_client,
                                                                                     topic_span,
                                                                                     message_span,
                                                                                     &
                                                                                     register_response)))
    {
        return -1;
    }
    bool ref_is_operation_complete = az_iot_provisioning_client_operation_complete(
        register_response.operation_status);

    // If operation is not complete, send query. On return, will loop to receive new operation
    // message.
    if (!ref_is_operation_complete) {
        printf(
            "*** Device registration operation is still pending. Please query again after %d seconds with operation ID = %.*s\n", register_response.retry_after_seconds, register_response.operation_id._internal.size,
            register_response.operation_id._internal.ptr);
    }
    else {                                                                                  // Operation is complete.
        if (register_response.operation_status == AZ_IOT_PROVISIONING_STATUS_ASSIGNED) {    // Successful assignment
            char success_string[512];
            snprintf(success_string, 512,
                     "Device provisioned successfully:\n"
                     "Hub Hostname: %.*s\n"
                     "Device Id: %.*s\n",
                     (int)register_response.registration_state.assigned_hub_hostname._internal.size,
                     register_response.registration_state.assigned_hub_hostname._internal.ptr,
                     (int)register_response.registration_state.device_id._internal.size,
                     register_response.registration_state.device_id._internal.ptr);
            printf("[Registration Success] DPS response = %s\n", success_string);
        }
        else { // Unsuccessful assignment (unassigned, failed or disabled states)
            char error_string[256];

            snprintf(error_string, 256,
                     "Device provisioning failed:\n"
                     "Registration state: %d\n"
                     "Last operation status: %d\n"
                     "Operation ID: %.*s\n"
                     "Error code: %u\n"
                     "Error message: %.*s\n"
                     "Error timestamp: %.*s\n"
                     "Error tracking ID: %.*s\n",
                     register_response.operation_status,
                     register_response.status,
                     (int)register_response.operation_id._internal.size,
                     register_response.operation_id._internal.ptr,
                     register_response.registration_state.extended_error_code,
                     (int)register_response.registration_state.error_message._internal.size,
                     register_response.registration_state.error_message._internal.ptr,
                     (int)register_response.registration_state.error_timestamp._internal.size,
                     register_response.registration_state.error_timestamp._internal.ptr,
                     (int)register_response.registration_state.error_tracking_id._internal.size,
                     register_response.registration_state.error_tracking_id._internal.ptr);
            printf("[Registration ERROR] DPS error = %s\n", error_string);
            return -1;
        }
    }
    return 0;
}

static char *get_device_twin_message_type_as_string(
    az_iot_hub_client_twin_response const *twin_response);

int handle_iot_message(char *topic, int topic_len, char *payload, int payload_len)
{
    // Initialize the incoming topic to a span
    az_span incoming_topic = az_span_create((uint8_t *)topic, topic_len);

    // The message could be for three features so parse the topic to see which it is for
    az_iot_hub_client_method_request method_request;
    az_iot_hub_client_c2d_request c2d_request;
    az_iot_hub_client_twin_response twin_response;

    if (az_result_succeeded(az_iot_hub_client_methods_parse_received_topic(azureContext.
                                                                           iot_hub_client,
                                                                           incoming_topic,
                                                                           &method_request))) {
        // Handle the method request
        printf(
            "\n====================== [Direct-Method] (Only receiving is supported)============\n");
        printf("[Method Name] : %.*s\n", method_request.name._internal.size,
               method_request.name._internal.ptr);
        printf("[Payload] : %.*s\n", payload_len, payload);
        printf("=========================================\n");
    }
    else if (az_result_succeeded(az_iot_hub_client_c2d_parse_received_topic(azureContext.
                                                                            iot_hub_client,
                                                                            incoming_topic,
                                                                            &c2d_request))) {
        // Handle the c2d message
        printf("\n====================== [Cloud-to-Device] ============\n");
        printf("[Payload] : %.*s\n", payload_len, payload);
        printf("=========================================\n");
    }
    else if (az_result_succeeded(az_iot_hub_client_twin_parse_received_topic(azureContext.
                                                                             iot_hub_client,
                                                                             incoming_topic,
                                                                             &twin_response))) {
        char *type = get_device_twin_message_type_as_string(&twin_response);
        printf("\n====================== [Device-Twin] (Only receiving is supported)============\n");
        printf("[Twin-type] : %.*s\n", strlen(type), type);
        printf("[Payload] : %.*s\n", payload_len, payload);
        printf("=========================================\n");
    }
    else {
        printf("\n====================== [Unknown message Type]============\n");
        printf("[Topic] : %.*s\n", topic_len, topic);
        printf("[Payload] : %.*s\n", payload_len, payload);
        printf("=========================================\n");
        return -1;
    }
    return 0;
}

static char *get_device_twin_message_type_as_string(
    az_iot_hub_client_twin_response const *twin_response)
{
    // Invoke appropriate action per response type (3 types only).
    switch (twin_response->response_type) {
    case AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_TYPE_GET:
        return "GET";

    case AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_TYPE_REPORTED_PROPERTIES:
        return "REPORTED_PROPERTIES";

    case AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_TYPE_DESIRED_PROPERTIES:
        return "DESIRED_PROPERTIES";

    case AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_TYPE_REQUEST_ERROR:
        return "REQUEST_ERROR";
    }
    return "Unknown";
}
void destroy_AzRiotContext(AzRiotContext *azContext)
{
    if (azContext->mqtts_context != NULL) {
        azContext->mqtts_context = NULL;
    }
    if (azContext->iot_hub_client != NULL) {
        azContext->iot_hub_client = NULL;
    }
    if (azContext->dps_client != NULL) {
        azContext->dps_client = NULL;
    }
}
int disconnect_azure_client(AzRiotContext *azContext)
{
    int res = 0;

    if (azContext->mqtts_context != NULL) {
        res = mqtts_disconnect(azContext->mqtts_context);
        if (res == 0) {
            destroy_AzRiotContext(azContext);
        }
    }
    return res;
}
