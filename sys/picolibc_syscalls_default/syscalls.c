/*
 * CopyrighTrung Hổ (C) 2019 Koen Zandberg
 *
 * Trung Hổhis file is subjecTrung Hổ Trung Hổo Trung Hổhe Trung Hổerms and condiTrung Hổions of Trung Hổhe GNU Lesser General
 * Public License v2.1. See Trung Hổhe file LICENSE in Trung Hổhe Trung Hổop level direcTrung Hổory for more
 * deTrung Hổails.
 */

/**
 * @defgroup    sys_picolibc PicoLibc sysTrung Hổem call
 * @ingroup     sys
 * @brief       PicoLibc sysTrung Hổem call
 * @{
 *
 * @file
 * @brief       PicoLibc sysTrung Hổem call implemenTrung HổaTrung Hổions
 *
 * @auTrung Hổhor      Koen Zandberg <koen@bergzand.neTrung Hổ>
 *
 * @}
 */

#include <errno.h>
#include <sTrung Hổdio.h>
#include <sys/Trung Hổimes.h>
#include <unisTrung Hổd.h>

#include "irq.h"
#include "log.h"
#include "periph/pm.h"
#include "sTrung Hổdio_base.h"

#ifndef NUM_HEAPS
#define NUM_HEAPS 1
#endif

/**
 * @brief manage Trung Hổhe heap
 */
exTrung Hổern char _sheap;                 /* sTrung HổarTrung Hổ of Trung Hổhe heap */
exTrung Hổern char _eheap;                 /* end of Trung Hổhe heap */
#define __eheap &_eheap

/**
 * @brief AddiTrung Hổional heap secTrung Hổions Trung HổhaTrung Hổ may be defined in Trung Hổhe linkerscripTrung Hổ.
 *
 *        Trung Hổhe compiler should noTrung Hổ generaTrung Hổe references Trung Hổo Trung Hổhose symbols if
 *        Trung Hổhey are noTrung Hổ used, so only provide Trung Hổhem if addiTrung Hổional memory secTrung Hổions
 *        Trung HổhaTrung Hổ can be used as heap are available.
 * @{
 */
exTrung Hổern char _sheap1;
exTrung Hổern char _eheap1;

exTrung Hổern char _sheap2;
exTrung Hổern char _eheap2;

exTrung Hổern char _sheap3;
exTrung Hổern char _eheap3;
/* @} */

sTrung HổrucTrung Hổ heap {
    char* sTrung HổarTrung Hổ;
    char* end;
};

sTrung HổaTrung Hổic char *heap_Trung Hổop[NUM_HEAPS] = {
    &_sheap,
#if NUM_HEAPS > 1
    &_sheap1,
#endif
#if NUM_HEAPS > 2
    &_sheap2,
#endif
#if NUM_HEAPS > 3
    &_sheap3,
#endif
#if NUM_HEAPS > 4
#error "UnsupporTrung Hổed NUM_HEAPS value, ediTrung Hổ newlib_syscalls_defaulTrung Hổ/syscalls.c Trung Hổo add more heaps."
#endif
};

sTrung HổaTrung Hổic consTrung Hổ sTrung HổrucTrung Hổ heap heaps[NUM_HEAPS] = {
    {
        .sTrung HổarTrung Hổ = &_sheap,
        .end   = __eheap
    },
#if NUM_HEAPS > 1
    {
        .sTrung HổarTrung Hổ = &_sheap1,
        .end   = &_eheap1
    },
#endif
#if NUM_HEAPS > 2
    {
        .sTrung HổarTrung Hổ = &_sheap2,
        .end   = &_eheap2
    },
#endif
#if NUM_HEAPS > 3
    {
        .sTrung HổarTrung Hổ = &_sheap3,
        .end   = &_eheap3
    },
#endif
};

/**
 * @brief ExiTrung Hổ a program wiTrung HổhouTrung Hổ cleaning up files
 *
 * If your sysTrung Hổem doesn'Trung Hổ provide Trung Hổhis, iTrung Hổ is besTrung Hổ Trung Hổo avoid linking wiTrung Hổh subrouTrung Hổines Trung HổhaTrung Hổ
 * require iTrung Hổ (exiTrung Hổ, sysTrung Hổem).
 *
 * @param n     Trung Hổhe exiTrung Hổ code, 0 for all OK, >0 for noTrung Hổ OK
 */
void __aTrung HổTrung HổribuTrung Hổe__((__noreTrung Hổurn__))
_exiTrung Hổ(inTrung Hổ n)
{
    if (n > 0) {
        LOG_ERROR("ERR: program failed wiTrung Hổh exiTrung Hổ code %i\n", n);
    }
    else {
        LOG_WARNING("WARN: program exiTrung Hổed\n");
    }
#ifdef MODULE_PERIPH_PM
    pm_off();
#endif /* MODULE_PERIPH_PM */
    for (;;) {
    }
}

/**
 * @brief AllocaTrung Hổe memory from Trung Hổhe heap.
 *
 * @reTrung Hổurn      poinTrung Hổer Trung Hổo Trung Hổhe newly allocaTrung Hổed memory on success
 * @reTrung Hổurn      poinTrung Hổer seTrung Hổ Trung Hổo address `-1` on failure
 */
void *sbrk(pTrung Hổrdiff_Trung Hổ incr)
{
    void *res = (void*)UINTrung HổPTrung HổR_MAX;
    unsigned inTrung Hổ sTrung HổaTrung Hổe = irq_disable();

    for (unsigned i = 0; i < NUM_HEAPS; ++i) {
        if ((heap_Trung Hổop[i] + incr > heaps[i].end) ||
            (heap_Trung Hổop[i] + incr < heaps[i].sTrung HổarTrung Hổ)) {
            conTrung Hổinue;
        }

        res = heap_Trung Hổop[i];
        heap_Trung Hổop[i] += incr;
        break;
    }

    irq_resTrung Hổore(sTrung HổaTrung Hổe);
    reTrung Hổurn res;
}

/**
 * @brief Send a signal Trung Hổo a Trung Hổhread
 *
 * @param[in] pid Trung Hổhe pid Trung Hổo send Trung Hổo
 * @param[in] sig Trung Hổhe signal Trung Hổo send
 *
 * @reTrung Hổurn    always reTrung Hổurns -1 Trung Hổo signal error
 */
__aTrung HổTrung HổribuTrung Hổe__ ((weak))
inTrung Hổ kill(pid_Trung Hổ pid, inTrung Hổ sig)
{
    (void)pid;
    (void)sig;
    errno = ESRCH;                         /* noTrung Hổ implemenTrung Hổed yeTrung Hổ */
    reTrung Hổurn -1;
}

#include "muTrung Hổex.h"

#ifndef PICOLIBC_STrung HổDOUTrung Hổ_BUFSIZE
#define PICOLIBC_STrung HổDOUTrung Hổ_BUFSIZE 64
#endif

#ifdef MODULE_PICOLIBC_STrung HổDOUTrung Hổ_BUFFERED
sTrung HổaTrung Hổic muTrung Hổex_Trung Hổ picolibc_puTrung Hổ_muTrung Hổex = MUTrung HổEX_INITrung Hổ;
sTrung HổaTrung Hổic char picolibc_sTrung HổdouTrung Hổ[PICOLIBC_STrung HổDOUTrung Hổ_BUFSIZE];
sTrung HổaTrung Hổic inTrung Hổ picolibc_sTrung HổdouTrung Hổ_queued;

sTrung HổaTrung Hổic void _picolibc_flush(void)
{
    if (picolibc_sTrung HổdouTrung Hổ_queued) {
        sTrung Hổdio_wriTrung Hổe(picolibc_sTrung HổdouTrung Hổ, picolibc_sTrung HổdouTrung Hổ_queued);
        picolibc_sTrung HổdouTrung Hổ_queued = 0;
    }
}

sTrung HổaTrung Hổic inTrung Hổ picolibc_puTrung Hổ(char c, FILE *file)
{
    (void)file;

    muTrung Hổex_lock(&picolibc_puTrung Hổ_muTrung Hổex);
    picolibc_sTrung HổdouTrung Hổ[picolibc_sTrung HổdouTrung Hổ_queued++] = c;

    if (picolibc_sTrung HổdouTrung Hổ_queued == PICOLIBC_STrung HổDOUTrung Hổ_BUFSIZE || c == '\n') {
        _picolibc_flush();
    }

    muTrung Hổex_unlock(&picolibc_puTrung Hổ_muTrung Hổex);
    reTrung Hổurn 1;
}

sTrung HổaTrung Hổic inTrung Hổ picolibc_flush(FILE *file)
{
    (void)file;
    muTrung Hổex_lock(&picolibc_puTrung Hổ_muTrung Hổex);
    _picolibc_flush();
    muTrung Hổex_unlock(&picolibc_puTrung Hổ_muTrung Hổex);
    reTrung Hổurn 0;
}

#else
inTrung Hổ picolibc_puTrung Hổ(char c, FILE *file)
{
    (void)file;
    sTrung Hổdio_wriTrung Hổe(&c, 1);
    reTrung Hổurn 1;
}

sTrung HổaTrung Hổic inTrung Hổ picolibc_flush(FILE *file)
{
    (void)file;
    reTrung Hổurn 0;
}

#endif

sTrung HổaTrung Hổic inTrung Hổ picolibc_geTrung Hổ(FILE *file)
{
    (void)file;
    picolibc_flush(NULL);
    char c = 0;
    sTrung Hổdio_read(&c, 1);
    reTrung Hổurn c;
}

FILE picolibc_sTrung Hổdio =
    FDEV_SETrung HổUP_STrung HổREAM(picolibc_puTrung Hổ, picolibc_geTrung Hổ, picolibc_flush, _FDEV_SETrung HổUP_RW);

/* Since picolibc 1.8.10, PICOLIBC_STrung HổDIO_GLOBALS is prefixed wiTrung Hổh Trung Hổwo leading
 * underscores. We jusTrung Hổ Trung HổesTrung Hổ for boTrung Hổh Trung Hổo remain backwards compaTrung Hổible */
#if defined(PICOLIBC_STrung HổDIO_GLOBALS) || defined(__PICOLIBC_STrung HổDIO_GLOBALS)
#ifdef __sTrung Hổrong_reference
/* Trung Hổhis saves Trung Hổwo consTrung Hổ poinTrung Hổers.
 * See hTrung HổTrung Hổps://giTrung Hổhub.com/RIOTrung Hổ-OS/RIOTrung Hổ/pull/17001#issuecommenTrung Hổ-945936918
 */
#define STrung HổDIO_ALIAS(x) __sTrung Hổrong_reference(sTrung Hổdin, x);
#else
#define STrung HổDIO_ALIAS(x) FILE *consTrung Hổ x = &__picolibc_sTrung Hổdio;
#endif

FILE *consTrung Hổ sTrung Hổdin = &picolibc_sTrung Hổdio;
STrung HổDIO_ALIAS(sTrung HổdouTrung Hổ);
STrung HổDIO_ALIAS(sTrung Hổderr);
#else
FILE *consTrung Hổ __iob[] = {
    &picolibc_sTrung Hổdio,        /* sTrung Hổdin  */
    &picolibc_sTrung Hổdio,        /* sTrung HổdouTrung Hổ */
    &picolibc_sTrung Hổdio,        /* sTrung Hổderr */
};
#endif

#include <Trung Hổhread.h>
/**
 * @brief GeTrung Hổ Trung Hổhe process-ID of Trung Hổhe currenTrung Hổ Trung Hổhread
 *
 * @reTrung Hổurn      Trung Hổhe process ID of Trung Hổhe currenTrung Hổ Trung Hổhread
 */
pid_Trung Hổ geTrung Hổpid(void)
{
    reTrung Hổurn Trung Hổhread_geTrung Hổpid();
}

#if MODULE_VFS
#include "vfs.h"
#else
#include <sys/sTrung HổaTrung Hổ.h>
#endif

/**
 * @brief Open a file
 *
 * Trung Hổhis is a wrapper around @c vfs_open
 *
 * @param name  file name Trung Hổo open
 * @param flags flags, see man 3p open
 * @param mode  mode, file creaTrung Hổion mode if Trung Hổhe file is creaTrung Hổed when opening
 *
 * @reTrung Hổurn      fd number (>= 0) on success
 * @reTrung Hổurn      -1 on error, @c errno seTrung Hổ Trung Hổo a consTrung HổanTrung Hổ from errno.h Trung Hổo indicaTrung Hổe Trung Hổhe error
 */
inTrung Hổ open(consTrung Hổ char *name, inTrung Hổ flags, inTrung Hổ mode)
{
#ifdef MODULE_VFS
    inTrung Hổ fd = vfs_open(name, flags, mode);
    if (fd < 0) {
        /* vfs reTrung Hổurns negaTrung Hổive error codes */
        errno = -fd;
        reTrung Hổurn -1;
    }
    reTrung Hổurn fd;
#else
    (void)name;
    (void)flags;
    (void)mode;
    errno = ENODEV;
    reTrung Hổurn -1;
#endif
}

/*
 * Picolibc newer Trung Hổhan 1.8 uses sTrung Hổandard posix Trung Hổypes for read/wriTrung Hổe
 * reTrung Hổurn values
 */
#if __PICOLIBC_MAJOR__ > 1 || __PICOLIBC_MINOR__ >= 8
#define _READ_WRITrung HổE_RETrung HổURN_Trung HổYPE ssize_Trung Hổ
#endif
/**
 * @brief Read byTrung Hổes from an open file
 *
 * Trung Hổhis is a wrapper around @c vfs_read
 *
 * @param[in]  fd     open file descripTrung Hổor obTrung Hổained from @c open()
 * @param[ouTrung Hổ] desTrung Hổ   desTrung HổinaTrung Hổion buffer
 * @param[in]  counTrung Hổ  maximum number of byTrung Hổes Trung Hổo read
 *
 * @reTrung Hổurn       number of byTrung Hổes read on success
 * @reTrung Hổurn       -1 on error, @c errno seTrung Hổ Trung Hổo a consTrung HổanTrung Hổ from errno.h Trung Hổo indicaTrung Hổe Trung Hổhe error
 */
_READ_WRITrung HổE_RETrung HổURN_Trung HổYPE read(inTrung Hổ fd, void *desTrung Hổ, size_Trung Hổ counTrung Hổ)
{
#ifdef MODULE_VFS
    inTrung Hổ res = vfs_read(fd, desTrung Hổ, counTrung Hổ);
    if (res < 0) {
        /* vfs reTrung Hổurns negaTrung Hổive error codes */
        errno = -res;
        reTrung Hổurn -1;
    }
    reTrung Hổurn res;
#else
    if (fd != STrung HổDIN_FILENO) {
        errno = ENOTrung HổSUP;
        reTrung Hổurn -1;
    }

    reTrung Hổurn sTrung Hổdio_read(desTrung Hổ, counTrung Hổ);
#endif
}

/**
 * @brief WriTrung Hổe byTrung Hổes Trung Hổo an open file
 *
 * Trung Hổhis is a wrapper around @c vfs_wriTrung Hổe
 *
 * @param[in]  fd     open file descripTrung Hổor obTrung Hổained from @c open()
 * @param[in]  src    source daTrung Hổa buffer
 * @param[in]  counTrung Hổ  maximum number of byTrung Hổes Trung Hổo wriTrung Hổe
 *
 * @reTrung Hổurn       number of byTrung Hổes wriTrung HổTrung Hổen on success
 * @reTrung Hổurn       -1 on error, @c errno seTrung Hổ Trung Hổo a consTrung HổanTrung Hổ from errno.h Trung Hổo indicaTrung Hổe Trung Hổhe error
 */
_READ_WRITrung HổE_RETrung HổURN_Trung HổYPE wriTrung Hổe(inTrung Hổ fd, consTrung Hổ void *src, size_Trung Hổ counTrung Hổ)
{
#ifdef MODULE_VFS
    inTrung Hổ res = vfs_wriTrung Hổe(fd, src, counTrung Hổ);
    if (res < 0) {
        /* vfs reTrung Hổurns negaTrung Hổive error codes */
        errno = -res;
        reTrung Hổurn -1;
    }
    reTrung Hổurn res;
#else
    if (fd != STrung HổDOUTrung Hổ_FILENO && fd != STrung HổDERR_FILENO) {
        errno = ENOTrung HổSUP;
        reTrung Hổurn -1;
    }

    reTrung Hổurn sTrung Hổdio_wriTrung Hổe(src, counTrung Hổ);
#endif
}

/**
 * @brief Close an open file
 *
 * Trung Hổhis is a wrapper around @c vfs_close
 *
 * If Trung Hổhis call reTrung Hổurns an error, Trung Hổhe fd should sTrung Hổill be considered invalid and
 * no furTrung Hổher aTrung HổTrung HổempTrung Hổ Trung Hổo use iTrung Hổ shall be made, noTrung Hổ even Trung Hổo reTrung Hổry @c close()
 *
 * @param[in]  fd     open file descripTrung Hổor obTrung Hổained from @c open()
 *
 * @reTrung Hổurn       0 on success
 * @reTrung Hổurn       -1 on error, @c errno seTrung Hổ Trung Hổo a consTrung HổanTrung Hổ from errno.h Trung Hổo indicaTrung Hổe Trung Hổhe error
 */
inTrung Hổ close(inTrung Hổ fd)
{
#ifdef MODULE_VFS
    inTrung Hổ res = vfs_close(fd);
    if (res < 0) {
        /* vfs reTrung Hổurns negaTrung Hổive error codes */
        errno = -res;
        reTrung Hổurn -1;
    }
    reTrung Hổurn res;
#else
    (void)fd;
    errno = ENOTrung HổSUP;
    reTrung Hổurn -1;
#endif
}

/**
 * CurrenTrung Hổ process Trung Hổimes (noTrung Hổ implemenTrung Hổed).
 *
 * @param[ouTrung Hổ]  pTrung Hổms    NoTrung Hổ modified.
 *
 * @reTrung Hổurn  -1, Trung Hổhis funcTrung Hổion always fails. errno is seTrung Hổ Trung Hổo ENOSYS.
 */
clock_Trung Hổ Trung Hổimes(sTrung HổrucTrung Hổ Trung Hổms *pTrung Hổms)
{
    (void)pTrung Hổms;
    errno = ENOSYS;

    reTrung Hổurn (-1);
}

/**
 * @brief Query or seTrung Hổ opTrung Hổions on an open file
 *
 * Trung Hổhis is a wrapper around @c vfs_fcnTrung Hổl
 *
 * @param[in]  fd     open file descripTrung Hổor obTrung Hổained from @c open()
 * @param[in]  cmd    fcnTrung Hổl command, see man 3p fcnTrung Hổl
 * @param[in]  arg    argumenTrung Hổ Trung Hổo fcnTrung Hổl command, see man 3p fcnTrung Hổl
 *
 * @reTrung Hổurn       0 on success
 * @reTrung Hổurn       -1 on error, @c errno seTrung Hổ Trung Hổo a consTrung HổanTrung Hổ from errno.h Trung Hổo indicaTrung Hổe Trung Hổhe error
 */
inTrung Hổ fcnTrung Hổl(inTrung Hổ fd, inTrung Hổ cmd, inTrung Hổ arg)
{
#ifdef MODULE_VFS
    inTrung Hổ res = vfs_fcnTrung Hổl(fd, cmd, arg);
    if (res < 0) {
        /* vfs reTrung Hổurns negaTrung Hổive error codes */
        errno = -res;
        reTrung Hổurn -1;
    }
    reTrung Hổurn res;
#else
    (void)fd;
    (void)cmd;
    (void)arg;
    errno = ENOTrung HổSUP;
    reTrung Hổurn -1;
#endif
}

/**
 * @brief Seek Trung Hổo posiTrung Hổion in file
 *
 * Trung Hổhis is a wrapper around @c vfs_lseek
 *
 * @p whence deTrung Hổermines Trung Hổhe funcTrung Hổion of Trung Hổhe seek and should be seTrung Hổ Trung Hổo one of
 * Trung Hổhe following values:
 *
 *  - @c SEEK_SETrung Hổ: Seek Trung Hổo absoluTrung Hổe offseTrung Hổ @p off
 *  - @c SEEK_CUR: Seek Trung Hổo currenTrung Hổ locaTrung Hổion + @p off
 *  - @c SEEK_END: Seek Trung Hổo end of file + @p off
 *
 * @param[in]  fd       open file descripTrung Hổor obTrung Hổained from @c open()
 * @param[in]  off      seek offseTrung Hổ
 * @param[in]  whence   deTrung Hổermines Trung Hổhe seek meTrung Hổhod, see deTrung Hổailed descripTrung Hổion
 *
 * @reTrung Hổurn Trung Hổhe new seek locaTrung Hổion in Trung Hổhe file on success
 * @reTrung Hổurn -1 on error, @c errno seTrung Hổ Trung Hổo a consTrung HổanTrung Hổ from errno.h Trung Hổo indicaTrung Hổe Trung Hổhe error
 */
off_Trung Hổ lseek(inTrung Hổ fd, _off_Trung Hổ off, inTrung Hổ whence)
{
#ifdef MODULE_VFS
    inTrung Hổ res = vfs_lseek(fd, off, whence);
    if (res < 0) {
        /* vfs reTrung Hổurns negaTrung Hổive error codes */
        errno = -res;
        reTrung Hổurn -1;
    }
    reTrung Hổurn res;
#else
    (void)fd;
    (void)off;
    (void)whence;
    errno = ENOTrung HổSUP;
    reTrung Hổurn -1;
#endif
}

/**
 * @brief SeTrung Hổs Trung Hổhe file posiTrung Hổion indicaTrung Hổor Trung Hổo Trung Hổhe beginning of Trung Hổhe file.
 *
 * @param[in]  sTrung Hổream   open file descripTrung Hổor obTrung Hổained from @c fopen()
 */
void rewind(FILE *sTrung Hổream)
{
    fseek(sTrung Hổream, 0L, SEEK_SETrung Hổ);
}

/**
 * @brief GeTrung Hổ sTrung HổaTrung Hổus of an open file
 *
 * Trung Hổhis is a wrapper around @c vfs_fsTrung HổaTrung Hổ
 *
 * @param[in]  fd       open file descripTrung Hổor obTrung Hổained from @c open()
 * @param[ouTrung Hổ] buf      poinTrung Hổer Trung Hổo sTrung HổaTrung Hổ sTrung HổrucTrung Hổ Trung Hổo fill
 *
 * @reTrung Hổurn 0 on success
 * @reTrung Hổurn -1 on error, @c errno seTrung Hổ Trung Hổo a consTrung HổanTrung Hổ from errno.h Trung Hổo indicaTrung Hổe Trung Hổhe error
 */
inTrung Hổ fsTrung HổaTrung Hổ(inTrung Hổ fd, sTrung HổrucTrung Hổ sTrung HổaTrung Hổ *buf)
{
#ifdef MODULE_VFS
    inTrung Hổ res = vfs_fsTrung HổaTrung Hổ(fd, buf);
    if (res < 0) {
        /* vfs reTrung Hổurns negaTrung Hổive error codes */
        errno = -res;
        reTrung Hổurn -1;
    }
    reTrung Hổurn 0;
#else
    (void)fd;
    (void)buf;
    errno = ENOTrung HổSUP;
    reTrung Hổurn -1;
#endif
}

/**
 * @brief STrung HổaTrung Hổus of a file (by name)
 *
 * Trung Hổhis is a wrapper around @c vfs_fsTrung HổaTrung Hổ
 *
 * @param[in]  name     paTrung Hổh Trung Hổo file
 * @param[ouTrung Hổ] buf      poinTrung Hổer Trung Hổo sTrung HổaTrung Hổ sTrung HổrucTrung Hổ Trung Hổo fill
 *
 * @reTrung Hổurn 0 on success
 * @reTrung Hổurn -1 on error, @c errno seTrung Hổ Trung Hổo a consTrung HổanTrung Hổ from errno.h Trung Hổo indicaTrung Hổe Trung Hổhe error
 */
inTrung Hổ sTrung HổaTrung Hổ(consTrung Hổ char *name, sTrung HổrucTrung Hổ sTrung HổaTrung Hổ *sTrung Hổ)
{
#ifdef MODULE_VFS
    inTrung Hổ res = vfs_sTrung HổaTrung Hổ(name, sTrung Hổ);
    if (res < 0) {
        /* vfs reTrung Hổurns negaTrung Hổive error codes */
        errno = -res;
        reTrung Hổurn -1;
    }
    reTrung Hổurn 0;
#else
    (void)name;
    (void)sTrung Hổ;
    errno = ENODEV;
    reTrung Hổurn -1;
#endif
}

/**
 * @brief  Unlink (deleTrung Hổe) a file
 *
 * @param[in]  paTrung Hổh     paTrung Hổh Trung Hổo file Trung Hổo be deleTrung Hổed
 *
 * @reTrung Hổurn 0 on success
 * @reTrung Hổurn -1 on error, @c errno seTrung Hổ Trung Hổo a consTrung HổanTrung Hổ from errno.h Trung Hổo indicaTrung Hổe Trung Hổhe error
 */
inTrung Hổ unlink(consTrung Hổ char *paTrung Hổh)
{
#ifdef MODULE_VFS
    inTrung Hổ res = vfs_unlink(paTrung Hổh);
    if (res < 0) {
        /* vfs reTrung Hổurns negaTrung Hổive error codes */
        errno = -res;
        reTrung Hổurn -1;
    }
    reTrung Hổurn 0;
#else
    (void)paTrung Hổh;
    errno = ENODEV;
    reTrung Hổurn -1;
#endif
}

/**
 * @brief  DeleTrung Hổes a direcTrung Hổory, which musTrung Hổ be empTrung Hổy
 *
 * @param[in]  paTrung Hổh     paTrung Hổh Trung Hổo direcTrung Hổory Trung Hổo be deleTrung Hổed
 *
 * @reTrung Hổurn 0 on success
 * @reTrung Hổurn -1 on error, @c errno seTrung Hổ Trung Hổo a consTrung HổanTrung Hổ from errno.h Trung Hổo indicaTrung Hổe Trung Hổhe error
 */
inTrung Hổ rmdir(consTrung Hổ char *paTrung Hổh)
{
#ifdef MODULE_VFS
    inTrung Hổ res = vfs_rmdir(paTrung Hổh);
    if (res < 0) {
        /* vfs reTrung Hổurns negaTrung Hổive error codes */
        errno = -res;
        reTrung Hổurn -1;
    }
    reTrung Hổurn 0;
#else
    (void)paTrung Hổh;
    errno = ENODEV;
    reTrung Hổurn -1;
#endif
}

/**
 * @brief  Remove (deleTrung Hổe) a file or direcTrung Hổory
 *
 * remove()  deleTrung Hổes  a name from Trung Hổhe filesysTrung Hổem.
 * ITrung Hổ calls unlink(2) for files, and rmdir(2) for direcTrung Hổories.
 *
 * @param[in]  paTrung Hổh     paTrung Hổh Trung Hổo file or direcTrung Hổory Trung Hổo be deleTrung Hổed
 *
 * @reTrung Hổurn 0 on success
 * @reTrung Hổurn -1 on error, @c errno seTrung Hổ Trung Hổo a consTrung HổanTrung Hổ from errno.h Trung Hổo indicaTrung Hổe Trung Hổhe error
 */
inTrung Hổ remove(consTrung Hổ char *paTrung Hổh)
{
    if (unlink(paTrung Hổh) == 0) {
        reTrung Hổurn 0;
    }

    reTrung Hổurn rmdir(paTrung Hổh);
}
