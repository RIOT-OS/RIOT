/******************************************************************************
*
*   Copyright (C) 2014 Texas Instruments Incorporated
*
*   All rights reserved. Property of Texas Instruments Incorporated.
*   Restricted rights to use, duplicate or disclose this code are
*   granted through contract.
*
*   The program may not be used without the written permission of
*   Texas Instruments Incorporated or against the terms and conditions
*   stipulated in the agreement under which this program has been supplied,
*   and under no circumstances can it be used with non-TI connectivity device.
*
******************************************************************************/

#include "sl_mqtt_client.h"
#include "cc31xx_sl_net.h"
#include "osi.h"

#ifndef CFG_SL_CL_BUF_LEN
#define BUF_LEN          1024 /*Buffer length*/
#else
#define BUF_LEN          CFG_SL_CL_BUF_LEN
#endif

#ifndef CFG_SL_CL_MAX_MQP
#define MAX_MQP          2 /* # of buffers */
#else
#define MAX_MQP          CFG_SL_CL_MAX_MQP
#endif

#ifndef CFG_SL_CL_STACK
#define OSI_STACK_SIZE 2048
#else
#define OSI_STACK_SIZE CFG_SL_CL_STACK
#endif
//*****************************************************************************
// global variables used
//*****************************************************************************


struct sl_client_ctx {
        /* Client information details */
        char *client_id;
        char *usr_name;
        char *usr_pwd;
        SlMqttWill_t mqtt_will;

        /* Client management variables */
        bool in_use;
        char awaited_ack;
        _u16 conn_ack;

        /*Sync Object used to ensure single inflight message -
          used in blocking mode to wait on ack*/
        _SlSyncObj_t ack_sync_obj;
        /* Suback QoS pointer to store passed by application */
        char *suback_qos;

        /* Application information */
        void* app_hndl;
        SlMqttClientCbs_t app_cbs;

        /* Library information */
        void *cli_hndl;
        bool blocking_send;
};

#ifndef CFG_CL_MQTT_CTXS
#define MAX_SIMULTANEOUS_SERVER_CONN 4
#else
#define MAX_SIMULTANEOUS_SERVER_CONN CFG_MQTT_CL_CTXS
#endif


static struct sl_client_ctx sl_cli_ctx[MAX_SIMULTANEOUS_SERVER_CONN];

/* Lock Object to be passed to the MQTT lib */
_SlLockObj_t mqtt_lib_lockobj;

/* Creating a pool of MQTT coonstructs that can be used by the MQTT Lib
     mqp_vec =>pointer to a pool of the mqtt packet constructs
      buf_vec => the buffer area which is attached with each of mqp_vec*/
DEFINE_MQP_BUF_VEC(MAX_MQP, mqp_vec, BUF_LEN, buf_vec);

/*Task Priority and Response Time*/
u32 g_wait_secs;

bool g_multi_srvr_conn = false;

/* Receive task handle */
OsiTaskHandle g_rx_task_hndl;

/* Synchronization object used between the Rx and App task */
_SlSyncObj_t g_rx_tx_sync_obj;

#define RX_TX_SIGNAL_WAIT()     sl_SyncObjWait(&g_rx_tx_sync_obj,SL_OS_WAIT_FOREVER)
#define RX_TX_SIGNAL_POST()     sl_SyncObjSignal(&g_rx_tx_sync_obj)

/* MQTT Quality of Service */
static _const enum mqtt_qos qos[] ={
        MQTT_QOS0,
        MQTT_QOS1,
        MQTT_QOS2
};

/* Network Services specific to cc3200*/
struct device_net_services net={comm_open,tcp_send,tcp_recv,send_dest,recv_from,comm_close,
                                        tcp_listen,tcp_accept,tcp_select,rtc_secs};
        
/* Defining Retain Flag. Getting retain flag bit from fixed header*/
#define MQP_PUB_RETAIN(mqp)    ( ((mqp->fh_byte1 ) & 0x01)?true:false )

/*Defining Duplicate flag. Getting Duplicate flag bit from fixed header*/
#define MQP_PUB_DUP(mqp)        (((mqp->fh_byte1>>3) & 0x01)?true:false )

/*Defining QOS value. Getting QOS value from fixed header*/
#define MQP_PUB_QOS(mqp)        ((mqp->fh_byte1 >>1)  & 0x03)

#define ACK_RX_SIGNAL_WAIT(ack_sync_obj)     sl_SyncObjWait(ack_sync_obj,SL_OS_WAIT_FOREVER)    
#define ACK_RX_SIGNAL_POST(ack_sync_obj)     sl_SyncObjSignal(ack_sync_obj)

#define STR2UTF_CONV(utf_s, str) {utf_s.buffer = (char *)str; utf_s.length = strlen(str);}

/*Defining Event Messages*/
#define MQTT_ACK "Ack Received from server"
#define MQTT_ERROR "Connection Lost with broker"

//*****************************************************************************
// process_notify_ack_cb 
//*****************************************************************************
static void 
process_notify_ack_cb(void *app, u8 msg_type, u16 msg_id, u8 *buf, u32 len)
{
        struct sl_client_ctx *client_ctx = (struct sl_client_ctx *)app;
        _i32 loopcnt;

        switch(msg_type) {

        case MQTT_CONNACK:
                client_ctx->conn_ack = ((buf[0] << 8) | buf[1]);
                client_ctx->awaited_ack = 0;
                ACK_RX_SIGNAL_POST(&client_ctx->ack_sync_obj);
        break;
        case MQTT_SUBACK:
                if(true == client_ctx->blocking_send) {
                        for(loopcnt = 0; loopcnt < len; loopcnt++) {
                                client_ctx->suback_qos[loopcnt] = buf[loopcnt];
                        }
                                
                        if(client_ctx->awaited_ack == msg_type) {
                                client_ctx->awaited_ack = 0;
                                ACK_RX_SIGNAL_POST(&client_ctx->ack_sync_obj);
                        }
                } else {
                        client_ctx->app_cbs.sl_ExtLib_MqttEvent(
                                client_ctx->app_hndl,
                                SL_MQTT_CL_EVT_SUBACK, buf, len);
                }
        break;
        /* Error returns --- TBD */
        default:
                if(true == client_ctx->blocking_send) {
                        if(client_ctx->awaited_ack == msg_type) {
                                client_ctx->awaited_ack = 0;
                                ACK_RX_SIGNAL_POST(&client_ctx->ack_sync_obj);
                        }
                } else {
                        client_ctx->app_cbs.sl_ExtLib_MqttEvent(
                                client_ctx->app_hndl,
                                msg_type, MQTT_ACK, strlen(MQTT_ACK));
                }
        break;

        }
       
        return ;
}

//*****************************************************************************
// process_publish_rx_cb 
//*****************************************************************************
static bool 
process_publish_rx_cb(void *app, bool dup, enum mqtt_qos qos, bool retain, 
                   struct mqtt_packet *mqp)
{
        struct sl_client_ctx *client_ctx = (struct sl_client_ctx *)app;
        /*If incoming message is a publish message from broker */
        /* Invokes the event handler with topic,topic length,payload and 
           payload length values*/
        client_ctx->app_cbs.sl_ExtLib_MqttRecv(client_ctx->app_hndl,
                               (char _const*)MQP_PUB_TOP_BUF(mqp),
                               MQP_PUB_TOP_LEN(mqp),MQP_PUB_PAY_BUF(mqp),
                               MQP_PUB_PAY_LEN(mqp), dup,
                               MQP_PUB_QOS(mqp), retain);
        return true;
}

//*****************************************************************************
// process_disconn_cb
//*****************************************************************************
static void process_disconn_cb(void *app, _i32 cause)
{
        struct sl_client_ctx *client_ctx = (struct sl_client_ctx *)app;

        if(client_ctx->awaited_ack != 0) {
                client_ctx->awaited_ack = MQTT_DISCONNECT;
                ACK_RX_SIGNAL_POST(&client_ctx->ack_sync_obj);
        } else {
                if(false == client_ctx->blocking_send) {
                        /* Invoke the disconnect callback */
                        client_ctx->app_cbs.sl_ExtLib_MqttDisconn(
                                               client_ctx->app_hndl);
                }
        }
        return;
}


//*****************************************************************************
// Receive Task. Invokes in the context of a task 
//*****************************************************************************
void 
VMqttRecvTask(void *pArgs)  
{
        _i32 retval;
        do {
                if(false == g_multi_srvr_conn) {
                        /* wait on broker connection */
                        RX_TX_SIGNAL_WAIT();
                        while(1) {
                                /* Forcing to use the index 0 always - caution */
                                retval = mqtt_client_ctx_run(
                                                sl_cli_ctx[0].cli_hndl, 
                                                g_wait_secs);
                                if((retval < 0) && 
                                        (retval != MQP_ERR_TIMEOUT)) {
                                        break;
                                }
                        }
                } else {
                        mqtt_client_run(g_wait_secs);
                }
        }while(1);
}

static struct sl_client_ctx *get_available_clictx_mem()
{
        _i32 loopcnt, max_store;

        if(false == g_multi_srvr_conn) {
                max_store = 1;
        } else {
                max_store = MAX_SIMULTANEOUS_SERVER_CONN;
        }

        for(loopcnt = 0; loopcnt < max_store; loopcnt++) {
                if(false == sl_cli_ctx[loopcnt].in_use) {
                        sl_cli_ctx[loopcnt].in_use = true;
                        return(&(sl_cli_ctx[loopcnt]));
                }
        }

        return NULL;
}

//*****************************************************************************
// sl_ExtLib_MqttClientCtxCreate
//*****************************************************************************
void *sl_ExtLib_MqttClientCtxCreate(_const SlMqttClientCtxCfg_t *ctx_cfg,
                                      _const SlMqttClientCbs_t *msg_cbs,
                                      void *app_hndl)
{
        struct sl_client_ctx *client_ctx_ptr;
        struct mqtt_client_ctx_cfg lib_ctx_cfg;
        struct mqtt_client_ctx_cbs lib_cli_cbs;
        struct secure_conn lib_nw_security;
        _i32 retval;
       
        /* Get a client context storage area */
        client_ctx_ptr = get_available_clictx_mem();
        if(client_ctx_ptr == NULL) {
                return NULL;
        }

        /* Create the sync object to signal on arrival of ACK packet */
        sl_SyncObjCreate(&client_ctx_ptr->ack_sync_obj, "AckSyncObject");
        sl_SyncObjWait(&client_ctx_ptr->ack_sync_obj, SL_OS_NO_WAIT);
        
        /* Store the application handle */
        client_ctx_ptr->app_hndl = app_hndl;
        /* Initialize the ACK awaited */
        client_ctx_ptr->awaited_ack = 0;
        
       
        /* Store the application callbacks, to be invoked later */
        client_ctx_ptr->app_cbs.sl_ExtLib_MqttRecv = \
                                        msg_cbs->sl_ExtLib_MqttRecv;
        client_ctx_ptr->app_cbs.sl_ExtLib_MqttEvent = \
                                        msg_cbs->sl_ExtLib_MqttEvent;
        client_ctx_ptr->app_cbs.sl_ExtLib_MqttDisconn = \
                                        msg_cbs->sl_ExtLib_MqttDisconn;

        /* Initialize the client lib */
        lib_ctx_cfg.config_opts = ctx_cfg->mqtt_mode31;

        if(true == g_multi_srvr_conn) {
                lib_ctx_cfg.config_opts |=
                        (MQTT_CFG_APP_HAS_RTSK | MQTT_CFG_MK_GROUP_CTX);
        } else {
                lib_ctx_cfg.config_opts |= MQTT_CFG_APP_HAS_RTSK;
        }

        /* get the network connection options */
        client_ctx_ptr->blocking_send = ctx_cfg->blocking_send;
        lib_ctx_cfg.nwconn_opts = ctx_cfg->server_info.netconn_flags;
        lib_ctx_cfg.nwconn_opts |= DEV_NETCONN_OPT_TCP;
        lib_ctx_cfg.server_addr = (char*)ctx_cfg->server_info.server_addr;
        lib_ctx_cfg.port_number = ctx_cfg->server_info.port_number;
        /*initialize secure socket parameters */
        if(ctx_cfg->server_info.netconn_flags & SL_MQTT_NETCONN_SEC) {
        		lib_ctx_cfg.nw_security = &lib_nw_security;
                /* initialize secure socket parameters */
                lib_ctx_cfg.nw_security->cipher = 
                        (void*)&(ctx_cfg->server_info.cipher);
                lib_ctx_cfg.nw_security->method = 
                        (void*)&(ctx_cfg->server_info.method);
                lib_ctx_cfg.nw_security->n_file = 
                        ctx_cfg->server_info.n_files;
                lib_ctx_cfg.nw_security->files = 
                        (char**)ctx_cfg->server_info.secure_files;
        }
        else {
                lib_ctx_cfg.nw_security=NULL;
        }

        lib_cli_cbs.publish_rx = process_publish_rx_cb;
        lib_cli_cbs.ack_notify = process_notify_ack_cb;
        lib_cli_cbs.disconn_cb = process_disconn_cb;

        retval = mqtt_client_ctx_create(&lib_ctx_cfg,
                                        &lib_cli_cbs, client_ctx_ptr,
                                        &client_ctx_ptr->cli_hndl);
        
        if(retval < 0) {
                client_ctx_ptr->in_use = false;
                return NULL;
        }

        return (void*)client_ctx_ptr;
}

//*****************************************************************************
// sl_ExtLib_MqttClientCtxDelete
//*****************************************************************************
_i32 sl_ExtLib_MqttClientCtxDelete(void *cli_ctx)
{
        struct sl_client_ctx *client_ctx=(struct sl_client_ctx *)cli_ctx;
        _i32 retval;

        retval = mqtt_client_ctx_delete(client_ctx->cli_hndl);
        if(retval >= 0) {
                /* Check for more paramaters --- TBD */
                sl_SyncObjDelete(&client_ctx->ack_sync_obj);
                /* Free up the context */
                memset(client_ctx, 0, sizeof(struct sl_client_ctx));
        }

        return (retval < 0)? -1: 0;
}

void
mutex_lockup(void *mqtt_lib_lock)
{
        sl_LockObjLock((_SlLockObj_t*)mqtt_lib_lock,OSI_WAIT_FOREVER);
}

void
mqtt_mutex_unlock(void *mqtt_lib_lock)
{
        sl_LockObjUnlock((_SlLockObj_t*)mqtt_lib_lock);
}

//*****************************************************************************
// sl_ExtLib_MqttClientInit
//*****************************************************************************
_i32 sl_ExtLib_MqttClientInit(_const SlMqttClientLibCfg_t  *cfg)
{
        struct mqtt_client_lib_cfg lib_cfg;
        /* Initialize the control variables */
        memset(sl_cli_ctx, 0, sizeof(sl_cli_ctx));

        /* Setup the MQTT client lib configurations */
        lib_cfg.loopback_port = cfg->loopback_port;
        /* Initializing the sync object */
        g_rx_tx_sync_obj = 0;

        if(cfg->loopback_port) {
                g_multi_srvr_conn = true;
                lib_cfg.grp_uses_cbfn = true;   /* Does use the group callback func */
        } else {
                g_multi_srvr_conn = false;
                lib_cfg.grp_uses_cbfn = false;   /* Doesnt use the group callback func */
                /* Create the sync object between Rx and App tasks */
                sl_SyncObjCreate(&g_rx_tx_sync_obj, "RxTxSyncObject");
                sl_SyncObjWait(&g_rx_tx_sync_obj, SL_OS_NO_WAIT);
        }

        g_wait_secs = cfg->resp_time;
        /* Setup the mutex operations */
        sl_LockObjCreate(&mqtt_lib_lockobj,"MQTT Lock");
        lib_cfg.mutex = (void *)&mqtt_lib_lockobj;
        lib_cfg.mutex_lockin = mutex_lockup;
        lib_cfg.mutex_unlock = mqtt_mutex_unlock;
        /* hooking DBG print function */
        lib_cfg.debug_printf = cfg->dbg_print;
        lib_cfg.aux_debug_en = cfg->aux_debug_en;
        /* Initialize client library */
        if(mqtt_client_lib_init(&lib_cfg) < 0) {
                return -1;
        }
        /* provide MQTT Lib information to create a pool of MQTT constructs. */
        mqtt_client_buffers_register(MAX_MQP,mqp_vec,BUF_LEN,&buf_vec[0][0]);
        /* Register network services speicific to CC3200 */
        mqtt_client_net_svc_register(&net);
        /* start the receive task */
        osi_TaskCreate( VMqttRecvTask, 
                (_const signed char *) "MQTTRecv",
                OSI_STACK_SIZE,
                NULL, 
                cfg->rx_tsk_priority, &g_rx_task_hndl );
        
        return 0;
}

//*****************************************************************************
// sl_ExtLib_MqttClientExit
//*****************************************************************************
_i32 
sl_ExtLib_MqttClientExit()
{
        _i32 retval;

        /* Deinitialize the MQTT client lib */
        retval = mqtt_client_lib_exit();
        if(retval >= 0) {
                if(mqtt_lib_lockobj != NULL)
                {
                        /* Delete the MQTT lib lock object */
                        sl_LockObjDelete(&mqtt_lib_lockobj);
                }
                if(g_rx_task_hndl != NULL)
                {
                        /* Delete the Rx Task */
                        osi_TaskDelete(&g_rx_task_hndl);
                }
                
                if(g_rx_tx_sync_obj != NULL)
                {
                        /* Delete the Rx-Tx task sync object */
                        sl_SyncObjDelete(&g_rx_tx_sync_obj);
                }
                
                g_rx_task_hndl = NULL;
                g_rx_tx_sync_obj = NULL;
                mqtt_lib_lockobj = NULL;
        }
        
        return (retval < 0)? -1: 0;
}

//*****************************************************************************
// sl_ExtLib_MqttClientSet
//*****************************************************************************
_i32 
sl_ExtLib_MqttClientSet(void *app, _i32 param, _const void *value, _u32 len)
{
        struct sl_client_ctx *client_ctx = (struct sl_client_ctx *)app;
        switch(param)
        {
        case SL_MQTT_PARAM_CLIENT_ID:
                /* Save the reference to the Client ID */
                client_ctx->client_id =(char*)value;
                break;

        case SL_MQTT_PARAM_USER_NAME:
                /* Save the reference to the Username */
                client_ctx->usr_name =(char*)value;
                break;

        case SL_MQTT_PARAM_PASS_WORD:
                /* Save the reference to the password */
                client_ctx->usr_pwd =(char*)value;
                break;

        case SL_MQTT_PARAM_WILL_PARAM:
                /* Save the reference to will parameters*/
                client_ctx->mqtt_will = *((SlMqttWill_t*)value);
                break;
        default:
                break;
        }
        return 0;
   
}

//*****************************************************************************
// sl_ExtLib_MqttClientConnect
//*****************************************************************************
_i32
sl_ExtLib_MqttClientConnect(void *cli_ctx, bool clean, _u16 keep_alive_time)
{ 
        _i32 ret = -1;
        struct sl_client_ctx *client_ctx = (struct sl_client_ctx *)cli_ctx;

        /*utf8 strings into which client info will be stored*/
        struct utf8_string client_id, username, usr_pwd, will_topic, will_msg;
        struct utf8_string *usrname = NULL, *usrpasswd = NULL, *clientid=NULL;

        /* Provide Client ID,user name and password into MQTT Library */
        if(client_ctx->client_id != NULL) {
            STR2UTF_CONV(client_id, client_ctx->client_id);
            clientid = &client_id;
        }
        
        if(client_ctx->usr_name != NULL) {
                STR2UTF_CONV(username, client_ctx->usr_name);
                usrname = &username;
        }
        if(client_ctx->usr_pwd != NULL) {
                STR2UTF_CONV(usr_pwd, client_ctx->usr_pwd);
                usrpasswd = &usr_pwd;
        }
        ret = mqtt_client_ctx_info_register(client_ctx->cli_hndl,
                                clientid, usrname, usrpasswd);
        if(ret < 0) {
                goto mqtt_connect_exit1;
        }
        
        /* Register a will message, if specified, into MQTT Library */
        if(NULL != client_ctx->mqtt_will.will_topic ) {
                STR2UTF_CONV(will_topic, client_ctx->mqtt_will.will_topic);
                STR2UTF_CONV(will_msg, client_ctx->mqtt_will.will_msg);
                ret = mqtt_client_ctx_will_register(client_ctx->cli_hndl,
                                        &will_topic, &will_msg,
                                        qos[client_ctx->mqtt_will.will_qos],
                                        client_ctx->mqtt_will.retain);
                if(ret < 0) {
                        goto mqtt_connect_exit1;
                }
        }

        client_ctx->awaited_ack = MQTT_CONNACK;
        client_ctx->conn_ack = 0;

        /* Connect to the server */
        ret = mqtt_connect_msg_send(client_ctx->cli_hndl, clean,
                                keep_alive_time);
        /*Network or Socket Error happens*/
        if(ret < 0) {
                goto mqtt_connect_exit1;
        }

        if(false == g_multi_srvr_conn) {
                /* Unblock the receive task here */        
                RX_TX_SIGNAL_POST();
        }
        /* Wait for a CONNACK here */
        ACK_RX_SIGNAL_WAIT(&client_ctx->ack_sync_obj);

        if(MQTT_DISCONNECT == client_ctx->awaited_ack) {
                ret = -1;
        } else {
                ret = (_i32)client_ctx->conn_ack;
        }

mqtt_connect_exit1:
        client_ctx->awaited_ack = 0;
        client_ctx->conn_ack = 0;
        return (ret < 0)? -1: ret;
}

//*****************************************************************************
// sl_ExtLib_MqttClientDisconnect
//*****************************************************************************
_i32
sl_ExtLib_MqttClientDisconnect(void *cli_ctx)
{
        struct sl_client_ctx *client_ctx = (struct sl_client_ctx *)cli_ctx;
        _i32 retval;

        if(!(mqtt_client_is_connected(client_ctx->cli_hndl)))
                return -1;

        client_ctx->awaited_ack = MQTT_DISCONNECT;

        /* send the disconnect command. */
        retval = mqtt_disconn_send(client_ctx->cli_hndl);
        
        if(retval >= 0) {
                /* wait on Rx task to acknowledge */
                ACK_RX_SIGNAL_WAIT(&client_ctx->ack_sync_obj);
        }
        client_ctx->awaited_ack = 0;

        return (retval < 0)? -1: 0;
}


//*****************************************************************************
// sl_ExtLib_MqttClientSub
//*****************************************************************************
_i32 
sl_ExtLib_MqttClientSub(void *cli_ctx, char* _const *topics,
                        _u8 *qos_level, _i32 count)
{
#define MAX_SIMULTANEOUS_SUB_TOPICS 4

        _i32 ret = -1, i;

        struct utf8_strqos qos_topics[MAX_SIMULTANEOUS_SUB_TOPICS];
        struct sl_client_ctx *client_ctx = (struct sl_client_ctx *)cli_ctx;

        if(!(mqtt_client_is_connected(client_ctx->cli_hndl)) || (count > MAX_SIMULTANEOUS_SUB_TOPICS)) {
                goto mqtt_sub_exit1;  /* client not connected*/
        }

        for(i = 0; i < count; i++) {
                qos_topics[i].buffer = topics[i];
                qos_topics[i].qosreq = qos[qos_level[i]];
                qos_topics[i].length = strlen(topics[i]);
        }

        /* Set up all variables to receive an ACK for the message to be sent*/
        if(true == client_ctx->blocking_send) {      
                /* Update the qos_level to be in-out parameter */
                client_ctx->suback_qos = (char*)qos_level;
                client_ctx->awaited_ack = MQTT_SUBACK;
        }

        /* Send the subscription MQTT message */
        ret = mqtt_sub_msg_send(client_ctx->cli_hndl, qos_topics, count);
        if(ret < 0) {
                goto mqtt_sub_exit1;
        }

        if(true == client_ctx->blocking_send) {
                ACK_RX_SIGNAL_WAIT(&client_ctx->ack_sync_obj);
                if(MQTT_DISCONNECT == client_ctx->awaited_ack) {
                        ret = -1;
                }
        }

mqtt_sub_exit1:
        client_ctx->awaited_ack = 0;
        return (ret < 0)? -1: 0;
}


//*****************************************************************************
// sl_ExtLib_MqttClientUnsub
//*****************************************************************************
_i32
sl_ExtLib_MqttClientUnsub(void *cli_ctx, char* _const *topics, _i32 count)
{
#define MAX_SIMULTANEOUS_UNSUB_TOPICS 4

        _i32 ret = -1, i;
        struct utf8_string unsub_topics[MAX_SIMULTANEOUS_UNSUB_TOPICS];
        struct sl_client_ctx *client_ctx = (struct sl_client_ctx *)cli_ctx;

        if(!(mqtt_client_is_connected(client_ctx->cli_hndl)) || (count > MAX_SIMULTANEOUS_UNSUB_TOPICS)) {
                goto mqtt_unsub_exit1; /* Check client status */
        }

        for(i = 0; i < count; i++) {
                STR2UTF_CONV(unsub_topics[i], topics[i]);
        }

        /* Set up all variables to receive an ACK for the message to be sent*/
        if(true == client_ctx->blocking_send) {      
                client_ctx->awaited_ack = MQTT_UNSUBACK;
        }

        /* Send the unsubscription MQTT message */
        ret = mqtt_unsub_msg_send(client_ctx->cli_hndl, unsub_topics, count);
        if(ret < 0) {
                goto mqtt_unsub_exit1;
        }

        if(true == client_ctx->blocking_send) {
                ACK_RX_SIGNAL_WAIT(&client_ctx->ack_sync_obj);
                if(MQTT_DISCONNECT == client_ctx->awaited_ack) {
                        ret = -1;
                }
        }

mqtt_unsub_exit1:
        client_ctx->awaited_ack = 0;
        return (ret < 0)? -1: 0;
}
//*****************************************************************************
// sl_ExtLib_MqttClientSend
//*****************************************************************************
_i32 
sl_ExtLib_MqttClientSend(void *cli_ctx, _const char *topic,
                          _const void *data, _i32 len, 
                          char qos_level, bool retain)
{
        _i32 ret = -1;
        struct utf8_string topic_utf8;
        struct sl_client_ctx *client_ctx = (struct sl_client_ctx *)cli_ctx;

        /* Check whether Client is connected or not? */
        if(!(mqtt_client_is_connected(client_ctx->cli_hndl))) {
                return ret;
        }

        STR2UTF_CONV(topic_utf8, topic);

        /*Set up all variables to receive an ACK for the message to be sent*/
        if(MQTT_QOS1 == qos[qos_level]) {
                client_ctx->awaited_ack = MQTT_PUBACK;
        } else if(MQTT_QOS2 == qos[qos_level]) {
                client_ctx->awaited_ack = MQTT_PUBCOMP;
        }
        /*publish the message*/
        ret = mqtt_client_pub_msg_send(client_ctx->cli_hndl, &topic_utf8,
                                        data, len,
                                        qos[qos_level], retain);
        if(ret < 0) {
                goto mqtt_pub_exit1;
        }

        if(MQTT_QOS0 != qos[qos_level]) {
                if(true == client_ctx->blocking_send) {
                        ACK_RX_SIGNAL_WAIT(&client_ctx->ack_sync_obj);
                        if(MQTT_DISCONNECT == client_ctx->awaited_ack) {
                                ret = -1;
                        }
                }
        }

mqtt_pub_exit1:
        client_ctx->awaited_ack = 0;
        return (ret < 0)? -1: 0;
}


