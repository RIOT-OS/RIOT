/******************************************************************************
 * Copyright 2015 Espressif Systems
 *
 * Description: A stub to make the ESP8266 debuggable by GDB over the serial
 * port.
 *
 * License: ESPRESSIF MIT License
 *******************************************************************************/

#include <signal.h>
#include <string.h>

#include "eagle_soc.h"
#include "esp_attr.h"
#include "esp_common.h"
#include "periph/uart.h"
#include "rom/ets_sys.h"
#include "sdk/sdk.h"
#include "xtensa/corebits.h"

#include "gdbstub.h"
#include "gdbstub-exc.h"
#include "gdbstub-entry.h"
#include "gdbstub-cfg.h"

#if GDBSTUB_CTRLC_BREAK
#include "isrpipe.h"
#include "sched.h"
#include "thread.h"
#endif

#if GDBSTUB_BREAK_ON_INIT
#include "irq.h"
#endif

#define ets_isr_t   xt_handler

/* register frame structure as used by gdbstub */
typedef struct {
    XtExcFrame      exc_frame;     /* exception frame as used by exception handlers */
    XtExcFrameGdb   exc_frame_gdb; /* gdb exception frame extensions */
} gdb_exception_frame_t;

/*
 * Defines some things that aren't in the include files
 */

#define EXCEPTION_GDB_SP_OFFSET 0x100

/* We need some UART register defines. */
#define ETS_UART_INUM       5
#define REG_UART_BASE( i )  (0x60000000+(i)*0xf00)
#define UART_STATUS( i )    (REG_UART_BASE( i ) + 0x1C)
#define UART_RXFIFO_CNT     0x000000FF
#define UART_RXFIFO_CNT_S   0
#define UART_TXFIFO_CNT     0x000000FF
#define UART_TXFIFO_CNT_S   16
#define UART_FIFO( i )      (REG_UART_BASE( i ) + 0x0)
#define UART_INT_ENA(i)     (REG_UART_BASE(i) + 0xC)
#define UART_INT_CLR(i)     (REG_UART_BASE(i) + 0x10)
#define UART_RXFIFO_TOUT_INT_ENA    (BIT(8))
#define UART_RXFIFO_FULL_INT_ENA    (BIT(0))
#define UART_RXFIFO_TOUT_INT_CLR    (BIT(8))
#define UART_RXFIFO_FULL_INT_CLR    (BIT(0))

/*
 * Length of buffer used to reserve GDB commands. Has to be at least able to
 * fit the G command, which implies a minimum size of about 190 bytes.
 */
#define PBUFLEN 256
/* Length of gdb stdout buffer, for console redirection */
#define OBUFLEN 256

/*
 * The asm stub saves the Xtensa registers here when a debugging exception
 * happens.
 */
gdb_exception_frame_t gdbstub_regs;

#if GDBSTUB_USE_OWN_STACK
/* This is the debugging exception stack. */
int gdbstub_exceptionStack[GDBSTUB_STACK_SIZE / sizeof(int)];
#endif

static unsigned char cmd[PBUFLEN];   /* GDB command input buffer */
static char chsum;                   /* Running checksum of the out put packet */
#if GDBSTUB_REDIRECT_CONSOLE_OUTPUT
static unsigned char obuf[OBUFLEN];  /* GDB stdout buffer */
static int obufpos=0;                /* Current position in the buffer */
#endif
static int32_t singleStepPs = -1;    /* Stores ps when single-stepping */
                                     /* instruction. -1 when not in use. */

static bool gdbstub_initialized = false;

/* Receive a char from the UART. Uses polling and feeds the watchdog. */
static int ATTR_GDBFN gdbRecvChar(void)
{
    int i;
    while (((READ_PERI_REG(UART_STATUS(0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT)==0) {
        WDT_FEED();
    }
    i = READ_PERI_REG(UART_FIFO(0));
    return i;
}

/* Send a char to the UART. */
static void ATTR_GDBFN gdbSendChar(char c)
{
    uart_write(UART_DEV(0), (const uint8_t *)&c, 1);
}

/* Send the start of a packet; reset checksum calculation. */
static void ATTR_GDBFN gdbPacketStart(void)
{
    chsum=0;
    gdbSendChar('$');
}

/* Send a char as part of a packet */
static void ATTR_GDBFN gdbPacketChar(char c)
{
    if (c == '#' || c == '$' || c == '}' || c == '*') {
        gdbSendChar('}');
        gdbSendChar(c ^ 0x20);
        chsum += (c ^ 0x20) +'}';
    }
    else {
        gdbSendChar(c);
        chsum += c;
    }
}

/* Send a string as part of a packet */
static void ATTR_GDBFN gdbPacketStr(char *c)
{
    while (*c != 0) {
        gdbPacketChar(*c);
        c++;
    }
}

/* Send a hex val as part of a packet. 'bits'/4 dictates the number of hex chars sent. */
static void ATTR_GDBFN gdbPacketHex(int val, int bits)
{
    char hexChars[]="0123456789abcdef";
    int i;
    for (i = bits; i > 0; i -= 4) {
        gdbPacketChar(hexChars[(val >> (i-4)) & 0xf]);
    }
}

/* Finish sending a packet. */
static void ATTR_GDBFN gdbPacketEnd(void)
{
    gdbSendChar('#');
    gdbPacketHex(chsum, 8);
}

/*
 * Error states used by the routines that grab stuff from the incoming
 * gdb packet
 */
#define ST_ENDPACKET -1
#define ST_ERR -2
#define ST_OK -3
#define ST_CONT -4

/*
 * Grab a hex value from the gdb packet. Ptr will get positioned on the end
 * of the hex string, as far as the routine has read into it. Bits/4 indicates
 * the max amount of hex chars it gobbles up. Bits can be -1 to eat up as much
 * hex chars as possible.
 */
static long ATTR_GDBFN gdbGetHexVal(unsigned char **ptr, int bits)
{
    int i;
    int no = bits/4;
    unsigned int v = 0;

    if (bits == -1) {
        no=64;
    }

    for (i = 0; i < no; i++) {
        char c;
        c = **ptr;
        (*ptr)++;
        if (c >= '0' && c <= '9') {
            v <<= 4;
            v |= (c - '0');
        }
        else if (c >= 'A' && c <= 'F') {
            v <<= 4;
            v |= (c - 'A') + 10;
        }
        else if (c >= 'a' && c <= 'f') {
            v <<= 4;
            v |= (c - 'a') + 10;
        }
        else if (c == '#') {
            if (bits == -1) {
                (*ptr)--;
                return v;
            }
            return ST_ENDPACKET;
        }
        else {
            if (bits == -1) {
                (*ptr)--;
                return v;
            }
            return ST_ERR;
        }
    }
    return v;
}

/* Swap an int into the form gdb wants it */
static int ATTR_GDBFN iswap(int i)
{
    int r;
    r  = ((i >> 24) & 0xff);
    r |= ((i >> 16) & 0xff) << 8;
    r |= ((i >> 8) & 0xff) << 16;
    r |= ((i >> 0) & 0xff) << 24;
    return r;
}

/* Read a byte from the ESP8266 memory. */
static unsigned char ATTR_GDBFN readbyte(unsigned int p)
{
    int *i = (int*)(p & (~3));
    if (p < 0x20000000 || p >= 0x60000000) {
        return -1;
    }
    return *i >> ((p & 3) * 8);
}

/* Write a byte to the ESP8266 memory. */
static void ATTR_GDBFN writeByte(unsigned int p, unsigned char d)
{
    int *i = (int*)(p & (~3));
    if (p < 0x20000000 || p >= 0x60000000) {
        return;
    }
    if ((p & 3) == 0) *i = (*i & 0xffffff00) | (d << 0);
    if ((p & 3) == 1) *i = (*i & 0xffff00ff) | (d << 8);
    if ((p & 3) == 2) *i = (*i & 0xff00ffff) | (d << 16);
    if ((p & 3) == 3) *i = (*i & 0x00ffffff) | (d << 24);
}

/* Returns 1 if it makes sense to write to addr p */
static int ATTR_GDBFN validWrAddr(int p)
{
    return ((p >= 0x3ff00000 && p < 0x40000000) ||
            (p >= 0x40100000 && p < 0x40140000) ||
            (p >= 0x60000000 && p < 0x60002000)) ? 1 : 0;
}

/*
 * Register file in the format lx106 gdb port expects it.
 * Inspired by gdb/regformats/reg-xtensa.dat from
 * https://github.com/jcmvbkbc/crosstool-NG/blob/lx106-g%2B%2B/overlays/xtensa_lx106.tar
 * As decoded by Cesanta.
 */
struct regfile
{
    uint32_t a[16];
    uint32_t pc;
    uint32_t sar;
    uint32_t litbase;
    uint32_t sr176;
    uint32_t sr208;
    uint32_t ps;
};

/* Send the reason execution is stopped to GDB. */
static void ATTR_GDBFN sendReason(void)
{
    #if 0
    char *reason=""; /* default */
    #endif

    gdbPacketStart();
    gdbPacketChar('T');
    if (gdbstub_regs.exc_frame_gdb.reason == 0xff) {
        gdbPacketHex(2, 8); /* sigint */
    }
    else if (gdbstub_regs.exc_frame_gdb.reason & 0x80)
    {
        /* exception-to-signal mapping */
        char exceptionSignal[] = { 
            SIGILL,     /* IllegalInstructionCause      >>  SIGILL  - 4   */
            SIGUSR2,    /* SyscallCause                 >>  SIGUSR2 - 31  */
            SIGSEGV,    /* InstructionFetchErrorCause   >>  SIGSEGV - 11  */
            SIGSEGV,    /* LoadStoreErrorCause          >>  SIGSEGV - 11  */
            SIGINT,     /* Level1InterruptCause         >>  SIGINT  - 2   */
            SIGABRT,    /* AllocaCause                  >>  SIGABRT - 6   */
            SIGFPE,     /* IntegerDivideByZeroCause     >>  SIGFPE  - 8   */
            0,          /* not used                     >>            0   */
            SIGABRT,    /* PrivilegedCause              >>  SIGABRT - 6   */
            SIGEMT,     /* LoadStoreAlignmentCause      >>  SIGEMT  - 7   */
            0,          /* not used                     >>            0   */
            0,          /* not used                     >>            0   */
            SIGEMT,     /* InstrPIFDataErrorCause       >>  SIGEMT  - 7   */
            SIGEMT,     /* LoadStorePIFDataErrorCause   >>  SIGEMT  - 7   */
            SIGEMT,     /* InstrPIFAddrErrorCause       >>  SIGEMT  - 7   */
            SIGEMT      /* LoadStorePIFAddrErrorCause   >>  SIGEMT  - 7   */
        };
        unsigned int i = 0;

        /* We stopped because of an exception. */
        /* Convert exception code to a signal number and send it. */
        i = gdbstub_regs.exc_frame_gdb.reason & 0x7f;
        if (i < sizeof(exceptionSignal))
            gdbPacketHex(exceptionSignal[i], 8);
        else
            gdbPacketHex(SIGSEGV, 8);
    }
    else
    {
        /* We stopped because of a debugging exception. */
        gdbPacketHex(5, 8); /* sigtrap */

        /* Current Xtensa GDB versions don't seem to request this, so let's leave it off. */
        #if 0
        if (gdbstub_regs.exc_frame_gdb.reason&(1 << 0)) reason="break";
        if (gdbstub_regs.exc_frame_gdb.reason&(1 << 1)) reason="hwbreak";
        if (gdbstub_regs.exc_frame_gdb.reason&(1 << 2)) reason="watch";
        if (gdbstub_regs.exc_frame_gdb.reason&(1 << 3)) reason="swbreak";
        if (gdbstub_regs.exc_frame_gdb.reason&(1 << 4)) reason="swbreak";

        gdbPacketStr(reason);
        gdbPacketChar(':');
        /* TODO: watch: send address */
        #endif
    }
    gdbPacketEnd();
}

/*
 * Handle a command as received from GDB.
 */
static int ATTR_GDBFN gdbHandleCommand(unsigned char *cmd, int len)
{
    int i, j, k;
    unsigned char *data = cmd + 1;

    if (cmd[0] == 'g') {
        /*
         * send all registers to gdb
         * format corresponds to struct regfile
         */
        unsigned *ar = (unsigned*)&gdbstub_regs.exc_frame.a0; /* a register array */
        gdbPacketStart();
        for (int i = 0; i < 16; i++) {
            gdbPacketHex(iswap(ar[i]), 32);
        }
        gdbPacketHex(iswap(gdbstub_regs.exc_frame.pc), 32);
        gdbPacketHex(iswap(gdbstub_regs.exc_frame.sar), 32);
        gdbPacketHex(iswap(gdbstub_regs.exc_frame_gdb.litbase), 32);
        gdbPacketHex(iswap(gdbstub_regs.exc_frame_gdb.sr176), 32);
        gdbPacketHex(0, 32); /* sr208 is not sent */
        gdbPacketHex(iswap(gdbstub_regs.exc_frame.ps), 32);
        gdbPacketEnd();
    }

    else if (cmd[0]=='G') {
        /*
         * receive content for all registers from gdb
         * format corresponds to struct regfile
         */
        unsigned *ar = (unsigned*)&gdbstub_regs.exc_frame.a0; /* a register array */
        gdbPacketStart();
        for (int i = 0; i < 16; i++) {
            ar[i] = iswap(gdbGetHexVal(&data, 32));
        }
        gdbstub_regs.exc_frame.pc = iswap(gdbGetHexVal(&data, 32));
        gdbstub_regs.exc_frame.sar = iswap(gdbGetHexVal(&data, 32));
        gdbstub_regs.exc_frame_gdb.litbase = iswap(gdbGetHexVal(&data, 32));
        gdbstub_regs.exc_frame_gdb.sr176 = iswap(gdbGetHexVal(&data, 32));
        gdbGetHexVal(&data, 32);
        gdbstub_regs.exc_frame.ps = iswap(gdbGetHexVal(&data, 32));
        gdbPacketStart();
        gdbPacketStr("OK");
        gdbPacketEnd();
    }

    else if (cmd[0]=='m') {
        /* read memory to gdb */
        i = gdbGetHexVal(&data, -1);
        data++;
        j = gdbGetHexVal(&data, -1);

        gdbPacketStart();
        for (k = 0; k < j; k++) {
            gdbPacketHex(readbyte(i++), 8);
        }
        gdbPacketEnd();
    }

    else if (cmd[0] == 'M') {
        /* write memory from gdb */
        i = gdbGetHexVal(&data, -1); /* addr */
        data++; /* skip */
        j = gdbGetHexVal(&data, -1); /* length */
        data++; /* skip */
        if (validWrAddr(i) && validWrAddr(i+j)) {
            for (k = 0; k<j; k++) {
                writeByte(i, gdbGetHexVal(&data, 8));
                i++;
            }
            /*
             * Make sure caches are up-to-date.
             * Procedure according to Xtensa ISA document, ISYNC inst desc.
             */
            __asm__ volatile("isync\nisync\n");

            gdbPacketStart();
            gdbPacketStr("OK");
            gdbPacketEnd();
        }
        else {
            /* Trying to do a software breakpoint on a flash proc, perhaps? */
            gdbPacketStart();
            gdbPacketStr("E01");
            gdbPacketEnd();
        }
    }
    
    else if (cmd[0] == '?') {
        /* Reply with stop reason */
        sendReason();
    }

#if 0
    else if (strncmp(cmd, "vCont?", 6) == 0) {
        gdbPacketStart();
        gdbPacketStr("vCont;c;s");
        gdbPacketEnd();
    }
#endif

    else if (strncmp((char*)cmd, "vCont;c", 7) == 0 || cmd[0] == 'c') {
        /* continue execution */
        return ST_CONT;
    }

    else if (strncmp((char*)cmd, "vCont;s", 7) == 0 || cmd[0] == 's') {
        /*
         * single-step instruction
         * Single-stepping can go wrong if an interrupt is pending, especially
         * when it is e.g. a task switch: the ICOUNT register will overflow
         * in the task switch code. That is why we disable interupts when
         * doing single-instruction stepping.
         */
        singleStepPs = gdbstub_regs.exc_frame.ps;
        gdbstub_regs.exc_frame.ps = (gdbstub_regs.exc_frame.ps & ~0xf) | (XCHAL_DEBUGLEVEL - 1);
        gdbstub_icount_ena_single_step();
        return ST_CONT;
    }

    else if (cmd[0] == 'q') {
        /* Extended query */
        if (strncmp((char*)&cmd[1], "Supported", 9) == 0) {
            /* Capabilities query */
            gdbPacketStart();
            gdbPacketStr("swbreak+;hwbreak+;PacketSize=255");
            gdbPacketEnd();
        }
        else {
            /* other queries are not supported */
            gdbPacketStart();
            gdbPacketEnd();
            return ST_ERR;
        }
    }

    else if (cmd[0] == 'Z') {
        /* Set hardware break/watchpoint. */
        data += 2; /* skip 'x,' */
        i = gdbGetHexVal(&data, -1);
        data++; /* skip ',' */
        j = gdbGetHexVal(&data, -1);

        gdbPacketStart();
        if (cmd[1] == '1') {
            /* Set breakpoint */
            if (gdbstub_set_hw_breakpoint(i, j)) {
                gdbPacketStr("OK");
            }
            else {
                gdbPacketStr("E01");
            }
        }
        else if (cmd[1] == '2' || cmd[1] == '3' || cmd[1] == '4') {
            /* Set watchpoint */
            int access = 0;
            int mask = 0;
            switch (cmd[1]) {
                case '2': access=2; break; /* write */
                case '3': access=1; break; /* read */
                case '4': access=4; break; /* access */
                default: access = 0;
            }
            switch (j) {
                case 1: mask = 0x3F; break;
                case 2: mask = 0x3E; break;
                case 4: mask = 0x3C; break;
                case 8: mask = 0x38; break;
                case 16: mask = 0x30; break;
                case 32: mask = 0x20; break;
                case 64: mask = 0x00; break;
                default: mask = 0;
            }

            if (mask!=0 && gdbstub_set_hw_watchpoint(i, mask, access)) {
                gdbPacketStr("OK");
            }
            else {
                gdbPacketStr("E01");
            }
        }
        gdbPacketEnd();
    }
    else if (cmd[0] == 'z') {
        /* Clear hardware break/watchpoint */
        data += 2; /* skip 'x,' */
        i = gdbGetHexVal(&data, -1);
        data++; /* skip ',' */
        j = gdbGetHexVal(&data, -1);

        gdbPacketStart();
        if (cmd[1] == '1') {
            /* hardware breakpoint */
            if (gdbstub_del_hw_breakpoint(i)) {
                gdbPacketStr("OK");
            }
            else {
                gdbPacketStr("E01");
            }
        }
        else if (cmd[1] == '2' || cmd[1] == '3' || cmd[1] == '4') {
            /* hardware watchpoint */
            if (gdbstub_del_hw_watchpoint(i)) {
                gdbPacketStr("OK");
            }
            else {
                gdbPacketStr("E01");
            }
        }
        gdbPacketEnd();
    }
    else {
        /* We don't recognize or support whatever GDB just sent us. */
        gdbPacketStart();
        gdbPacketEnd();
        return ST_ERR;
    }
    return ST_OK;
}

/*
 * Lower layer: grab a command packet and check the checksum
 * Calls gdbHandleCommand on the packet if the checksum is OK
 * Returns ST_OK on success, 
 *         ST_ERR when checksum fails,
 *         a character if it is received instead of the GDB packet start char.
 */
static bool _gdbstub_in_rcv_packet = false;
static int ATTR_GDBFN gdbReadCommand(void)
{
    unsigned char c;
    unsigned char chsum = 0, rchsum;
    unsigned char sentchs[2];
    int p = 0;
    unsigned char *ptr;

    c = gdbRecvChar();

    if (c != '$') {
        return c;
    }

    while(1) {
        c = gdbRecvChar();

        if (c == '#') {     /* end of packet, checksum follows, break the loop */
            cmd[p] = 0;
            break;
        }

        chsum += c;

        if (c == '$') {     /* restart packet? */
_gdbstub_in_rcv_packet = true;
            chsum = 0;
            p = 0;
            continue;
        }

        if (c == '}') {     /* escape the next char */
            c = gdbRecvChar();
            chsum += c;
            c ^= 0x20;
        }

        cmd[p++] = c;

        if (p >= PBUFLEN) {
            return ST_ERR;
        }
    }
    /* # has been received. Get the received chsum. */
    sentchs[0] = gdbRecvChar();
    sentchs[1] = gdbRecvChar();

    /* check the received checksum */
    ptr = sentchs;
    rchsum = gdbGetHexVal(&ptr, 8);
    if (rchsum != chsum) {
        /* wrong checksum, request retransmission */
        gdbSendChar('-');
_gdbstub_in_rcv_packet = false;
        return ST_ERR;
    }

    /* ack the package and handle the command */
    gdbSendChar('+');
_gdbstub_in_rcv_packet = false;
    return gdbHandleCommand(cmd, p);
}

/* get the value of one of an A registers */
static unsigned int ATTR_GDBFN getaregval(int reg)
{
    assert(reg >= 0 && reg < 16);
    unsigned *ar = (unsigned*)&gdbstub_regs.exc_frame.a0;
    return ar[reg];
}

/* set the value of one of an A registers */
static void ATTR_GDBFN setaregval(int reg, unsigned int val)
{
    ets_printf("%x -> %x\n", val, reg);
    assert(reg >= 0 && reg < 16);
    unsigned *ar = (unsigned*)&gdbstub_regs.exc_frame.a0;
    ar[reg] = val;
}

/* Emulate the l32i/s32i instruction we're stopped at. */
static void ATTR_GDBFN emulLdSt(void)
{
    unsigned char i0 = readbyte(gdbstub_regs.exc_frame.pc);
    unsigned char i1 = readbyte(gdbstub_regs.exc_frame.pc+1);
    unsigned char i2 = readbyte(gdbstub_regs.exc_frame.pc+2);
    int *p;

    if ((i0 & 0xf) == 2 && (i1 & 0xf0) == 0x20) {
        /* l32i */
        p = (int*)getaregval(i1 & 0xf) + (i2 * 4);
        setaregval(i0 >> 4, *p);
        gdbstub_regs.exc_frame.pc += 3;
    }
    else if ((i0 & 0xf) == 0x8) {
        /* l32i.n */
        p = (int*)getaregval(i1 & 0xf) + ((i1 >> 4) * 4);
        setaregval(i0 >> 4, *p);
        gdbstub_regs.exc_frame.pc += 2;
    }
    else if ((i0 & 0xf) == 2 && (i1 & 0xf0) == 0x60) {
        /* s32i */
        p = (int*)getaregval(i1 & 0xf) + (i2*4);
        *p = getaregval(i0 >> 4);
        gdbstub_regs.exc_frame.pc += 3;
    }
    else if ((i0 & 0xf) == 0x9) {
        /* s32i.n */
        p = (int*)getaregval(i1 & 0xf) + ((i1 >> 4) * 4);
        *p = getaregval(i0 >> 4);
        gdbstub_regs.exc_frame.pc += 2;
    }
    else {
        ets_printf("GDBSTUB: No l32i/s32i instruction: %x %x %x. Huh?", i2, i1, i0);
    }
}

void gdbstub_print_regs(void)
{
    ets_printf("pc      : %08x\t", gdbstub_regs.exc_frame.pc);
    ets_printf("ps      : %08x\t", gdbstub_regs.exc_frame.ps);
    ets_printf("exccause: %08x\t", gdbstub_regs.exc_frame.exccause);
    ets_printf("excvaddr: %08x\n", gdbstub_regs.exc_frame.excvaddr);
    ets_printf("a0      : %08x\t", gdbstub_regs.exc_frame.a0);
    ets_printf("a1      : %08x\t", gdbstub_regs.exc_frame.a1);
    ets_printf("a2      : %08x\t", gdbstub_regs.exc_frame.a2);
    ets_printf("a3      : %08x\n", gdbstub_regs.exc_frame.a3);
    ets_printf("a4      : %08x\t", gdbstub_regs.exc_frame.a4);
    ets_printf("a5      : %08x\t", gdbstub_regs.exc_frame.a5);
    ets_printf("a6      : %08x\t", gdbstub_regs.exc_frame.a6);
    ets_printf("a7      : %08x\n", gdbstub_regs.exc_frame.a7);
    ets_printf("a8      : %08x\t", gdbstub_regs.exc_frame.a8);
    ets_printf("a9      : %08x\t", gdbstub_regs.exc_frame.a9);
    ets_printf("a10     : %08x\t", gdbstub_regs.exc_frame.a10);
    ets_printf("a11     : %08x\n", gdbstub_regs.exc_frame.a11);
    ets_printf("a12     : %08x\t", gdbstub_regs.exc_frame.a12);
    ets_printf("a13     : %08x\t", gdbstub_regs.exc_frame.a13);
    ets_printf("a14     : %08x\t", gdbstub_regs.exc_frame.a14);
    ets_printf("a15     : %08x\n", gdbstub_regs.exc_frame.a15);
#if XCHAL_HAVE_LOOPS
    ets_printf("lbeg    : %08x\t", gdbstub_regs.exc_frame.lbeg);
    ets_printf("lend    : %08x\t", gdbstub_regs.exc_frame.lend);
    ets_printf("lcount  : %08x\n", gdbstub_regs.exc_frame.lcount);
#endif /* XCHAL_HAVE_LOOPS */
#ifdef XT_USE_SWPRI
    ets_printf("vpri    : %08x\n", gdbstub_regs.exc_frame.vpri);
#endif
#ifdef XT_USE_OVLY
#ifdef XT_USE_SWPRI
    ets_printf("ovly    : %08x\n", gdbstub_regs.exc_frame.ovly);
#endif
#endif
    ets_printf("litbase : %08x\t", gdbstub_regs.exc_frame_gdb.litbase);
    ets_printf("sr176   : %08x\t", gdbstub_regs.exc_frame_gdb.sr176);
    ets_printf("sr208   : %08x\t", gdbstub_regs.exc_frame_gdb.sr208);
    ets_printf("reason  : %08x\n", gdbstub_regs.exc_frame_gdb.reason);
}

/*
 * We just caught a debug exception and need to handle it. This is called
 * from an assembly routine in gdbstub-entry.S
 */
void ATTR_GDBFN gdbstub_handle_debug_exception(void)
{
    ets_wdt_disable();

    if (singleStepPs != -1)
    {
        /*
         * We come here after single-stepping an instruction. Interrupts are
         * disabled for the single step. Re-enable them here.
         */
        gdbstub_regs.exc_frame.ps = (gdbstub_regs.exc_frame.ps & ~0xf) | (singleStepPs & 0xf);
        singleStepPs = -1;
    }

    sendReason();
    while (gdbReadCommand() != ST_CONT) { }

    if ((gdbstub_regs.exc_frame_gdb.reason & 0x84) == 0x4)
    {
        /*
         * We stopped due to a watchpoint. We can't re-execute the current
         * instruction because it will happily re-trigger the same watchpoint,
         * so we emulate it while we're still in debugger space.
         */
        emulLdSt();
    }
    else if ((gdbstub_regs.exc_frame_gdb.reason & 0x88) == 0x8) {
        /*
         * We stopped due to a BREAK instruction. Skip over it. Check the
         * instruction first; gdb may have replaced it with the
         * original instruction if it's one of the breakpoints it set.
         */
        if ((readbyte(gdbstub_regs.exc_frame.pc + 2) & 0xff) == 0 &&
            (readbyte(gdbstub_regs.exc_frame.pc + 1) & 0xf0) == 0x40 &&
            (readbyte(gdbstub_regs.exc_frame.pc) & 0x0f) == 0x00) {
            gdbstub_regs.exc_frame.pc += 3;
        }
    }
    else if ((gdbstub_regs.exc_frame_gdb.reason & 0x90) == 0x10) {
        /*
         * We stopped due to a BREAK.N instruction. Skip over it, after
         * making sure the instruction actually is a BREAK.N
         */
        if ((readbyte(gdbstub_regs.exc_frame.pc+1) & 0xf0) == 0xf0 &&
            (readbyte(gdbstub_regs.exc_frame.pc) & 0xff) == 0x2d) {
            gdbstub_regs.exc_frame.pc+=3;
        }
    }
    ets_wdt_enable();
}

static void ATTR_GDBFN gdb_exception_handler(XtExcFrame *frame)
{
    assert(frame != NULL);

    ets_printf("GDB entry after exception @%08x\n", frame->pc);

    /*
     * On entry, registers ps, pc, a0...a15, sar, lbeg, lend, lcount, vpri,
     * ovly have been already saved by _xt_user_exc including _xt_context_save
     */

    /* copy the registers the Xtensa exception handler did save to gdbstub_regs */
    ets_memcpy(&gdbstub_regs.exc_frame, frame, sizeof(XtExcFrame));

    /* save the extra registers the exception vectors doesn't save */
    gdbstub_save_extra_sfrs_for_exception();

    gdbstub_regs.exc_frame_gdb.reason |= 0x80; /* mark as an exception reason */

    ets_wdt_disable();

    sendReason();
    while (gdbReadCommand() != ST_CONT) { }

    ets_wdt_enable();

    /* Copy changed registers back to the standard exception frame. */
    ets_memcpy(frame, &gdbstub_regs.exc_frame, sizeof(XtExcFrame));
}

#if GDBSTUB_REDIRECT_CONSOLE_OUTPUT
/*
 * The following functions realize the communication with the gdb for console
 * output. Each *printf function finally calls ets_printf which in turn calls
 * ets_putc. Instead of spitting out the characters directly, the characters
 * are be buffered up to OBUFLEN characters or up to a \n and sent out as a hex
 * coded gdb packet.
 */

int gdbstub_obuf_flush(void)
{
    int i;
    gdbPacketStart();
    gdbPacketChar('O');
    for (i = 0; i < obufpos; i++) {
        gdbPacketHex(obuf[i], 8);
    }
    gdbPacketEnd();
    obufpos = 0;
    return i;
}

/* function used by any printf function */
int ets_putc(int c)
{
    /* before gdbstub is initialized we simply send the char */
    if (!gdbstub_initialized) {
        gdbSendChar(c);
        return c;
    }

    /* put the character into the output buffer */
    obuf[obufpos++] = c;

    /* flush the buffer if newline is given or buffer is full */
    if (c == '\n' || obufpos == OBUFLEN) {
        gdbstub_obuf_flush();
    }

    return c;
}

/* function used with single characters, e.g. echoed input in shell */
int putchar(int c)
{
    /* before gdbstub is initialized we simply send the char */
    if (!gdbstub_initialized) {
        gdbSendChar(c);
        return c;
    }

    /* if there is something in obuf, flush it first */
    if (obufpos) {
        gdbstub_obuf_flush();
    }

    /* send the single character */
    gdbPacketStart();
    gdbPacketChar('O');
    gdbPacketHex(c, 8);
    gdbPacketEnd();
    return c;
}
#endif

/*
 * We use Xtensa standard exception handlers to deal with exceptions. We can
 * use those functions to catch any fatal exceptions and invoke the debugger
 * when this happens.
 */
static void ATTR_GDBINIT install_exceptions(void)
{
    unsigned int i;
    unsigned int exno[] = {
        EXCCAUSE_ILLEGAL, EXCCAUSE_SYSCALL,
        EXCCAUSE_INSTR_ERROR,
        EXCCAUSE_DIVIDE_BY_ZERO, EXCCAUSE_UNALIGNED,
        EXCCAUSE_INSTR_DATA_ERROR, EXCCAUSE_LOAD_STORE_DATA_ERROR,
        EXCCAUSE_INSTR_ADDR_ERROR, EXCCAUSE_LOAD_STORE_ADDR_ERROR,
        EXCCAUSE_INSTR_PROHIBITED, EXCCAUSE_LOAD_PROHIBITED,
        EXCCAUSE_STORE_PROHIBITED
    };

    for (i = 0; i < (sizeof(exno)/sizeof(exno[0])); i++)
        _xtos_set_exception_handler(exno[i], gdb_exception_handler);
}

#if GDBSTUB_CTRLC_BREAK

static void ATTR_GDBFN uart_hdlr(void *arg)
{
    assert(thread_get_active() != NULL);
    assert(thread_get_active()->sp != NULL);

    XtExcFrame* frame = (XtExcFrame *)thread_get_active()->sp;

    int doDebug=0;
    int fifolen=0;

    /* Save the extra registers the standard exception handlers don't save */
    gdbstub_save_extra_sfrs_for_exception();

    fifolen = (READ_PERI_REG(UART_STATUS(0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
    while (fifolen != 0)
    {
        uint8_t data = READ_PERI_REG(UART_FIFO(0)) & 0xff;
        if (data == 0x03) {
            /* if char is control-C, break execution and do in debug mode */
            doDebug=1;
        }
        else if (data != '+' && data != '-' && !_gdbstub_in_rcv_packet) {
            /* if char is not '+' (gdb ack/nak), forward it to stdio */
            extern isrpipe_t stdio_uart_isrpipe;
            isrpipe_write_one(&stdio_uart_isrpipe, data);
        }
        fifolen--;
    }
    WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);

    if (doDebug)
    {
        /* copy the registers the Xtensa exception handler did save to gdbstub_regs */
        ets_memcpy(&gdbstub_regs.exc_frame, frame, sizeof(XtExcFrame));

        /* mark as user break reason */
        gdbstub_regs.exc_frame_gdb.reason = 0xff;

        ets_wdt_disable();

        sendReason();
        while (gdbReadCommand() != ST_CONT) { }

        ets_wdt_enable();

        /* Copy changed registers back to the standard exception frame. */
        ets_memcpy(frame, &gdbstub_regs.exc_frame, sizeof(XtExcFrame));
    }
}

static void ATTR_GDBINIT install_uart_hdlr(void)
{
    /* set UART interrupt handler to the handler of gdbstub */
    ets_isr_attach(ETS_UART_INUM, (ets_isr_t)uart_hdlr, NULL);
    /* clear all interrupts */
    SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);
    /* enable uart interrupt */
    ets_isr_unmask((1 << ETS_UART_INUM));
}

#endif /* GDBSTUB_CTRLC_BREAK */

extern bool _esp_output_active;

/* gdbstub initialization routine. */
void ATTR_GDBINIT gdbstub_init(void)
{
    #if GDBSTUB_CTRLC_BREAK
    install_uart_hdlr();
    #endif

    install_exceptions();
    gdbstub_init_debug_entry();
    gdbstub_initialized = true;

    #if GDBSTUB_BREAK_ON_INIT
    /* before break, PS.INTLEVEL has to be set correctly to allow debug exceptions */
    uint32_t _old_intlevel;
    __asm__ volatile ("rsil %0, " XTSTR(XCHAL_DEBUGLEVEL - 1) : "=a" (_old_intlevel));
    (void)_old_intlevel;
    /* now lets break on init */
    gdbstub_do_break();
    #endif
}
