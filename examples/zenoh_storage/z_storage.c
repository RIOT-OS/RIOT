#include <stdio.h>
#include <unistd.h>
#include "zenoh.h"
#include "zenoh/recv_loop.h"
#include "zenoh/rname.h"


typedef struct sample {
  char *rname;
  char *data;
  size_t length;
} sample_t;

z_list_t *stored = 0;

int remove_data(void *elem, void*args){
  sample_t *sample = (sample_t*)elem;
  if(strcmp(sample->rname, (char *)args) == 0){
    free(sample->rname);
    free(sample->data);
    return 1;
  }
  return 0;
}

void listener(const z_resource_id_t *rid, const unsigned char *data, size_t length, const z_data_info_t *info, void *arg) {    
  Z_UNUSED_ARG_2(info, arg);
  printf("Received data: %s\n", rid->id.rname);
  stored = z_list_remove(stored, remove_data, rid->id.rname);

  sample_t *sample = (sample_t *)malloc(sizeof(sample_t));
  sample->rname = strdup(rid->id.rname);
  sample->data = malloc(length);
  memcpy(sample->data, data, length);
  sample->length = length;

  stored = z_list_cons(stored, sample);
}

void query_handler(const char *rname, const char *predicate, z_array_resource_t *replies, void *arg) {
  Z_UNUSED_ARG_2(predicate, arg);
  printf("Handling Query: %s\n", rname);
  z_list_t *matching_samples = 0;

  z_list_t *samples = stored;
  sample_t *sample;
  while (samples != z_list_empty) {
    sample = (sample_t *) z_list_head(samples);
    if(intersect((char *)rname, sample->rname))
    {
      matching_samples = z_list_cons(matching_samples, sample);
    }
    samples = z_list_tail(samples);
  }
  replies->length = z_list_len(matching_samples);
  replies->elem = (z_resource_t**)malloc(sizeof(z_resource_t *) * replies->length);
  
  samples = matching_samples; 
  int i =0;
  while (samples != z_list_empty) {
    sample = (sample_t *) z_list_head(samples);
    replies->elem[i] = (z_resource_t *)malloc(sizeof(z_resource_t));
    replies->elem[i]->rname = sample->rname;
    replies->elem[i]->data = (const unsigned char *)sample->data;
    replies->elem[i]->length = sample->length;
    replies->elem[i]->encoding = 0;
    replies->elem[i]->kind = 0;
    samples = z_list_tail(samples);
    ++i;
  }
  z_list_free(&matching_samples);
}

void replies_cleaner(z_array_resource_t *replies, void *arg)
{
  Z_UNUSED_ARG(arg);
  printf("Cleaning Replies.\n");
  for (unsigned int i = 0; i < replies->length; ++i) {
    free(replies->elem[i]);
  }  
  free(replies->elem);

}


int main(void) {
  char *locator = strdup("tcp/127.0.0.1:7447");

  char *uri="/demo/**";


  printf("Connecting to %s...\n", locator);
  z_zenoh_p_result_t r_z = z_open(locator, 0, 0);
  ASSERT_RESULT(r_z, "Unable to open session with broker")
  z_zenoh_t *z = r_z.value.zenoh;

  z_start_recv_loop(z);
  printf("Declaring Storage: %s\n", uri);
  z_declare_storage(z, uri, listener, query_handler, replies_cleaner, NULL);

 // while (1) { 
   // sleep(1);
 // }
  return 0;
}
