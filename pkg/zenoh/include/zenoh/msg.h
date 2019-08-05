#ifndef ZENOH_C_MSG_H_
#define ZENOH_C_MSG_H_

#include "zenoh/types.h"

/* Message ID */

#define Z_SCOUT           0x01
#define Z_HELLO           0x02

#define Z_OPEN            0x03
#define Z_ACCEPT          0x04
#define Z_CLOSE           0x05

#define Z_DECLARE         0x06

#define Z_COMPACT_DATA    0x07
#define Z_STREAM_DATA     0x1a
#define Z_BATCH_DATA      0x08
#define Z_WRITE_DATA      0x09

#define Z_QUERY           0x0a
#define Z_PULL            0x0b

#define Z_PING_PONG       0x0c

#define Z_SYNCH           0x0e
#define Z_ACKNACK         0x0f

#define Z_KEEP_ALIVE      0x10
#define Z_CONDUIT_CLOSE   0x11
#define Z_FRAGMENT        0x12
#define Z_CONDUIT         0x13
#define Z_MIGRATE         0x14

#define Z_REPLY           0x19

#define Z_RSPACE          0x18

/* Message Header _FLAGs */
#define Z_S_FLAG  0x20
#define Z_M_FLAG  0x20
#define Z_P_FLAG  0x20

#define Z_R_FLAG  0x40
#define Z_N_FLAG  0x40
#define Z_C_FLAG  0x40

#define Z_A_FLAG  0x80
#define Z_U_FLAG  0x80

#define Z_Z_FLAG  0x80
#define Z_L_FLAG  0x20
#define Z_H_FLAG  0x40

#define Z_G_FLAG  0x80
#define Z_I_FLAG  0x20
#define Z_F_FLAG  0x80
#define Z_O_FLAG  0x20

#define Z_MID_MASK 0x1f
#define Z_FLAGS_MASK = 0xe0

#define Z_HAS_FLAG(h, f) ((h & f) != 0)
#define Z_MID(h) (Z_MID_MASK & h)
#define Z_FLAGS(h) (Z_FLAGS_MASK & h)

/* Scout Flags */
#define Z_SCOUT_BROKER 0x01


/* Declaration Id */ 

#define Z_RESOURCE_DECL  0x01
#define Z_PUBLISHER_DECL  0x02
#define Z_SUBSCRIBER_DECL  0x03
#define Z_SELECTION_DECL  0x04
#define Z_BINDING_DECL  0x05
#define Z_COMMIT_DECL  0x06
#define Z_RESULT_DECL  0x07
#define Z_FORGET_RESOURCE_DECL  0x08
#define Z_FORGET_PUBLISHER_DECL  0x09
#define Z_FORGET_SUBSCRIBER_DECL  0x0a
#define Z_FORGET_SELECTION_DECL  0x0b
#define Z_STORAGE_DECL  0x0c
#define Z_FORGET_STORAGE_DECL  0x0d

#define Z_PUSH_MODE 0x01
#define Z_PULL_MODE 0x02
#define Z_PERIODIC_PUSH_MODE 0x03
#define Z_PERIODIC_PULL_MODE 0x04

/* Close Reasons */
#define Z_PEER_CLOSE 0
#define Z_ERROR_CLOSE 1

/* Payload Header */
#define Z_SRC_ID 0x01
#define Z_SRC_SN 0x02
#define Z_BRK_ID 0x04
#define Z_BRK_SN 0x08
#define Z_T_STAMP 0x10
#define Z_KIND 0x20
#define Z_ENCODING 0x40

typedef struct {
  z_vle_t id;
  z_array_uint8_t value;
} z_property_t;
Z_RESULT_DECLARE (z_property_t, property)

/*
 * Creates a new property with the given id and name. Notice that the ownership
 * for the name remains with the caller.
 */ 
z_property_t* z_property_make(z_vle_t id, z_array_uint8_t value);
void z_property_free(z_property_t** p);

#define HAS_PROPERTIES (m) (m.properties != 0) 

/*------------------ Scout Message ------------------*/
typedef struct {  
  z_vle_t mask;  
} z_scout_t;

/*------------------ Hello Message ------------------*/
typedef struct {  
  z_vle_t mask;
  z_vec_t *locators;  
} z_hello_t;

/*------------------ Open Message ------------------*/
typedef struct {  
  uint8_t version;  
  z_array_uint8_t pid; 
  z_vle_t lease;    
  // z_vec_t *locators; 
} z_open_t;


/*------------------ Accept Message ------------------*/
typedef struct {    
  z_array_uint8_t client_pid;
  z_array_uint8_t broker_pid; 
  z_vle_t lease;   
} z_accept_t;

/*------------------ Close Message ------------------*/
typedef struct {  
  z_array_uint8_t pid;
  uint8_t reason;
} z_close_t; 

/*------------------  Resource Declaration Message ------------------*/
// in types.h

/*------------------ Delcare Publisher ------------------*/
typedef struct {   
  z_vle_t rid;    
} z_pub_decl_t;

/*------------------ Forget Publisher Message ------------------*/
typedef struct {   
  z_vle_t rid; 
} z_forget_pub_decl_t;

/*------------------ Declare Storage ------------------*/
typedef struct {   
  z_vle_t rid;    
} z_storage_decl_t;

/*------------------ Forget Storage Message ------------------*/
typedef struct {   
  z_vle_t rid; 
} z_forget_sto_decl_t;

/*------------------ Declare Subscriber Message ------------------*/
typedef struct {   
  z_vle_t rid;  
  z_sub_mode_t sub_mode;  
} z_sub_decl_t;

/*------------------ Forget Subscriber Message ------------------*/
typedef struct {   
  z_vle_t rid; 
} z_forget_sub_decl_t;

/*------------------ Declaration Commit Message ------------------*/
typedef struct {  
  uint8_t cid;
} z_commit_decl_t;

/*------------------ Declaration Result  Message ------------------*/
typedef struct {  
  uint8_t cid;
  uint8_t status;
} z_result_decl_t;

/**
 *  On the wire this is represented as:
 *     |header|payload|properties|
 *  The declaration of the structure does not follow this
 *  convention for alignement purposes.
 */
typedef struct {  
  z_vec_t* properties;
  union {
    z_res_decl_t resource;
    z_pub_decl_t pub;
    z_sub_decl_t sub;
    z_storage_decl_t storage;
    z_forget_pub_decl_t forget_pub;
    z_forget_sub_decl_t forget_sub;
    z_forget_sto_decl_t forget_sto;
    z_commit_decl_t commit;
    z_result_decl_t result;
  } payload;  
  uint8_t header; 
} z_declaration_t;

Z_ARRAY_DECLARE(z_declaration_t)

/*------------------ Declare Messages ------------------*/
typedef struct  {  
  z_vle_t sn;
  z_array_z_declaration_t declarations;
} z_declare_t;


/*------------------ Compact Data Message ------------------*/
typedef struct {  
  z_vle_t sn;
  z_vle_t rid;
  z_iobuf_t payload;
} z_compact_data_t;


/*------------------ Payload Header ------------------*/
typedef struct {  
  z_vle_t src_sn;
  z_vle_t brk_sn;
  z_vle_t kind;
  z_vle_t encoding;
  uint8_t src_id[16];
  uint8_t brk_id[16];
  uint8_t flags;  
  z_timestamp_t tstamp;
  z_iobuf_t payload;
} z_payload_header_t;

/*------------------ StreamData Message ------------------*/
typedef struct {  
  z_vle_t sn;
  z_vle_t rid;
  z_iobuf_t payload_header;
} z_stream_data_t;

/*------------------ Write Data Message ------------------*/
typedef struct {  
  z_vle_t sn;
  char* rname;
  z_iobuf_t payload_header;
} z_write_data_t;

/*------------------ Query Message ------------------*/
typedef struct {
  z_array_uint8_t pid; 
  z_vle_t qid;
  char* rname;
  char* predicate;
} z_query_t;

/*------------------ Reply Message ------------------*/
typedef struct {
  z_array_uint8_t qpid; 
  z_vle_t qid;
  z_array_uint8_t stoid;
  z_vle_t rsn;
  char* rname;
  z_iobuf_t payload_header;
} z_reply_t;


/**
 *  On the wire this is represented as:
 *     |header|payload|properties|
 *  The declaration of the structure does not follow this
 *  convention for alignement purposes.
 */
typedef struct {
  const z_vec_t* properties;  
  union {
    z_open_t open;
    z_accept_t accept;
    z_close_t close;
    z_declare_t declare;
    z_compact_data_t compact_data;
    z_stream_data_t stream_data;
    z_write_data_t write_data;
    z_query_t query;
    z_reply_t reply;
    z_scout_t scout;
    z_hello_t hello;
  } payload;
  uint8_t header; 
} z_message_t;

Z_RESULT_DECLARE (z_accept_t, accept)
Z_RESULT_DECLARE (z_close_t, close)
Z_RESULT_DECLARE (z_declare_t, declare)
Z_RESULT_DECLARE (z_declaration_t, declaration)
Z_RESULT_DECLARE (z_res_decl_t, res_decl)
Z_RESULT_DECLARE (z_pub_decl_t, pub_decl)
Z_RESULT_DECLARE (z_temporal_property_t, temporal_property)
Z_RESULT_DECLARE (z_sub_mode_t, sub_mode)
Z_RESULT_DECLARE (z_sub_decl_t, sub_decl)
Z_RESULT_DECLARE (z_commit_decl_t, commit_decl)
Z_RESULT_DECLARE (z_result_decl_t, result_decl)
Z_RESULT_DECLARE (z_compact_data_t, compact_data)
Z_RESULT_DECLARE (z_payload_header_t, payload_header)
Z_RESULT_DECLARE (z_stream_data_t, stream_data)
Z_RESULT_DECLARE (z_write_data_t, write_data)
Z_RESULT_DECLARE (z_query_t, query)
Z_RESULT_DECLARE (z_reply_t, reply)
Z_P_RESULT_DECLARE (z_message_t, message)

#endif /* ZENOH_C_MSG_H_ */
