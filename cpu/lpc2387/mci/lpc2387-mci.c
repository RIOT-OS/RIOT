/*-----------------------------------------------------------------------*/
/* MMCv3/SDv1/SDv2 (in native mode via MCI) control module (C)ChaN, 2010 */
/*-----------------------------------------------------------------------*/
/* This program is opened under license policy of following trems.
/
/  Copyright (C) 2010, ChaN, all right reserved.
/
/ * This program is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial use UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/---------------------------------------------------------------------------*/

#include <string.h>
#include "cpu.h"
#include "VIC.h"
#include "xtimer.h"
#include "diskio.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* --- MCI configurations --- */
#define N_BUF       4           /* Block transfer FIFO depth (>= 2) */
#define USE_4BIT    1           /* Use wide bus mode if SDC is detected */
#define PCLK        36000000UL  /* PCLK supplied to MCI module */
#define MCLK_ID     400000UL    /* MCICLK for ID state (100k-400k) */
#define MCLK_RW     18000000UL  /* MCICLK for data transfer (PCLK divided by even number) */

/* This MCI driver assumes that MCLK_RW is CCLK/4 or slower. If block buffer underrun/overrun
/  occured due to any interrupt by higher priority process or slow external memory, increasing
/  N_BUF or decreasing MCLK_RW will solve it. */


/* ----- Port definitions ----- */
#define SOCKINS     !(FIO0PIN2 & 0x20)  /* Card detect switch */
#define SOCKWP      (FIO0PIN2 & 0x04)   /* Write protect switch */


/* ----- MMC/SDC command ----- */
#define CMD0    (0)             /* GO_IDLE_STATE */
#define CMD1    (1)             /* SEND_OP_COND (MMC) */
#define CMD2    (2)             /* ALL_SEND_CID */
#define CMD3    (3)             /* SEND_RELATIVE_ADDR */
#define ACMD6   (6|0x80)        /* SET_BUS_WIDTH (SDC) */
#define CMD7    (7)             /* SELECT_CARD */
#define CMD8    (8)             /* SEND_IF_COND */
#define CMD9    (9)             /* SEND_CSD */
#define CMD10   (10)            /* SEND_CID */
#define CMD12   (12)            /* STOP_TRANSMISSION */
#define CMD13   (13)            /* SEND_STATUS */
#define ACMD13  (13|0x80)       /* SD_STATUS (SDC) */
#define CMD16   (16)            /* SET_BLOCKLEN */
#define CMD17   (17)            /* READ_SINGLE_BLOCK */
#define CMD18   (18)            /* READ_MULTIPLE_BLOCK */
#define CMD23   (23)            /* SET_BLK_COUNT (MMC) */
#define ACMD23  (23|0x80)       /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24   (24)            /* WRITE_BLOCK */
#define CMD25   (25)            /* WRITE_MULTIPLE_BLOCK */
#define CMD32   (32)            /* ERASE_ER_BLK_START */
#define CMD33   (33)            /* ERASE_ER_BLK_END */
#define CMD38   (38)            /* ERASE */
#define ACMD41  (41|0x80)       /* SEND_OP_COND (SDC) */
#define CMD55   (55)            /* APP_CMD */

/* Card type flags (CardType) */
#define CT_MMC      0x01        /* MMC ver 3 */
#define CT_SD1      0x02        /* SD ver 1 */
#define CT_SD2      0x04        /* SD ver 2 */
#define CT_SDC      (CT_SD1|CT_SD2) /* SD */
#define CT_BLOCK    0x08        /* Block addressing */


/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

static volatile diskio_sta_t Stat = DISKIO_STA_NOINIT;  /* Disk status */

static unsigned short CardRCA;          /* Assigned RCA */
static unsigned char CardType,          /* Card type flag */
       CardInfo[16 + 16 + 4];   /* CSD(16), CID(16), OCR(4) */

static volatile unsigned char XferStat,             /* b3:MCI error, b2:Overrun, b1:Write, b0:Read */
       XferWc,          /* Write block counter */
       XferWp, XferRp;  /* R/W index of block FIFO */

static unsigned long DmaBuff[N_BUF][128]  __attribute__((section(".usbdata")));  /* Block transfer FIFO */
static unsigned long LinkList [N_BUF][4]  __attribute__((section(".usbdata")));  /* DMA link list */

void Isr_MCI(void) __attribute__((interrupt("IRQ")));
void Isr_GPDMA(void) __attribute__((interrupt("IRQ")));

/*-----------------------------------------------------------------------*/
/* Interrupt service routine for data transfer                           */
/*-----------------------------------------------------------------------*/

void Isr_MCI(void)
{
    unsigned long ms;
    unsigned char xs;

    ms = MCI_STATUS & 0x073A;   /* Clear MCI interrupt status */
    MCI_CLEAR = ms;

    xs = XferStat;

    if (ms & 0x400) {           /* A block transfer completed (DataBlockEnd) */
        if (xs & 1) {               /* In card read operation */
            if (ms & 0x100) {           /* When last block is received (DataEnd), */
                GPDMA_SOFT_BREQ = 0x10;    /* Pop off remaining data in the MCIFIFO */
            }

            unsigned char n = (XferWp + 1) % N_BUF;   /* Next write buffer */
            XferWp = n;

            if (n == XferRp) {
                xs |= 4;    /* Check block overrun */
            }
        }
        else {                      /* In card write operation */
            unsigned char n = (XferRp + 1) % N_BUF;   /* Next read buffer */
            XferRp = n;

            if (n == XferWp) {
                xs |= 4;    /* Check block underrun */
            }
        }
    }
    else {                  /* An MCI error occured (not DataBlockEnd) */
        xs |= 8;
    }

    XferStat = xs;

    VICVectAddr = 0;
}

void Isr_GPDMA(void)
{
    if (GPDMA_INT_TCSTAT & BIT0) {
        GPDMA_INT_TCCLR = 0x01;             /* Clear GPDMA interrupt flag */

        if (XferStat & 2) {
            /* In write operation */
            if (--XferWc == N_BUF) { /* Terminate LLI */
                LinkList[XferRp % N_BUF][2] = 0;
            }
        }
    }
    else {
        GPDMA_INT_TCCLR = 0x3;
    }

    VICVectAddr = 0;
}


/*-----------------------------------------------------------------------*/
/* Ready for data reception                                              */
/*-----------------------------------------------------------------------*/

/**
 * @param blks          Number of blocks to receive (1..127)
 * @param bs            Block size (64 or 512)
 * */
static void ready_reception(unsigned int blks, unsigned int bs)
{
    unsigned int n;
    unsigned long dma_ctrl;

    /* ------ Setting up GPDMA Ch-0 ------ */
    GPDMA_CH0_CFG &= 0xFFF80420;        /* Disable ch-0 */
    GPDMA_INT_TCCLR = 0x01;             /* Clear interrupt flag */
    dma_ctrl = 0x88492000 | (bs / 4);   /* 1_000_1_0_00_010_010_010_010_************ */

    /* Create link list */
    for (n = 0; n < N_BUF; n++) {
        LinkList[n][0] = (unsigned long)&MCI_FIFO;
        LinkList[n][1] = (unsigned long)DmaBuff[n];
        LinkList[n][2] = (unsigned long)LinkList[(n + 1) % N_BUF];
        LinkList[n][3] = dma_ctrl;
    }

    /* Load first LLI */
    GPDMA_CH0_SRC = LinkList[0][0];
    GPDMA_CH0_DEST = LinkList[0][1];
    GPDMA_CH0_LLI = LinkList[0][2];
    GPDMA_CH0_CTRL = LinkList[0][3];

    /* Enable ch-0 */
    GPDMA_CH0_CFG |= 0x19009;           /* *************_0_0_1_1_0_010_*_0000_*_0100_1 */

    /* --------- Setting up MCI ---------- */

    XferRp = 0;
    XferWp = 0;                 /* Block FIFO R/W index */
    XferStat = 1;                           /* Transfer status: MCI --> Memory */

    MCI_DATA_LEN = bs * blks;               /* Set total data length */
    MCI_DATA_TMR = (unsigned long)(MCLK_RW * 0.2);  /* Data timer: 0.2sec */
    MCI_CLEAR = 0x72A;                      /* Clear status flags */
    MCI_MASK0 = 0x72A;                      /* DataBlockEnd StartBitErr DataEnd RxOverrun DataTimeOut DataCrcFail */

    for (n = 0; bs > 1; bs >>= 1, n += 0x10);

    MCI_DATA_CTRL  = n | 0xB;               /* Start to receive data blocks */
}


/*-----------------------------------------------------------------------*/
/* Start to transmit a data block                                        */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
/*
 * @param blks   Number of blocks to be transmitted (1..127)
 * */
static void start_transmission(unsigned char blks)
{
    unsigned int n;
    unsigned long dma_ctrl;


    /* ------ Setting up GPDMA Ch-0 ------ */

    GPDMA_CH0_CFG &= 0xFFF80420;        /* Disable ch-0 */
    GPDMA_INT_TCCLR = 0x01;             /* Clear interrupt flag */
    dma_ctrl = 0x84492080;              /* 1_000_0_1_00_010_010_010_010_000010000000 */

    /* Create link list */
    for (n = 0; n < N_BUF; n++) {
        LinkList[n][0] = (unsigned long)DmaBuff[n];
        LinkList[n][1] = (unsigned long)&MCI_FIFO;
        LinkList[n][2] = (n == (unsigned int) (blks - 1)) ? 0 : (unsigned long)LinkList[(n + 1) % N_BUF];
        LinkList[n][3] = dma_ctrl;
    }

    /* Load first LLI */
    GPDMA_CH0_SRC = LinkList[0][0];
    GPDMA_CH0_DEST = LinkList[0][1];
    GPDMA_CH0_LLI = LinkList[0][2];
    GPDMA_CH0_CTRL = LinkList[0][3];

    /* Enable ch-0 */
    GPDMA_CH0_CFG |= 0x18901;           /* *************_0_0_1_1_0_001_*_0100_*_0000_1 */

    /* --------- Setting up MCI ---------- */

    XferRp = 0;                             /* Block FIFO read index */
    XferWc = blks;
    XferStat = 2;                           /* Transfer status: Memroy --> MCI */

    MCI_DATA_LEN = 512 * (blks + 1);        /* Set total data length */
    MCI_DATA_TMR = (unsigned long)(MCLK_RW * 0.5);  /* Data timer: 0.5sec */
    MCI_CLEAR = 0x51A;                      /* Clear status flags */
    MCI_MASK0 = 0x51A;                      /* DataBlockEnd DataEnd TxUnderrun DataTimeOut DataCrcFail */
    MCI_DATA_CTRL  = (9 << 4) | 0x9;        /* Start to transmit data blocks */
}
#endif  /* _READONLY */




/*-----------------------------------------------------------------------*/
/* Stop data transfer                                                    */
/*-----------------------------------------------------------------------*/

static void stop_transfer(void)
{
    MCI_MASK0 = 0;          /* Disable MCI interrupt */
    MCI_DATA_CTRL = 0;      /* Stop MCI data transfer */

    GPDMA_CH0_CFG &= 0xFFF80420;    /* Disable DMA ch-0 */
}




/*-----------------------------------------------------------------------*/
/* Power Control (Device dependent)                                      */
/*-----------------------------------------------------------------------*/

static int power_status(void)
{
    return (MCI_POWER & 3) ? 1 : 0;
}


static void power_on(void)
{
    /* Enable MCI and GPDMA clock */
    PCONP |= (3 << 28);

    /* Enable GPDMA controller with little-endian */
    GPDMA_CH0_CFG &= 0xFFF80000;    /* Disable DMA ch-0 */
    GPDMA_CONFIG = 0x01;

    /* Select PCLK for MCI, CCLK/2 = 36MHz */
    PCLKSEL1 = (PCLKSEL1 & 0xFCFFFFFF) | 0x02000000;

    //0.19 0.20 0.21 0.22
    PINMODE1 &= ~((BIT6 | BIT7) | (BIT8 | BIT9) | (BIT10 | BIT11) | (BIT12 | BIT13));
    PINMODE1 |= (BIT7) | (BIT9) | (BIT11) | (BIT13);    // no resistors
    //2.11 2.12 2.13
    PINMODE4 &= ~((BIT22 | BIT23) | (BIT24 | BIT25) | (BIT26 | BIT27));
    PINMODE4 |= (BIT23) | (BIT25) | (BIT27);            // no resistors
    /* Attach MCI unit to I/O pad */
    PINSEL1 = (PINSEL1 & 0xFFFFC03F) | 0x00002A80;  /* MCICLK, MCICMD, MCIDATA0, MCIPWR */
#if USE_4BIT
    PINSEL4 = (PINSEL4 & 0xF03FFFFF) | 0x0A800000;  /* MCIDATA1-3 */
#endif
    MCI_MASK0 = 0;
    MCI_COMMAND = 0;
    MCI_DATA_CTRL = 0;

    // pin 0.21 high active
    SCS |= 0x08;

    /* Register interrupt handlers for MCI,DMA event */
    //RegisterIrq(MCI_INT, Isr_MCI, PRI_LOWEST-1);
    install_irq(MCI_INT, Isr_MCI, 5);

    //RegisterIrq(GPDMA_INT, Isr_GPDMA, PRI_LOWEST-1);
    install_irq(GPDMA_INT, Isr_GPDMA, 5);


    /* Power-on (VCC is always tied to the socket on this board) */
    MCI_POWER = 0x01;                   /* Power on */

    //for (Timer[0] = 10; Timer[0]; ) ; /* 10ms */
    xtimer_usleep(1000);

    MCI_POWER = 0x03;                   /* Enable signals */
}


static void power_off(void)
{
    MCI_MASK0 = 0;
    MCI_COMMAND = 0;
    MCI_DATA_CTRL = 0;

    MCI_POWER = 0;          /* Power-off */
    MCI_CLOCK = 0;

    // pin 0.21 low inactive
    SCS &= ~0x08;

    //0.21 MCI led Pin (turns sd card off, too)
    //0.19 0.20 0.21 0.22 with pull-down
    PINMODE1 |= (BIT6 | BIT7) | (BIT8 | BIT9) | (BIT10 | BIT11) | (BIT12 | BIT13);
    PINSEL1 &= ~((BIT6 | BIT7) | (BIT8 | BIT9) | (BIT10 | BIT11) | (BIT12 | BIT13));
    //  Pins should be now configured as standard input (see board_init.c if you accidentally reconfigured them)

    //2.11 2.12 2.13 with pull-down
    PINMODE4 |= (BIT22 | BIT23) | (BIT24 | BIT25) | (BIT26 | BIT27);
    PINSEL4 &= ~((BIT22 | BIT23) | (BIT24 | BIT25) | (BIT26 | BIT27));
    //  Pins should be now configured as standard input (see board_init.c if you accidentally reconfigured them)

    Stat |= DISKIO_STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Send a command packet to the card and receive a response              */
/*-----------------------------------------------------------------------*/

/*
 * @param idx       Command index (bit[5..0]), ACMD flag (bit7)
 * @param arg       Command argument
 * @param rt        Expected response type. None(0), Short(1) or Long(2)
 * @param *buff     Response return buffer
 * @return 1 when function succeeded otherwise returns 0
 * */
static int send_cmd(unsigned int idx, unsigned long arg, unsigned int rt, unsigned long *buff)
{
    unsigned int s, mc;

    assert(buff != NULL);

    if (idx & 0x80) {               /* Send a CMD55 prior to the specified command if it is ACMD class */
        if (!send_cmd(CMD55, (unsigned long)CardRCA << 16, 1, buff) /* When CMD55 is faild, */
           || !(buff[0] & 0x00000020)) {
            return 0;    /* exit with error */
        }
    }

    idx &= 0x3F;                    /* Mask out ACMD flag */

    do {                            /* Wait while CmdActive bit is set */
        MCI_COMMAND = 0;            /* Cancel to transmit command */
        MCI_CLEAR = 0x0C5;          /* Clear status flags */

        for (s = 0; s < 10; s++) {
            MCI_STATUS;    /* Skip lock out time of command reg. */
        }
    }
    while (MCI_STATUS & 0x00800);

    MCI_ARGUMENT = arg;             /* Set the argument into argument register */
    mc = 0x400 | idx;               /* Enable bit + index */

    if (rt == 1) {
        mc |= 0x040;    /* Set Response bit to reveice short resp */
    }

    if (rt > 1) {
        mc |= 0x0C0;    /* Set Response and LongResp bit to receive long resp */
    }

    MCI_COMMAND = mc;               /* Initiate command transaction */

    //Timer[1] = 100;
    uint32_t timerstart = xtimer_now_usec();

    while (1) {                     /* Wait for end of the cmd/resp transaction */

        //if (!Timer[1]) return 0;
        if ((xtimer_now_usec() - timerstart) > 10000) {
            return 0;
        }

        s = MCI_STATUS;             /* Get the transaction status */

        if (rt == 0) {
            if (s & 0x080) {
                return 1;    /* CmdSent */
            }
        }
        else {
            if (s & 0x040) {
                break;    /* CmdRespEnd */
            }

            if (s & 0x001) {
                /* CmdCrcFail */
                if (idx == 1 || idx == 12 || idx == 41) { /* Ignore CRC error on CMD1/12/41 */
                    break;
                }

                return 0;
            }

            if (s & 0x004) {
                return 0;    /* CmdTimeOut */
            }
        }
    }

    buff[0] = MCI_RESP0;            /* Read the response words */

    if (rt == 2) {
        buff[1] = MCI_RESP1;
        buff[2] = MCI_RESP2;
        buff[3] = MCI_RESP3;
    }

    return 1;       /* Return with success */
}




/*-----------------------------------------------------------------------*/
/* Wait card ready                                                       */
/*-----------------------------------------------------------------------*/

/**
 * @param tmr   Timeout in unit of 1ms
 * @returns 1 when card is tran state, otherwise returns 0
 */
static int wait_ready(unsigned short tmr)
{
    unsigned long rc;

    uint32_t stoppoll = xtimer_now_usec() + tmr * US_PER_MS;
    bool bBreak = false;

    while (xtimer_now_usec() < stoppoll/*Timer[0]*/) {
        if (send_cmd(CMD13, (unsigned long) CardRCA << 16, 1, &rc) && ((rc & 0x01E00) == 0x00800)) {
            bBreak = true;
            break;
        }

        /* This loop will take a time. Insert rot_rdq() here for multitask envilonment. */
    }

    return bBreak;//Timer[0] ? 1 : 0;
}

/*-----------------------------------------------------------------------*/
/* Swap byte order                                                       */
/*-----------------------------------------------------------------------*/
static void bswap_cp(unsigned char *dst, const unsigned long *src)
{
    unsigned long d;


    d = *src;
    *dst++ = (unsigned char)(d >> 24);
    *dst++ = (unsigned char)(d >> 16);
    *dst++ = (unsigned char)(d >> 8);
    *dst++ = (unsigned char)(d >> 0);
}

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
diskio_sta_t mci_initialize(void)
{
    unsigned int cmd, n;
    unsigned long resp[4];
    unsigned char ty;

    if (Stat & DISKIO_STA_NODISK) {
        return Stat;    /* No card in the socket */
    }

    power_off();

    xtimer_usleep(1000);

    power_on();                             /* Force socket power on */
    MCI_CLOCK = 0x100 | (PCLK / MCLK_ID / 2 - 1);   /* Set MCICLK = MCLK_ID */
    //for (Timer[0] = 2; Timer[0]; );
    xtimer_usleep(250);

    send_cmd(CMD0, 0, 0, resp);             /* Enter idle state */
    CardRCA = 0;

    /*---- Card is 'idle' state ----*/

    /* Initialization timeout of 1000 msec */
    uint32_t start = xtimer_now_usec();

    /* SDC Ver2 */
    if (send_cmd(CMD8, 0x1AA, 1, resp) && (resp[0] & 0xFFF) == 0x1AA) {
        /* The card can work at vdd range of 2.7-3.6V */
        DEBUG("SDC Ver. 2\n");

        do {
            /* Wait while card is busy state (use ACMD41 with HCS bit) */
            /* This loop will take a time. Insert wai_tsk(1) here for multitask envilonment. */
            if (xtimer_now_usec() > (start + 1000000/* !Timer[0] */)) {
                DEBUG("%s, %d: Timeout #1\n", RIOT_FILE_RELATIVE, __LINE__);
                goto di_fail;
            }
        }
        while (!send_cmd(ACMD41, 0x40FF8000, 1, resp) || !(resp[0] & 0x80000000));

        ty = (resp[0] & 0x40000000) ? CT_SD2 | CT_BLOCK : CT_SD2;   /* Check CCS bit in the OCR */
    }
    else {                                  /* SDC Ver1 or MMC */
        if (send_cmd(ACMD41, 0x00FF8000, 1, resp)) {
            DEBUG("SDC Ver. 1\n");
            ty = CT_SD1;
            cmd = ACMD41;           /* ACMD41 is accepted -> SDC Ver1 */
        }
        else {
            DEBUG("MMC\n");
            ty = CT_MMC;
            cmd = CMD1;         /* ACMD41 is rejected -> MMC */
        }

        do {
            /* Wait while card is busy state (use ACMD41 or CMD1) */
            DEBUG("%s, %d: %lX\n", RIOT_FILE_RELATIVE, __LINE__, resp[0]);

            /* This loop will take a time. Insert wai_tsk(1) here for multitask envilonment. */
            if (xtimer_now_usec() > (start + 1000000/* !Timer[0] */)) {
                DEBUG("now: %" PRIu32 ", started at: %" PRIu32 "\n",
                      xtimer_now_usec(), start);
                DEBUG("%s, %d: Timeout #2\n", RIOT_FILE_RELATIVE, __LINE__);
                goto di_fail;
            }
        }
        while (!send_cmd(cmd, 0x00FF8000, 1, resp) || !(resp[0] & 0x80000000));
    }

    CardType = ty;                          /* Save card type */
    bswap_cp(&CardInfo[32], resp);          /* Save OCR */

    /*---- Card is 'ready' state ----*/

    if (!send_cmd(CMD2, 0, 2, resp)) {
        DEBUG("%s, %d: Failed entering ident state", RIOT_FILE_RELATIVE,
              __LINE__);
        goto di_fail;   /* Enter ident state */
    }

    for (n = 0; n < 4; n++) {
        bswap_cp(&CardInfo[n * 4 + 16], &resp[n]);    /* Save CID */
    }

    /*---- Card is 'ident' state ----*/

    if (ty & CT_SDC) {                      /* SDC: Get generated RCA and save it */
        if (!send_cmd(CMD3, 0, 1, resp)) {
            DEBUG("%s, %d: Failed generating RCA\n", RIOT_FILE_RELATIVE, __LINE__);
            goto di_fail;
        }

        CardRCA = (unsigned short)(resp[0] >> 16);
    }
    else {                              /* MMC: Assign RCA to the card */
        if (!send_cmd(CMD3, 1 << 16, 1, resp)) {
            goto di_fail;
        }

        CardRCA = 1;
    }

    /*---- Card is 'stby' state ----*/

    if (!send_cmd(CMD9, (unsigned long)CardRCA << 16, 2, resp)) {   /* Get CSD and save it */
        goto di_fail;
    }

    for (n = 0; n < 4; n++) {
        bswap_cp(&CardInfo[n * 4], &resp[n]);
    }

    if (!send_cmd(CMD7, (unsigned long)CardRCA << 16, 1, resp)) {   /* Select card */
        //printf("MCI CMD7 fail\n");
        goto di_fail;
    }

    /*---- Card is 'tran' state ----*/

    if (!(ty & CT_BLOCK)) {     /* Set data block length to 512 (for byte addressing cards) */
        if (!send_cmd(CMD16, 512, 1, resp) || (resp[0] & 0xFDF90000)) {
            //printf("MCI CMD16 fail\n");
            goto di_fail;
        }
    }

#if USE_4BIT

    if (ty & CT_SDC) {      /* Set wide bus mode (for SDCs) */
        if (!send_cmd(ACMD6, 2, 1, resp)    /* Set bus mode of SDC */
           || (resp[0] & 0xFDF90000)) {
            //printf("MCI ACMD6 fail\n");
            goto di_fail;
        }

        MCI_CLOCK |= 0x800;                 /* Set bus mode of MCI */
    }

#endif

    MCI_CLOCK = (MCI_CLOCK & 0xF00) | 0x200 | (PCLK / MCLK_RW / 2 - 1);     /* Set MCICLK = MCLK_RW, power-save mode */

    Stat &= ~DISKIO_STA_NOINIT;    /* Clear DISKIO_STA_NOINIT */
    return Stat;

di_fail:
    power_off();
    Stat |= DISKIO_STA_NOINIT;     /* Set DISKIO_STA_NOINIT */
    return Stat;
}




/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

diskio_sta_t mci_status(void)
{
    return Stat;
}




/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

/**
 * @param buff      Pointer to the data buffer to store read data
 * @param sector    Start sector number (LBA)
 * @param count     Sector count (1..127)
 */
diskio_result_t mci_read(unsigned char *buff, unsigned long sector, unsigned char count)
{
    unsigned long resp;
    unsigned int cmd;

    if (count < 1 || count > 127) {
        return DISKIO_RES_PARERR;    /* Check parameter */
    }

    if (Stat & DISKIO_STA_NOINIT) {
        return DISKIO_RES_NOTRDY;    /* Check drive status */
    }

    if (!(CardType & CT_BLOCK)) {
        sector *= 512;    /* Convert LBA to byte address if needed */
    }

    if (!wait_ready(500)) {
        return DISKIO_RES_ERROR;    /* Make sure that card is tran state */
    }

    ready_reception(count, 512);            /* Ready to receive data blocks */

    cmd = (count > 1) ? CMD18 : CMD17;      /* Transfer type: Single block or Multiple block */

    if (send_cmd(cmd, sector, 1, &resp)     /* Start to read */
       && !(resp & 0xC0580000)) {
        unsigned char rp = 0;

        do {
            while ((rp == XferWp) && !(XferStat & 0xC)) {
                /* Wait for block arrival */
                /* This loop will take a time. Replace it with sync process for multitask envilonment. */
            }

            if (XferStat & 0xC) {
                break; /* Abort if any error has occured */
            }

            copy_al2un(buff, DmaBuff[rp], 512); /* Pop an block */

            XferRp = rp = (rp + 1) % N_BUF; /* Next DMA buffer */

            if (XferStat & 0xC) {
                break; /* Abort if overrun has occured */
            }

            buff += 512; /* Next user buffer address */
        } while (--count);

        if (cmd == CMD18) { /* Terminate to read (MB) */
            send_cmd(CMD12, 0, 1, &resp);
        }
    }

    stop_transfer(); /* Close data path */

    return count ? DISKIO_RES_ERROR : DISKIO_RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0

/**
 * @param buff     Pointer to the data to be written
 * @param sector   Start sector number (LBA)
 * @param count    Sector count (1..127)
 * */
diskio_result_t mci_write(const unsigned char *buff, unsigned long sector, unsigned char count)
{
    unsigned long rc;
    unsigned int cmd;
    unsigned char wp, xc;

    if (count < 1 || count > 127) {
        return DISKIO_RES_PARERR;    /* Check parameter */
    }

    if (Stat & DISKIO_STA_NOINIT) {
        return DISKIO_RES_NOTRDY;    /* Check drive status */
    }

    if (Stat & DISKIO_STA_PROTECT) {
        return DISKIO_RES_WRPRT;    /* Check write protection */
    }

    if (!(CardType & CT_BLOCK)) {
        sector *= 512;    /* Convert LBA to byte address if needed */
    }

    if (!wait_ready(500)) {
        return DISKIO_RES_ERROR;    /* Make sure that card is tran state */
    }

    if (count == 1) {   /* Single block write */
        cmd = CMD24;
    }
    else {          /* Multiple block write */
        cmd = (CardType & CT_SDC) ? ACMD23 : CMD23;

        if (!send_cmd(cmd, count, 1, &rc)       /* Preset number of blocks to write */
           || (rc & 0xC0580000)) {
            return DISKIO_RES_ERROR;
        }

        cmd = CMD25;
    }

    if (!send_cmd(cmd, sector, 1, &rc)          /* Send a write command */
       || (rc & 0xC0580000)) {
        return DISKIO_RES_ERROR;
    }

    wp = 0;
    xc = count;

    do {                                        /* Fill block FIFO */
        copy_un2al(DmaBuff[wp], (unsigned char *)(unsigned int)buff, 512);  /* Push a block */
        wp++;                                       /* Next DMA buffer */
        count--;
        buff += 512;                        /* Next user buffer address */
    }
    while (count && wp < N_BUF);

    XferWp = wp = wp % N_BUF;
    start_transmission(xc);                     /* Start transmission */

    while (count) {
        while ((wp == XferRp) && !(XferStat & 0xC)) {   /* Wait for block FIFO not full */
            /* This loop will take a time. Replace it with sync process for multitask envilonment. */
        }

        if (XferStat & 0xC) {
            break;    /* Abort if block underrun or any MCI error has occured */
        }

        copy_un2al(DmaBuff[wp], (unsigned char *)(unsigned int)buff, 512);  /* Push a block */
        XferWp = wp = (wp + 1) % N_BUF;             /* Next DMA buffer */

        if (XferStat & 0xC) {
            break;    /* Abort if block underrun has occured */
        }

        count--;
        buff += 512;             /* Next user buffer address */
    }

    while (!(XferStat & 0xC)) {} /* Wait for all blocks sent (block underrun) */

    if (XferStat & 0x8) {
        count = 1;    /* Abort if any MCI error has occured */
    }

    stop_transfer();                            /* Close data path */

    if (cmd == CMD25 && (CardType & CT_SDC)) {  /* Terminate to write (SDC w/MB) */
        send_cmd(CMD12, 0, 1, &rc);
    }

    return count ? DISKIO_RES_ERROR : DISKIO_RES_OK;
}
#endif /* _READONLY */




/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

diskio_result_t mci_ioctl(
    unsigned char ctrl,     /* Control code */
    void *buff      /* Buffer to send/receive data block */
)
{
    diskio_result_t res;
    unsigned char *ptr = (unsigned char *)buff;
    unsigned long resp[4], d, *dp, st, ed;


    if (Stat & DISKIO_STA_NOINIT) {
        return DISKIO_RES_NOTRDY;
    }

    res = DISKIO_RES_ERROR;

    switch(ctrl) {
        case CTRL_SYNC :    /* Make sure that all data has been written on the media */
            if (wait_ready(500)) {  /* Wait for card enters tarn state */
                res = DISKIO_RES_OK;
            }

            break;

        case GET_SECTOR_COUNT : /* Get number of sectors on the disk (unsigned long) */
            if ((CardInfo[0] >> 6) == 1) {  /* SDC CSD v2.0 */
                d = ((unsigned short)CardInfo[8] << 8) + CardInfo[9] + 1;
                *(unsigned long *)buff = d << 10;
            }
            else {                      /* MMC or SDC CSD v1.0 */
                unsigned char b = (CardInfo[5] & 15) + ((CardInfo[10] & 128) >> 7) + ((CardInfo[9] & 3) << 1) + 2;
                d = (CardInfo[8] >> 6) + ((unsigned short)CardInfo[7] << 2) + ((unsigned short)(CardInfo[6] & 3) << 10) + 1;
                *(unsigned long *)buff = d << (b - 9);
            }

            res = DISKIO_RES_OK;
            break;

        case GET_SECTOR_SIZE :  /* Get sectors on the disk (unsigned short) */
            *(unsigned short *)buff = 512;
            res = DISKIO_RES_OK;
            break;

        case GET_BLOCK_SIZE :   /* Get erase block size in unit of sectors (unsigned long) */
            if (CardType & CT_SD2) {    /* SDC ver 2.00 */
                *(unsigned long *)buff = 16UL << (CardInfo[10] >> 4);
            }
            else {                  /* SDC ver 1.XX or MMC */
                if (CardType & CT_SD1) {    /* SDC v1 */
                    *(unsigned long *)buff = (((CardInfo[10] & 63) << 1) + ((unsigned short)(CardInfo[11] & 128) >> 7) + 1) << ((CardInfo[13] >> 6) - 1);
                }
                else {              /* MMC */
                    *(unsigned long *)buff = ((unsigned short)((CardInfo[10] & 124) >> 2) + 1) * (((CardInfo[11] & 3) << 3) + ((CardInfo[11] & 224) >> 5) + 1);
                }
            }

            res = DISKIO_RES_OK;
            break;

        case CTRL_ERASE_SECTOR :    /* Erase a block of sectors */
            if (!(CardType & CT_SDC) || (!(CardInfo[0] >> 6) && !(CardInfo[10] & 0x40))) {
                break;    /* Check if sector erase can be applied to the card */
            }

            dp = (unsigned long *)buff;
            st = dp[0];
            ed = dp[1];

            if (!(CardType & CT_BLOCK)) {
                st *= 512;
                ed *= 512;
            }

            if (send_cmd(CMD32, st, 1, resp) && send_cmd(CMD33, ed, 1, resp) && send_cmd(CMD38, 0, 1, resp) && wait_ready(30000)) {
                res = DISKIO_RES_OK;
            }

            break;

        case CTRL_POWER :
            switch(ptr[0]) {
                case 0:     /* Sub control code == 0 (POWER_OFF) */
                    power_off();        /* Power off */
                    res = DISKIO_RES_OK;
                    break;

                case 1:     /* Sub control code == 1 (POWER_GET) */
                    ptr[1] = (unsigned char)power_status();
                    res = DISKIO_RES_OK;
                    break;

                default :
                    res = DISKIO_RES_PARERR;
            }

            break;

        case MMC_GET_TYPE :     /* Get card type flags (1 byte) */
            *ptr = CardType;
            res = DISKIO_RES_OK;
            break;

        case MMC_GET_CSD :      /* Get CSD (16 bytes) */
            memcpy(buff, &CardInfo[0], 16);
            res = DISKIO_RES_OK;
            break;

        case MMC_GET_CID :      /* Get CID (16 bytes) */
            memcpy(buff, &CardInfo[16], 16);
            res = DISKIO_RES_OK;
            break;

        case MMC_GET_OCR :      /* Get OCR (4 bytes) */
            memcpy(buff, &CardInfo[32], 4);
            res = DISKIO_RES_OK;
            break;

        case MMC_GET_SDSTAT :   /* Receive SD status as a data block (64 bytes) */
            if (CardType & CT_SDC) {    /* SDC */
                if (wait_ready(500)) {
                    ready_reception(1, 64);             /* Ready to receive data blocks */

                    if (send_cmd(ACMD13, 0, 1, resp)    /* Start to read */
                       && !(resp[0] & 0xC0580000)) {
                        while ((XferWp == 0) && !(XferStat & 0xC)) {}

                        if (!(XferStat & 0xC)) {
                            copy_al2un((unsigned char *)buff, DmaBuff[0], 64);
                            res = DISKIO_RES_OK;
                        }
                    }
                }

                stop_transfer();                    /* Close data path */
            }

            break;

        default:
            res = DISKIO_RES_PARERR;
    }

    return res;
}
