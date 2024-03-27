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
#define REG32(ADDR) (*(uint32_t volatile (*))(ADDR))

/**
 * @brief   Type for 16-bit registers
 */
#define REG16(ADDR) (*(uint16_t volatile (*))(ADDR))

/**
 * @brief   Type for 16-bit memory addresses
 */
#define MEM16(ADDR) ((uint16_t(*))(ADDR))

#define GBA_IRQ_ISR 0x03FFFFFC
#define GBA_IRQ_BIOS_ACK REG16(0x03FFFFF8)

#define GBA_DISPCNT REG16(0x4000000)

#define GBA_DISPCNT_BGMODE_0 0x0000
#define GBA_DISPCNT_BGMODE_1 0x0001
#define GBA_DISPCNT_BGMODE_2 0x0002
/* Bitmap, 240x160, 15-bit RGB */
#define GBA_DISPCNT_BGMODE_3 0x0003


#define GBA_DISPCNT_SDBG_0 0x0100
#define GBA_DISPCNT_SDBG_1 0x0200
/* Display BG2 */
#define GBA_DISPCNT_SDBG_2 0x0400

#define GBA_DISPSTAT REG16(0x4000004)

#define GBA_BG0CNT REG16(0x4000008)
#define GBA_BG1CNT REG16(0x400000a)
#define GBA_BG2CNT REG16(0x400000c)
#define GBA_BG3CNT REG16(0x400000e)

#define GBA_KEYPAD REG16(0x4000130)
#define GBA_KEYPAD_IRQCNTL REG16(0x4000132)
#define GBA_KEY_A 0x0001
#define GBA_KEY_B 0x0002
#define GBA_KEY_SELECT 0x0004
#define GBA_KEY_START 0x0008
#define GBA_KEY_RIGHT 0x0010
#define GBA_KEY_LEFT 0x0020
#define GBA_KEY_UP 0x0040
#define GBA_KEY_DOWN 0x0080
#define GBA_KEY_R 0x0100
#define GBA_KEY_L 0x0200
#define GBA_KEY_ALL 0x03FF
#define GBA_KEY_NC0 0x0400
#define GBA_KEY_NC1 0x0800
#define GBA_KEY_NC2 0x1000
#define GBA_KEY_NC3 0x2000
#define GBA_KEY_IE 0x4000
#define GBA_KEY_IC_AND 0x8000

#define GBA_DMA0_SRC REG32(0x40000B0)
#define GBA_DMA0_DST REG32(0x40000B4)
#define GBA_DMA0_CNT REG16(0x40000B8)
#define GBA_DMA0_CTL REG16(0x40000BA)

#define GBA_DMA1_SRC REG32(0x40000BC)
#define GBA_DMA1_DST REG32(0x40000C0)
#define GBA_DMA1_CNT REG16(0x40000C4)
#define GBA_DMA1_CTL REG16(0x40000C6)

#define GBA_DMA2_SRC REG32(0x40000C8)
#define GBA_DMA2_DST REG32(0x40000CC)
#define GBA_DMA2_CNT REG16(0x40000D0)
#define GBA_DMA2_CTL REG16(0x40000D2)

#define GBA_DMA3_SRC REG32(0x40000D4)
#define GBA_DMA3_DST REG32(0x40000D8)
#define GBA_DMA3_CNT REG16(0x40000DC)
#define GBA_DMA3_CTL REG16(0x40000DE)

/* Interrupt Enable */
#define GBA_IE REG16(0x4000200)

/* Interrupt Request Flags */
#define GBA_IRF REG16(0x4000202)

/* Interrupt Master Enable */
#define GBA_IME REG16(0x4000208)

#define GBA_VRAM_PALETTE MEM16(0x5000000)

/* 96Kb, 0x6017FFF - depends on display mode! */
#define GBA_VRAM MEM16(0x6000000)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
