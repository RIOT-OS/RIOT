#pragma GCC diagnostic ignored "-Wstrict-prototypes"

#ifndef ZENOH_C_MVAR_H_
#define ZENOH_C_MVAR_H_


typedef void* z_mvar_t;

z_mvar_t *z_mvar_empty();
int z_mvar_is_empty(z_mvar_t *mv);

z_mvar_t *z_mvar_of(void *e);
void * z_mvar_get(z_mvar_t *mv);
void z_mvar_put(z_mvar_t *mv, void *e);


#endif /* ZENOH_C_MVAR_H_ */

