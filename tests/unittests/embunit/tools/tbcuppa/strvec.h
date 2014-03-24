#ifndef STRVEC_H
#define STRVEC_H

#ifdef __cplusplus
extern "C" {
#endif

struct strvec_t {
  char** body;
  int    size;
  int    capa;
};

typedef struct strvec_t strvec;

strvec*     strvec_new(int initial_capacity);
void        strvec_del(strvec* sv);

void        strvec_push_back(strvec* sv, const char* str);
void        strvec_insert_before(strvec* sv, int n, const char* str);
void        strvec_erase(strvec* sv, int n);
int         strvec_size(const strvec* sv);
int         strvec_capacity(const strvec* sv);
const char* strvec_get(const strvec* sv, int n);
void        strvec_clear(strvec* sv);
int         strvec_find(const strvec* sv, const char* str);

#ifdef __cplusplus
}
#endif

#endif
