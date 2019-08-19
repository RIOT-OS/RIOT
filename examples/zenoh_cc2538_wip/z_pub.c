#pragma GCC diagnostic ignored "-Wint-conversion"


#include <stdio.h>
#include <unistd.h>
#include "zenoh.h"
#include "zenoh/recv_loop.h"

#include "zenoh/codec.h"

int main(void) {
  //~ char *locator = strdup("tcp/127.0.0.1:7447");
  char *locator = "tcp/127.0.0.1:7447";

  char *uri="/demo/hello/alpha";

  char *value="Hello World!";


  printf("Connecting to %s...\n", locator);
  z_zenoh_p_result_t r_z = z_open_wup(locator, "user", "password");
  ASSERT_RESULT(r_z, "Unable to open session with broker")
  z_zenoh_t *z = r_z.value.zenoh;
  z_start_recv_loop(z);  


  
  printf("Declaring Publisher...\n");
  z_pub_p_result_t r = z_declare_publisher(z, uri);
  ASSERT_P_RESULT(r, "Unable to declare pub\n");
  
  z_pub_t *pub = r.value.pub;

  z_iobuf_t data = z_iobuf_make(512);
  z_string_encode(&data, value);
  size_t len = z_iobuf_readable(&data);
  printf("Streaming Data...\n");
  while (1) {    
    z_stream_data(pub, data.buf, len); 
    z_write_data(z, "/demo/hello/beta", data.buf, len);
    z_write_data(z, "/demo/hello/gamma", data.buf, len);  
    z_write_data(z, "/demo/hello/eta", data.buf, len);  
    sleep(1);
  }

  return 0;
}
