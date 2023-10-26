/*
 * Copyright (C) 2022 Juliusz Neuman
 */

/**
 * @defgroup    sys_ps IKE
 * @ingroup     sys
 * @brief       Control IKE daemon
 * @{
 *
 * @file
 * @brief       IKE
 *
 * @author      Juliusz Neuman <neuman.juliusz@gmail.com>
 */

#ifndef IKE_CHUNK_H
#define IKE_CHUNK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>


typedef struct {
    size_t len;
    char *ptr;
} chunk_t;

#define chunk_from_buff(buff) {.len = countof(buff), .ptr = (char*)buff}

void free_chunk(chunk_t *chunk);

chunk_t malloc_chunk(size_t size);

void printf_chunk(chunk_t chunk, uint8_t width);

extern chunk_t empty_chunk;

#ifdef __cplusplus
}
#endif

#endif /* IKE_CHUNK_H */
/** @} */
