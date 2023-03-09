/**
 * @ingroup         cpu_gba
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define PM_NUM_MODES 0

/**
 * @brief   Type for 32-bit registers
 */
#define REG32(ADDR) (*(uint32_t volatile(*))(ADDR))

/**
 * @brief   Type for 16-bit registers
 */
#define REG16(ADDR) (*(uint16_t volatile(*))(ADDR))

/**
 * @brief   Type for 16-bit memory addresses
 */
#define MEM16(ADDR) ((uint16_t(*))(ADDR))

#define GBA_DISPCNT REG16(0x4000000)
/* Bitmap, 240x160, 15-bit RGB */
#define GBA_DISPCNT_BGMODE_3 0x0003
/* Display BG2 */
#define GBA_DISPCNT_SDBG_2 0x0400

#define GBA_DISPSTAT REG16(0x4000004)

#define GBA_KEYPAD REG16(0x4000130)
#define GBA_KEYPAD_IRQCNTL REG16(0x4000132)
#define GBA_KEY_A 1 << 0
#define GBA_KEY_B 1 << 1
#define GBA_KEY_Select 1 << 2
#define GBA_KEY_Start 1 << 3
#define GBA_KEY_Right 1 << 4
#define GBA_KEY_Left 1 << 5
#define GBA_KEY_Up 1 << 6
#define GBA_KEY_Down 1 << 7
#define GBA_KEY_R 1 << 8
#define GBA_KEY_L 1 << 9

/* Interrupt Enable */
#define GBA_IE REG16(0x4000200)

/* Interrupt Request Flags */
#define GBA_IRF REG16(0x4000202)

/* Interrupt Master Enable */
#define GBA_IME REG16(0x4000208)

/* 96Kb, 0x6017FFF - depends on display mode! */
#define GBA_VRAM MEM16(0x6000000)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
