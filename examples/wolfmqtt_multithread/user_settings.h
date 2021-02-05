/* user_settings.h : custom configuration for wolfmqtt */

#ifndef WOLFMQTT_USER_SETTINGS_H
#define WOLFMQTT_USER_SETTINGS_H


#ifdef __cplusplus
extern "C" {
#endif

/* MANDATORY */
#undef  RIOT_OS
#define RIOT_OS

#undef  WOLFMQTT_MULTITHREAD
#define WOLFMQTT_MULTITHREAD

#define WOLFMQTT_USE_HOSTNAME 1

#if WOLFMQTT_USE_HOSTNAME
    #undef  DEFAULT_MQTT_HOST
    #define DEFAULT_MQTT_HOST   "test.mosquitto.org"

    #if defined(MODULE_IPV6_ADDR)
        #undef  WOLFMQTT_DNS_SERVER_IPV6
        #define WOLFMQTT_DNS_SERVER_IPV6 "2001:4860:4860::8888"
    #else
        #undef  WOLFMQTT_DNS_SERVER_IPV4
        #define WOLFMQTT_DNS_SERVER_IPV4 "8.8.8.8"
    #endif
#else
    #if defined(MODULE_IPV6_ADDR)
        #undef  DEFAULT_MQTT_HOST
        #define DEFAULT_MQTT_HOST   "2001:41d0:a:fed0::1"
    #else
        #undef  DEFAULT_MQTT_HOST
        #define DEFAULT_MQTT_HOST   "5.196.95.208"
    #endif
#endif

/* OPTIONAL */
#undef  TEST_MESSAGE
#define TEST_MESSAGE    "wolfmqtt multithread on RIOT OS"

#undef  WOLFMQTT_DISCONNECT_CB
#define WOLFMQTT_DISCONNECT_CB

#ifdef __cplusplus
}
#endif


#endif /* WOLFMQTT_USER_SETTINGS_H */

