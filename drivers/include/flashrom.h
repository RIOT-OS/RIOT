#ifndef FLASHROM_H
#define FLASHROM_H

#include <stdint.h>
#include <stddef.h>

/*
 * @brief Erase sector
 *
 * @param addr      Address within a flash sector to erase
 *
 * @return          1 on success, 0 otherwise
 */
uint8_t          	flashrom_erase(uint8_t *addr);

/* @brief Write buffer from ram to flash
 *
 * @param dst       Address within a flash sector to write, must be a 256 byte boundary
 * @param src       Address within ram, must be a word boundary
 * @param size      Bytes to write
 *
 * @return          1 on success, 0 otherwise
 */
uint8_t 			flashrom_write(uint8_t *dst, char *src, size_t size);


#endif /* FLASHROM_H */
