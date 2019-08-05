#ifndef ZENOH_C_CODEC_H
#define ZENOH_C_CODEC_H

#include "zenoh/msg.h"
#include "zenoh/result.h"

#define DECLARE_MSG_ENCODE(name) \
void z_ ##name ##_encode(z_iobuf_t* buf, const z_ ##name ##_t* m)

#define DECLARE_MSG_DECODE(name) \
z_ ##name ##_result_t z_ ##name ## _decode(z_iobuf_t* buf, uint8_t header); \
void  z_ ## name ## _decode_na(z_iobuf_t* buf, uint8_t header, z_ ##name ##_result_t *r)

#define DECLARE_MSG_DECODE_NOH(name) \
z_ ##name ##_result_t z_ ##name ## _decode(z_iobuf_t* buf); \
void  z_ ## name ## _decode_na(z_iobuf_t* buf, z_ ##name ##_result_t *r)



#define DECLARE_MSG_CODEC(name)\
  DECLARE_MSG_DECODE(name); \
  DECLARE_MSG_ENCODE(name)


void z_vle_encode(z_iobuf_t* buf, z_vle_t v);
z_vle_result_t z_vle_decode(z_iobuf_t* buf);

void z_array_uint8_encode(z_iobuf_t* buf, const z_array_uint8_t* bs);
z_array_uint8_result_t z_array_uint8_decode(z_iobuf_t* buf);

void z_string_encode(z_iobuf_t* buf, const char* s);
z_string_result_t z_string_decode(z_iobuf_t* buf);


DECLARE_MSG_ENCODE(open);
DECLARE_MSG_DECODE_NOH(accept);

DECLARE_MSG_ENCODE(close);
DECLARE_MSG_DECODE_NOH(close);

DECLARE_MSG_ENCODE(property);
DECLARE_MSG_DECODE_NOH(property);

DECLARE_MSG_ENCODE(declare);
DECLARE_MSG_DECODE_NOH(declare);

DECLARE_MSG_ENCODE(temporal_property);
DECLARE_MSG_DECODE_NOH(temporal_property);

DECLARE_MSG_ENCODE(sub_mode);
DECLARE_MSG_DECODE_NOH(sub_mode);

DECLARE_MSG_ENCODE(compact_data);
DECLARE_MSG_DECODE_NOH(compact_data);

DECLARE_MSG_ENCODE(payload_header);
DECLARE_MSG_DECODE_NOH(payload_header);

DECLARE_MSG_ENCODE(stream_data);
DECLARE_MSG_DECODE_NOH(stream_data);

DECLARE_MSG_ENCODE(write_data);
DECLARE_MSG_DECODE_NOH(write_data);

DECLARE_MSG_ENCODE(query);
DECLARE_MSG_DECODE_NOH(query);

void z_message_encode(z_iobuf_t* buf, const z_message_t* m);
z_message_p_result_t z_message_decode(z_iobuf_t* buf);
void z_message_decode_na(z_iobuf_t* buf, z_message_p_result_t *r);
#endif /* ZENOH_C_CODEC_H */

