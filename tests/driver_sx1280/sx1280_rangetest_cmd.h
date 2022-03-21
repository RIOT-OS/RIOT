/*
 * Copyright (C) 2020-2022 Université Grenoble Alpes
 */

/**
 * @file
 * @brief       shell commands for using the SX1280 as a LoRa 2.4 GHz modem.
 *
 * @author      Nicolas Albarel
 * @author      Didier Donsez
 * @author      Olivier Alphand
 */

#ifndef SX1280_RANGETEST_H
#define SX1280_RANGETEST_H

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "ral_defs.h"

#include "ism2400.h"

#ifndef DELAY_BETWEEN_TX
#define DELAY_BETWEEN_TX                    (2000000U)
#endif

/*
 * @brief Range test by sending periodically LoRaWAN-like packets
 */
int sx1280_rangetest_cmd(int argc, char **argv);

#endif
