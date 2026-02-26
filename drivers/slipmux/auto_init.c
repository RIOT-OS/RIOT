#include "log.h"
#include "board.h"

#include "slipdev.h"
#include "slipdev_params.h"

#define SLIPDEV_NUM ARRAY_SIZE(slipdev_params)

static slipdev_t slipdevs[SLIPDEV_NUM];

void slipmux_init(void)
{
    for (unsigned i = 0; i < SLIPDEV_NUM; i++) {
        const slipdev_params_t *p = &slipdev_params[i];

        LOG_DEBUG("[auto_init] initializing slipmux #%u\n", i);

        slipdev_setup(&slipdevs[i], p, i);

    }
}
/** @} */
