#ifndef __IO_H__
#define __IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

static inline u8_t __read8(physical_addr_t addr)
{
    return( *((volatile u8_t *)(addr)) );
}

static inline u16_t __read16(physical_addr_t addr)
{
    return( *((volatile u16_t *)(addr)) );
}

static inline u32_t __read32(physical_addr_t addr)
{
    return( *((volatile u32_t *)(addr)) );
}

static inline u64_t __read64(physical_addr_t addr)
{
    return( *((volatile u64_t *)(addr)) );
}

static inline void __write8(physical_addr_t addr, u8_t value)
{
    *((volatile u8_t *)(addr)) = value;
}

static inline void __write16(physical_addr_t addr, u16_t value)
{
    *((volatile u16_t *)(addr)) = value;
}

static inline void __write32(physical_addr_t addr, u32_t value)
{
    *((volatile u32_t *)(addr)) = value;
}

static inline void __write64(physical_addr_t addr, u64_t value)
{
    *((volatile u64_t *)(addr)) = value;
}

static inline physical_addr_t virt_to_phys(virtual_addr_t virt)
{
    return (physical_addr_t)virt;
}

static inline virtual_addr_t phys_to_virt(physical_addr_t phys)
{
    return (virtual_addr_t)phys;
}

#define read8(a)        __read8(a)
#define read16(a)       __read16(a)
#define read32(a)       __read32(a)
#define read64(a)       __read64(a)

#define write8(a, v)    __write8((a), (v))
#define write16(a, v)   __write16((a), (v))
#define write32(a, v)   __write32((a), (v))
#define write64(a, v)   __write64((a), (v))

#define readl(a)        read32(a)
#define writel(v, a)    write32((a), (v))

#define IO_ADDRESS(addr) (addr)

#ifdef __cplusplus
}
#endif

#endif /* __IO_H__ */
