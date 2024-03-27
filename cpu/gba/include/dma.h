#ifndef DMA_H
#define DMA_H



#define GBA_DMA_DST_INC (0 << 5)
#define GBA_DMA_DST_DEC (1 << 5)
#define GBA_DMA_DST_FIX (2 << 5)
#define GBA_DMA_DST_RLD (3 << 5)

#define GBA_DMA_SRC_INC (0 << 7)
#define GBA_DMA_SRC_DEC (1 << 7)
#define GBA_DMA_SRC_FIX (2 << 7)
#define GBA_DMA_SRC_NO  (3 << 7)

#define GBA_DMA_REPEAT  (1 << 9)
#define GBA_DMA_32BIT   (1 << 10)
#define GBA_DMA_DRQ     (1 << 11)
#define GBA_DMA_TIME_IM (0 << 12)
#define GBA_DMA_TIME_VBLANK (1 << 12)
#define GBA_DMA_TIME_HBLANK (2 << 12)
#define GBA_DMA_TIME_SPECIAL (3 << 12)
#define GBA_DMA_IRQ_ENABLE (1 << 14)
#define GBA_DMA_ENABLE (1 << 15)

void dma_memcpy16(unsigned char channel, unsigned short *dest, unsigned short *src,
                  unsigned short size);
void dma_memcpy32(unsigned char channel, unsigned short *dest, unsigned short *src,
                  unsigned short size);

#endif
