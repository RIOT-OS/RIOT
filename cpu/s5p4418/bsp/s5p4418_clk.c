/*
 * (C) Copyright 2009
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include "s5p4418_chip.h"
#include "s5p4418_clkpwr.h"
#include "s5p4418_clk.h"

#ifdef ENABLE_DEBUG
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

/*
 * clock generator macros
 */
#define _InPLL0_        (0)
#define _InPLL1_        (1)
#define _InPLL2_        (2)
#define _InPLL3_        (3)
#define _InEXT1_        (4)
#define _InEXT2_        (5)
#define _InCLKn_        (7)

#define INPUT_CLKS      (6) /* PLL0, PLL1, PLL2, PLL3, EXT1, EXT2 */
#define DVFS_BCLK_PLL   (-1UL)
#define DVFS_CPU_PLL    (-1UL)

#define IGNORE_PLLs     (DVFS_CPU_PLL & DVFS_BCLK_PLL)
#define INPUT_MASK      (((1<<INPUT_CLKS) - 1) & IGNORE_PLLs)

#define _PLL0_          (1 << _InPLL0_)
#define _PLL1_          (1 << _InPLL1_)
#define _PLL2_          (1 << _InPLL2_)
#define _PLL3_          (1 << _InPLL3_)
#define _EXTCLK1_       (1 << _InEXT1_)
#define _EXTCLK2_       (1 << _InEXT2_)

#define _PLL_0_1_       (_PLL0_    | _PLL1_)
#define _PLL_0_2_       (_PLL_0_1_ | _PLL2_)
#define _PLL_0_3_       (_PLL_0_2_ | _PLL3_)
#define _CLKOUTn_       (0)

#define _PCLK_          (1<<16)
#define _BCLK_          (1<<17)
#define _GATE_PCLK_     (1<<20)
#define _GATE_BCLK_     (1<<21)
#define PLCK_MASK       (_GATE_PCLK_ | _PCLK_)
#define BLCK_MASK       (_GATE_BCLK_ | _BCLK_)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({\
    const typeof( ((type *)0)->member ) *__mptr = (ptr);\
    (type *)( (char *)__mptr - offsetof(type,member) );})

struct s5p4418_clk_periph {
    const char  *dev_name;
    int          dev_id;
    char         periph_id;
    void        *base_addr;
    /* clock config */
    char         level;
    unsigned int clk_mask0;
    unsigned int clk_mask1;
    short        clk_src0;
    short        clk_div0;
    char         clk_inv0;
    short        clk_src1;
    short        clk_div1;
    char         clk_inv1;
    long         clk_ext1;
    long         clk_ext2;
};

struct s5p4418_clk_dev {
    struct clk  clk;
    struct clk *link;
    const char *name;
    struct s5p4418_clk_periph *peri;
};

struct clklink_dev {
    char *name;
    int   id;
};

struct clkgen_register {
    volatile uint32_t CLKENB;
    volatile uint32_t CLKGEN[4];
};

#define CLK_PERI_1S(name, devid, id, base, mk) [id] = {\
    .dev_name = name, .dev_id = devid,.periph_id = id, .level = 1,\
    .base_addr = (void*)base, .clk_mask0 = mk, }

#define CLK_PERI_2S(name, devid, id, base, mk, mk2) [id] = {\
    .dev_name = name, .dev_id = devid, .periph_id = id, .level = 2,\
    .base_addr = (void*)base, .clk_mask0 = mk, .clk_mask1 = mk2, }

static const char *clk_plls[] = {
    CORECLK_NAME_PLL0, CORECLK_NAME_PLL1, CORECLK_NAME_PLL2, CORECLK_NAME_PLL3,
    CORECLK_NAME_FCLK, CORECLK_NAME_MCLK, CORECLK_NAME_BCLK, CORECLK_NAME_PCLK,
    CORECLK_NAME_HCLK,
};

static struct s5p4418_clk_periph clk_periphs[] = {
    CLK_PERI_1S(DEV_NAME_TIMER, 0, CLK_ID_TIMER_0, PHY_BASEADDR_CLKGEN14, (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_TIMER, 1, CLK_ID_TIMER_1, PHY_BASEADDR_CLKGEN0,  (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_TIMER, 2, CLK_ID_TIMER_2, PHY_BASEADDR_CLKGEN1,  (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_TIMER, 3, CLK_ID_TIMER_3, PHY_BASEADDR_CLKGEN2,  (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_UART,  0, CLK_ID_UART_0,  PHY_BASEADDR_CLKGEN22, (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_UART,  1, CLK_ID_UART_1,  PHY_BASEADDR_CLKGEN24, (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_UART,  2, CLK_ID_UART_2,  PHY_BASEADDR_CLKGEN23, (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_UART,  3, CLK_ID_UART_3,  PHY_BASEADDR_CLKGEN25, (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_UART,  4, CLK_ID_UART_4,  PHY_BASEADDR_CLKGEN26, (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_UART,  5, CLK_ID_UART_5,  PHY_BASEADDR_CLKGEN27, (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_PWM,   0, CLK_ID_PWM_0,   PHY_BASEADDR_CLKGEN13, (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_PWM,   1, CLK_ID_PWM_1,   PHY_BASEADDR_CLKGEN3,  (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_PWM,   2, CLK_ID_PWM_2,   PHY_BASEADDR_CLKGEN4,  (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_PWM,   3, CLK_ID_PWM_3,   PHY_BASEADDR_CLKGEN5,  (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_I2C,   0, CLK_ID_I2C_0,   PHY_BASEADDR_CLKGEN6,  (_GATE_PCLK_)),
    CLK_PERI_1S(DEV_NAME_I2C,   1, CLK_ID_I2C_1,   PHY_BASEADDR_CLKGEN7,  (_GATE_PCLK_)),
    CLK_PERI_1S(DEV_NAME_I2C,   2, CLK_ID_I2C_2,   PHY_BASEADDR_CLKGEN8,  (_GATE_PCLK_)),
    CLK_PERI_2S(DEV_NAME_I2S,   0, CLK_ID_I2S_0,   PHY_BASEADDR_CLKGEN15, (_PLL_0_3_|_EXTCLK1_), (_CLKOUTn_)),
    CLK_PERI_2S(DEV_NAME_I2S,   1, CLK_ID_I2S_1,   PHY_BASEADDR_CLKGEN16, (_PLL_0_3_|_EXTCLK1_), (_CLKOUTn_)),
    CLK_PERI_2S(DEV_NAME_I2S,   2, CLK_ID_I2S_2,   PHY_BASEADDR_CLKGEN17, (_PLL_0_3_|_EXTCLK1_), (_CLKOUTn_)),
    CLK_PERI_1S(DEV_NAME_SDHC,  0, CLK_ID_SDHC_0,  PHY_BASEADDR_CLKGEN18, (_PLL_0_2_|_GATE_PCLK_)),
    CLK_PERI_1S(DEV_NAME_SDHC,  1, CLK_ID_SDHC_1,  PHY_BASEADDR_CLKGEN19, (_PLL_0_2_|_GATE_PCLK_)),
    CLK_PERI_1S(DEV_NAME_SDHC,  2, CLK_ID_SDHC_2,  PHY_BASEADDR_CLKGEN20, (_PLL_0_2_|_GATE_PCLK_)),
    CLK_PERI_1S(DEV_NAME_SPI,   0, CLK_ID_SPI_0,   PHY_BASEADDR_CLKGEN37, (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_SPI,   1, CLK_ID_SPI_1,   PHY_BASEADDR_CLKGEN38, (_PLL_0_2_)),
    CLK_PERI_1S(DEV_NAME_SPI,   2, CLK_ID_SPI_2,   PHY_BASEADDR_CLKGEN39, (_PLL_0_2_)),
};

static struct clklink_dev clk_link[] = {};

#define PERIPH_NUM  ((int)ARRAY_SIZE(clk_periphs))
#define CLKPLL_NUM  ((int)ARRAY_SIZE(clk_plls))
#define CLKLINK_NUM ((int)ARRAY_SIZE(clk_link))
#define DEVICE_NUM  (CLKPLL_NUM + PERIPH_NUM + CLKLINK_NUM)
#define MAX_DIVIDER ((1<<8) - 1) // 256, align 2

static struct s5p4418_clk_dev (clk_devices[DEVICE_NUM]);
#define clk_dev_get(n)   ((struct s5p4418_clk_dev *)&clk_devices[n])
#define clk_container(p) (container_of(p, struct s5p4418_clk_dev, clk))

/* dynamic frequency pll num */
static unsigned int core_hz[15];    /* core clock */
static unsigned int support_dvfs = 1;

/*
 * CLKGEN HW
 */
static inline void peri_clk_bclk(void *base, int on)
{
    struct clkgen_register *preg = base;
    register uint32_t val;

    val  = readl((uint32_t)&preg->CLKENB);
    val &= ~(0x3);
    val |=  (on ? 3 : 0) & 0x3; /* always BCLK */
    writel(val, (uint32_t)&preg->CLKENB);
}

static inline void peri_clk_pclk(void *base, int on)
{
    struct clkgen_register *preg = base;
    register uint32_t val;

    if (!on) return;

    val  = readl((uint32_t)&preg->CLKENB);
    val &= ~(1 << 3);
    val |=  (1 << 3);
    writel(val, (uint32_t)&preg->CLKENB);
}

static inline void peri_clk_rate(void *base, int level, int src, int div)
{
    struct clkgen_register *preg = base;
    register uint32_t val;

    val  = readl((uint32_t)&preg->CLKGEN[level<<1]);
    val &= ~(0x07   << 2);
    val |=  (src    << 2);    /* source */
    val &= ~(0xFF   << 5);
    val |=  (div-1) << 5;    /* divider */
    writel(val, (uint32_t)&preg->CLKGEN[level<<1]);
}

static inline void peri_clk_invert(void *base, int level, int inv)
{
    struct clkgen_register *preg = base;
    register uint32_t val;

    val  = readl((uint32_t)&preg->CLKGEN[level<<1]);
    val &= ~(1  << 1);
    val |=  (inv<< 1);
    writel(val, (uint32_t)&preg->CLKGEN[level<<1]);
}

static inline void peri_clk_enable(void *base)
{
    struct clkgen_register *preg = base;
    register uint32_t val;

    val  = readl((uint32_t)&preg->CLKENB);
    val &= ~(1 << 2);
    val |=  (1 << 2);
    writel(val, (uint32_t)&preg->CLKENB);
}

static inline void peri_clk_disable(void *base)
{
    struct clkgen_register *preg = base;
    register uint32_t val;

    val  = readl((uint32_t)&preg->CLKENB);
    val &= ~(1 << 2);
    val |=  (0 << 2);
    writel(val, (uint32_t)&preg->CLKENB);
}

/*
 * Core clocks
 */
static unsigned int pll_get_rate(unsigned int pllN, unsigned int xtal)
{
    unsigned int val, val1, nP, nM, nS, nK;
    val   = CLKPWR->PLLSETREG[pllN];
    val1  = CLKPWR->PLLSETREG_SSCG[pllN];
    xtal /= 1000;    /* Unit Khz */

    nP = (val >> 18) & 0x03F;
    nM = (val >>  8) & 0x3FF;
    nS = (val >>  0) & 0x0FF;
    nK = (val1 >> 16) & 0xFFFF;

    if(pllN < 2)
        return (unsigned int)(((nM * xtal)/nP)>>nS)*1000;
    else
        return (unsigned int)((((nM * xtal)/nP)>>nS)+((((nK * xtal)/nP)>>nS)>>16))*1000;
}

/* dvo : 0=CPU, 1=BUS, 2=MEM, 3=3D, 4=MPEG */
static unsigned int pll_get_dvo(unsigned int dvo)
{
    return (CLKPWR->DVOREG[dvo] & 0x7);
}

static unsigned int pll_get_div(unsigned int dvo)
{
    unsigned int val = CLKPWR->DVOREG[dvo];
    unsigned int div = ((((val>>21)&0x3F)+1)<<24) |
                        ((((val>>15)&0x3F)+1)<<16) |
                        ((((val>> 9)&0x3F)+1)<< 8) |
                        ((((val>> 3)&0x3F)+1)<< 0);
    return div;
}

#define CFG_SYS_PLLFIN      24000000UL // XTAL frequency

#define PLLN_RATE(n)         (pll_get_rate(n, CFG_SYS_PLLFIN))   /* 0~3 */
#define FCLK_RATE(n)         (pll_get_rate(pll_get_dvo (0), CFG_SYS_PLLFIN) / ((pll_get_div(0)>> 0)&0x3F))
#define MCLK_RATE(n)         (pll_get_rate(pll_get_dvo (2), CFG_SYS_PLLFIN) / \
                            ((pll_get_div(2)>> 0)&0x3F) / ((pll_get_div(2)>> 8)&0x3F))
#define BCLK_RATE(n)         (pll_get_rate(pll_get_dvo (1), CFG_SYS_PLLFIN) / \
                            ((pll_get_div(1)>> 0)&0x3F))
#define PCLK_RATE(n)         (pll_get_rate(pll_get_dvo (1), CFG_SYS_PLLFIN) / \
                            ((pll_get_div(1)>> 0)&0x3F) / ((pll_get_div(1)>> 8)&0x3F))
#define HCLK_RATE(n)         (pll_get_rate(pll_get_dvo (0), CFG_SYS_PLLFIN) / \
                            ((pll_get_div(0)>> 0)&0x3F) / ((pll_get_div(0)>> 8)&0x3F))
#define MDCLK_RATE(n)        (pll_get_rate(pll_get_dvo (2), CFG_SYS_PLLFIN) / \
                            ((pll_get_div(2)>> 0)&0x3F) / ((pll_get_div(2)>> 8)&0x3F))
#define MBCLK_RATE(n)        (pll_get_rate(pll_get_dvo (2), CFG_SYS_PLLFIN) / \
                            ((pll_get_div(2)>> 0)&0x3F) / \
                            ((pll_get_div(2)>> 8)&0x3F) / \
                            ((pll_get_div(2)>>16)&0x3F))
#define    MPCLK_RATE(n)     (pll_get_rate(pll_get_dvo (2), CFG_SYS_PLLFIN) / \
                            ((pll_get_div(2)>> 0)&0x3F) / \
                            ((pll_get_div(2)>> 8)&0x3F) / \
                            ((pll_get_div(2)>>16)&0x3F) / \
                            ((pll_get_div(2)>>24)&0x3F))
#define    G3D_BCLK_RATE(n)  (pll_get_rate(pll_get_dvo (3), CFG_SYS_PLLFIN) / \
                            ((pll_get_div(3)>> 0)&0x3F))
#define    MPG_BCLK_RATE(n)  (pll_get_rate(pll_get_dvo (4), CFG_SYS_PLLFIN) / \
                            ((pll_get_div(4)>> 0)&0x3F))
#define    MPG_PCLK_RATE(n)  (pll_get_rate(pll_get_dvo (4), CFG_SYS_PLLFIN) / \
                            ((pll_get_div(4)>> 0)&0x3F) / \
                            ((pll_get_div(4)>> 8)&0x3F))

static unsigned long core_update_rate(int type)
{
    unsigned long rate = 0;
    switch (type) {
    case  0: rate = core_hz[ 0] = PLLN_RATE ( 0);    break;  // PLL 0
    case  1: rate = core_hz[ 1] = PLLN_RATE ( 1);    break;  // PLL 1
    case  2: rate = core_hz[ 2] = PLLN_RATE ( 2);    break;  // PLL 2
    case  3: rate = core_hz[ 3] = PLLN_RATE ( 3);    break;  // PLL 3
    case  4: rate = core_hz[ 4] = FCLK_RATE ( 4);    break;  // FCLK
    case  5: rate = core_hz[ 5] = MCLK_RATE ( 5);    break;  // MCLK
    case  6: rate = core_hz[ 6] = BCLK_RATE ( 6);    break;  // BCLK
    case  7: rate = core_hz[ 7] = PCLK_RATE ( 7);    break;  // PCLK
    case  8: rate = core_hz[ 8] = HCLK_RATE ( 8);    break;  // HCLK
    case  9: rate = core_hz[ 9] = MDCLK_RATE( 9);    break;  // MDCLK
    case 10: rate = core_hz[10] = MBCLK_RATE(10);    break;  // MBCLK
    case 11: rate = core_hz[11] = MPCLK_RATE(11);    break;  // MPCLK
    case 12: rate = core_hz[12] = G3D_BCLK_RATE(12); break;  // G3D BCLK
    case 13: rate = core_hz[13] = MPG_BCLK_RATE(13); break;  // MPG BCLK
    case 14: rate = core_hz[14] = MPG_PCLK_RATE(14); break;  // MPG PCLK
    };
    return rate;
}

static inline long core_rate(int type)
{
    return core_update_rate(type);
}

static inline long core_set_rate(struct clk *clk, long rate)
{
    return clk->rate;
}

static inline long get_rate_divide(long rate, long request,
                int align, int *divide)
{
    int div = (rate/request);
    int max = MAX_DIVIDER & ~(align-1);
    int adv = (div & ~(align-1)) + align;

    if (!div) {
        if (divide)
            *divide = 1;
        return rate;
    }

    if (1 != div)
        div &= ~(align-1);

    if (div != adv &&
        abs(request - rate/div) >
        abs(request - rate/adv))
        div = adv;

    div = (div > max ? max : div);
    if (divide)
        *divide = div;

    return (rate/div);
}

/*
 * CLK Interfaces
 */
struct clk *clk_get(struct device *dev, const char *id)
{
    struct s5p4418_clk_dev *cdev = clk_dev_get(0);
    struct clk *clk = NULL;
    const char *str = NULL, *c = NULL;
    int i, devid;

    if (dev)
        str = dev_name(dev);

    if (id)
        str = id;

    for (i = 0; DEVICE_NUM > i; i++, cdev++) {
        if (NULL == cdev->name)
            continue;
        if (!strncmp(cdev->name, str, strlen(cdev->name))) {
            c = strrchr((const char*)str, (int)'.');
            if (NULL == c || !cdev->peri)
                break;
            devid = strtoul(++c, NULL, 10);
            if (cdev->peri->dev_id == devid)
                break;
        }
    }

    if (DEVICE_NUM > i)
        clk = &cdev->clk;
    else
        clk = &(clk_dev_get(7))->clk;    /* pclk */

    return clk ? clk : ERR_PTR(-ENOENT);
}

struct clk *clk_get_sys(const char *dev_id, const char *con_id)
{
    /* SMP private timer */
    if (!strcmp(dev_id, "smp_twd"))
        return clk_get(NULL, CORECLK_NAME_HCLK);

    return clk_get(NULL, (char *)dev_id);
}

void clk_put(struct clk *clk)
{

}

unsigned long clk_get_rate(struct clk *clk)
{
    struct s5p4418_clk_dev *cdev = clk_container(clk);

    if (cdev->link)
        clk = cdev->link;

    return clk->rate;
}

/*
 *                 -----------       8Bit
 *        PLL0 -- |           |     -------
 *        PLL1 -- | CLKGEN 0  | -  | DIV0  | -----------
 *        .... -- |           |     -------   |
 *                 -----------                |
 *        ------------------------------------|
 *        |
 *        |         -----------       8Bit
 *        -------  | (7)       |    -------
 *        PLL0 --  | CLKGEN 1  | - | DIV1  | ----------
 *        PLL1 --  |           |    -------
 *                 -----------
 */
long clk_round_rate(struct clk *clk, unsigned long rate)
{
    struct s5p4418_clk_dev *pll = NULL, *cdev = clk_container(clk);
    struct s5p4418_clk_periph *peri = cdev->peri;
    unsigned long request = rate, rate_hz = 0;
#ifdef ENABLE_DEBUG    
    unsigned long clock_hz;
    unsigned long freq_hz;
#endif    
    unsigned int mask;
    int level, div[2] = { 0, };
    int i, n, clk2 = 0;
    short s1 = 0, s2 = 0, d1 = 0, d2 = 0;

    if (!peri)
        return core_set_rate(clk, rate);

    level = peri->level;
    mask = peri->clk_mask0;
    DEBUG("clk: %s.%d request = %ld [input=0x%x]\r\n",
            peri->dev_name, peri->dev_id, rate, mask);

    if (!(INPUT_MASK & mask)) {
        if (PLCK_MASK & mask)
            return core_rate(CORECLK_ID_PCLK);
        else if (BLCK_MASK & mask)
            return core_rate(CORECLK_ID_BCLK);
        else
            return clk->rate;
    }

next:
    for (n = 0; INPUT_CLKS > n; n++) {
        if (!(((mask & INPUT_MASK) >> n) & 0x1))
            continue;

        if (_InEXT1_ == n) {
            rate = peri->clk_ext1;
        } else if (_InEXT2_ == n) {
            rate = peri->clk_ext2;
        } else {
            pll  = clk_dev_get(n);
            rate = pll->clk.rate;
        }

        if (! rate)
            continue;
#ifdef ENABLE_DEBUG
        clock_hz = rate;
#endif
        for (i = 0; level > i ; i++)
            rate = get_rate_divide(rate, request, 2, &div[i]);

        if (rate_hz && (abs(rate-request) > abs(rate_hz-request)))
            continue;

        DEBUG("clk: %s.%d, pll.%d[%u] request[%ld] calc[%ld]\r\n",
            peri->dev_name, peri->dev_id, n, (unsigned int)pll->clk.rate, (long int)request, (long int)rate);

        if (clk2) {
            s1 = -1, d1 = -1;    /* not use */
            s2 =  n, d2 = div[0];
        } else {
            s1 = n, d1 = div[0];
            s2 = _InCLKn_, d2 = div[1];
        }
        rate_hz = rate;
#ifdef ENABLE_DEBUG           
        freq_hz = clock_hz;
#endif        
    }

    /* search 2th clock from input */
    if (!clk2 && abs(rate_hz-request) &&
        peri->clk_mask1 & ((1<<INPUT_CLKS) - 1)) {
        clk2 = 1;
        mask = peri->clk_mask1;
        level = 1;
        goto next;
    }

    peri->clk_src0 = s1;
    peri->clk_div0 = d1;
    peri->clk_src1 = s2;
    peri->clk_div1 = d2;

    clk->rate = rate_hz;

    DEBUG("clk: %s.%d, level[%d] src[%d,%d] %ld /(div0: %d * div1: %d) = %ld, %ld diff (%ld)\r\n",
        peri->dev_name, peri->dev_id, peri->level, peri->clk_src0, peri->clk_src1, (long int)freq_hz,
        peri->clk_div0, peri->clk_div1, (long int)rate_hz, (long int)request, (long int)abs(rate_hz-request));

    return clk->rate;
}

int clk_set_rate(struct clk *clk, unsigned long rate)
{
    struct s5p4418_clk_dev *cdev = clk_container(clk);
    struct s5p4418_clk_periph *peri = cdev->peri;
    int i;

    if (!peri)
        return core_set_rate(clk, rate);

    clk_round_rate(clk, rate);

    for (i = 0; peri->level > i ; i++)    {
        int s = (0 == i ? peri->clk_src0: peri->clk_src1);
        int d = (0 == i ? peri->clk_div0: peri->clk_div1);
        if (-1 == s)
            continue;
        peri_clk_rate(peri->base_addr, i, s, d);
        DEBUG("clk: %s.%d (%p) set_rate [%d] src[%d] div[%d]\r\n",
            peri->dev_name, peri->dev_id, peri->base_addr, i, s, d);
    }

    return clk->rate;
}

int clk_enable(struct clk *clk)
{
    struct s5p4418_clk_dev *cdev = clk_container(clk);
    struct s5p4418_clk_periph *peri = cdev->peri;
    int i = 0, inv = 0;

    if (! peri)
        return 0;

    DEBUG("clk: %s.%d enable (BCLK=%s, PCLK=%s)\r\n",
        peri->dev_name, peri->dev_id, _GATE_BCLK_ & peri->clk_mask0 ? "ON":"PASS",
        _GATE_PCLK_ & peri->clk_mask0 ? "ON":"PASS");

    if (!(INPUT_MASK & peri->clk_mask0)) {
        /*
         * Gated BCLK/PCLK enable
         */
        if (_GATE_BCLK_ & peri->clk_mask0)
            peri_clk_bclk(peri->base_addr, 1);

        if (_GATE_PCLK_ & peri->clk_mask0)
            peri_clk_pclk(peri->base_addr, 1);

        return 0;
    }

    /* invert */
    inv = peri->clk_inv0;
    for (; peri->level > i; i++, inv = peri->clk_inv1)
        peri_clk_invert(peri->base_addr, i, inv);

    /*
     * Gated BCLK/PCLK enable
     */
    if (_GATE_BCLK_ & peri->clk_mask0)
        peri_clk_bclk(peri->base_addr, 1);

    if (_GATE_PCLK_ & peri->clk_mask0)
        peri_clk_pclk(peri->base_addr, 1);

    /* CLKGEN enable */
    peri_clk_enable(peri->base_addr);

    return 0;
}

void clk_disable(struct clk *clk)
{
    struct s5p4418_clk_dev *cdev = clk_container(clk);
    struct s5p4418_clk_periph *peri = cdev->peri;

    if (! peri)
        return;

    DEBUG("clk: %s.%d disable\r\n", peri->dev_name, peri->dev_id);

    if (!(INPUT_MASK & peri->clk_mask0)) {
        /*
         * Gated BCLK/PCLK disable
         */
        if (_GATE_BCLK_ & peri->clk_mask0)
            peri_clk_bclk(peri->base_addr, 0);

        if (_GATE_PCLK_ & peri->clk_mask0)
            peri_clk_pclk(peri->base_addr, 0);

        return;
    }

    peri_clk_disable(peri->base_addr);

    /*
     * Gated BCLK/PCLK disable
     */
    if (_GATE_BCLK_ & peri->clk_mask0)
        peri_clk_bclk(peri->base_addr, 0);

    if (_GATE_PCLK_ & peri->clk_mask0)
        peri_clk_pclk(peri->base_addr, 0);

    return;
}

/*
 * Core clocks APIs
 */
void s5p4418_clk_init(void)
{
    struct s5p4418_clk_dev *cdev = clk_devices;
    struct s5p4418_clk_periph *peri = clk_periphs;
    struct clk *clk = NULL;
    int i = 0;

    for (i = 0; ARRAY_SIZE(core_hz) > i; i++)
        core_update_rate(i);

    for (i = 0; (CLKPLL_NUM+PERIPH_NUM) > i; i++, cdev++) {
        if (CLKPLL_NUM > i) {
            cdev->name = clk_plls[i];
            clk = &cdev->clk;
            clk->rate = core_rate(i);
            continue;
        }

        peri = &clk_periphs[i-CLKPLL_NUM];
        peri->base_addr = IO_ADDRESS(peri->base_addr);

        cdev->peri = peri;
        cdev->name = peri->dev_name;

        if (!(INPUT_MASK & peri->clk_mask0)) {
            if (BLCK_MASK & peri->clk_mask0)
                cdev->clk.rate = core_rate(CORECLK_ID_BCLK);
            if (PLCK_MASK & peri->clk_mask0)
                cdev->clk.rate = core_rate(CORECLK_ID_PCLK);
        }

        /* prevent uart clock for low level debug message */
        #ifndef CONFIG_DEBUG_NX_UART
        //if (peri->dev_name) {
        //  peri_clk_disable(peri->base_addr);
        //  peri_clk_bclk(peri->base_addr, 0);
        //  peri_clk_pclk(peri->base_addr, 0);
        //}
        #endif
    }

    DEBUG("CPU : Clock Generator = %d EA, ", DEVICE_NUM);
}

void s5p4418_clk_print(void)
{
    int pll, cpu, i = 0;
    for (i = 0; ARRAY_SIZE(core_hz) > i; i++)
        core_update_rate(i);

    DEBUG("PLL : [0] = %10u, [1] = %10u, [2] = %10u, [3] = %10u\r\n",
        core_hz[0], core_hz[1], core_hz[2], core_hz[3]);

    /* CPU */
    pll = pll_get_dvo (0), cpu = pll, support_dvfs = 1;
    DEBUG("PLL%d: CPU FCLK = %10u, HCLK = %9u\r\n", pll, core_hz[4], core_hz[8]);

    /* BUS */
    pll = pll_get_dvo (1), support_dvfs = pll == cpu ? 0 : 1;
    DEBUG("PLL%d: BUS BCLK = %10u, PCLK = %9u\r\n", pll, core_hz[6], core_hz[7]);
    if (pll == cpu) support_dvfs = 0;

    /* MEM */
    pll = pll_get_dvo (2), support_dvfs = pll == cpu ? 0 : 1;
    DEBUG("PLL%d: MEM MCLK = %10u, DCLK = %9u, BCLK = %9u, PCLK = %9u\r\n",
        pll, core_hz[5], core_hz[9], core_hz[10], core_hz[11]);

    /* G3D */
    pll = pll_get_dvo (3), support_dvfs = pll == cpu ? 0 : 1;
    DEBUG("PLL%d: G3D BCLK = %10u\r\n", pll, core_hz[12]);

    /* MPEG */
    pll = pll_get_dvo (4), support_dvfs = pll == cpu ? 0 : 1;
    DEBUG("PLL%d: MPG BCLK = %10u, PCLK = %9u\r\n", pll, core_hz[13], core_hz[14]);
}

