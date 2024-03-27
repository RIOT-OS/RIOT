#include "dma.h"
#include "periph_cpu.h"

void dma0(unsigned int dest, unsigned int src, unsigned short size, unsigned short flags);
void dma1(unsigned int dest, unsigned int src, unsigned short size, unsigned short flags);
void dma2(unsigned int dest, unsigned int src, unsigned short size, unsigned short flags);
void dma3(unsigned int dest, unsigned int src, unsigned short size, unsigned short flags);

void dma_memcpy16(unsigned char channel, unsigned short* dest, unsigned short* src, unsigned short size) {
    switch(channel) {
        case 0:
            dma0((unsigned int) dest, (unsigned int) src, size, 0);
            break;
        case 1:
            dma1((unsigned int) dest, (unsigned int) src, size, 0);
            break;
        case 2:
            dma2((unsigned int) dest, (unsigned int) src, size, 0);
            break;
        case 3:
            dma3((unsigned int) dest, (unsigned int) src, size, 0);
            break;
    }
}

void dma_memcpy32(unsigned char channel, unsigned short* dest, unsigned short* src, unsigned short size) {
    switch(channel) {
        case 0:
            dma0((unsigned int) dest, (unsigned int) src, size, GBA_DMA_32BIT);
            break;
        case 1:
            dma1((unsigned int) dest, (unsigned int) src, size, GBA_DMA_32BIT);
            break;
        case 2:
            dma2((unsigned int) dest, (unsigned int) src, size, GBA_DMA_32BIT);
            break;
        case 3:
            dma3((unsigned int) dest, (unsigned int) src, size, GBA_DMA_32BIT);
            break;
    }
}

void dma0(unsigned int dest, unsigned int src, unsigned short size, unsigned short flags) {
    GBA_DMA0_SRC = src;
    GBA_DMA0_DST = dest;
    GBA_DMA0_CNT = size;
    GBA_DMA0_CTL = flags | GBA_DMA_ENABLE;
}

void dma1(unsigned int dest, unsigned int src, unsigned short size, unsigned short flags) {
    GBA_DMA1_SRC = src;
    GBA_DMA1_DST = dest;
    GBA_DMA1_CNT = size;
    GBA_DMA1_CTL = flags | GBA_DMA_ENABLE;
}

void dma2(unsigned int dest, unsigned int src, unsigned short size, unsigned short flags) {
    GBA_DMA2_SRC = src;
    GBA_DMA2_DST = dest;
    GBA_DMA2_CNT = size;
    GBA_DMA2_CTL = flags | GBA_DMA_ENABLE;
}

void dma3(unsigned int dest, unsigned int src, unsigned short size, unsigned short flags) {
    GBA_DMA3_SRC = src;
    GBA_DMA3_DST = dest;
    GBA_DMA3_CNT = size;
    GBA_DMA3_CTL = flags | GBA_DMA_ENABLE;
}
