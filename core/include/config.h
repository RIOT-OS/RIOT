#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define CONFIG_KEY      (0x1701)

extern char configmem[];

/* @brief:  Stores configuration data of the node */
typedef struct {
    uint16_t id;                ///< unique node identifier
    uint8_t radio_address;      ///< address for radio communication
    uint8_t radio_channel;      ///< current frequency
} config_t;

/* @brief:  Element to store in flashrom */
typedef struct {
    uint16_t magic_key;         ///< validity check
    config_t config;            ///< the node's configuration
} configmem_t;

extern config_t sysconfig;

/**
 * @brief:  Write configuration back to flashrom
 *
 * @return 1 on success, 0 otherwise
 */
uint8_t config_save(void);

/**
 * @brief: Read configuration from flashrom and stores it to sysconfig
 *
 * @note: If no configuration is present within flashrom a new configuration will be created
 */
void config_load(void);

#endif /* CONFIG_H */
