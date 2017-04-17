#ifndef __ARM32_CP15_H__
#define __ARM32_CP15_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void     __irq_enable(void);
void     __irq_disable(void);
void     __fiq_enable(void);
void     __fiq_disable(void);
void     __icache_enable(void);
void     __icache_disable(void);
int      __icache_status(void);
void     __dcache_enable(void);
void     __dcache_disable(void);
int      __dcache_status(void);
void     __wbuffer_enable(void);
void     __wbuffer_disable(void);
int      __wbuffer_status(void);
void     __mmu_enable(void);
void     __mmu_disable(void);
int      __mmu_status(void);
void     __vic_enable(void);
void     __vic_disable(void);
int      __vic_status(void);
void     __branch_enable(void);
void     __branch_disable(void);
int      __branch_status(void);
void     __ttb_set(uint32_t base);
uint32_t __ttb_get(void);
void     __domain_set(uint32_t domain);
uint32_t __domain_get(void);
void     __tlb_invalidate(void);
uint32_t __get_cpuid(void);

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_CP15_H__ */
