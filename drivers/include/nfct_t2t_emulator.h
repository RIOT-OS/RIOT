#ifndef NFCT_T2T_EMULATOR_H
#define NFCT_T2T_EMULATOR_H

#include <stdint.h>
#include "net/nfc/t2t/t2t.h"

typedef enum {
    T2T_READ_COMMAND = 0x30,
    T2T_HALT_COMMAND = 0x50,
    T2T_WRITE_COMMAND = 0xA2,
    T2T_SECTOR_SELECT_COMMAND = 0XC2
} t2t_command_t;

typedef enum {
    T2T_ACK_RESPONSE = 0x0A,
    T2T_NACK_RESPONSE = 0x00
} t2t_response_t;

typedef struct {
    void (*init) (nfc_t2t_t *t2t);
    void (*enable) (void);
    void (*disable) (void);
    void (*uninit) (void);
} nfct_t2t_emulator_t;

#endif