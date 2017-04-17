/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_stm32f1
 * @{
 *
 * @file            dev_uid_cpu.h
 * @brief           CPU specific Unique device ID provider declarations
 *
 * @author          Kévin Roussel <Kevin.Roussel@inria.fr>
 */

/* STM32F10x MCUs offer a 12-byte (96-bit) Unique IDentifier
   (stored in read-only, special-purpose MCU registers) */
#define DEV_UID_SIZE 12


