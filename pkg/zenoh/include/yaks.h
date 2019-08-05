#ifndef ZENOH_C_H_YAKS_DEFINED_
#define ZENOH_C_H_YAKS_DEFINED_

#define Y_RAW_ENC             0x00
#define Y_CUSTOM_ENC          0x01
#define Y_STRING_ENC          0x02
#define Y_PROPERTIES_ENC      0x03
#define Y_JSON_ENC            0x04
#define Y_SQL_ENC             0x05

#define Y_PUT 0x00
#define Y_UPDATE 0x01
#define Y_REMOVE 0x02

#include "zenoh/config.h"
#include "zenoh/msg.h"

int y_put(z_zenoh_t *z, const char *path, const unsigned char *data, size_t length, int encoding);
int y_remove(z_zenoh_t *z, const char *path, int encoding);

z_sub_p_result_t 
y_subscribe(z_zenoh_t *z, const char *selector, subscriber_callback_t callback, void *arg);


#endif /* ZENOH_C_H_YAKS_DEFINED_ */
