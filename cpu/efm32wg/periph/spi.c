/**
 * @ingroup     cpu_efm32wg
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Mateusz Kubaszek <mathir.km.riot@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "sched.h"
#include "thread.h"
#include <stdbool.h>
#include "periph_conf.h"
#include "periph_cpu.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "em_usart.h"
#include "em_cmu.h"

/* guard this file in case no SPI device is defined */
#if SPI_NUMOF

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[] = {
#if SPI_0_EN
        [SPI_0] = MUTEX_INIT,
#endif
#if SPI_1_EN
        [SPI_1] = MUTEX_INIT,
#endif
#if SPI_2_EN
        [SPI_2] = MUTEX_INIT
#endif
};

typedef struct {
    char
    (*cb)(char data);
} spi_state_t;

static inline void
irq_handler_transfer(USART_TypeDef *spi, spi_t dev);

static spi_state_t spi_config[SPI_NUMOF];

void spi_poweron(spi_t dev)
{
    switch(dev) {
#if SPI_0_EN
    case SPI_0:
        SPI_0_CLKEN()
        ;
        break;
#endif /* SPI_0_EN */
#if SPI_1_EN
        case SPI_1:
        SPI_1_CLKEN();

        break;
#endif /* SPI_0_EN */
#if SPI_2_EN
    case SPI_2:
        SPI_2_CLKEN()
        ;
        break;
#endif /* SPI_0_EN */
    }
}

void spi_poweroff(spi_t dev)
{
    switch(dev) {
#if SPI_0_EN
    case SPI_0:
        while(!(SPI_0_DEV->STATUS & USART_STATUS_TXC))
            ; /* not busy anymore */
        SPI_0_CLKDIS()
        ;
        NVIC_DisableIRQ(SPI_0_IRQ_TX);
        NVIC_DisableIRQ(SPI_0_IRQ_RX);
        break;
#endif /* SPI_0_EN */
#if SPI_1_EN
        case SPI_1:
        while (!(SPI_1_DEV->STATUS & USART_STATUS_TXC)); /* not busy anymore */
        SPI_1_CLKDIS();
        NVIC_DisableIRQ(SPI_1_IRQ_TX);
        NVIC_DisableIRQ(SPI_1_IRQ_RX);
        break;
#endif /* SPI_0_EN */
#if SPI_2_EN
    case SPI_2:
        while(!(SPI_2_DEV->STATUS & USART_STATUS_TXC))
            ; /* not busy anymore */
        SPI_2_CLKDIS()
        ;
        NVIC_DisableIRQ(SPI_2_IRQ_TX);
        NVIC_DisableIRQ(SPI_2_IRQ_RX);
        break;
#endif /* SPI_0_EN */
    }
}

static int spi_init(spi_t dev, spi_conf_t conf, spi_speed_t speed,
        uint8_t location,
        bool master)
{
    USART_TypeDef *spi = NULL;
    int SpiNumber;

    spi_poweron(dev);

    /* Determining USART */
    switch(dev) {
#if SPI_0_EN
    case SPI_0:
        SpiNumber = 0;
        spi = (USART_TypeDef *)SPI_0_DEV;
        break;
#endif
#if SPI_1_EN
        case SPI_1:
        SpiNumber = 1;
        spi = SPI_1_DEV;
        break;
#endif
#if SPI_2_EN
    case SPI_2:
        SpiNumber = 2;
        spi = SPI_2_DEV;
        break;
#endif
    default:
        return -1;
    }

    uint32_t baud;
    /* Setting baudrate */
    switch(speed) {
    case SPI_SPEED_100KHZ:
        baud = 100;
        break;
    case SPI_SPEED_400KHZ:
        baud = 400;
        break;
    case SPI_SPEED_1MHZ:
        baud = 1000;
        break;
    case SPI_SPEED_5MHZ:
        baud = 5000;
        break;
    case SPI_SPEED_10MHZ:
        baud = 10000;
        break;
    default:
        return -1;
    }
    baud *= 1000;
    spi->CLKDIV = 128 * (SPI_PERCLK_FREQ / baud - 2);

    /* Configure SPI */
    /* Using synchronous (SPI) mode*/
    spi->CTRL = USART_CTRL_SYNC | USART_CTRL_MSBF;
    /* Clearing old transfers/receptions, and disabling interrupts */
    spi->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
    spi->IEN = 0;
    spi->IFC = 0x1FF9;
    /* Configuring edges (data setting and sampling) */
    switch(conf) {
    case SPI_CONF_FIRST_FALLING:
        /* CPOL=1 CPHA=0 */
        spi->CTRL |= USART_CTRL_CLKPOL_IDLEHIGH
                | USART_CTRL_CLKPHA_SAMPLELEADING;
        break;
    case SPI_CONF_FIRST_RISING:
        /* CPOL=0 CPHA=0 */
        spi->CTRL |=
        USART_CTRL_CLKPOL_IDLELOW | USART_CTRL_CLKPHA_SAMPLELEADING;
        break;
    case SPI_CONF_SECOND_RISING:
        /* CPOL=1 CPHA=1 */
        spi->CTRL |=
        USART_CTRL_CLKPOL_IDLEHIGH | USART_CTRL_CLKPHA_SAMPLETRAILING;
        break;
    case SPI_CONF_SECOND_FALLING:
        /* CPOL=0 CPHA=1 */
        spi->CTRL |= USART_CTRL_CLKPOL_IDLELOW
                | USART_CTRL_CLKPHA_SAMPLETRAILING;
        break;
    default:
        return -1;
    }
    /* Enabling pins and setting location */
    spi->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN
            | (location << 8); //USART_ROUTE_CSPEN |

    /* Set GPIO config to slave */
    gpio_dir_t gpioModeMosi = GPIO_DIR_INPUT;
    gpio_dir_t gpioModeMiso = GPIO_DIR_PUSH_PULL_DRIVE;
//	gpio_dir_t gpioModeCs = GPIO_DIR_INPUT;
    gpio_dir_t gpioModeClk = GPIO_DIR_INPUT;

    /* Set to master and to control the CS line */
    if(master) {
        /* Enabling Master, TX and RX */
        spi->CMD = USART_CMD_MASTEREN | USART_CMD_TXEN | USART_CMD_RXEN;

        /* CS should be manually triggered */
//		spi->CTRL |= USART_CTRL_AUTOCS;
        /* Set GPIO config to master */
        gpioModeMosi = GPIO_DIR_PUSH_PULL_DRIVE;
        gpioModeMiso = GPIO_DIR_INPUT;
//		gpioModeCs = GPIO_DIR_PUSH_PULL;
        gpioModeClk = GPIO_DIR_PUSH_PULL_DRIVE;
    } else {
        /* Enabling TX and RX */
        spi->CMD = USART_CMD_MASTERDIS | USART_CMD_TXEN | USART_CMD_RXEN;
        /* Enabling SPI data ready interrupts */
        spi->IEN = USART_IEN_RXDATAV;
    }

    /* Clear previous interrupts */
    spi->IFC = _USART_IFC_MASK;

    /* IO configuration */
    switch(SpiNumber) {
    case 0:
        switch(location) {
        case 0: /* IO configuration (USART 0, Location #0) */
            gpio_init(GPIO_T(GPIO_PORT_E, 10), gpioModeMosi, GPIO_PULLDOWN); /* MOSI */
            gpio_init(GPIO_T(GPIO_PORT_E, 11), gpioModeMiso, GPIO_PULLDOWN); /* MISO */
//			gpio_init(GPIO_T(GPIO_PORT_E, 13), gpioModeCs, GPIO_PULLDOWN); /* CS */
            gpio_init(GPIO_T(GPIO_PORT_E, 12), gpioModeClk, GPIO_PULLDOWN); /* Clock */
            break;
        case 1: /* IO configuration (USART 0, Location #1) */
            gpio_init(GPIO_T(GPIO_PORT_E, 7), gpioModeMosi, GPIO_PULLDOWN); /* MOSI */
            gpio_init(GPIO_T(GPIO_PORT_E, 6), gpioModeMiso, GPIO_PULLDOWN); /* MISO */
//			gpio_init(GPIO_T(GPIO_PORT_E, 4), gpioModeCs, GPIO_PULLDOWN); /* CS */
            gpio_init(GPIO_T(GPIO_PORT_E, 5), gpioModeClk, GPIO_PULLDOWN); /* Clock */
            break;
        case 2: /* IO configuration (USART 0, Location #2) */
            gpio_init(GPIO_T(GPIO_PORT_C, 11), gpioModeMosi, GPIO_PULLDOWN); /* MOSI */
            gpio_init(GPIO_T(GPIO_PORT_C, 10), gpioModeMiso, GPIO_PULLDOWN); /* MISO */
//			gpio_init(GPIO_T(GPIO_PORT_C, 8), gpioModeCs, GPIO_PULLDOWN); /* CS */
            gpio_init(GPIO_T(GPIO_PORT_C, 9), gpioModeClk, GPIO_PULLDOWN); /* Clock */
            break;
        case 4:
            gpio_init(GPIO_T(GPIO_PORT_B, 7), gpioModeMosi, GPIO_PULLDOWN); /* MOSI */
            gpio_init(GPIO_T(GPIO_PORT_B, 8), gpioModeMiso, GPIO_PULLDOWN); /* MISO */
//			gpio_init(GPIO_T(GPIO_PORT_B, 14), gpioModeCs, GPIO_PULLDOWN); /* CS */
            gpio_init(GPIO_T(GPIO_PORT_B, 13), gpioModeClk, GPIO_PULLDOWN); /* Clock */
            break;
        case 5:
            gpio_init(GPIO_T(GPIO_PORT_C, 0), gpioModeMosi, GPIO_PULLDOWN); /* MOSI */
            gpio_init(GPIO_T(GPIO_PORT_C, 1), gpioModeMiso, GPIO_PULLDOWN); /* MISO */
//			gpio_init(GPIO_T(GPIO_PORT_C, 14), gpioModeCs, GPIO_PULLDOWN); /* CS */
            gpio_init(GPIO_T(GPIO_PORT_C, 13), gpioModeClk, GPIO_PULLDOWN); /* Clock */
            break;
        default:
            return -1;
            break;
        }
        break;
    case 1:
        switch(location) {
        case 0: /* IO configuration (USART 1, Location #0) */
            gpio_init(GPIO_T(GPIO_PORT_C, 0), gpioModeMosi, GPIO_PULLDOWN); /* MOSI */
            gpio_init(GPIO_T(GPIO_PORT_C, 1), gpioModeMiso, GPIO_PULLDOWN); /* MISO */
//			gpio_init(GPIO_T(GPIO_PORT_B, 8), gpioModeCs, GPIO_PULLDOWN); /* CS */
            gpio_init(GPIO_T(GPIO_PORT_B, 7), gpioModeClk, GPIO_PULLDOWN); /* Clock */
            break;
        case 1: /* IO configuration (USART 1, Location #1) */
            gpio_init(GPIO_T(GPIO_PORT_D, 0), gpioModeMosi, GPIO_PULLDOWN); /* MOSI */
            gpio_init(GPIO_T(GPIO_PORT_D, 1), gpioModeMiso, GPIO_PULLDOWN); /* MISO */
//			gpio_init(GPIO_T(GPIO_PORT_D, 3), gpioModeCs, GPIO_PULLDOWN); /* CS */
            gpio_init(GPIO_T(GPIO_PORT_D, 2), gpioModeClk, GPIO_PULLDOWN); /* Clock */
            break;
        case 2: /* IO configuration (USART 1, Location #1) */
            gpio_init(GPIO_T(GPIO_PORT_D, 7), gpioModeMosi, GPIO_PULLDOWN); /* MOSI */
            gpio_init(GPIO_T(GPIO_PORT_D, 6), gpioModeMiso, GPIO_PULLDOWN); /* MISO */
//			gpio_init(GPIO_T(GPIO_PORT_F, 1), gpioModeCs, GPIO_PULLDOWN); /* CS */
            gpio_init(GPIO_T(GPIO_PORT_F, 0), gpioModeClk, GPIO_PULLDOWN); /* Clock */
            break;
        default:
            return -1;
            break;
        }
        break;
    case 2:
        switch(location) {
        case 0: /* IO configuration (USART 2, Location #0) */
            gpio_init(GPIO_T(GPIO_PORT_C, 2), gpioModeMosi, GPIO_PULLDOWN); /* MOSI */
            gpio_init(GPIO_T(GPIO_PORT_C, 3), gpioModeMiso, GPIO_PULLDOWN); /* MISO */
//			gpio_init(GPIO_T(GPIO_PORT_C, 5), gpioModeCs, GPIO_PULLDOWN); /* CS */
            gpio_init(GPIO_T(GPIO_PORT_C, 4), gpioModeClk, GPIO_PULLDOWN); /* Clock */
            break;
        case 1: /* IO configuration (USART 2, Location #1) */
            gpio_init(GPIO_T(GPIO_PORT_B, 3), gpioModeMosi, GPIO_PULLDOWN); /* MOSI */
            gpio_init(GPIO_T(GPIO_PORT_B, 4), gpioModeMiso, GPIO_PULLDOWN); /* MISO */
//			gpio_init(GPIO_T(GPIO_PORT_B, 6), gpioModeCs, GPIO_PULLDOWN); /* CS */
            gpio_init(GPIO_T(GPIO_PORT_B, 5), gpioModeClk, GPIO_PULLDOWN); /* Clock */
            break;
        default:
            return -1;
            break;
        }
        break;
    default:
        return -1;
        break;
    }
    return 0;
}

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    int res;
    switch(dev) {
#if SPI_0_EN
    case SPI_0:
        res = spi_init(dev, conf, speed, SPI_0_LOC, true);
        break;
#endif /* SPI_0_EN */
#if SPI_1_EN
        case SPI_1:
        res = spi_init(dev, conf, speed, SPI_1_LOC, true);
        break;
#endif /* SPI_1_EN */
#if SPI_2_EN
    case SPI_2:
        res = spi_init(dev, conf, speed, SPI_2_LOC, true);
        break;
#endif /* SPI_2_EN */
    default:
        return -2;
    }
    return res;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char data))
{
    int res;
    switch(dev) {
#if SPI_0_EN
    case SPI_0:
        res = spi_init(dev, conf, 0, SPI_0_LOC, false);
        SPI_0_DEV->CMD = USART_CMD_CLEARRX;
        NVIC_SetPriority(SPI_0_IRQ_RX, SPI_IRQ_PRIO); /* set SPI interrupt priority */
        NVIC_EnableIRQ(SPI_0_IRQ_RX); /* set SPI interrupt priority */
        break;
#endif /* SPI_0_EN */
#if SPI_1_EN
        case SPI_1:
        res = spi_init(dev, conf, 0, SPI_1_LOC, false);
        SPI_1_DEV->CMD = USART_CMD_CLEARRX;
        NVIC_SetPriority(SPI_1_IRQ_RX, SPI_IRQ_PRIO); /* set SPI interrupt priority */
        NVIC_EnableIRQ(SPI_1_IRQ_RX); /* set SPI interrupt priority */
        break;
#endif /* SPI_1_EN */
#if SPI_2_EN
    case SPI_2:
        res = spi_init(dev, conf, 0, SPI_2_LOC, false);
        SPI_2_DEV->CMD = USART_CMD_CLEARRX;
        NVIC_SetPriority(SPI_2_IRQ_RX, SPI_IRQ_PRIO); /* set SPI interrupt priority */
        NVIC_EnableIRQ(SPI_2_IRQ_RX); /* set SPI interrupt priority */
        break;
#endif /* SPI_2_EN */
    default:
        return -2;
    }
    spi_config[dev].cb = cb;

    return res;
}

int spi_acquire(spi_t dev)
{
    if(dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int spi_release(spi_t dev)
{
    if(dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    USART_TypeDef *spi;

    /* Determining USART */
    switch(dev) {
#if SPI_0_EN
    case SPI_0:
        spi = (USART_TypeDef *)SPI_0_DEV;
        break;
#endif
#if SPI_1_EN
        case SPI_1:
        spi = (USART_TypeDef *)SPI_1_DEV;
        break;
#endif
#if SPI_2_EN
    case SPI_2:
        spi = (USART_TypeDef *)SPI_2_DEV;
        break;
#endif
    default:
        return -1;
    }

    /* Wait until the transmit buffer is free */
    while(!(spi->STATUS & USART_STATUS_TXBL)) {
//		printf("REGISTERS\n CONROL = %lx\n TRIGCTRL = %lx\n STATUS - %lx\n", (uint32_t)USART0->CTRL, (uint32_t)USART0->TRIGCTRL, (uint32_t)USART0->STATUS);
    }
    /* Write data to send register */
    spi->TXDATA = out;
    /* Wait until the transmission is completed */
    while(!(spi->STATUS & USART_STATUS_RXDATAV))
        ;

    if(in != NULL) {
        *in = (uint8_t)spi->RXDATA;
    } else {
        spi->RXDATA;
    }

    return 1;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    switch(dev) {
#if SPI_0_EN
    case SPI_0:
        SPI_0_DEV->TXDATA = reset_val;
        break;
#endif
#if SPI_1_EN
        case SPI_1:
        SPI_1_DEV->TXDATA = reset_val;
        break;
#endif
#if SPI_2_EN
    case SPI_2:
        SPI_2_DEV->TXDATA = reset_val;
        break;
#endif
    }
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    spi_transfer_byte(dev, reg, 0);
    return spi_transfer_byte(dev, out, in);
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in,
        unsigned int length)
{
    if(0 > spi_transfer_byte(dev, reg, 0))
        return -1;
    if(in) {
        for(unsigned int i = 0; i < length; ++i) {
            if(0 > spi_transfer_byte(dev, out[i], in + i))
                return -1;
        }
    } else {
        for(unsigned int i = 0; i < length; ++i) {
            if(0 > spi_transfer_byte(dev, out[i], NULL))
                return -1;
        }
    }
    return 0;
}

static inline void irq_handler_transfer(USART_TypeDef *spi, spi_t dev)
{
    uint8_t data;
    data = spi->RXDATA;
    data = spi_config[dev].cb(data);
    spi->TXDATA = data;

    /* see if a thread with higher priority wants to run now */
    if(sched_context_switch_request) {
        thread_yield();
    }
}

#if SPI_0_EN
void SPI_0_IRQ_TX_HANDLER(void)
{
    if(SPI_0_DEV->IF & USART_IF_RXDATAV) {
        irq_handler_transfer(SPI_0_DEV, SPI_0);
    }
}

void SPI_0_IRQ_RX_HANDLER(void)
{
    if(SPI_0_DEV->IF & USART_IF_RXDATAV) {
        irq_handler_transfer(SPI_0_DEV, SPI_0);
    }
}
#endif

#if SPI_1_EN
void SPI_1_IRQ_TX_HANDLER(void)
{
    irq_handler_transfer(SPI_1_DEV, SPI_1);
}

void SPI_1_IRQ_RX_HANDLER(void)
{
    irq_handler_transfer(SPI_1_DEV, SPI_1);
}
#endif

#if SPI_2_EN
void SPI_2_IRQ_TX_HANDLER(void)
{
    irq_handler_transfer(SPI_2_DEV, SPI_2);
}

void SPI_2_IRQ_RX_HANDLER(void)
{
    irq_handler_transfer(SPI_2_DEV, SPI_2);
}
#endif

#endif /* SPI_NUMOF */
