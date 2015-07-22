/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_zynq7000
 * @{
 *
 * @file            zynq7000.h
 * @brief           CPU specific definitions
 *
 * @author          Simon Vincent <simon.vincent@xsilon.com>
 */

#ifndef __ZYNQ7000_H
#define __ZYNQ7000_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Types
 */

/**
 * @brief Uart registers
 */
typedef struct {
	uint32_t control_reg0;
	uint32_t mode_reg0;
	uint32_t intpt_en_reg0;
	uint32_t intpt_dis_reg0;
	uint32_t intpt_mask_reg0;
	uint32_t chnl_int_sts_reg0;
	uint32_t baud_rate_gen_reg0;
	uint32_t rcvr_timeout_reg0;
	uint32_t rcvr_fifo_trigger_level0;
	uint32_t modem_ctrl_reg0;
	uint32_t modem_sts_reg0;
	uint32_t channel_sts_reg0;
	uint32_t tx_rx_fifo0;
	/* TODO finish this list */
}xuart_ps;

/**
 * @brief Triple Timer Counter registers
 */
typedef struct {
	uint32_t clock_control_1;
	uint32_t clock_control_2;
	uint32_t clock_control_3;
	uint32_t counter_control_1;
	uint32_t counter_control_2;
	uint32_t counter_control_3;
	uint32_t counter_value_1;
	uint32_t counter_value_2;
	uint32_t counter_value_3;
	uint32_t interval_counter_1;
	uint32_t interval_counter_2;
	uint32_t interval_counter_3;
	uint32_t match_1_counter_1;
	uint32_t match_1_counter_2;
	uint32_t match_1_counter_3;
	uint32_t match_2_counter_1;
	uint32_t match_2_counter_2;
	uint32_t match_2_counter_3;
	uint32_t match_3_counter_1;
	uint32_t match_3_counter_2;
	uint32_t match_3_counter_3;
	uint32_t interrupt_register_1;
	uint32_t interrupt_register_2;
	uint32_t interrupt_register_3;
	uint32_t interrupt_enable_1;
	uint32_t interrupt_enable_2;
	uint32_t interrupt_enable_3;
	uint32_t event_control_timer_1;
	uint32_t event_control_timer_2;
	uint32_t event_control_timer_3;
	uint32_t event_register_1;
	uint32_t event_register_2;
	uint32_t event_register_3;
}xttc_ps;

/**
 * @brief Baseaddress for Zynq7000 peripherals
 */
#define TTC0        		((volatile xttc_ps     *)0xF8001000U)
#define TTC1        		((volatile xttc_ps     *)0xF8002000U)
#define UART0				((volatile xuart_ps    *)0xE0000000U)
#define UART1				((volatile xuart_ps    *)0xE0001000U)

/**
 * @brief Interrupt ids
 */
#define TTC0_T0_INT_ID	42
#define TTC0_T1_INT_ID	43
#define TTC0_T2_INT_ID	44
#define TTC1_T0_INT_ID	69
#define TTC1_T1_INT_ID	70
#define TTC1_T2_INT_ID	71
#define UART0_INT_ID	59
#define UART1_INT_ID	82
#define GPIO_INT_ID		52

#ifdef __cplusplus
}
#endif

#endif /* __ZYNQ7000_H */
/** @} */
