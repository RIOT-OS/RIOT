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

#if defined(MODULE_IPV4_ADDR)
    #undef  DEFAULT_MQTT_HOST
    #define DEFAULT_MQTT_HOST   "192.168.2.110" // local
#elif defined(MODULE_IPV6_ADDR)
    #undef  DEFAULT_MQTT_HOST
    #define DEFAULT_MQTT_HOST   "2003:e9:f44:2f25:95dc:c243:6b3b:a257" // local
#endif

#undef  DEFAULT_MQTT_PORT
#define DEFAULT_MQTT_PORT   10000

/* OPTIONAL */
#undef  TEST_MESSAGE
#define TEST_MESSAGE    "wolfmqtt snclient on RIOT OS"

#ifdef __cplusplus
}
#endif


#endif /* WOLFMQTT_USER_SETTINGS_H */

