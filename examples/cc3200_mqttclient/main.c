/*
 * Original work Copyright (C) 2014 Texas Instruments Incorporated - www.ti.com
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 *
 * This demo is slightly adapated to RIOT from the original mqttclient
 * demo from Texas Instruments
 *
 * see //http://processors.wiki.ti.com/index.php/CC32xx_MQTT_Client
 * about demo setup
 *
 */
#include "simplelink.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "panic.h"
#include "xtimer.h"
#include "periph/gpio.h"

#include "network_if.h"

#include "common.h"
#include "driverlib/utils.h"

#include "osi.h"

#include "sl_mqtt_client.h"

#define MQTTCLIENT_QUEUE_SIZE (8U)

#define APPLICATION_VERSION 	"1.1.1"

/*Operate Lib in MQTT 3.1 mode.*/
#define MQTT_3_1_1              false /*MQTT 3.1.1 */
#define MQTT_3_1                true /*MQTT 3.1*/

#define WILL_TOPIC              "Client"
#define WILL_MSG                "Client Stopped"
#define WILL_QOS                QOS2
#define WILL_RETAIN             false

/*Defining Broker IP address and port Number*/
#define SERVER_ADDRESS          "messagesight.demos.ibm.com"
#define PORT_NUMBER             1883

#define MAX_BROKER_CONN         1

#define SERVER_MODE             MQTT_3_1
/*Specifying Receive time out for the Receive task*/
#define RCV_TIMEOUT             30

/*Background receive task priority*/
#define TASK_PRIORITY           14

/* Keep Alive Timer value*/
#define KEEP_ALIVE_TIMER        25

/*Clean session flag*/
#define CLEAN_SESSION           true

/*Retain Flag. Used in publish message. */
#define RETAIN                  1

/*Defining Publish Topic*/
#define PUB_TOPIC_FOR_SW3       "/cc3200/sw3"
#define PUB_TOPIC_FOR_SW2       "/cc3200/sw2"

/*Defining Number of topics*/
#define TOPIC_COUNT             3

/*Defining Subscription Topic Values*/
#define TOPIC1                  "/cc3200/green"
#define TOPIC2                  "/cc3200/yellow"
#define TOPIC3                  "/cc3200/red"

/*Defining QOS levels*/
#define QOS0                    0
#define QOS1                    1
#define QOS2                    2

typedef struct connection_config {
    SlMqttClientCtxCfg_t broker_config;
    void *clt_ctx;
    const char *client_id;
    unsigned char *usr_name;
    unsigned char *usr_pwd;
    bool is_clean;
    unsigned int keep_alive_time;
    SlMqttClientCbs_t CallBAcks;
    int num_topics;
    char *topic[TOPIC_COUNT];
    unsigned char qos[TOPIC_COUNT];
    SlMqttWill_t will_params;
    bool is_connected;
} connect_config;

//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//*****************************************************************************
static void
Mqtt_Recv(void *app_hndl, const char *topstr, long top_len, const void *payload,
        long pay_len, bool dup, unsigned char qos, bool retain);

static void sl_MqttEvt(void *app_hndl, long evt, const void *buf,
        unsigned long len);

static void sl_MqttDisconnect(void *app_hndl);

typedef enum events {
    PUSH_BUTTON_SW2_PRESSED,
    PUSH_BUTTON_SW3_PRESSED,
    BROKER_DISCONNECTION,
    YELLOW_TIMEOUT_EVENT
} osi_messages;

#define YELLOW_TIMER_PERIOD MSEC_TO_TICKS(100)

typedef struct {
    xtimer_t timer;
    uint8_t enabled;
} lsts_t;

static lsts_t periodic_command;

// NWP driver thread runs at higher priority
//#define SPAWN_TASK_PRIORITY  2

// application run at lower priority
#define APP_TASK_PRIORITY    12

#define SIMPLELINK_STACKSIZE 4096

char simplelink_stack[SIMPLELINK_STACKSIZE];

/* AP Security Parameters */
SlSecParams_t SecurityParams = { 0 };

/* connection configuration */
connect_config usr_connect_config[] = { { { {
SL_MQTT_NETCONN_URL,
SERVER_ADDRESS,
PORT_NUMBER, 0, 0, 0,
NULL },
SERVER_MODE,
true, },
NULL, "user1",
NULL,
NULL,
true,
KEEP_ALIVE_TIMER, { Mqtt_Recv, sl_MqttEvt, sl_MqttDisconnect },
TOPIC_COUNT, { TOPIC1, TOPIC2, TOPIC3 }, { QOS2, QOS2, QOS2 }, { WILL_TOPIC,
        WILL_MSG, WILL_QOS, WILL_RETAIN },
false } };

/* library configuration */
SlMqttClientLibCfg_t Mqtt_Client = { 1882,
TASK_PRIORITY, 30,
false,  // enable mqtt driver debug
        (long (*)(const char *, ...)) UART_PRINT };

/*Publishing topics and messages*/
const char *pub_topic_sw2 = PUB_TOPIC_FOR_SW2;
const char *pub_topic_sw3 = PUB_TOPIC_FOR_SW3;
const char *data_sw2 = { "sw2 button pressed" };
const char *data_sw3 = { "Great news! cc3200 is powered by RIOT" };

void *app_hndl = (void*) usr_connect_config;

kernel_pid_t mqtt_tid, main_tid;

//****************************************************************************
//! Defines Mqtt_Pub_Message_Receive event handler.
//! Client App needs to register this event handler with sl_ExtLib_mqtt_Init
//! API. Background receive task invokes this handler whenever MQTT Client
//! receives a Publish Message from the broker.
//!
//!\param[out]     topstr => pointer to topic of the message
//!\param[out]     top_len => topic length
//!\param[out]     payload => pointer to payload
//!\param[out]     pay_len => payload length
//!\param[out]     retain => Tells whether its a Retained message or not
//!\param[out]     dup => Tells whether its a duplicate message or not
//!\param[out]     qos => Tells the Qos level
//!
//!\return none
//****************************************************************************
static void Mqtt_Recv(void *app_hndl, const char *topstr, long top_len,
        const void *payload, long pay_len, bool dup, unsigned char qos,
        bool retain) {

    char *output_str = (char*) malloc(top_len + 1);
    memset(output_str, '\0', top_len + 1);
    strncpy(output_str, (char*) topstr, top_len);
    output_str[top_len] = '\0';

    if (strncmp(output_str, TOPIC1, top_len) == 0) {
        //ToggleLedState(LED1);
        LED_GREEN_TOGGLE;
    } else if (strncmp(output_str, TOPIC2, top_len) == 0) {
        //ToggleLedState(LED2);
        LED_YELLOW_TOGGLE;
    } else if (strncmp(output_str, TOPIC3, top_len) == 0) {
        //ToggleLedState(LED3);
        LED_RED_TOGGLE;
    }

    UART_PRINT("\n\rPublish Message Received");
    UART_PRINT("\n\rTopic: ");
    UART_PRINT("%s", output_str);
    free(output_str);
    UART_PRINT(" [Qos: %d] ", qos);
    if (retain)
        UART_PRINT(" [Retained]");
    if (dup)
        UART_PRINT(" [Duplicate]");

    output_str = (char*) malloc(pay_len + 1);
    memset(output_str, '\0', pay_len + 1);
    strncpy(output_str, (char*) payload, pay_len);
    output_str[pay_len] = '\0';
    UART_PRINT("\n\rData is: ");
    UART_PRINT("%s", (char*) output_str);
    UART_PRINT("\n\r");
    free(output_str);

    return;
}

//****************************************************************************
//! Defines sl_MqttEvt event handler.
//! Client App needs to register this event handler with sl_ExtLib_mqtt_Init
//! API. Background receive task invokes this handler whenever MQTT Client
//! receives an ack( whenever user is in non-blocking mode)
//! or encounters an error.
//!
//! param[out]      evt => Event that invokes the handler. Event can be of the
//!                        following types:
//!                        MQTT_ACK - Ack Received
//!                        MQTT_ERROR - unknown error
//!
//!
//! \param[out]     buf => points to buffer
//! \param[out]     len => buffer length
//!
//! \return none
//****************************************************************************
static void sl_MqttEvt(void *app_hndl, long evt, const void *buf,
        unsigned long len) {
    int i;
    switch (evt) {
    case SL_MQTT_CL_EVT_PUBACK:
        UART_PRINT("PubAck:\n\r");
        UART_PRINT("%s\n\r", (const char*) buf);
        break;

    case SL_MQTT_CL_EVT_SUBACK:
        UART_PRINT("\n\rGranted QoS Levels are:\n\r");

        for (i = 0; i < len; i++) {
            UART_PRINT("QoS %d\n\r", ((unsigned char*) buf)[i]);
        }
        break;

    case SL_MQTT_CL_EVT_UNSUBACK:
        UART_PRINT("UnSub Ack \n\r");
        UART_PRINT("%s\n\r", (const char*) buf);
        break;

    default:
        break;

    }
}

//****************************************************************************
//
//! callback event in case of MQTT disconnection
//!
//! \param app_hndl is the handle for the disconnected connection
//!
//! return none
//
//****************************************************************************
static void sl_MqttDisconnect(void *app_hndl) {
    msg_t msg;

    connect_config *local_con_conf;
    //osi_messages var = BROKER_DISCONNECTION;
    local_con_conf = app_hndl;
    sl_ExtLib_MqttClientUnsub(local_con_conf->clt_ctx, local_con_conf->topic,
    TOPIC_COUNT);
    UART_PRINT("disconnect from broker %s\r\n",
            (local_con_conf->broker_config).server_info.server_addr);
    local_con_conf->is_connected = false;
    sl_ExtLib_MqttClientCtxDelete(local_con_conf->clt_ctx);

    //
    // write message indicating publish message
    //
    // osi_MsgQWrite(&g_PBQueue,&var,OSI_NO_WAIT);
    msg.type = BROKER_DISCONNECTION;
    msg_send(&msg, mqtt_tid);
}

void yellow_pulse(void *arg) {
    msg_t msg;
    msg.type = YELLOW_TIMEOUT_EVENT;

    msg_send(&msg, main_tid);
}

void* mqttClient(void *pvParameters) {

    long lRetVal = -1;
    int iCount = 0;
    int iNumBroker = 0;
    int iConnBroker = 0;

    msg_t msg, msg_queue[MQTTCLIENT_QUEUE_SIZE];

    /* setup the mqttclient message queue */
    msg_init_queue(msg_queue, MQTTCLIENT_QUEUE_SIZE);

    connect_config *local_con_conf = (connect_config *) app_hndl;

    LED_GREEN_OFF;
    LED_YELLOW_OFF;
    LED_RED_OFF;

    //
    // Reset The state of the machine
    //
    Network_IF_ResetMCUStateMachine();

    //
    // Start the driver
    //
    lRetVal = Network_IF_InitDriver(ROLE_STA);
    if (lRetVal < 0) {
        core_panic(lRetVal, "Failed to start SimpleLink Device");
    }

    // switch on Green LED to indicate Simplelink is properly up
    LED_GREEN_ON;

    // Initialize AP security params
    SecurityParams.Key = (signed char *) SECURITY_KEY;
    SecurityParams.KeyLen = strlen(SECURITY_KEY);
    SecurityParams.Type = SECURITY_TYPE;

    // start blinking yellow led
    xtimer_set(&periodic_command.timer, YELLOW_TIMER_PERIOD);

    //
    // Connect to the Access Point
    //
    lRetVal = Network_IF_ConnectAP(SSID_NAME, SecurityParams);

    periodic_command.enabled = false;
    if (lRetVal < 0) {
        core_panic(lRetVal, "Connection to an AP failed\n\r");
    }

    //
    // Switch ON RED LED to indicate that Device acquired an IP
    //
    LED_RED_ON;

    UtilsDelay(20000000);

    LED_GREEN_OFF;
    LED_YELLOW_OFF;
    LED_RED_OFF;

    //
    // Initialze MQTT client lib
    //
    lRetVal = sl_ExtLib_MqttClientInit(&Mqtt_Client);
    if (lRetVal != 0) {
        // lib initialization failed
        core_panic(lRetVal, "MQTT Client lib initialization failed\n\r");
    }

    /******************* connection to the broker ***************************/
    iNumBroker = sizeof(usr_connect_config) / sizeof(connect_config);
    if (iNumBroker > MAX_BROKER_CONN) {
        UART_PRINT("Num of brokers are more then max num of brokers\n\r");
        LOOP_FOREVER()
        ;
    }

    while (iCount < iNumBroker) {
        //create client context
        local_con_conf[iCount].clt_ctx = sl_ExtLib_MqttClientCtxCreate(
                &local_con_conf[iCount].broker_config,
                &local_con_conf[iCount].CallBAcks, &(local_con_conf[iCount]));

        //
        // Set Client ID
        //
        sl_ExtLib_MqttClientSet((void*) local_con_conf[iCount].clt_ctx,
        SL_MQTT_PARAM_CLIENT_ID, local_con_conf[iCount].client_id,
                strlen((char*) (local_con_conf[iCount].client_id)));

        //
        // Set will Params
        //
        if (local_con_conf[iCount].will_params.will_topic != NULL) {
            sl_ExtLib_MqttClientSet((void*) local_con_conf[iCount].clt_ctx,
            SL_MQTT_PARAM_WILL_PARAM, &(local_con_conf[iCount].will_params),
                    sizeof(SlMqttWill_t));
        }

        //
        // setting username and password
        //
        if (local_con_conf[iCount].usr_name != NULL) {
            sl_ExtLib_MqttClientSet((void*) local_con_conf[iCount].clt_ctx,
            SL_MQTT_PARAM_USER_NAME, local_con_conf[iCount].usr_name,
                    strlen((char*) local_con_conf[iCount].usr_name));

            if (local_con_conf[iCount].usr_pwd != NULL) {
                sl_ExtLib_MqttClientSet((void*) local_con_conf[iCount].clt_ctx,
                SL_MQTT_PARAM_PASS_WORD, local_con_conf[iCount].usr_pwd,
                        strlen((char*) local_con_conf[iCount].usr_pwd));
            }
        }

        //
        // connecting to the broker
        //
        if ((sl_ExtLib_MqttClientConnect((void*) local_con_conf[iCount].clt_ctx,
                local_con_conf[iCount].is_clean,
                local_con_conf[iCount].keep_alive_time) & 0xFF) != 0) {
            UART_PRINT("\n\rBroker connect fail for conn no. %d \n\r",
                    iCount + 1);

            //delete the context for this connection
            sl_ExtLib_MqttClientCtxDelete(local_con_conf[iCount].clt_ctx);

            break;
        } else {
            UART_PRINT("\n\rSuccess: conn to Broker no. %d\n\r ", iCount + 1);
            local_con_conf[iCount].is_connected = true;
            iConnBroker++;
        }

        //
        // Subscribe to topics
        //

        if (sl_ExtLib_MqttClientSub((void*) local_con_conf[iCount].clt_ctx,
                local_con_conf[iCount].topic, local_con_conf[iCount].qos,
                TOPIC_COUNT) < 0) {
            UART_PRINT("\n\r Subscription Error for conn no. %d\n\r",
                    iCount + 1);
            UART_PRINT("Disconnecting from the broker\r\n");
            sl_ExtLib_MqttClientDisconnect(local_con_conf[iCount].clt_ctx);
            local_con_conf[iCount].is_connected = false;

            //delete the context for this connection
            sl_ExtLib_MqttClientCtxDelete(local_con_conf[iCount].clt_ctx);
            iConnBroker--;
            break;
        } else {
            int iSub;
            UART_PRINT("Client subscribed on following topics:\n\r");
            for (iSub = 0; iSub < local_con_conf[iCount].num_topics; iSub++) {
                UART_PRINT("%s\n\r", local_con_conf[iCount].topic[iSub]);
            }
        }
        iCount++;
    }

    if (iConnBroker < 1) {
        //
        // no succesful connection to broker
        //
        sl_ExtLib_MqttClientExit();
        core_panic(0, "Broker connection failed");
    }

    iCount = 0;

    for (;;) {
        msg_receive(&msg);

        switch (msg.type) {
        case PUSH_BUTTON_SW2_PRESSED:
            gpio_irq_enable(PUSH1);
            //
            // send publish message
            //
            sl_ExtLib_MqttClientSend((void*) local_con_conf[iCount].clt_ctx,
                    pub_topic_sw2, data_sw2, strlen((char*) data_sw2), QOS2,
                    RETAIN);
            UART_PRINT("\n\r CC3200 Publishes the following message \n\r");
            UART_PRINT("Topic: %s\n\r", pub_topic_sw2);
            UART_PRINT("Data: %s\n\r", data_sw2);

            break;
        case PUSH_BUTTON_SW3_PRESSED:
            gpio_irq_enable(PUSH2);
            //
            // send publish message
            //
            sl_ExtLib_MqttClientSend((void*) local_con_conf[iCount].clt_ctx,
                    pub_topic_sw3, data_sw3, strlen((char*) data_sw3), QOS2,
                    RETAIN);
            UART_PRINT("\n\r CC3200 Publishes the following message \n\r");
            UART_PRINT("Topic: %s\n\r", pub_topic_sw3);
            UART_PRINT("Data: %s\n\r", data_sw3);

            break;
        case BROKER_DISCONNECTION:
            printf("broker disconnected\n");
            break;
        default:
            core_panic(1, "Unknown message received");
        }
    }

    return NULL;
}

void button1_handler(void *arg) {
    msg_t msg;

    gpio_irq_disable(PUSH1);

    msg.type = PUSH_BUTTON_SW2_PRESSED;
    msg_send(&msg, mqtt_tid);

}

void button2_handler(void *arg) {
    msg_t msg;

    gpio_irq_disable(PUSH2);

    msg.type = PUSH_BUTTON_SW3_PRESSED;
    msg_send(&msg, mqtt_tid);

}

int main(void) {
    long ret = -1;
    msg_t msg;

    (void) puts("Welcome to RIOT!");

    main_tid = thread_getpid();

    periodic_command.timer.callback = yellow_pulse;
    periodic_command.enabled = true;

    gpio_init_int(PUSH1, GPIO_NOPULL, GPIO_FALLING, button1_handler, 0);
    gpio_init_int(PUSH2, GPIO_NOPULL, GPIO_FALLING, button2_handler, 0);

    ret = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if (ret < 0) {
        core_panic(ret, "unable to start wifi");
    }

    mqtt_tid = thread_create(simplelink_stack, SIMPLELINK_STACKSIZE,
    APP_TASK_PRIORITY, CREATE_STACKTEST, mqttClient, NULL, "simplelink");

    printf("simplelink tid: %d\n", mqtt_tid);

    while (1) {
        msg_receive(&msg);

        if (msg.type == YELLOW_TIMEOUT_EVENT) {
            LED_YELLOW_TOGGLE;
            if (periodic_command.enabled) {
                xtimer_set(&periodic_command.timer, YELLOW_TIMER_PERIOD);
            } else {
                LED_YELLOW_OFF;
            }

        }
    }

    return 0;
}
