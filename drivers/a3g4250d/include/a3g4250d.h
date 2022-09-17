#ifndef A3G4250D_H
#define A3G4250D_H

#include "periph/i2c.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define A3G4250D_I2C_ADD_L (0x34)
#define A3G4250D_I2C_ADD_H (0x35)

enum {
    A3G4250D_OK             =  0,
    A3G4250D_NULL_PTR       = -1,
    A3G4250D_COM_FAILED     = -2,
    A3G4250D_NO_SPI         = -3,
    A3G4250D_NO_I2C         = -4,
    A3G4250D_INVALID        = -4,
    A3G4250D_NO_NEW_DATA    = -5,
};

#ifdef DOXYGEN

typedef struct {
    int16_t temperature;
    int16_t x_angular_rate;
    int16_t y_angular_rate;
    int16_t z_angular_rate;
} a3g4250d_raw_data;

typedef struct {
    int16_t temperature;
    int16_t x_angular_rate;
    int16_t y_angular_rate;
    int16_t z_angular_rate;
} a3g4250d_calib_data;

typedef enum {
    A3G4250D_I2C
    A3G4250D_SPI
} a3g4250d_itf_t;

#if MODULE_A3G4250D_I2C || DOXYGEN
typedef struct {
    i2c_t   dev;
    uint8_t addr;
} a3g4250d_i2c_params_t;
#endif

#if MODULE_A3G4250D_SPI || DOXYGEN
typedef struct {
    spi_t   dev;
    gpio_t  nss_pin;
} a3g4250d_spi_params_t;
#endif

typedef struct {
    union {
        a3g4250d_i2c_params_t i2c;
        a3g4250d_spi_params_t spi;
    };
}a3g4250d_itf_params_t;

typedef struct {
    a3g4250d_itf_params_t   itf_params;
    a3g4250d_dr_t           data_rate;
#ifdef MODULE_A3G4250D_INT1 || DOXYGEN
    gpio_t                  int1_pin;
    a3g4250d_int1_cfg_t     int1_cfg;
#endif
#ifdef MODULE_A3G4250D_INT2 || DOXYGEN
    gpio_t                  int2_pin;
#endif
#ifdef MODULE_A3G4250D_FIFO || DOXYGEN
    a3g4250d_fifo_mode_t    fifo_mode;
#endif
} a3g4250d_params_t;

typedef struct {
    a3g4250d_h_lactive_t    int1_active;
    a3g4250d_pp_od_t        int_pin_type;
} a3g4250d_t;

typedef struct a3g4250d_status_reg_t a3g4250d_status_t;

typedef struct a3g4250d_fifo_src_reg_t a3g4250d_fifo_status_t;

typedef struct a3g4250d_int1_src_t a3g4250d_int1_status_t;

#endif  /* DOXYGEN */
#ifdef __cplusplus
}
#endif
#endif /* A3G4250D_H */