/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_nrf51822qfaa
 * @{
 *
 * @file            radio.h
 * @brief           CPU specific radio function definitions
 *
 * @author          Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author          Timo Ziegler <timo.ziegler@fu-berlin.de>
 */

char receivePacket(void);
void sendPacket(uint8_t addr, char msg);
void radioConfig(void);
