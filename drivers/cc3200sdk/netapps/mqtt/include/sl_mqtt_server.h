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

#ifndef __SL_MQTT_SRVR_H__
#define __SL_MQTT_SRVR_H__

#ifdef __cplusplus
extern "C"
{
#endif
        /*!
          \mainpage SimpleLink MQTT Server Layer
          
          \section intro_sec Introduction

          The SimpleLink MQTT Server Layer provides an easy-to-use API(s) to enable
          constrained and deeply embedded microcontroller based products to interact
          with cloud or network based clients for telemetry. The users of SL MQTT
          server services, while benefiting from the abstraction of the MQTT protocol
          would find them suitable for development of MQTT Servers.

                   
          \section descrypt_sec Description

          The SL MQTT Server Layer, in addition to providing services to the server
          application, encompasses an RTOS task to handle the incoming messages from the
          various clients conencted to the server.
          Such a dedicated context to process the messages from the clients facilitates the
          app to receive data (for ex. PUBLISH messages) even when it is 
          transacting with another client. This is important considering that a server 
          application has to manage messages from multiple clients connected
          to it simulatneously.
          The receive task in the SL MQTT Server Layer can not be disabled anytime, 
          however its system wide priority is configurable and can be set.

          Some of the salient features of the SL MQTT Layer are 
          
          - Easy-to-use, intuitive and small set of MQTT API
          - App can indicate its choice to await ACK for a message transaction

          \section config_sec Configurations
          The SL implementation enables the application developers to configure the
	  following parameters using the compile line flags (-D option):
          - <b> CFG_SR_MAX_MQP_RX_LEN</b> max length of the RX buffer for the server.
          - <b> CFG_SR_MAX_MQP_TX_LEN: </b> the constant buffer length allocated for a TX.\n\n
          - <b> CFG_SR_MAX_SUBTOP_LEN: </b> the maximum buffer size to hold a sub-topic.
          For e.g., in the topic /x/y/z, the phrase /x, /y and z are sub-topics. \n\n
          - <b> CFG_SR_MAX_TOPIC_NODE: </b> the maximum number of topic nodes in server.
          For e.g., in the topic /x/y/z, there are three nodes (/x, /y and z). \n\n
          - <b> CFG_SR_MAX_CL_ID_SIZE: </b> the maximum length of the client-id string. \n\n
          - <b> CFG_SR_MAX_NUM_CLIENT: </b> the maximum number of clients to be managed.
          Note this is different from the maximum number of 'contexts'. A large number
          of clients can be managed using fewer number of 'contexts' (connections). \n\n

          \note An app that has chosen not to await an ACK from the client for a
          scheduled transaction can benefit from the availability of control to
          pursue other activities to make overall progress in the system. However,
          an attempt to schedule another transaction with the client, while the
          previous one is still active, will cause the application to block. The
          app will unblock, once the previous transaction is completed.         

        */

        /** @defgroup sl_mqtt_srvr_api SL MQTT Server APIs
            @{
        */
  
        /** @defgroup sl_mqtt_srvr_evt SL MQTT Server Events
            @{ 
        */
#define SL_MQTT_SRVR_EVT_PUBACK   0x11  /**< PUBACK has been received from a client */
#define SL_MQTT_SRVR_EVT_NOCONN   0x12  /**< Server has lost network connection with a client */
        /** @} */ /* End SL MQTT Server events */
        
           
        /** Callbacks Routines
            The routines are invoked by SL MQTT Server Implementation onto Server application

            * \note The user applications implement the callbacks that are registered 
                    with the libraries. While using the MQTT library, invoking the 
                    core library APIs from a callback should be avoided and 
                    can lead to lockup scenarios. It is recommended to signal another 
                    task from the callback routines invoked from the library 
                    and invoke the core library API calls from that task.
         */
        typedef struct {
                
                /** Connect Request: Callback routine to indicate to server application that a CONNECT 
                request has been received by the server.
                                        
                    \param[in] clientId_str clientId field in the CONNECT message received.
                    \param[in] clientId_len length of ClientId
                    \param[in] username_str Username field in teh CONNECT message received.
                    \param[in] username_len length of username
                    \param[in] password_str Password field in the CONNECT message received.
                    \param[in] password_len length of password
                    \param[out] usr placeholder to provision app's handle to this connection.
                    \return 0x0000 or 0x0100 for success; Otherwise 0x02, 0x04 or 0x05 
                    refer to spec for connack codes
                    
                */
                _u16  (*sl_ExtLib_MqttConn)(_const char *clientId_str, _i32 clientId_len,
                                            _const char *username_str, _i32 username_len,
                                            _const char *password_str, _i32 password_len,
                                            void **usr);

                                      
                /** Callback routine to receive a PUBLISH from a client.
                    The server app must provide this routine for the instances where it 
                    receives PUBLISH messages from clients. The callback is
                    invoked in the context of the internal SL MQTT server Receive Task.
                    
                    \param[in] topstr name of topic on which PUBLISH is received by the server. Not NULL terminated.
                    \param[in] toplen length of the topic name 
                    \param[in] payload refers to payload published by the server.
                    \param[in] pay_len length of the payload.
                    \param[in] dup assert to indicate that it is a duplicate message sent by the client
                    \param[in] qoS quality of service of the received published message.
                    \param[in] retain asserted to indicate that a retained message has been received
                    \return none.
                */
                void (*sl_ExtLib_MqttRecv)(_const char *topstr,   _i32 toplen,
                                            _const void *payload, _i32 pay_len,
                                            bool dup, _u8 qos,
                                            bool retain);

                // Note: Double check whether retain, dup, qos would be required for a server app (with no topic management)

                /** DISCONNECT: Callback routine to indicate to the Server Application
                    that a client has disconnected.
                
                    \param[in] usr app's handle to this connection.
                    \param[in] due2err set to 1, if connection has been closed, without server
                    receiving a DISCONNECT messsage.
                    \return none.
                */
                void (*sl_ExtLib_MqttDisconn)(void *usr, bool due2err);

                
                /** Indication of event either from the server library or implementation generated.
                    TBD - Reserved for future use.
                    
                    \param[in] usr app's handle to this connection.
                    \param[in] evt identifier to the reported event. Refer to @ref sl_mqtt_srvr_evt - TBD
                    \param[in] buf points to buffer
                    \param[in] len length of buffer   
                    \return none.             
                */
                void (*sl_ExtLib_MqttEvent)(void *usr, _i32 evt, _const void *buf, _u32 len);
                
        } SlMqttServerCbs_t;

        /** Secure Socket Parameters to open a secure connection */
        typedef struct {

                #define SL_MQTT_SRVR_NETCONN_IP6  0x01  /**< Assert for IPv6 connection, otherwise  IPv4 */
                #define SL_MQTT_SRVR_NETCONN_URL  0x02  /**< Server address is an URL and not IP address */
                #define SL_MQTT_SRVR_NETCONN_SEC  0x04  /**< Connection to server  must  be secure (TLS) */

                _u32         netconn_flags; /**< Enumerate connection type  */
                _const char          *server_addr;   /**< Server Address: URL or IP  */
                _u16       port_number;   /**< Port number of MQTT server */
                _u8        method;        /**< Method to tcp secured socket */
                _u32         cipher;        /**< Cipher to tcp secured socket */
                _u32         n_files;       /**< Number of files for secure transfer */
                char * _const        *secure_files; /*SL needs 4 files*/
                
        } SlMqttServerParams_t;
        

        /** MQTT Server Lib structure which holds Initialization Data */
        typedef struct
        {
                SlMqttServerParams_t  server_info;      /**< Server information */
                _u16  loopback_port;    /**< Loopback port to manage lib internal functioning - Mandatory */
                _u32   rx_tsk_priority;  /**< Priority of the receive task in server */
                _u32    resp_time;        /**< Reasonable response wait time (seconds) for server */                
                bool aux_debug_en;  /**< Assert to indicate additional debug info */
                _i32 (*dbg_print)(_const char *pcFormat, ...); /**< Print debug information */
        } SlMqttServerCfg_t;
    
        /** Initialize the SL MQTT Server Implementation.
            A caller must initialize the MQTT Server implementation prior to using its services.
            
            \param[in] cfg refers to configuration parameters
            \param[in] cbs callbacks into server application 
            \return Success (0) or Failure (-1)
        */
        _i32 sl_ExtLib_MqttServerInit(_const SlMqttServerCfg_t *cfg,
                                       _const SlMqttServerCbs_t *cbs);

        /** Enroll a topic to receive data
            App can enroll a topic of interest and the SL layer will forward to the app any
            data subsequently published for the enrolled topic to the server by any of the
            connected MQTT clients.
 
            This is analogous or similar to a subsription to a topic by an MQTT client.
            
            \param[in] topic  enrollment topic for data to be received. It is NULL terminated.
            \return Success(0) or Failure (-1).
        */
        _i32 sl_ExtLib_MqttTopicEnroll(_const char *topic);

        static inline _i32 sl_ExtLib_MqttTopicSub(_const char *topic)
        {
                return sl_ExtLib_MqttTopicEnroll(topic);
        }
        
        /** DisEnroll a topic.
            App can cancel the previous enrollment of a topic and the SL layer will now stop
            forwarding data published for that topic to the server by the connected clients. 
            This is analogous oe similar to an unsubsription by an MQTT client.
            
            \param[in] topic  disenrollment topic. It is NULL terminated.
            \return Success(0) or Failure(-1).
        */
        _i32 sl_ExtLib_MqttTopicDisenroll(_const char *topic);

        static inline _i32 sl_ExtLib_MqttTopicUnsub(_const char *topic)
        {
                return sl_ExtLib_MqttTopicDisenroll(topic);
        }

        /** PUBLISH a named message to a client.
            In addition to the PUBLISH specific parameters, the caller can indicate
            whether the routine should block until the time, the message has been
            acknowleged by the client. This is applicable only for non-QoS0 messages.

            In case, the app has chosen not to await for the ACK from the client, 
            the SL MQTT Server implementation will notify the app about the received ACK
            through the callback routine.

            \note Only QoS0 and QoS1 messages are supported.

            \param[in] topic  topic of the data to be published. It is NULL terminated.
            \param[in] data   binary data to be published
            \param[in] len    length of the data
            \param[in] qos    QoS for the publish message
            \param[in] retain Retain bit in the PUBLISH message sent out
            \param[in] flags  Reserved for future use.
            \return Success(0) or Failure(-1).
        */
        _i32 sl_ExtLib_MqttServerSend(_const char *topic, _const void *data, _i32 len,
                                       _u8 qos, bool retain, _u32 flags);

        /** @} */ /* End SL MQTT Server API */

#ifdef __cplusplus  
}
#endif  



#endif // __SL_MQTT_SRVR_H__


