/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis_common_usbdev
 *
 * @note        This driver only implements USB device mode.
 *
 * @{
 *
 * @file
 * @brief       Low-level USB device driver implementation
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "kernel.h"
#include "msg.h"
#include "cpu.h"
#include "init.h"
#include "board.h"

#include "usbdev.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define USBDEV_MAX_PACKET_SIZE          64

#ifndef USBDEV_MAX_ENDPOINT_NUM
#define USBDEV_MAX_ENDPOINT_NUM         16
#endif

#ifndef USBDEV_EP0_MAX_PACKET
#define USBDEV_EP0_MAX_PACKET           8
#endif

#if (USBDEV_MAX_ENDPOINT_NUM > 16)
#error Kinetis USB Device driver supports maximum 16 endpoints!
#endif

#define KINETIS_USB_EPIDX_MAX           0x0f

#define BD_IDX_EP0RX_ODD                0
#define BD_IDX_EP0RX_EVEN               1
#define BD_IDX_EP0TX_ODD                2
#define BD_IDX_EP0TX_EVEN               3

#define BD_OWN_MASK                     (1 << 7)
#define BD_DATA01_MASK                  (1 << 6)
#define BD_DATA01_SHIFT                 6
#define BD_KEEP_MASK                    (1 << 5)
#define BD_NINC_MASK                    (1 << 4)
#define BD_DTS_MASK                     (1 << 3)
#define BD_STALL_MASK                   (1 << 2)
#define BD_TOK_PID_SHIFT                2

#define KINETIS_SETUP_TOKEN             0x0d
#define KINETIS_IN_TOKEN                0x09
#define KINETIS_OUT_TOKEN               0x01

#define KINETIS_USB_SUPPORTED_PERID     0x04
#define KINETIS_USB_SUPPORTED_REV       0x33

usbdev_ops_t kinetis_usb_driver;

typedef struct  __attribute__((packed))
{
    uint8_t    stat;
    uint8_t    reserved;
    uint16_t   bc;
    uint32_t   buf_addr;
} bd_table_t;

bd_table_t __attribute__((aligned(512))) bd_table[(USBDEV_MAX_ENDPOINT_NUM) * 2 * 2];

uint8_t __attribute__((aligned(4))) ep0_buf[4][USBDEV_EP0_MAX_PACKET];

uint8_t ep_out_size[USBDEV_MAX_ENDPOINT_NUM];

uint32_t toggle_bits = 0;

inline static uint8_t get_bdt_idx(uint8_t ep, uint8_t even)
{
    if (ep & ENDPOINT_IN_MASK) {
        return ((((ENDPOINT_NUM(ep)) * 4) + 2  + (even & 1)));
    }
    return ((((ENDPOINT_NUM(ep)) * 4) + (even & 1)));
}

inline static uint8_t get_token_pid(uint8_t idx)
{
    return (bd_table[idx].stat >> BD_TOK_PID_SHIFT) & 0x0f;
}

inline static void protected_and(uint32_t *addr, uint32_t val)
{
    while (__STREXW((__LDREXW(addr) & val), addr));
}

inline static void protected_or(uint32_t *addr, uint32_t val)
{
    while (__STREXW((__LDREXW(addr) | val), addr));
}

inline static void protected_xor(uint32_t *addr, uint32_t val)
{
    while (__STREXW((__LDREXW(addr) ^ val), addr));
}

inline static void kinetis_usb_init_clock(void)
{
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
}

usbdev_ops_t* kinetis_usb_init(usbdev_t *dev)
{
    ep_out_size[0] = USBDEV_EP0_MAX_PACKET;

    kinetis_usb_init_clock();

    SIM->SOPT1 |= SIM_SOPT1_USBREGEN_MASK;
    /* USB0->USBTRC0 |= 0x40; */

    USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
    __ASM volatile ("nop");
    __ASM volatile ("nop");
    __ASM volatile ("nop");

    USB0->CTL = 0;
    /* enable USB module, AKA USBEN bit in CTL1 register */
    USB0->CTL |= USB_CTL_USBENSOFEN_MASK;

    if ((USB0->PERID != KINETIS_USB_SUPPORTED_PERID) ||
	(USB0->REV !=KINETIS_USB_SUPPORTED_REV)) {
        return NULL;
    }

    USB0->BDTPAGE1 = (uint8_t)((uint32_t) bd_table >> 8);
    USB0->BDTPAGE2 = (uint8_t)((uint32_t) bd_table >> 16);
    USB0->BDTPAGE3 = (uint8_t)((uint32_t) bd_table >> 24);

    /* clear interrupt flags */
    USB0->ISTAT = 0xFF;

    /* enable interrupts */
    USB0->INTEN = USB_INTEN_USBRSTEN_MASK;

    USB0->USBCTRL = USB_USBCTRL_PDE_MASK;

    NVIC_EnableIRQ(USB0_IRQn);

    kinetis_usb_driver.dev = dev;

    return &kinetis_usb_driver;
}

static inline void kinetis_usb_set_data1(uint8_t idx)
{
    protected_or(&toggle_bits, (1 << (idx / 2)));
}

static inline void kinetis_usb_clr_data1(uint8_t idx)
{
    protected_and(&toggle_bits, ~(1 << (idx / 2)));
}

static inline void kinetis_usb_toggle_data1(uint8_t idx)
{
    protected_xor(&toggle_bits, (1 << (idx / 2)));
}

static inline uint8_t kinetis_usb_get_data1(uint8_t idx)
{
    return (uint8_t)((toggle_bits >> (idx / 2)) & 1);
}

static inline int kinetis_usb_set_ep_enable(usb_ep_t ep)
{
    if (ep & ENDPOINT_IN_MASK) {
        USB0->ENDPOINT[ENDPOINT_NUM(ep)].ENDPT |= USB_ENDPT_EPTXEN_MASK;
    }
    else {
        USB0->ENDPOINT[ENDPOINT_NUM(ep)].ENDPT |= USB_ENDPT_EPRXEN_MASK;
    }

    /* enable handshaking */
    USB0->ENDPOINT[ENDPOINT_NUM(ep)].ENDPT |= USB_ENDPT_EPHSHK_MASK;

    return 0;
}

static inline int kinetis_usb_set_ep_disable(usb_ep_t ep)
{
    USB0->ENDPOINT[ENDPOINT_NUM(ep)].ENDPT &= ~(USB_ENDPT_EPHSHK_MASK |
                                                USB_ENDPT_EPTXEN_MASK |
                                                USB_ENDPT_EPRXEN_MASK);
    return 0;
}

static inline int kinetis_usb_set_ep_reset(usb_ep_t ep)
{
    uint8_t idx_odd = get_bdt_idx(ep, 0);

    if (ep & ENDPOINT_IN_MASK) {
        kinetis_usb_clr_data1(idx_odd);
        bd_table[idx_odd].stat = 0;
    }
    else {
        kinetis_usb_clr_data1(idx_odd);
        bd_table[idx_odd].stat = BD_OWN_MASK | BD_DTS_MASK;
    }
    return 0;
}

static inline int kinetis_usb_set_ep_stall(usb_ep_t ep)
{
    USB0->ENDPOINT[ENDPOINT_NUM(ep)].ENDPT |= USB_ENDPT_EPSTALL_MASK;
    return 0;
}

static inline int kinetis_usb_set_ep_clr_stall(usb_ep_t ep)
{
    USB0->ENDPOINT[ENDPOINT_NUM(ep)].ENDPT &= ~USB_ENDPT_EPSTALL_MASK;
    kinetis_usb_set_ep_reset(ep);
    return 0;
}

static inline void kinetis_usb_disable_all_eps(void)
{
    for (uint8_t i = 0; i < 16; i++) {
        USB0->ENDPOINT[i].ENDPT = 0;
    }
    memset(bd_table, 0, sizeof(bd_table));

    toggle_bits = 0;
}

static inline int kinetis_usb_set_reset(void)
{
    kinetis_usb_disable_all_eps();
    /* TODO add free for ep buffers */
    
    /* configure BDT for ep0 */
    bd_table[BD_IDX_EP0RX_ODD].bc = USBDEV_EP0_MAX_PACKET;
    bd_table[BD_IDX_EP0RX_ODD].buf_addr = (uint32_t)(&(ep0_buf[BD_IDX_EP0RX_ODD][0]));
    bd_table[BD_IDX_EP0RX_ODD].stat = BD_OWN_MASK | BD_DTS_MASK;
    bd_table[BD_IDX_EP0RX_EVEN].stat = 0;

    bd_table[BD_IDX_EP0TX_ODD].buf_addr = (uint32_t)(&(ep0_buf[BD_IDX_EP0TX_ODD][0]));
    bd_table[BD_IDX_EP0TX_EVEN].buf_addr = 0;

    /* enable ep0 */
    USB0->ENDPOINT[0].ENDPT = USB_ENDPT_EPHSHK_MASK
                            | USB_ENDPT_EPTXEN_MASK
                            | USB_ENDPT_EPRXEN_MASK;

    /* set all BDT ODDs to 1, which then specifies the ODD BDT bank (single buffer mode) */
    USB0->CTL |= USB_CTL_ODDRST_MASK;

    /* clear interrupt status flags */
    USB0->ISTAT   =  0xFF;
    /* clear error flags */
    USB0->ERRSTAT =  0xFF;
    /* enable all error interrupt sources */
    USB0->ERREN   =  0xFF;
    /* set default address */
    USB0->ADDR    =  0x00;

    USB0->INTEN = USB_INTEN_USBRSTEN_MASK |
                  USB_INTEN_TOKDNEEN_MASK |
                  USB_INTEN_SLEEPEN_MASK  |
                  USB_INTEN_SOFTOKEN_MASK |
                  USB_INTEN_ERROREN_MASK;

    return 0;
}

static inline int kinetis_usb_set_connect(void)
{
    /* non-OTG device mode, enable DP Pullup */
    USB0->CONTROL = USB_CONTROL_DPPULLUPNONOTG_MASK;
    /* enable USB module, AKA USBEN bit in CTL1 register */
    //USB0->CTL  |= USB_CTL_USBENSOFEN_MASK;
    return 0;
}

static inline int kinetis_usb_set_disconnect(void)
{
    /* disable USB and DP Pullup */
    USB0->CTL  &= ~USB_CTL_USBENSOFEN_MASK;
    USB0->CONTROL &= ~USB_CONTROL_DPPULLUPNONOTG_MASK;
    return 0;
}

static inline void kinetis_usb_enable_resume(void)
{
    USB0->INTEN |= USB_INTEN_RESUMEEN_MASK;
}

static inline void kinetis_usb_disable_resume(void)
{
    USB0->INTEN &= ~USB_INTEN_RESUMEEN_MASK;
}

int kinetis_set_address(uint8_t address)
{
    USB0->ADDR = address & 0x7f;
    return 0;
}

int kinetis_configure_ep(usb_ep_t ep, size_t size)
{
    uint8_t idx_odd = get_bdt_idx(ep, 0);
    uint8_t idx_even = get_bdt_idx(ep, 1);

    if (size > USBDEV_MAX_PACKET_SIZE) {
        return -ENOMEM;
    }

    ep_out_size[ENDPOINT_NUM(ep)] = size;
    bd_table[idx_odd].bc = size;
    bd_table[idx_odd].buf_addr = (uint32_t)malloc(size);
    bd_table[idx_even].buf_addr = 0;

    return kinetis_usb_set_ep_reset(ep);
}

int kinetis_set_toggle_bit(usb_ep_t ep)
{
    uint8_t idx_odd = get_bdt_idx(ep, 0);

    kinetis_usb_set_data1(idx_odd);
    return 0;
}

int kinetis_clr_toggle_bit(usb_ep_t ep)
{
    uint8_t idx_odd = get_bdt_idx(ep, 0);

    kinetis_usb_clr_data1(idx_odd);
    return 0;
}

int kinetis_read_ep(usb_ep_t ep, uint8_t *data, size_t size)
{
    ep &= ~ENDPOINT_IN_MASK;
    uint8_t idx_odd = get_bdt_idx(ep, 0);
    size_t sz  = bd_table[idx_odd].bc;
    uint8_t *bufp = (uint8_t*)bd_table[idx_odd].buf_addr;

    if (sz > size) {
        sz = size;
    }

    for (uint32_t i = 0; i < sz; i++) {
        data[i] = bufp[i];
    }

    /* Update next toggle bit */
    kinetis_usb_toggle_data1(idx_odd);

    /* Reset buffer descriptor data and fields, set next toggle bit  */
    bd_table[idx_odd].bc = ep_out_size[ENDPOINT_NUM(ep)];
    if (kinetis_usb_get_data1(idx_odd)) {
        bd_table[idx_odd].stat = BD_DTS_MASK | BD_DATA01_MASK;
    }
    else {
        bd_table[idx_odd].stat = BD_DTS_MASK;
    }
    bd_table[idx_odd].stat |= BD_OWN_MASK;

    /* Resume TX toke processing, see USBx_CTL field descriptions */
    USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;

    return sz;
}

int kinetis_write_ep(usb_ep_t ep, uint8_t *data, size_t data_len)
{
    uint8_t idx_odd = get_bdt_idx(ep, 0);
    uint8_t *bufp = (uint8_t*)bd_table[idx_odd].buf_addr;

    while (bd_table[idx_odd].stat & BD_OWN_MASK) {
        LED_R_TOGGLE;
    }

    bd_table[idx_odd].bc = data_len;

    for (uint32_t n = 0; n < data_len; n++) {
        bufp[n] = data[n];
    }

    if (kinetis_usb_get_data1(idx_odd)) {
        bd_table[idx_odd].stat = BD_DTS_MASK | BD_DATA01_MASK;
    }
    else {
        bd_table[idx_odd].stat = BD_DTS_MASK;
    }

    bd_table[idx_odd].stat |= BD_OWN_MASK;

    kinetis_usb_toggle_data1(idx_odd);

    return data_len;
}

int kinetis_usb_ioctl(usbdev_cmd_t cmd, uint32_t arg)
{
    switch (cmd) {
        case USBDEV_SET_CONNECT:
            return kinetis_usb_set_connect();
        case USBDEV_SET_DISCONNECT:
            return kinetis_usb_set_disconnect();
        case USBDEV_SET_ADDRESS:
            return kinetis_set_address(arg);
        case USBDEV_OPT_EP_RESET:
            return kinetis_usb_set_ep_reset(arg);
        case USBDEV_SET_EP_ENABLE:
            return kinetis_usb_set_ep_enable(arg);
        case USBDEV_SET_EP_DISABLE:
            return kinetis_usb_set_ep_disable(arg);
        case USBDEV_SET_EP_STALL:
            return kinetis_usb_set_ep_stall(arg);
        case USBDEV_SET_EP_CLR_STALL:
            return kinetis_usb_set_ep_clr_stall(arg);
        default:
            return -ENOTSUP;
    }
}

void isr_usb0(void)
{
    uint32_t istatus  = USB0->ISTAT;
    uint32_t status  = USB0->STAT;

    /* clear interrupt status bits */
    USB0->ISTAT = istatus;
    
    if (istatus & USB_ISTAT_USBRST_MASK) {
        kinetis_usb_set_reset();
        kinetis_usb_driver.dev->irq_ep_event(kinetis_usb_driver.dev,
                                             USBDEV_CTRL_EP, USBDEV_EVENT_RESET);
        kinetis_usb_driver.dev->irq_bc_event(kinetis_usb_driver.dev,
                                              USBDEV_EVENT_RESET);
    }

    if (istatus == USB_ISTAT_ERROR_MASK) {
        USB0->ERRSTAT = 0xFF;
        kinetis_usb_driver.dev->irq_ep_event(kinetis_usb_driver.dev,
                                             USBDEV_CTRL_EP, USBDEV_EVENT_ERROR);
    }

    if (istatus & USB_ISTAT_SOFTOK_MASK) {
        kinetis_usb_driver.dev->irq_bc_event(kinetis_usb_driver.dev,
                                              USBDEV_EVENT_SOF);
    }

    if (istatus & USB_ISTAT_TOKDNE_MASK) {

        uint32_t ep_num = status >> USB_STAT_ENDP_SHIFT;
        usb_ep_t ep = ((status << 4) & ENDPOINT_IN_MASK) | ep_num;
        uint8_t idx_odd = get_bdt_idx(ep, 0);
        uint8_t token_pid = get_token_pid(idx_odd);

        switch (token_pid) {
            case KINETIS_SETUP_TOKEN:
                kinetis_usb_driver.dev->irq_ep_event(kinetis_usb_driver.dev,
                                                     USBDEV_CTRL_EP, USBDEV_EVENT_SETUP);
                break;

            case KINETIS_OUT_TOKEN:
                kinetis_usb_driver.dev->irq_ep_event(kinetis_usb_driver.dev,
                                                     ep, USBDEV_EVENT_OUT);
                break;

            case KINETIS_IN_TOKEN:
                kinetis_usb_driver.dev->irq_ep_event(kinetis_usb_driver.dev,
                                                     ep, USBDEV_EVENT_IN);
                break;
            default:
                break;
        }
    }

    if (istatus & USB_ISTAT_SLEEP_MASK) {
        kinetis_usb_enable_resume();
        kinetis_usb_driver.dev->irq_ep_event(kinetis_usb_driver.dev,
                                             USBDEV_CTRL_EP, USBDEV_EVENT_SUSPEND);
    }

    if (istatus & USB_ISTAT_RESUME_MASK) {
        kinetis_usb_disable_resume();
        kinetis_usb_driver.dev->irq_ep_event(kinetis_usb_driver.dev,
                                             USBDEV_CTRL_EP, USBDEV_EVENT_RESUME);
    }

    /*
    if (istatus & USB_ISTAT_STALL_MASK) {
        kinetis_usb_driver.dev->irq_bc_event(kinetis_usb_driver.dev,
                                              USBDEV_EVENT_STALL);
    }
    */

    if (sched_context_switch_request) {
        thread_yield();
    }
}

/* implementation of the usbdev driver interface */
usbdev_ops_t kinetis_usb_driver = {
    .dev = NULL,
    .configure_ep = kinetis_configure_ep,
    .set_ep_toggle_bit = kinetis_set_toggle_bit,
    .clr_ep_toggle_bit = kinetis_clr_toggle_bit,
    .write_ep = kinetis_write_ep,
    .read_ep = kinetis_read_ep,
    .ioctl = kinetis_usb_ioctl,
};

int kinetis_usb_register(void)
{
    usbdev_register_driver(kinetis_usb_init);
    return 0;
}

driver_init(kinetis_usb_register);
