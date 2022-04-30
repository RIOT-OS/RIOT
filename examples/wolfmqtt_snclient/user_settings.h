/* user_settings.h : custom configuration for wolfmqtt */

#ifndef WOLFMQTT_USER_SETTINGS_H
#define WOLFMQTT_USER_SETTINGS_H


#ifdef __cplusplus
extern "C" {
#endif

/* MANDATORY */
#undef  RIOT_OS
#define RIOT_OS

#undef  WOLFMQTT_SN
#define WOLFMQTT_SN

#define WOLFMQTT_USE_HOSTNAME 0

#if WOLFMQTT_USE_HOSTNAME
    #undef DEFAULT_MQTT_HOST
    #ifdef SOCK_HAS_IPV6
        #define DEFAULT_MQTT_HOST   "TODO" /* if you know a public mqttsn service put it here */
    #else
        #define DEFAULT_MQTT_HOST   "TODO" /* if you know a public mqttsn service put it here */
    #endif
#else
    #ifdef SOCK_HAS_IPV4
        #undef  DEFAULT_MQTT_HOST_IPV4
        #define DEFAULT_MQTT_HOST_IPV4   "192.168.2.113" /* local */
    #endif
    #ifdef SOCK_HAS_IPV6
        #undef  DEFAULT_MQTT_HOST_IPV6
        #define DEFAULT_MQTT_HOST_IPV6   "2003:e9:f48:f000:6c89:4139:63e5:f3ff" /* local */
    #endif
#endif

#undef  DEFAULT_MQTT_PORT
#define DEFAULT_MQTT_PORT   10006

/* OPTIONAL */
#undef  TEST_MESSAGE
#define TEST_MESSAGE    "wolfmqtt snclient on RIOT OS"

#ifdef __cplusplus
}
#endif


#endif /* WOLFMQTT_USER_SETTINGS_H */

