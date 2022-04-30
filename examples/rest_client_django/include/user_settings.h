/* user_settings.h : custom configuration for wolfmqtt */

#ifndef WOLFMQTT_USER_SETTINGS_H
#define WOLFMQTT_USER_SETTINGS_H

#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_REST_CLIENT_TRANSPORT_MQTTSN)
#undef  WOLFMQTT_SN
#define WOLFMQTT_SN
#endif

/* OPTIONAL */

//#undef  WOLFMQTT_NONBLOCK
//#define WOLFMQTT_NONBLOCK

//#undef  WOLFMQTT_NO_TIMEOUT
//#define WOLFMQTT_NO_TIMEOUT

//#undef  WOLFMQTT_CLIENT_ID
//#define WOLFMQTT_CLIENT_ID

#if !IS_USED(MODULE_REST_CLIENT_DEBUG)
#undef  WOLFMQTT_NO_ERROR_STRINGS
#define WOLFMQTT_NO_ERROR_STRINGS
#endif

#ifdef __cplusplus
}
#endif


#endif /* WOLFMQTT_USER_SETTINGS_H */

