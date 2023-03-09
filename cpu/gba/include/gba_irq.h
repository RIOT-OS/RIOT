#ifndef gba_irq_H
#define gba_irq_H

#ifdef __cplusplus
extern "C"
{
#endif

    void init_isr(void);

    void vblank_irq_enable(void);

#ifdef __cplusplus
}
#endif

#endif /* gba_irq_H */
/** @} */
