/*******************************************************************************
Copyright (c) (2014) Texas Instruments Incorporated
All rights reserved not granted herein.

Limited License.  

Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive 
license under copyrights and patents it now or hereafter owns or controls to make, 
have made, use, import, offer to sell and sell ("Utilize") this software subject 
to the terms herein.  With respect to the foregoing patent license, such license 
is granted  solely to the extent that any such patent is necessary to Utilize the 
software alone.  The patent license shall not apply to any combinations which 
include this software, other than combinations with devices manufactured by or 
for TI (“TI Devices”).  No hardware patent is licensed hereunder.

Redistributions must preserve existing copyright notices and reproduce this license 
(including the above copyright notice and the disclaimer and (if applicable) source 
code license limitations below) in the documentation and/or other materials provided 
with the distribution

Redistribution and use in binary form, without modification, are permitted provided 
that the following conditions are met:
* No reverse engineering, decompilation, or disassembly of this software is 
  permitted with respect to any software provided in binary form.
* any redistribution and use are licensed by TI for use only with TI Devices.
* Nothing shall obligate TI to provide you with source code for the software 
  licensed and provided to you in object code.

If software source code is provided to you, modification and redistribution of the 
source code are permitted provided that the following conditions are met;
* any redistribution and use of the source code, including any resulting derivative 
  works, are licensed by TI for use only with TI Devices.
* any redistribution and use of any object code compiled from the source code and 
  any resulting derivative works, are licensed by TI for use only with TI Devices.

Neither the name of Texas Instruments Incorporated nor the names of its suppliers 
may be used to endorse or promote products derived from this software without 
specific prior written permission.

DISCLAIMER.

THIS SOFTWARE IS PROVIDED BY TI AND TI’S LICENSORS "AS IS" AND ANY EXPRESS OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TI AND TI’S 
LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "simplelink.h"

#ifndef __SL_MQTT_H__
#define __SL_MQTT_H__

#ifdef __cplusplus
extern "C"
{
#endif
        /**
          \mainpage SimpleLink MQTT Client Layer
          
          * \section intro_sec Introduction

          The SimpleLink MQTT Client Layer provides an easy-to-use API(s) to enable
          constrained and deeply embedded microcontroller based products to interact
          with cloud or network based server for telemetery. The users of SL MQTT
          Client services, while benefiting from the abstraction of the MQTT protocol
          would find them suitable for varied deployments of MQTT subscribers and / or
          publishers.

          The following figure outlines the composition of the SL MQTT Client Layer.

          * \image html ./sl_mqtt_client_view.png
          
          * \section descrypt_sec Description

          The SL MQTT Client Layer, in addition to providing services to the application,
          encompasses a RTOS task to handle the incoming messages from the server. Such
          a dedicated context to process the messages from the server facilitates the
          apps to receive data (i.e. PUBLISH messages) even when they are blocked, whilst
          awaiting ACK for a previous transaction with the server. The receive task in
          the SL MQTT Layer can not be disabled anytime, however its system wide priority
          is configurable and can be set.

          Some of the salient features of the SL MQTT Layer are 
          
          - Easy-to-use, intuitive and small set of MQTT API
          - App can indicate its choice to await ACK for a message transaction
          - Supports MQTT 3.1 protocol

          * \section config_sec Configuration
          The SL implementation enables the application developers to configure the
          following parameters using the compile line flags (-D option)
          * - <b> CFG_SL_CL_MAX_MQP: </b> the count of TX + RX resources in the buffer pool
	  for the library to use. \n\n
          * - <b> CFG_SL_CL_BUF_LEN: </b> the length of the TX and RX buffers in the pool. \n\n
          * - <b> CFG_SL_CL_STACK: </b> Max stack (bytes) for RX Task executed by SL-MQTT. \n\n
          * - <b> CFG_CL_MQTT_CTXS: </b> the max number of simultaneous network connections
          to one or more servers. \n\n

          * \section seq_sec Sequence Diagram
          The following sequence diagram outlines one of the possible usecases
	  (Blocking APIs with QOS1) using the SL MQTT Client Layer
          * \image html ./sl_mqtt_client_seq.jpg

          * \note An app that has chosen not to await an ACK from the server for an
          scheduled transaction can benefit from the availability of control to
          pursue other activities to make overall progress in the system. However,
          an attempt to schedule another transaction with the server, while the
          previous one is still active, will depend on the availability of buffers for transaction.
          

          \subsection seq_subsec Typical Sequences:
          
          - Publishers:  INIT --> CONTEXT_CREATE --> CONNECT --> PUBLISH (TX) --> DISCONNECT --> CONTEXT_DELETE --> EXIT
          - Subscribers: INIT --> CONTEXT_CREATE --> CONNECT --> SUBSCRIBE --> PUBLISH (RX) --> UNSUBSCRIBE --> DISCONNECT --> CONTEXT_DELETE --> EXIT
          
        */

        /** @defgroup sl_mqtt_cl_api SL MQTT Client API
            @{
        */
  
        /** @defgroup sl_mqtt_cl_evt SL MQTT Client Events
            @{ 
        */
#define SL_MQTT_CL_EVT_PUBACK   0x04  /**< PUBACK has been received from the server */
#define SL_MQTT_CL_EVT_PUBCOMP  0x07  /**< PUBCOMP has been received from the server */
#define SL_MQTT_CL_EVT_SUBACK   0x09  /**< SUBACK has been received from the server */
#define SL_MQTT_CL_EVT_UNSUBACK 0x0B  /**< UNSUBACK has been received from the server */
        /** @} */ /* End Client events */


      /* Define server structure which holds , server address and port number.
      These values are set by the sl_MqttSet API and retrieved by sl_MqttGet API*/

        /** Callbacks Routines
            The routines are invoked by SL Implementation onto Client application

            * \note The user applications implement the callbacks that are registered 
                    with the libraries. While using the MQTT library, invoking the 
                    core library APIs from a callback should be avoided and 
                    can lead to lockup scenarios. It is recommended to signal another 
                    task from the callback routines invoked from the library 
                    and invoke the core library API calls from that task.
         */
        typedef struct {
                
                /** Callback routine to receive a PUBLISH from the server.
                    The client app must provide this routine for the instances where it has
                    subscribed to certain set of topics from the server. The callback is
                    invoked in the context of the internal SL Receive Task.
                    
                    \param[in] app_hndl application handle returned
                    \param[in] topstr name of topic published by the server. Not NUL terminated.
                    \param[in] toplen length of the topic name published by the server.
                    \param[in] payload refers to payload published by the server.
                    \param[in] pay_len length of the payload.
                    \param[in] dup assert to indicate that it is re-send by the server
                    \param[in] qoS quality of service of the published message
                    \param[in] retain asserted to indicate that a retained message has been published
                    \return none.
                */
                void (*sl_ExtLib_MqttRecv)(void *app_hndl, const char *topstr, _i32 toplen,
                                            const void *payload, _i32 pay_len,
                                            bool dup, unsigned char qos,
                                            bool retain);
                
                /** Indication of event either from the server or implementation generated.
                    These events are notified as part of the processing carried out by the
                    internal recv task of the SL implementation. The application must populate
                    the callback to receive events about the progress made by the SL Mqtt layer.
                    
                    This handler is used by the SL Mqtt Layer to report acknowledgements from the
                    server, in case, the application has chosen not to block the service invokes
                    till the arrival of the corresponding ACK.
                    
                    \param[in] app_hndl application handle returned
                    \param[in] evt identifier to the reported event. Refer to @ref sl_mqtt_cl_evt
                    \param[in] buf points to buffer
                    \param[in] len length of buffer
                    
                    \note
                */
                void (*sl_ExtLib_MqttEvent)(void *app_hndl, _i32 evt, const void *buf, 
                                             _u32 len);

                /** Notifies the client app about the termination of MQTT connection.
                    After servicing this callback, the client-app can destroy associated
                    context if it no longer required

                    \param[in] app_hndl application handle returned
                */
                void (*sl_ExtLib_MqttDisconn)(void *app_hndl);

                
        } SlMqttClientCbs_t;
        
        typedef struct {

                const char        *will_topic;   /**< Will Topic    */
                const char        *will_msg;     /**< Will message  */
                char      will_qos;     /**< Will Qos      */
                bool               retain;       /**< Retain Flag   */
                         
        } SlMqttWill_t;
        
         
        /** Secure Socket Parameters to open a secure connection */
        /*
        Note: value of n_files can vary from 1 to 4, with corresponding pointers to
        the files in files field. Value of 1(n_files) will assume the corresponding
        pointer is for CA File Name. Any other value of n_files expects the files to
        be in following order:
        1.  Private Key File
        2.  Certificate File Name
        3.  CA File Name
        4.  DH Key File Name

        example: 
        If you want to provide only CA File Name, following are the two way of doing it:
        for n_files = 1
        char *security_file_list[] = {"/cert/testcacert.der"};
        for n_files = 4
        char *security_file_list[] = {NULL, NULL, "/cert/testcacert.der", NULL};

        where secure_files = security_file_list
        */
        typedef struct {

#define SL_MQTT_NETCONN_IP6  0x04  /**< Assert for IPv6 connection, otherwise  IPv4 */
#define SL_MQTT_NETCONN_URL  0x08  /**< Server address is an URL and not IP address */
#define SL_MQTT_NETCONN_SEC  0x10  /**< Connection to server  must  be secure (TLS) */

                _u32         netconn_flags; /**< Enumerate connection type  */
                const char          *server_addr;   /**< Server Address: URL or IP  */
                _u16       port_number;   /**< Port number of MQTT server */
                char        method;        /**< Method to tcp secured socket */
                _u32         cipher;        /**< Cipher to tcp secured socket */
                _u32         n_files;       /**< Number of files for secure transfer */
                char * const        *secure_files;  /* SL needs 4 files*/

        } SlMqttServer_t;

    
        /** MQTT Lib structure which holds Initialization Data */
        typedef struct
        {
                /**< Loopback port is used to manage lib internal functioning in case of connections to 
                     multiple servers simultaneously is desired. */
                _u16  loopback_port;  /**< Loopback port = 0, implies connection to only single server */
                                                /**< Loopback port != 0, implies connection to multiple servers */
                _u32   rx_tsk_priority;  /**< Priority of the receive task */
                _u32    resp_time;        /**< Reasonable response time (seconds) from server */
                bool  aux_debug_en;               /**< Assert to indicate additional debug info */
                _i32 (*dbg_print)(const char *pcFormat, ...); /**< Print debug information */

        } SlMqttClientLibCfg_t;


        /** MQTT client context configuration structure */
        typedef struct
        {
                SlMqttServer_t  server_info;      /**< Server information */
                bool            mqtt_mode31;      /**< Operate LIB in MQTT 3.1 mode; default is 3.1.1. false - default( 3.1.1) & true - 3.1) */
                bool            blocking_send;    /**< Select the mode of operation for send APIs (PUB, SUB, UNSUB). false - callback, true - blocking */

        } SlMqttClientCtxCfg_t;

        /** Initialize the SL MQTT Implementation.
            A caller must initialize the MQTT implementation prior to using its services.
            
            \param[in] cfg refers to client lib configuration parameters
            \return Success (0) or Failure (-1)
        */
        _i32 sl_ExtLib_MqttClientInit(const SlMqttClientLibCfg_t  *cfg);

        /** Exit the SL MQTT Implementation.
            
            \return Success (0) or Failure (-1)
        */
        _i32 sl_ExtLib_MqttClientExit();

        /** Create a new client context to connect to a server.
            A context has to be created prior to invoking the client services.

            \param[in] ctx_cfg refers to client context configuration parameters
            \param[in] msg_cbs refers to callbacks into application
            \param[in] app refers to the application callback to be returned on callback
        */
        void *sl_ExtLib_MqttClientCtxCreate(const SlMqttClientCtxCfg_t *ctx_cfg,
                                              const SlMqttClientCbs_t *msg_cbs,
                                              void *app_hndl);

        /** Deletes the specified client context.

            \param[in] cli_ctx refers to client context to be deleted
            \return Success (0) or Failure (< 0)
        */
        _i32 sl_ExtLib_MqttClientCtxDelete(void *cli_ctx);

        /** @defgroup sl_mqtt_cl_params SL MQTT Oper Paramters
            @{
        */
#define SL_MQTT_PARAM_CLIENT_ID  0x01   /**< Refers to Client ID */
#define SL_MQTT_PARAM_USER_NAME  0x02   /**< User name of client */
#define SL_MQTT_PARAM_PASS_WORD  0x03   /**< Pass-word of client */
#define SL_MQTT_PARAM_TOPIC_QOS1 0x04   /**< Set a QoS1 SUB topic */
#define SL_MQTT_PARAM_WILL_PARAM 0x05   /**< Set a WILL topic,Will Message, Will QOS,Will Retain */
          
          /** @} */
        
        /** Set parameters in SL MQTT implementation.
            The caller must configure these paramters prior to invoking any MQTT
            transaction.
            
            \note The implementation does not copy the contents referred. Therefore,
            the caller must ensure that contents are persistent in the memory.
            
            \param[in] cli_ctx refers to the handle to the client context
            \param[in] param identifies parameter to set. Refer to @ref sl_mqtt_cl_params
            \param[in] value refers to the place-holder of value to be set
            \param[in] len length of the value of the parameter
            \return Success (0) or Failure (-1)          
        */
        _i32 sl_ExtLib_MqttClientSet(void *cli_ctx, _i32 param, const void *value, _u32 len);
        
        /*\brief None defined at the moment
        */
        _i32 sl_ExtLib_MqttClientGet(void *cli_ctx, _i32 param, void *value, _u32 len);
  
  
        /** CONNECT to the server. 
            This routine establishes a connection with the server for MQTT transactions.
            The caller should specify a time-period with-in which the implementation
            should send a message to the server to keep-alive the connection.
            
            \param[in] cli_ctx refers to the handle to the client context
            \param[in] clean assert to make a clean start and purge the previous session
            \param[in] keep_alive_time the maximum time within which client should send
            a message to server. The unit of the interval is in seconds.
            \return on success, variable header of CONNACK message in network byte
            order. Lowest Byte[Byte0] contains CONNACK Return Code. Byte1 Contains 
            Session Present Bit.  
            on failure  returns(-1)
        */
        _i32 sl_ExtLib_MqttClientConnect(void *cli_ctx, bool clean, _u16 keep_alive_time);
        
        /** DISCONNECT from the server.
            The caller must use this service to close the connection with the 
            server.

            \param[in] cli_ctx refers to the handle to the client context
         
            \return Success (0) or Failure (< 0)
        */
        _i32 sl_ExtLib_MqttClientDisconnect(void *cli_ctx);
        

        /** SUBSCRIBE a set of topics.
            To receive data about a set of topics from the server, the app through
            this routine must subscribe to those topic names with the server. The
            caller can indicate whether the routine should block until a time, the
            message has been acknowledged by the server.

            In case, the app has chosen not to await for the ACK from the server, 
            the SL MQTT implementation will notify the app about the subscription
            through the callback routine.
            
            \param[in] cli_ctx refers to the handle to the client context
            \param[in] topics set of topic names to subscribe. It is an array of
            pointers to NUL terminated strings.
            \param[in,out] qos array of qos values for each topic in the same order
            of the topic array. If configured to await for SUB-ACK from server, the
            array will contain qos responses for topics from the server.
            \param[in] count number of such topics
  
            \return Success(transaction Message ID) or Failure(< 0)
        */
        _i32 sl_ExtLib_MqttClientSub(void *cli_ctx, char* const *topics,
                                    _u8 *qos, _i32 count);

        
        /** UNSUBSCRIBE a set of topics.
            The app should use this service to stop receiving data for the named
            topics from the server. The caller can indicate whether the routine
            should block until a time, the message has been acknowleged by the
            server.

            In case, the app has chosen not to await for the ACK from the server, 
            the SL MQTT implementation will notify the app about the subscription
            through the callback routine.
            
            \param[in] cli_ctx refers to the handle to the client context
            \param[in] topics set of topics to be unsubscribed. It is an array of
            pointers to NUL terminated strings.
            \param[in] count number of topics to be unsubscribed

            \return Success(transaction Message ID) or Failure(< 0) 
        */
        _i32 sl_ExtLib_MqttClientUnsub(void *cli_ctx, char* const *topics, _i32 count);
        
        
        /** PUBLISH a named message to the server.
            In addition to the PUBLISH specific parameters, the caller can indicate
            whether the routine should block until the time, the message has been
            acknowleged by the server. This is applicable only for non-QoS0 messages.
            
            In case, the app has chosen not to await for the ACK from the server, 
            the SL MQTT implementation will notify the app about the subscription
            through the callback routine.

            \param[in] cli_ctx refers to the handle to the client context
            \param[in] topic  topic of the data to be published. It is NULL terminated.
            \param[in] data   binary data to be published
            \param[in] len    length of the data
            \param[in] qos    QoS for the publish message 
            \param[in] retain assert if server should retain the message
            \param[in] flags Command flag. Refer to @ref sl_mqtt_cl_cmdflags
            \return Success(transaction Message ID) or Failure(< 0)
        */
        _i32 sl_ExtLib_MqttClientSend(void *cli_ctx, const char *topic,
                                      const void *data, _i32 len, 
                                      char qos, bool retain);
        
        /** PUBLISH a named message to the server - wrapper function.
        */
        static inline _i32 sl_ExtLib_MqttClientPub(void *cli_ctx, const char *topic,
                                      const void *data, _i32 len,
                                      char qos, bool retain)
        {
                return sl_ExtLib_MqttClientSend(cli_ctx, topic, data, len,
                                          qos, retain);
        }

        /** @} */ /* End Client API */

#ifdef __cplusplus  
}
#endif  



#endif // __SL_MQTT_H__
