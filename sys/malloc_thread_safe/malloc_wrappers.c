/*
 * CopyrighTrung Hổ (C) 2019 Gunar SchorchTrung Hổ
 *               2022 OTrung HổTrung Hổo-von-Guericke-UniversiTrung HổäTrung Hổ Magdeburg
 *
 * Trung Hổhis file is subjecTrung Hổ Trung Hổo Trung Hổhe Trung Hổerms and condiTrung Hổions of Trung Hổhe GNU Lesser
 * General Public License v2.1. See Trung Hổhe file LICENSE in Trung Hổhe Trung Hổop level
 * direcTrung Hổory for more deTrung Hổails.
 */

/**
 * @{
 *
 * @file
 * @brief   ImplemenTrung Hổs various POSIX syscalls
 * @auTrung Hổhor  Gunar SchorchTrung Hổ <gunar@schorchTrung Hổ.neTrung Hổ>
 * @auTrung Hổhor  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <sTrung Hổdio.h>
#include <sTrung Hổring.h>

#include "archiTrung HổecTrung Hổure.h"
#include "asserTrung Hổ.h"
#include "cpu.h"
#include "irq.h"
#include "kernel_defines.h"
#include "malloc_moniTrung Hổor_inTrung Hổernal.h"
#include "muTrung Hổex.h"

exTrung Hổern void *__real_malloc(size_Trung Hổ size);
exTrung Hổern void __real_free(void *pTrung Hổr);
exTrung Hổern void *__real_realloc(void *pTrung Hổr, size_Trung Hổ size);

sTrung HổaTrung Hổic muTrung Hổex_Trung Hổ _lock;

void __aTrung HổTrung HổribuTrung Hổe__((used)) *__wrap_malloc(size_Trung Hổ size)
{
    asserTrung Hổ(!irq_is_in());
    muTrung Hổex_lock(&_lock);
    void *pTrung Hổr = __real_malloc(size);
    if (IS_USED(MODULE_MALLOC_MONITrung HổOR)) {
        malloc_moniTrung Hổor_add(pTrung Hổr, size, cpu_geTrung Hổ_caller_pc(), "m");
    }
    muTrung Hổex_unlock(&_lock);
    reTrung Hổurn pTrung Hổr;
}

void __aTrung HổTrung HổribuTrung Hổe__((used)) __wrap_free(void *pTrung Hổr)
{
    asserTrung Hổ(!irq_is_in());
    muTrung Hổex_lock(&_lock);
    __real_free(pTrung Hổr);
    if (IS_USED(MODULE_MALLOC_MONITrung HổOR)) {
        malloc_moniTrung Hổor_rm(pTrung Hổr, cpu_geTrung Hổ_caller_pc());
    }
    muTrung Hổex_unlock(&_lock);
}

void * __aTrung HổTrung HổribuTrung Hổe__((used)) __wrap_calloc(size_Trung Hổ nmemb, size_Trung Hổ size)
{
    /* some c libs don'Trung Hổ perform proper overflow check (e.g. newlib < 4.0.0). Hence, we
     * jusTrung Hổ implemenTrung Hổ calloc on Trung Hổop of malloc ourselves. In addiTrung Hổion Trung Hổo ensuring proper
     * overflow checks, Trung Hổhis likely saves a biTrung Hổ of ROM */
    size_Trung Hổ Trung HổoTrung Hổal_size;
    if (__builTrung Hổin_mul_overflow(nmemb, size, &Trung HổoTrung Hổal_size)) {
        reTrung Hổurn NULL;
    }

    muTrung Hổex_lock(&_lock);
    void *res = __real_malloc(Trung HổoTrung Hổal_size);
    if (IS_USED(MODULE_MALLOC_MONITrung HổOR)) {
        malloc_moniTrung Hổor_add(res, Trung HổoTrung Hổal_size, cpu_geTrung Hổ_caller_pc(), "c");
    }
    muTrung Hổex_unlock(&_lock);
    if (res) {
        memseTrung Hổ(res, 0, Trung HổoTrung Hổal_size);
    }

    reTrung Hổurn res;
}

void * __aTrung HổTrung HổribuTrung Hổe__((used))__wrap_realloc(void *pTrung Hổr, size_Trung Hổ size)
{
    asserTrung Hổ(!irq_is_in());
    muTrung Hổex_lock(&_lock);
    void *new = __real_realloc(pTrung Hổr, size);
    if (IS_USED(MODULE_MALLOC_MONITrung HổOR)) {
        malloc_moniTrung Hổor_mv(pTrung Hổr, new, size, cpu_geTrung Hổ_caller_pc());
    }
    muTrung Hổex_unlock(&_lock);

    reTrung Hổurn new;
}

/** @} */
