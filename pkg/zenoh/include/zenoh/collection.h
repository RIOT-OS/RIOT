#ifndef ZENOH_C_VECTOR_H
#define ZENOH_C_VECTOR_H

#define DEFAULT_I_MAP_CAPACITY 64

#define Z_ARRAY_DECLARE(T) \
typedef struct { \
  unsigned int length; \
  T* elem; \
} z_array_##T;

#define Z_ARRAY_P_DECLARE(T) \
typedef struct { \
  unsigned int length; \
  T** elem; \
} z_array_##T;

#define Z_ARRAY_DECLARE_Z_TYPE(T) \
typedef struct { \
  unsigned int length; \
  z_##T* elem; \
} z_array_##T;

#define Z_ARRAY_P_DECLARE_Z_TYPE(T) \
typedef struct { \
  unsigned int length; \
  z_##T** elem; \
} z_array_##T;

#define Z_ARRAY_S_DEFINE(T, arr, len) \
z_array_##T arr = {len, (T*)malloc(len*sizeof(T))};

#define Z_ARRAY_P_S_DEFINE(T, arr, len) \
z_array_##T arr = {len, (T**)malloc(len*sizeof(T*))};

#define Z_ARRAY_S_INIT(T, arr, len) \
arr.length = len; \
arr.elem =  (T*)malloc(len*sizeof(T));

#define Z_ARRAY_P_S_INIT(T, arr, len) \
arr.length = len; \
arr.elem =  (T**)malloc(len*sizeof(T*));

#define Z_ARRAY_H_INIT(T, arr, len) \
arr->length = len; \
arr->elem =  (T*)malloc(len*sizeof(T))

#define Z_ARRAY_S_COPY(T, dst, src) \
dst.length = src.length; \
dst.elem = (T*)malloc(dst.length*sizeof(T)); \
memcpy(dst.elem, src.elem, dst.length);

#define Z_ARRAY_H_COPY(T, dst, src) \
dst->length = src->length; \
dst->elem =  (T*)malloc(dst->length*sizeof(T)); \
memcpy(dst->elem, src->elem, dst->length);

#define Z_ARRAY_S_Z_TYPE_DEFINE(T, arr, len) \
z_array_##T arr = {len, (z_##T*)malloc(len*sizeof(z_##T))};


#define Z_ARRAY_H_DEFINE(T, arr, len) \
z_array_##T * arr = (z_array_##T*)malloc(sizeof(z_array_##T)); \
arr->length = len; \
arr->elem = (T*)malloc(len*sizeof(T));

#define Z_ARRAY_H_Z_TYPE_DEFINE(T, arr, len) \
z_array_##T * arr = (z_array_##T*)malloc(sizeof(z_array_##T)); \
arr->length = len; \
arr->elem = (z_##T*)malloc(len*sizeof(z_##T));


#define Z_ARRAY_S_FREE(arr) \
free(arr.elem); \
arr.elem = 0; \
arr.length = 0;

#define Z_ARRAY_H_FREE(arr) \
free(arr->elem); \
arr->elem = 0; \
arr->length = 0

typedef struct {
  unsigned int capacity_;
  unsigned int length_;
  void** elem_; 
} z_vec_t;

z_vec_t z_vec_make(unsigned int capacity);
z_vec_t z_vec_clone(const z_vec_t* v);
void z_vec_free(z_vec_t* v);

unsigned int z_vec_length(const z_vec_t* v);
/* 
 * Append an element ot the vector and takes ownership of the appended element.
 */
void z_vec_append(z_vec_t* v, void* e);

const void* z_vec_get(const z_vec_t* v, unsigned int i);

/* 
 * Set the element at the i-th position of the vector and takes ownership.
 */
void z_vec_set(z_vec_t* sv, unsigned int i, void* e);


/*-------- Linked List --------*/

typedef int (*z_list_predicate)(void *, void *);
typedef struct z_list  {
  void *elem;
  struct z_list *tail;
} z_list_t;
extern z_list_t * z_list_empty;
z_list_t * z_list_of(void *x);
z_list_t * z_list_cons(z_list_t *xs, void *x);
void * z_list_head(z_list_t *xs);
z_list_t * z_list_tail(z_list_t *xs);
unsigned int z_list_len(z_list_t *xs);
z_list_t * z_list_remove(z_list_t *xs, z_list_predicate p, void *arg);

/**
 * Drops the element at the specified position. 
 */
z_list_t * z_list_drop_elem(z_list_t *xs, unsigned int position);
void z_list_free(z_list_t **xs);

/*-------- Int Map --------*/
typedef struct {
  int key;
  void *value;
} z_i_map_entry_t;

typedef struct {
  z_list_t **elems;
  unsigned int capacity;
  unsigned int n;  
} z_i_map_t;

extern z_i_map_t *z_i_map_empty;
z_i_map_t *z_i_map_make(unsigned int capacity);
void z_i_map_free(z_i_map_t **map);
void z_i_map_set(z_i_map_t *map, int k, void *v);
void *z_i_map_get(z_i_map_t *map, int k);
void z_i_map_remove(z_i_map_t *map, int k);
unsigned int z_i_map_capacity(z_i_map_t *map);
unsigned int z_i_map_size(z_i_map_t *map);

#endif /* ZENOH_C_VECTOR_H */
