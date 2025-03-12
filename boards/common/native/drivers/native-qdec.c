/**
 * Native Board board implementation
 *
 * Copyright (C) 2018 Gilles DOFFE <g.doffe@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup boards_common_native
 * @{
 * @file
 * @author  Gilles DOFFE <g.doffe@gmail.com>
 * @}
 */
#include <inttypes.h>
#include <stdio.h>

/* RIOT includes */
#include <board.h>
#include <log.h>

#ifdef MODULE_PERIPH_QDEC

extern int32_t qdecs_value[QDEC_NUMOF];

void native_motor_driver_qdec_simulation(
    const motor_driver_t motor_driver, uint8_t motor_id,
    int32_t pwm_duty_cycle)
{
    uint32_t id = 0;

    for (uint32_t i = 0; i < motor_driver; i++) {
        const motor_driver_config_t motor_driver_conf =
            motor_driver_config[motor_driver];
        id += motor_driver_conf.nb_motors;
    }
    id += motor_id;

    if (id < QDEC_NUMOF) {
        qdecs_value[id] = pwm_duty_cycle;

        LOG_DEBUG("MOTOR-DRIVER=%u"             \
            "    MOTOR_ID = %u"                 \
            "    PWM_VALUE = %d"                \
            "    QDEC_ID = %"PRIu32""           \
            "    QDEC_VALUE = %d\n",            \
            motor_driver, motor_id, pwm_duty_cycle, id, pwm_duty_cycle);
    }
    else {
        LOG_ERROR("MOTOR-DRIVER=%u"             \
            "    MOTOR_ID = %u"                 \
            "    no QDEC device associated\n",  \
            motor_driver, motor_id);
    }
}

#endif /* MODULE_PERIPH_QDEC */
