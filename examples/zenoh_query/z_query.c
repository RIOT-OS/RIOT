#include <stdio.h>
#include <unistd.h>
#include "zenoh.h"
#include "zenoh/recv_loop.h"

#include "zenoh/codec.h"

void reply_handler(const z_reply_value_t *reply, void *arg) {
  Z_UNUSED_ARG(arg);
  z_string_result_t r_s;
  z_iobuf_t buf;
  switch (reply->kind) {
    case Z_STORAGE_DATA: 
      buf = z_iobuf_wrap_wo((unsigned char *)reply->data, reply->data_length, 0, reply->data_length);
      r_s = z_string_decode(&buf);        
      if (r_s.tag == Z_OK_TAG) {
        printf("Received Storage Data. (%s, %s)\n", reply->rname, r_s.value.string);
        free(r_s.value.string);
      } else {
        printf("Received Storage Data. %s:...\n", reply->rname);
      }
      break;
    case Z_STORAGE_FINAL:
      printf("Received Storage Final.\n");
      break;
    case Z_REPLY_FINAL:
      printf(".\n");
      break;
  }
}

int main(void) {
  char *locator = strdup("tcp/127.0.0.1:7447");

  char *uri="/demo/**";


  printf("Connecting to %s...\n", locator);
  z_zenoh_p_result_t r_z = z_open(locator, 0, 0);
  ASSERT_RESULT(r_z, "Unable to open session with broker")
  z_zenoh_t *z = r_z.value.zenoh;

  z_start_recv_loop(z);
  printf("Send Query...\n");
  while (1) {

    if (z_query(z, uri, "", reply_handler, NULL) != 0) {
      printf("Unable to query\n");
      return -1;
    }
    usleep(100000);
  }
  
  return 0;
}
