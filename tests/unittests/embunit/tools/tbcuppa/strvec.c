#include "strvec.h"

#include <string.h> /* str... */
#include <stdlib.h> /* malloc, free */

strvec*
strvec_new(int initial_capacity) {
  strvec* result = (strvec*)malloc(sizeof(strvec));
  if ( result ) {
    result->size = 0;
    result->capa = 0;
    result->body = (char**)malloc(sizeof(char*)*initial_capacity);
    if ( result->body ) {
      result->capa = initial_capacity;
    }
  }
  return result;
}

void
strvec_clear(strvec* sv) {
  int i;
  for ( i = 0; i < sv->size; ++i ) {
    free(sv->body[i]);
  }
  sv->size = 0;
}

void
strvec_del(strvec* sv) {
  strvec_clear(sv);
  free(sv);
}

static void
strvec_grow(strvec* sv) {
  if ( sv->size >= sv->capa ) {
    int new_capa = sv->capa + 8;
    char** new_body = (char**)malloc(sizeof(char*) * new_capa);
    if ( new_body ) {
      int i;
      for ( i = 0; i < sv->size; ++i ) {
        new_body[i] = sv->body[i];
      }
      free(sv->body);
      sv->body = new_body;
      sv->capa = new_capa;
    }
  }
}

void
strvec_push_back(strvec* sv, const char* str) {
  strvec_grow(sv);
  sv->body[sv->size++] = strdup(str);
}

void
strvec_insert_before(strvec* sv, int pos, const char* str) {
  int i;
  if ( pos < 0 ) return;
  if ( pos >= sv->size ) return;
  strvec_grow(sv);
  for ( i = sv->size; i > pos; --i ) {
    sv->body[i] = sv->body[i-1];
  }
  sv->body[pos] = strdup(str);
  ++sv->size;
}

int
strvec_size(const strvec* sv) {
  return sv->size;
}

void
strvec_erase(strvec* sv, int pos) {
  int i;
  if ( pos < 0 ) return;
  if ( pos >= sv->size ) return;
  free(sv->body[pos]);
  for ( i = pos; i < sv->size - 1; ++i ) {
    sv->body[i] = sv->body[i+1];
  }
  --sv->size;
}

int
strvec_capacity(const strvec* sv) {
  return sv->capa;
}

const
char* strvec_get(const strvec* sv, int index) {
  if ( index >= 0 || index < sv->size ) {
    return sv->body[index];
  }
  return 0;
}

int
strvec_find(const strvec* sv, const char* str) {
  int i;
  for ( i = 0; i < sv->size; ++i ) {
    if ( strcmp(str, sv->body[i]) == 0 ) return i;
  }
  return -1;
}
