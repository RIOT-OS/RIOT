#ifndef gba_irq_H
#define gba_irq_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef void IsrFn(void);

#define GBA_IRQ_VBLANK 0x0001
#define GBA_IRQ_HBLANK 0x0002
#define GBA_IRQ_VCOUNTER 0x0004
#define GBA_IRQ_TIMER0 0x0008
#define GBA_IRQ_TIMER1 0x0010
#define GBA_IRQ_TIMER2 0x0020
#define GBA_IRQ_TIMER3 0x0040
#define GBA_IRQ_SERIAL 0x0080
#define GBA_IRQ_DMA0 0x0100
#define GBA_IRQ_DMA1 0x0200
#define GBA_IRQ_DMA2 0x0400
#define GBA_IRQ_DMA3 0x0800
#define GBA_IRQ_KEYPAD 0x1000
#define GBA_IRQ_EXTERN 0x2000

void init_isr(void);

void gba_isr(void);

int gba_irq_register_cb(IsrFn *cb, uint16_t mask);

void gba_irq_unregister_cb(int id);

#ifdef __cplusplus
}
#endif

#endif /* gba_irq_H */
/** @} */
