/*
 * Copyright (c) 2020 LAAS-CNRS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_hrtim
 * @{
 *
 * @file
 * @brief       Low-level HRTIM peripheral driver
 *
 * @author      Hugues Larrive <hugues.larrive@laas.fr>
 *
 * @}
 */

#ifndef PERIPH_HRTIM_H
#define PERIPH_HRTIM_H

#include <stdint.h>
#include <limits.h>

#include "periph_cpu.h"
#include "periph_conf.h"
#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default HRTIM access macro
 */
#ifndef HRTIM_DEV
#define HRTIM_DEV(x)          (x)
#endif

/**
 * @brief  Default HRTIM undefined value
 */
#ifndef HRTIM_UNDEF
#define HRTIM_UNDEF           (UINT_MAX)
#endif

/**
 * @brief   Default HRTIM type definition
 */
#ifndef HAVE_HRTIM_T
typedef unsigned int hrtim_t;
#endif

/**
 * @brief   HRTIM Set/Reset trigger definition
 */
typedef enum {
    SOFT_TRIG = 0x00000001,     /*< Software Set/Reset trigger */
    RESYNC = 0x00000002,        /*< Timer x resynchronization */
    PER = 0x00000004,           /*< Timer x Period */
    CMP1 = 0x00000008,          /*< Timer x Compare 1 */
    CMP2 = 0x00000010,          /*< Timer x Compare 2 */
    CMP3 = 0x00000020,          /*< Timer x Compare 3 */
    CMP4 = 0x00000040,          /*< Timer x Compare 4 */
    MSTPER = 0x00000080,        /*< Master Period */
    MSTCMP1 = 0x00000100,       /*< Master Compare 1 */
    MSTCMP2 = 0x00000200,       /*< Master Compare 2 */
    MSTCMP3 = 0x00000400,       /*< Master Compare 3 */
    MSTCMP4 = 0x00000800,       /*< Master Compare 4 */
    TIMEVNT1 = 0x00001000,      /*< Timer Event 1 */
    TIMEVNT2 = 0x00002000,      /*< Timer Event 2 */
    TIMEVNT3 = 0x00004000,      /*< Timer Event 3 */
    TIMEVNT4 = 0x00008000,      /*< Timer Event 4 */
    TIMEVNT5 = 0x00010000,      /*< Timer Event 5 */
    TIMEVNT6 = 0x00020000,      /*< Timer Event 6 */
    TIMEVNT7 = 0x00040000,      /*< Timer Event 7 */
    TIMEVNT8 = 0x00080000,      /*< Timer Event 8 */
    TIMEVNT9 = 0x00100000,      /*< Timer Event 9 */
    EXTEVNT1 = 0x00200000,      /*< External Event 1 */
    EXTEVNT2 = 0x00400000,      /*< External Event 2 */
    EXTEVNT3 = 0x00800000,      /*< External Event 3 */
    EXTEVNT4 = 0x01000000,      /*< External Event 4 */
    EXTEVNT5 = 0x02000000,      /*< External Event 5 */
    EXTEVNT6 = 0x04000000,      /*< External Event 6 */
    EXTEVNT7 = 0x08000000,      /*< External Event 7 */
    EXTEVNT8 = 0x10000000,      /*< External Event 8 */
    EXTEVNT9 = 0x20000000,      /*< External Event 9 */
    EXTEVNT10 = 0x40000000,     /*< External Event 10 */
    UPDATE = 0x80000000         /*< Registers update (transfer preload
                                    to active) */
} hrtim_cb_t;

/**
 * @brief   HRTIM timing units definition
 */
typedef enum {
    TIMA,
    TIMB,
    TIMC,
    TIMD,
    TIME,
#if (HRTIM_STU_NUMOF == 6)
    TIMF,
#endif
    MSTR
} hrtim_tu_t;

/**
 * @brief   HRTIM comparators definition
 */
typedef enum {
    CMP1xR = 1,
    CMP2xR = 2,
    CMP3xR = 3,
    CMP4xR = 4,
    MCMP1R = 1,
    MCMP2R = 2,
    MCMP3R = 3,
    MCMP4R = 4
} hrtim_cmp_t;

/**
 * @brief   HRTIM outputs definition
 */
typedef enum {
    OUT1 = 1,
    OUT2 = 2
} hrtim_out_t;

/**
 * @brief   HRTIM Timerx reset event definition
 *
 * Note: Bit definitions in stm32xxxx.h are for RSTAR (Timer A)
 * where Bits 19:30 are reset signals come from TIMB, TIMC,
 * TIMD, TIME so they are not usable for another unit.
 */
typedef enum {
    RST_UPDT = 0x00000002,          /*< Timer x Update reset */
    RST_CMP2 = 0x00000004,          /*< Timer x compare 2 reset */
    RST_CMP4 = 0x00000008,          /*< Timer x compare 4 reset */
    RST_MSTPER = 0x00000010,        /*< Master timer Period */
    RST_MSTCMP1 = 0x00000020,       /*< Master Compare 1 */
    RST_MSTCMP2 = 0x00000040,       /*< Master Compare 2 */
    RST_MSTCMP3 = 0x00000080,       /*< Master Compare 3 */
    RST_MSTCMP4 = 0x00000100,       /*< Master Compare 4 */
    RST_EXTEVNT1 = 0x00000200,      /*< External Event 1 */
    RST_EXTEVNT2 = 0x00000400,      /*< External Event 2 */
    RST_EXTEVNT3 = 0x00000800,      /*< External Event 3 */
    RST_EXTEVNT4 = 0x00001000,      /*< External Event 4 */
    RST_EXTEVNT5 = 0x00002000,      /*< External Event 5 */
    RST_EXTEVNT6 = 0x00004000,      /*< External Event 6 */
    RST_EXTEVNT7 = 0x00008000,      /*< External Event 7 */
    RST_EXTEVNT8 = 0x00010000,      /*< External Event 8 */
    RST_EXTEVNT9 = 0x00020000,      /*< External Event 9 */
    RST_EXTEVNT10 = 0x00040000,     /*< External Event 10 */
    RSTA_TBCMP1 = 0x00080000,       /*< Timer B Compare 1 for TIMA */
    RSTA_TBCMP2 = 0x00100000,       /*< Timer B Compare 2 for TIMA */
    RSTA_TBCMP4 = 0x00200000,       /*< Timer B Compare 4 for TIMA */
    RSTA_TCCMP1 = 0x00400000,       /*< Timer C Compare 1 for TIMA */
    RSTA_TCCMP2 = 0x00800000,       /*< Timer C Compare 2 for TIMA */
    RSTA_TCCMP4 = 0x01000000,       /*< Timer C Compare 4 for TIMA */
    RSTA_TDCMP1 = 0x02000000,       /*< Timer D Compare 1 for TIMA */
    RSTA_TDCMP2 = 0x04000000,       /*< Timer D Compare 2 for TIMA */
    RSTA_TDCMP4 = 0x08000000,       /*< Timer D Compare 4 for TIMA */
    RSTA_TECMP1 = 0x10000000,       /*< Timer E Compare 1 for TIMA */
    RSTA_TECMP2 = 0x20000000,       /*< Timer E Compare 2 for TIMA */
    RSTA_TECMP4 = 0x40000000,       /*< Timer E Compare 4 for TIMA */
    RSTB_TACMP1 = 0x00080000,       /*< Timer A Compare 1 for TIMB */
    RSTB_TACMP2 = 0x00100000,       /*< Timer A Compare 2 for TIMB */
    RSTB_TACMP4 = 0x00200000,       /*< Timer A Compare 4 for TIMB */
    RSTB_TCCMP1 = 0x00400000,       /*< Timer C Compare 1 for TIMB */
    RSTB_TCCMP2 = 0x00800000,       /*< Timer C Compare 2 for TIMB */
    RSTB_TCCMP4 = 0x01000000,       /*< Timer C Compare 4 for TIMB */
    RSTB_TDCMP1 = 0x02000000,       /*< Timer D Compare 1 for TIMB */
    RSTB_TDCMP2 = 0x04000000,       /*< Timer D Compare 2 for TIMB */
    RSTB_TDCMP4 = 0x08000000,       /*< Timer D Compare 4 for TIMB */
    RSTB_TECMP1 = 0x10000000,       /*< Timer E Compare 1 for TIMB */
    RSTB_TECMP2 = 0x20000000,       /*< Timer E Compare 2 for TIMB */
    RSTB_TECMP4 = 0x40000000,       /*< Timer E Compare 4 for TIMB */
    RSTC_TACMP1 = 0x00080000,       /*< Timer A Compare 1 for TIMC */
    RSTC_TACMP2 = 0x00100000,       /*< Timer A Compare 2 for TIMC */
    RSTC_TACMP4 = 0x00200000,       /*< Timer A Compare 4 for TIMC */
    RSTC_TBCMP1 = 0x00400000,       /*< Timer B Compare 1 for TIMC */
    RSTC_TBCMP2 = 0x00800000,       /*< Timer B Compare 2 for TIMC */
    RSTC_TBCMP4 = 0x01000000,       /*< Timer B Compare 4 for TIMC */
    RSTC_TDCMP1 = 0x02000000,       /*< Timer D Compare 1 for TIMC */
    RSTC_TDCMP2 = 0x04000000,       /*< Timer D Compare 2 for TIMC */
    RSTC_TDCMP4 = 0x08000000,       /*< Timer D Compare 4 for TIMC */
    RSTC_TECMP1 = 0x10000000,       /*< Timer E Compare 1 for TIMC */
    RSTC_TECMP2 = 0x20000000,       /*< Timer E Compare 2 for TIMC */
    RSTC_TECMP4 = 0x40000000,       /*< Timer E Compare 4 for TIMC */
    RSTD_TACMP1 = 0x00080000,       /*< Timer A Compare 1 for TIMD */
    RSTD_TACMP2 = 0x00100000,       /*< Timer A Compare 2 for TIMD */
    RSTD_TACMP4 = 0x00200000,       /*< Timer A Compare 4 for TIMD */
    RSTD_TBCMP1 = 0x00400000,       /*< Timer B Compare 1 for TIMD */
    RSTD_TBCMP2 = 0x00800000,       /*< Timer B Compare 2 for TIMD */
    RSTD_TBCMP4 = 0x01000000,       /*< Timer B Compare 4 for TIMD */
    RSTD_TCCMP1 = 0x02000000,       /*< Timer C Compare 1 for TIMD */
    RSTD_TCCMP2 = 0x04000000,       /*< Timer C Compare 2 for TIMD */
    RSTD_TCCMP4 = 0x08000000,       /*< Timer C Compare 4 for TIMD */
    RSTD_TECMP1 = 0x10000000,       /*< Timer E Compare 1 for TIMD */
    RSTD_TECMP2 = 0x20000000,       /*< Timer E Compare 2 for TIMD */
    RSTD_TECMP4 = 0x40000000,       /*< Timer E Compare 4 for TIMD */
    RSTE_TACMP1 = 0x00080000,       /*< Timer A Compare 1 for TIME */
    RSTE_TACMP2 = 0x00100000,       /*< Timer A Compare 2 for TIME */
    RSTE_TACMP4 = 0x00200000,       /*< Timer A Compare 4 for TIME */
    RSTE_TBCMP1 = 0x00400000,       /*< Timer B Compare 1 for TIME */
    RSTE_TBCMP2 = 0x00800000,       /*< Timer B Compare 2 for TIME */
    RSTE_TBCMP4 = 0x01000000,       /*< Timer B Compare 4 for TIME */
    RSTE_TCCMP1 = 0x02000000,       /*< Timer C Compare 1 for TIME */
    RSTE_TCCMP2 = 0x04000000,       /*< Timer C Compare 2 for TIME */
    RSTE_TCCMP4 = 0x08000000,       /*< Timer C Compare 4 for TIME */
    RSTE_TDCMP1 = 0x10000000,       /*< Timer D Compare 1 for TIME */
    RSTE_TDCMP2 = 0x20000000,       /*< Timer D Compare 2 for TIME */
    RSTE_TDCMP4 = 0x40000000,       /*< Timer D Compare 4 for TIME */
#if (HRTIM_STU_NUMOF == 6)
    RSTA_TFCMP2 = 0x80000000,       /*< Timer F Compare 2 for TIMA */
    RSTB_TFCMP2 = 0x80000000,       /*< Timer F Compare 2 for TIMB */
    RSTC_TFCMP2 = 0x80000000,       /*< Timer F Compare 2 for TIMC */
    RSTD_TFCMP2 = 0x80000000,       /*< Timer F Compare 2 for TIMD */
    RSTE_TFCMP2 = 0x80000000,       /*< Timer F Compare 2 for TIME */
    RSTF_TACMP1 = 0x00080000,       /*< Timer A Compare 1 for TIMF */
    RSTF_TACMP2 = 0x00100000,       /*< Timer A Compare 2 for TIMF */
    RSTF_TACMP4 = 0x00200000,       /*< Timer A Compare 4 for TIMF */
    RSTF_TBCMP1 = 0x00400000,       /*< Timer B Compare 1 for TIMF */
    RSTF_TBCMP2 = 0x00800000,       /*< Timer B Compare 2 for TIMF */
    RSTF_TBCMP4 = 0x01000000,       /*< Timer B Compare 4 for TIMF */
    RSTF_TCCMP1 = 0x02000000,       /*< Timer C Compare 1 for TIMF */
    RSTF_TCCMP2 = 0x04000000,       /*< Timer C Compare 2 for TIMF */
    RSTF_TCCMP4 = 0x08000000,       /*< Timer C Compare 4 for TIMF */
    RSTF_TDCMP1 = 0x10000000,       /*< Timer D Compare 1 for TIMF */
    RSTF_TDCMP2 = 0x20000000,       /*< Timer D Compare 2 for TIMF */
    RSTF_TDCMP4 = 0x40000000,       /*< Timer D Compare 4 for TIMF */
    RSTF_TECMP2 = 0x80000000,       /*< Timer E Compare 2 for TIMF */
#endif
} hrtim_rst_evt_t;

/**
 * @brief   HRTIM timing units CEN bits
 */
typedef enum {
    MCEN = HRTIM_MCR_MCEN,
    TACEN = HRTIM_MCR_TACEN,
    TBCEN = HRTIM_MCR_TBCEN,
    TCCEN = HRTIM_MCR_TCCEN,
    TDCEN = HRTIM_MCR_TDCEN,
    TECEN = HRTIM_MCR_TECEN,
#if (HRTIM_STU_NUMOF == 6)
    TFCEN = HRTIM_MCR_TFCEN
#endif
} hrtim_cen_t;

/**
 * @brief   Initialize an HRTIM device and all these timing units for
 *          complementary pwm outputs with a dead time.
 *
 * @param[in] dev           HRTIM device to initialize
 * @param[inout] freq       HRTIM frequency in Hz
 * @param[in] dt            Desired dead time in ns
 *
 * @return                  actual HRTIM resolution on success
 * @return                  0 on error
 */
uint16_t hrtim_init(hrtim_t dev, uint32_t *freq, uint16_t dt);

/**
 * @brief   Set the duty-cycle, dead-time and phase shift for a given
 *          timing unit of a given HRTIM device
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] value         Duty cycle
 * @param[in] shift         Phase shifting
 */
void hrtim_pwm_set(hrtim_t dev, hrtim_tu_t tu, uint16_t value, uint16_t shift);

/**
 * @brief   Initialize an HRTIM device master timer
 *
 * @param[in] dev           HRTIM device to initialize
 * @param[inout] freq       HRTIM frequency in Hz
 *
 * @return                  actual HRTIM resolution on success
 * @return                  0 on error
 */
uint16_t hrtim_init_mstr(hrtim_t dev, uint32_t *freq);

/**
 * @brief   Initialize a timing unit
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit to initialize
 * @param[inout] freq       HRTIM frequency in Hz
 *
 * @return                  actual timing unit resolution on success
 * @return                  0 on error
 */
uint16_t hrtim_init_tu(hrtim_t dev, hrtim_tu_t tu, uint32_t *freq);

/**
 * @brief   Set crossbar(s) setting
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output 1 or 2
 * @param[in] cb            Set crossbar(s)
 */
void hrtim_set_cb_set(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out,
                        hrtim_cb_t cb);

/**
 * @brief   Unset set crossbar(s)
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output 1 or 2
 * @param[in] cb            Set crossbar(s)
 */
void hrtim_set_cb_unset(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out,
                        hrtim_cb_t cb);

/**
 * @brief   Reset crossbar(s) setting
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output 1 or 2
 * @param[in] cb            Reset crossbar(s)
 */
void hrtim_rst_cb_set(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out,
                        hrtim_cb_t cb);

/**
 * @brief   Unset reset crossbar(s)
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output 1 or 2
 * @param[in] cb            Reset crossbar(s)
 */
void hrtim_rst_cb_unset(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out,
                            hrtim_cb_t cb);

/**
 * @brief   Full timing unit outputs set/reset crossbars setting for
 *          complementary pwm.
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 */
void hrtim_cmpl_pwm_out(hrtim_t dev, hrtim_tu_t tu);

/**
 * @brief   Set a period value
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit (TIM{A..F} or MSTR for master)
 * @param[in] value         Raw value to set
 */
void hrtim_period_set(hrtim_t dev, hrtim_tu_t tu, uint16_t value);

/**
 * @brief   Set a comparator value
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit (TIM{A..F} or MSTR for master)
 * @param[in] cmp           Comparator from 1 to 4
 * @param[in] value         Raw value to set
 */
void hrtim_cmp_set(hrtim_t dev, hrtim_tu_t tu, hrtim_cmp_t cmp,
                    uint16_t value);

/**
 * @brief   Enable a timing unit counter.
 *
 * @param[in] dev           HRTIM device
 * @param[in] cen           CEN mask
 */
void hrtim_cnt_en(hrtim_t dev, hrtim_cen_t cen);

/**
 * @brief   Disable a timing unit counter.
 *
 * @param[in] dev           HRTIM device
 * @param[in] cen           CEN mask
 */
void hrtim_cnt_dis(hrtim_t dev, hrtim_cen_t cen);

/**
 * @brief   Enable a Timerx reset event
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] evt           Reset event
 */
void hrtim_rst_evt_en(hrtim_t dev, hrtim_tu_t tu, hrtim_rst_evt_t evt);

/**
 * @brief   Disbable a Timerx reset event
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] evt           Reset event
 */
void hrtim_rst_evt_dis(hrtim_t dev, hrtim_tu_t tu, hrtim_rst_evt_t evt);

/**
 * @brief   Enable a given output of a given timing unit.
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output to enable
 */
void hrtim_out_en(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out);

/**
 * @brief   Disable a given output of a given timing unit.
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] out           Output to disable
 */
void hrtim_out_dis(hrtim_t dev, hrtim_tu_t tu, hrtim_out_t out);

/**
 * @brief   Setup a dead time in nano second for given complementary
 *          outputs.
 *
 * @param[in] dev           HRTIM device
 * @param[in] tu            Timing unit
 * @param[in] ns            The desired dead time in nano second
 */
void hrtim_pwm_dt(hrtim_t dev, hrtim_tu_t tu, uint16_t ns);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_HRTIM_H */
/** @} */
