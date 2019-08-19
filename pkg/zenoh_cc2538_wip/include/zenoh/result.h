#ifndef ZENOH_C_RESULT_H
#define ZENOH_C_RESULT_H

#define Z_VLE_PARSE_ERROR 0x01
#define Z_ARRAY_PARSE_ERROR 0x02
#define Z_STRING_PARSE_ERROR 0x03
#define Z_PROPERTY_PARSE_ERROR 0x04
#define Z_PROPERTIES_PARSE_ERROR 0x05
#define Z_MESSAGE_PARSE_ERROR 0x06
#define Z_INSUFFICIENT_IOBUF_SIZE 0x07
#define Z_IO_ERROR 0x08
#define Z_RESOURCE_DECL_ERROR 0x09
#define Z_PAYLOAD_HEADER_PARSE_ERROR 0x0a
#define Z_TX_CONNECTION_ERROR 0x0b
#define Z_INVALID_ADDRESS_ERROR 0x0c
#define Z_FAILED_TO_OPEN_SESSION 0x0d
#define Z_UNEXPECTED_MESSAGE 0x7f



#define Z_RESULT_DECLARE(type, name) \
typedef struct { \
  enum result_kind tag; \
  union { \
    type  name; \
    int error; \
  } value;\
} z_ ## name ## _result_t;

#define Z_P_RESULT_DECLARE(type, name) \
typedef struct { \
  enum result_kind tag; \
  union { \
    type * name; \
    int error; \
  } value;\
} z_ ## name ## _p_result_t; \
inline static void z_ ## name ## _p_result_init(z_ ## name ## _p_result_t *r) { \
     r->value.name = (type *)malloc(sizeof(type)); \
} \
inline static void z_ ## name ## _p_result_free(z_ ## name ## _p_result_t *r) { \
    free(r->value.name); \
    r->value.name = 0; \
} 

#define ASSURE_RESULT(in_r, out_r, e) \
  if (in_r.tag == Z_ERROR_TAG) { \
    out_r.tag = Z_ERROR_TAG; \
    out_r.value.error = e; \
    return out_r; \
  }

#define ASSURE_P_RESULT(in_r, out_r, e) \
  if (in_r.tag == Z_ERROR_TAG) { \
    out_r->tag = Z_ERROR_TAG; \
    out_r->value.error = e; \
    return; \
  }
#define ASSERT_RESULT(r, msg) \
  if (r.tag == Z_ERROR_TAG) { \
    printf(msg); \
    printf("\n"); \
    exit(r.value.error); \
  }

#define ASSERT_P_RESULT(r, msg) \
  if (r.tag == Z_ERROR_TAG) { \
    printf(msg); \
    printf("\n"); \
    exit(r.value.error); \
  }

enum result_kind {
  Z_OK_TAG = 0,
  Z_ERROR_TAG = 1   
};

Z_RESULT_DECLARE (char*, string)

#endif /* ZENOH_C_RESULT_H */
