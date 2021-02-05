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
    #undef DEFAULT_MQTT_HOST
    #ifdef SOCK_HAS_IPV6
        #define DEFAULT_MQTT_HOST   "test6.mosquitto.org"
    #else
        #define DEFAULT_MQTT_HOST   "test.mosquitto.org"
    #endif
#else
    #ifdef SOCK_HAS_IPV4
        #undef  DEFAULT_MQTT_HOST_IPV4
        #define DEFAULT_MQTT_HOST_IPV4   "5.196.95.208"
    #endif
    #ifdef SOCK_HAS_IPV6
        #undef  DEFAULT_MQTT_HOST_IPV6
        #define DEFAULT_MQTT_HOST_IPV6   "2001:41d0:a:fed0::1"
    #endif
#endif

/* OPTIONAL */
#undef  TEST_MESSAGE
#define TEST_MESSAGE    "(  ) wolfmqtt multithread on RIOT OS"

#undef  WOLFMQTT_DISCONNECT_CB
#define WOLFMQTT_DISCONNECT_CB

#ifdef __cplusplus
}
#endif


#endif /* WOLFMQTT_USER_SETTINGS_H */

