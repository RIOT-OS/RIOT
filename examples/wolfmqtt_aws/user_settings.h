/* user_settings.h : custom configuration for wolfmqtt */

#ifndef WOLFMQTT_USER_SETTINGS_H
#define WOLFMQTT_USER_SETTINGS_H


#ifdef __cplusplus
extern "C" {
#endif

/* MANDATORY */
#undef  RIOT_OS
#define RIOT_OS

#undef  ENABLE_AWSIOT_EXAMPLE
#define ENABLE_AWSIOT_EXAMPLE

#define WOLFMQTT_USE_HOSTNAME 1

#if WOLFMQTT_USE_HOSTNAME
    #undef  AWSIOT_HOST
    #define AWSIOT_HOST   "a2dujmi05ideo2.iot.us-west-2.amazonaws.com"

    #if defined(MODULE_IPV6_ADDR)
        #undef  WOLFMQTT_DNS_SERVER_IPV6
        #define WOLFMQTT_DNS_SERVER_IPV6 "2001:4860:4860::8888"
    #else
        #undef  WOLFMQTT_DNS_SERVER_IPV4
        #define WOLFMQTT_DNS_SERVER_IPV4 "8.8.8.8"
    #endif
#else
    #if defined(MODULE_IPV6_ADDR)
        #undef  AWSIOT_HOST
        #define AWSIOT_HOST   "2620:108:700f::2351:30f8"
    #else
        #undef  AWSIOT_HOST
        #define AWSIOT_HOST   "34.209.16.27"
    #endif
#endif

#undef  ENABLE_MQTT_TLS
#define ENABLE_MQTT_TLS

/* OPTIONAL */
#undef  TEST_MESSAGE
#define TEST_MESSAGE    "wolfmqtt aws on RIOT OS"

#ifdef __cplusplus
}
#endif


#endif /* WOLFMQTT_USER_SETTINGS_H */

