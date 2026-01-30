#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "timex.h"
#include "ztimer.h"
#include "shell.h"
#include "thread.h"
#include "mutex.h"

#include "az_riot_pnp_iface.h"

#define BUF_SIZE 1024
static unsigned char writebuf[BUF_SIZE];
static unsigned char readbuf[BUF_SIZE];

AzRiotContext *context = NULL;

int azInit(void)
{
    context = create_AzRiotContext(writebuf, BUF_SIZE, readbuf, BUF_SIZE);
    if (context == NULL) {
        printf("[demo]: AzRiotContext is null\n");
        return -1;
    }
    int res = init_dps_client(context);

    if (res < 0) {
        printf("[demo]: init_dps_client failed with %d\n", res);
        return res;
    }
    return 0;
}

static int _cmd_discon(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int res = disconnect_azure_client(context);

    if (res < 0) {
        printf("[demo]: Unable to disconnect\n");
    }
    else {
        printf("[demo]: Disconnected successful from Azure\n");
    }
    return 0;
}

static int _cmd_conn_iot_hub(int argc, char **argv)
{
    if (argc < 3) {

        printf("usage: %s <hub addr> [deviceID]\n", argv[0]);
        return 1;
    }
    if (context == NULL) {
        printf("[demo]: Azure context is null\n");
        return -1;
    }
    int res = init_iot_hub_client(context, argv[1], argv[2]);

    if (res < 0) {
        printf("[demo]: init_iot_hub_client failed with %d\n", res);
        return res;
    }

    res = connect_azure_client(context, HUB);
    if (res != 0) {
        printf("[demo]: Unable to connect hub client return code =%d\n", res);
        _cmd_discon(0, NULL);
        return res;
    }
    else {
        printf("[demo]: IoT Hub Connection successfull\n");
    }
    return res;
}

static int _cmd_con_DPS(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("[demo]: connecting .... to DPS\n");
    int ret = connect_azure_client(context, DPS);

    if (ret != 0) {
        printf("[demo]: Unable to connect DPS client %d\n", ret);
        _cmd_discon(0, NULL);
        return ret;
    }
    else {
        printf("[demo]: DPS connection successfull\n");
    }

    return ret;
}

static int _cmd_register_device(int argc, char **argv)
{
    int rc;

    if ((rc = register_device_with_provisioning_service(context)) < 0) {
        printf("[demo]: Unable to register device returned with (%d)\n", rc);
    }
    return rc;
}
static int _cmd_send_telemetry(int argc, char **argv)
{

    char *telemetry_message_payload =
        "{\"riot_message\":\" This message is sent from riot device\"}";
    int rc;

    if ((rc =
             send_telemetry_message_to_iot_hub(context,
                                               (unsigned char *)telemetry_message_payload)) < 0) {
        printf("[demo]: Unable to publish (%d)\n", rc);
    }
    else {
        printf("[demo]: Message has been sent with payload => %s\n",
               telemetry_message_payload);
    }
    return rc;
}

static int _cmd_device_reg_query(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [operationID]\n", argv[0]);
        return 1;
    }

    if (send_operation_query_message_to_dps(context, argv[1]) < 0) {
        printf("[demo]: send_operation_query_message_to_dps failed\n");
    }
    return 0;
}

static int _cmd_sub_dps(int argc, char **argv)
{
    int ret = subscribe_to_azure_client_topics(context, DPS);

    if (ret < 0) {
        printf("[demo]: Unable to subscribe to DPS topic, return code = %d\n", ret);
    }
    else {
        printf("[demo]: Now subscribed to DPS topic\n");
    }
    return ret;
}
static int _cmd_sub_hub(int argc, char **argv)
{
    int ret = subscribe_to_azure_client_topics(context, HUB);

    if (ret < 0) {
        printf("[demo]: Unable to subscribe to HUB topic return code = %d\n", ret);
    }
    else {
        printf("[demo]: Now subscribed to HUB topics\n");
    }
    return ret;
}

static const shell_command_t shell_commands[] =
{
    { "con_dps", "connect to DPS", _cmd_con_DPS },
    { "discon", "disconnect from the current broker", _cmd_discon },
    { "reg_device", "device provisioning with DPS", _cmd_register_device },
    { "sub_hub", "subscribe to hub topics", _cmd_sub_hub },
    { "sub_dps", "subscribe to dps topic", _cmd_sub_dps },
    { "query", "device registration query", _cmd_device_reg_query },
    { "con_hub", "connect to iot hub", _cmd_conn_iot_hub },
    { "send_telemetry", "send telemetry to iot hub", _cmd_send_telemetry },
    { NULL, NULL, NULL }
};

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
int main(void)
{
    if (IS_USED(MODULE_GNRC_ICMPV6_ECHO)) {
        msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    }

    int ret = 0;

    if ((ret = azInit()) < 0) {
        printf("[demo]: az init failed\n");
    }
    else {
        printf("[demo]: azure sdk initilization successfull\n");
    }

    printf("Running azure example. Type help for commands info\n");

    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
