///*
// * Copyright (C) 2014 Hauke Petersen <mail@haukepetersen.de>
// *
// * This file is subject to the terms and conditions of the GPLv2 License.
// * See the file LICENSE in the top level directory for more details.
// */
//
///**
// * @ingroup     driver_periph
// * @{
// *
// * @file        gpio.c
// * @brief       Low-level GPIO driver implementation
// *
// * @author      Hauke Petersen <mail@haukepetersen.de>
// *
// * @}
// */
//
//#include "cpu.h"
//#include "periph/gpio.h"
//#include "periph_conf.h"
//
//typedef struct {
//    void (*cb)(void);
//} gpio_state_t;
//
//static gpio_state_t config[GPIO_NUMOF];
//
//int gpio_init_out(gpio_t dev, gpio_pp_t pullup)
//{
//    switch (dev) {
//#if GPIO_0_EN
//        case GPIO_0:
//            nrf_gpio_cfg_output(GPIO_0);
//            break;
//#endif
//#if GPIO_1_EN
//        case GPIO_1:
//            nrf_gpio_cfg_output(GPIO_1);
//            break;
//#endif
//#if GPIO_2_EN
//        case GPIO_2:
//            nrf_gpio_cfg_output(GPIO_2);
//            break;
//#endif
//#if GPIO_3_EN
//        case GPIO_3:
//            nrf_gpio_cfg_output(GPIO_3);
//            break;
//#endif
//#if GPIO_4_EN
//        case GPIO_4:
//            nrf_gpio_cfg_output(GPIO_4);
//            break;
//#endif
//#if GPIO_5_EN
//        case GPIO_5:
//            nrf_gpio_cfg_output(GPIO_5);
//            break;
//#endif
//#if GPIO_6_EN
//        case GPIO_6:
//            nrf_gpio_cfg_output(GPIO_6);
//            break;
//#endif
//#if GPIO_7_EN
//        case GPIO_7:
//            nrf_gpio_cfg_output(GPIO_7);
//            break;
//#endif
//#if GPIO_8_EN
//        case GPIO_8:
//            nrf_gpio_cfg_output(GPIO_8);
//            break;
//#endif
//#if GPIO_9_EN
//        case GPIO_9:
//            nrf_gpio_cfg_output(GPIO_9);
//            break;
//#endif
//#if GPIO_10_EN
//        case GPIO_10:
//            nrf_gpio_cfg_output(GPIO_10);
//            break;
//#endif
//#if GPIO_11_EN
//        case GPIO_11:
//            nrf_gpio_cfg_output(GPIO_11);
//            break;
//#endif
//#if GPIO_12_EN
//        case GPIO_12:
//            nrf_gpio_cfg_output(GPIO_12);
//            break;
//#endif
//#if GPIO_13_EN
//        case GPIO_13:
//            nrf_gpio_cfg_output(GPIO_13);
//            break;
//#endif
//#if GPIO_14_EN
//        case GPIO_14:
//            nrf_gpio_cfg_output(GPIO_14);
//            break;
//#endif
//#if GPIO_15_EN
//        case GPIO_15:
//            nrf_gpio_cfg_output(GPIO_15);
//            break;
//#endif
//
//        case GPIO_UNDEFINED:
//        default:
//            return -1;
//    }
///*    switch (pullup) {
//        case GPIO_PULLDOWN:
//            nrf_GPIO_PIN_PULLDOWN;
//            //return -2;
//        case GPIO_PULLUP:
//            nrf_GPIO_PIN_PULLUP;
//           // port->PIO_PUER = pin;
//            break;
//        case GPIO_NOPULL:
//            nrf_GPIO_PIN_NOPULL;
//            //port->PIO_PUDR = pin;
//            break;
//    }
//    */
//
//    return 1;
//}
//
//int gpio_init_in(gpio_t dev, gpio_pp_t pullup)
//{
//    uint32_t pin;
//
//    switch (dev) {
//#if GPIO_0_EN
//        case GPIO_0:
//            pin = GPIO_0;
//            break;
//#endif
//#if GPIO_1_EN
//        case GPIO_1:
//            pin = (GPIO_1);
//            break;
//#endif
//#if GPIO_2_EN
//        case GPIO_2:
//            pin = (GPIO_2);
//            break;
//#endif
//#if GPIO_3_EN
//        case GPIO_3:
//            pin = (GPIO_3);
//            break;
//#endif
//#if GPIO_4_EN
//        case GPIO_4:
//            pin = (GPIO_4);
//            break;
//#endif
//#if GPIO_5_EN
//        case GPIO_5:
//            pin = (GPIO_5);
//            break;
//#endif
//#if GPIO_6_EN
//        case GPIO_6:
//            pin = (GPIO_6);
//            break;
//#endif
//#if GPIO_7_EN
//        case GPIO_7:
//            pin = (GPIO_7);
//            break;
//#endif
//#if GPIO_8_EN
//        case GPIO_8:
//            pin = (GPIO_8);
//            break;
//#endif
//#if GPIO_9_EN
//        case GPIO_9:
//            pin = (GPIO_9);
//            break;
//#endif
//#if GPIO_10_EN
//        case GPIO_10:
//            pin = (GPIO_10);
//            break;
//#endif
//#if GPIO_11_EN
//        case GPIO_11:
//            pin = (GPIO_11);
//            break;
//#endif
//#if GPIO_12_EN
//        case GPIO_12:
//            pin = (GPIO_12);
//            break;
//#endif
//#if GPIO_13_EN
//        case GPIO_13:
//            pin = (GPIO_13);
//            break;
//#endif
//#if GPIO_14_EN
//        case GPIO_14:
//            pin = (GPIO_14);
//            break;
//#endif
//#if GPIO_15_EN
//        case GPIO_15:
//            pin = (GPIO_15);
//            break;
//#endif
//
//     case GPIO_UNDEFINED:
//        default:
//            return -1;
//    }
//    //configure dev as input and pullup as state
//
//    switch (pullup) {
//        case GPIO_PULLDOWN:
//            nrf_gpio_cfg_input(pin,nrf_GPIO_PIN_PULLDOWN);
//            break;
//        case GPIO_PULLUP:
//            nrf_gpio_cfg_input(pin,nrf_GPIO_PIN_PULLUP);
//            break;
//        case GPIO_NOPULL:
//            nrf_gpio_cfg_input(pin,nrf_GPIO_PIN_NOPULL);
//            break;
//    }
//    return 1;
//}
//
//int gpio_init_int(gpio_t dev, gpio_pp_t pullup, gpio_flank_t flank, void (*cb)(void))
//{
//    //TODO
//    return -1;
//}
//
//int gpio_read(gpio_t dev)
//{
//    switch (dev) {
//#if GPIO_0_EN
//        case GPIO_0:
//            return nrf_gpio_pin_read(GPIO_0);
//            break;
//#endif
//#if GPIO_1_EN
//        case GPIO_1:
//            return nrf_gpio_pin_read(GPIO_1);
//            break;
//#endif
//#if GPIO_2_EN
//        case GPIO_2:
//            return nrf_gpio_pin_read(GPIO_2);
//            break;
//#endif
//#if GPIO_3_EN
//        case GPIO_3:
//            return nrf_gpio_pin_read(GPIO_3);
//            break;
//#endif
//#if GPIO_4_EN
//        case GPIO_4:
//            return nrf_gpio_pin_read(GPIO_4);
//            break;
//#endif
//#if GPIO_5_EN
//        case GPIO_5:
//            return nrf_gpio_pin_read(GPIO_5);
//            break;
//#endif
//#if GPIO_6_EN
//        case GPIO_6:
//            return nrf_gpio_pin_read(GPIO_6);
//            break;
//#endif
//#if GPIO_7_EN
//        case GPIO_7:
//            return nrf_gpio_pin_read(GPIO_7);
//            break;
//#endif
//#if GPIO_8_EN
//        case GPIO_8:
//            return nrf_gpio_pin_read(GPIO_8);
//            break;
//#endif
//#if GPIO_9_EN
//        case GPIO_9:
//            return nrf_gpio_pin_read(GPIO_9);
//            break;
//#endif
//#if GPIO_10_EN
//        case GPIO_10:
//            return nrf_gpio_pin_read(GPIO_10);
//            break;
//#endif
//#if GPIO_11_EN
//        case GPIO_11:
//            return nrf_gpio_pin_read(GPIO_11);
//            break;
//#endif
//#if GPIO_12_EN
//        case GPIO_12:
//            return nrf_gpio_pin_read(GPIO_12);
//            break;
//#endif
//#if GPIO_13_EN
//        case GPIO_13:
//            return nrf_gpio_pin_read(GPIO_13);
//            break;
//#endif
//#if GPIO_14_EN
//        case GPIO_14:
//            return nrf_gpio_pin_read(GPIO_14);
//            break;
//#endif
//#if GPIO_15_EN
//        case GPIO_15:
//            return nrf_gpio_pin_read(GPIO_15);
//            break;
//#endif
//
//        case GPIO_UNDEFINED:
//           default:
//               return -1;
//       }
//}
//
//int gpio_set(gpio_t dev)
//{
//    switch (dev) {
//#if GPIO_0_EN
//        case GPIO_0:
//            nrf_gpio_pin_set(GPIO_0);
//            break;
//#endif
//#if GPIO_1_EN
//        case GPIO_1:
//            nrf_gpio_pin_set(GPIO_1);
//            break;
//#endif
//#if GPIO_2_EN
//        case GPIO_2:
//            nrf_gpio_pin_set(GPIO_2);
//            break;
//#endif
//#if GPIO_3_EN
//        case GPIO_3:
//            nrf_gpio_pin_set(GPIO_3);
//            break;
//#endif
//#if GPIO_4_EN
//        case GPIO_4:
//            nrf_gpio_pin_set(GPIO_4);
//            break;
//#endif
//#if GPIO_5_EN
//        case GPIO_5:
//            nrf_gpio_pin_set(GPIO_5);
//            break;
//#endif
//#if GPIO_6_EN
//        case GPIO_6:
//            nrf_gpio_pin_set(GPIO_6);
//            break;
//#endif
//#if GPIO_7_EN
//        case GPIO_7:
//            nrf_gpio_pin_set(GPIO_7);
//            break;
//#endif
//#if GPIO_8_EN
//        case GPIO_8:
//            nrf_gpio_pin_set(GPIO_8);
//            break;
//#endif
//#if GPIO_9_EN
//        case GPIO_9:
//            nrf_gpio_pin_set(GPIO_9);
//            break;
//#endif
//#if GPIO_10_EN
//        case GPIO_10:
//            nrf_gpio_pin_set(GPIO_10);
//            break;
//#endif
//#if GPIO_11_EN
//        case GPIO_11:
//            nrf_gpio_pin_set(GPIO_11);
//            break;
//#endif
//#if GPIO_12_EN
//        case GPIO_12:
//            nrf_gpio_pin_set(GPIO_12);
//            break;
//#endif
//#if GPIO_13_EN
//        case GPIO_13:
//            nrf_gpio_pin_set(GPIO_13);
//            break;
//#endif
//#if GPIO_14_EN
//        case GPIO_14:
//            nrf_gpio_pin_set(GPIO_14);
//            break;
//#endif
//#if GPIO_15_EN
//        case GPIO_15:
//            nrf_gpio_pin_set(GPIO_15);
//            break;
//#endif
//
//        case GPIO_UNDEFINED:
//        default:
//            return -1;
//    }
//    return 1;
//}
//
//int gpio_clear(gpio_t dev)
//{
//       switch (dev) {
//    #if GPIO_0_EN
//            case GPIO_0:
//                nrf_gpio_pin_clear(GPIO_0);
//                break;
//    #endif
//    #if GPIO_1_EN
//            case GPIO_1:
//                nrf_gpio_pin_clear(GPIO_1);
//                break;
//    #endif
//    #if GPIO_2_EN
//            case GPIO_2:
//                nrf_gpio_pin_clear(GPIO_2);
//                break;
//    #endif
//    #if GPIO_3_EN
//            case GPIO_3:
//                nrf_gpio_pin_clear(GPIO_3);
//                break;
//    #endif
//    #if GPIO_4_EN
//            case GPIO_4:
//                nrf_gpio_pin_clear(GPIO_4);
//                break;
//    #endif
//    #if GPIO_5_EN
//            case GPIO_5:
//                nrf_gpio_pin_clear(GPIO_5);
//                break;
//    #endif
//    #if GPIO_6_EN
//            case GPIO_6:
//                nrf_gpio_pin_clear(GPIO_6);
//                break;
//    #endif
//    #if GPIO_7_EN
//            case GPIO_7:
//                nrf_gpio_pin_clear(GPIO_7);
//                break;
//    #endif
//    #if GPIO_8_EN
//            case GPIO_8:
//                nrf_gpio_pin_clear(GPIO_8);
//                break;
//    #endif
//    #if GPIO_9_EN
//            case GPIO_9:
//                nrf_gpio_pin_clear(GPIO_9);
//                break;
//    #endif
//    #if GPIO_10_EN
//            case GPIO_10:
//                nrf_gpio_pin_clear(GPIO_10);
//                break;
//    #endif
//    #if GPIO_11_EN
//            case GPIO_11:
//                nrf_gpio_pin_clear(GPIO_11);
//                break;
//    #endif
//    #if GPIO_12_EN
//            case GPIO_12:
//                nrf_gpio_pin_clear(GPIO_12);
//                break;
//    #endif
//    #if GPIO_13_EN
//            case GPIO_13:
//                nrf_gpio_pin_clear(GPIO_13);
//                break;
//    #endif
//    #if GPIO_14_EN
//            case GPIO_14:
//                nrf_gpio_pin_clear(GPIO_14);
//                break;
//    #endif
//    #if GPIO_15_EN
//            case GPIO_15:
//                nrf_gpio_pin_clear(GPIO_15);
//                break;
//    #endif
//
//            case GPIO_UNDEFINED:
//            default:
//                return -1;
//        }
//    return 1;
//}
//
//
//int gpio_toggle(gpio_t dev)
//{
//    if (gpio_read(dev)) {
//        return gpio_clear(dev);
//    } else {
//        return gpio_set(dev);
//    }
//}
//
//int gpio_write(gpio_t dev, int value)
//{
//    if (value) {
//        return gpio_set(dev);
//    } else {
//        return gpio_clear(dev);
//    }
//}
