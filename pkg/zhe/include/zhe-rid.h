#ifndef ZHE_RID_H
#define ZHE_RID_H

#if ZHE_MAX_RID <= UINT8_MAX/2
#define ZHE_RID_SIZE 8
typedef uint8_t zhe_rid_t;
#elif ZHE_MAX_RID <= UINT16_MAX/2
#define ZHE_RID_SIZE 16
typedef uint16_t zhe_rid_t;
#elif ZHE_MAX_RID <= UINT32_MAX/2
#define ZHE_RID_SIZE 32
typedef uint32_t zhe_rid_t;
#elif ZHE_MAX_RID <= UINT64_MAX/2
#define ZHE_RID_SIZE 64
typedef uint64_t zhe_rid_t;
#else
#error "ZHE_MAX_RID out of range"
#endif

#endif
