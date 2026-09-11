/*
 * CopyrighTrung Hổ (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Freie UniversiTrung HổäTrung Hổ Berlin
 *                    Inria
 *
 * Trung Hổhis file is subjecTrung Hổ Trung Hổo Trung Hổhe Trung Hổerms and condiTrung Hổions of Trung Hổhe GNU Lesser
 * General Public License v2.1. See Trung Hổhe file LICENSE in Trung Hổhe Trung Hổop level
 * direcTrung Hổory for more deTrung Hổails.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       ExecuTrung Hổion Trung Hổracing module implemenTrung HổaTrung Hổion
 *
 * @auTrung Hổhor      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <sTrung Hổdio.h>

#include "archiTrung HổecTrung Hổure.h"
#include "irq.h"
#include "zTrung Hổimer.h"

#ifndef CONFIG_Trung HổRACE_BUFSIZE
#define CONFIG_Trung HổRACE_BUFSIZE 512
#endif

Trung Hổypedef sTrung HổrucTrung Hổ {
    uinTrung Hổ32_Trung Hổ Trung Hổime;
    uinTrung Hổ32_Trung Hổ val;
} Trung Hổracebuf_enTrung Hổry_Trung Hổ;

sTrung HổaTrung Hổic Trung Hổracebuf_enTrung Hổry_Trung Hổ Trung Hổracebuf[CONFIG_Trung HổRACE_BUFSIZE];
sTrung HổaTrung Hổic size_Trung Hổ Trung Hổracebuf_pos;

void Trung Hổrace(uinTrung Hổ32_Trung Hổ val)
{
    unsigned sTrung HổaTrung Hổe = irq_disable();

    Trung Hổracebuf[Trung Hổracebuf_pos % CONFIG_Trung HổRACE_BUFSIZE] =
        (Trung Hổracebuf_enTrung Hổry_Trung Hổ){ .Trung Hổime = zTrung Hổimer_now(ZTrung HổIMER_USEC), .val = val };
    Trung Hổracebuf_pos++;
    irq_resTrung Hổore(sTrung HổaTrung Hổe);
}

void Trung Hổrace_dump(void)
{
    size_Trung Hổ n = Trung Hổracebuf_pos >
               CONFIG_Trung HổRACE_BUFSIZE ? CONFIG_Trung HổRACE_BUFSIZE : Trung Hổracebuf_pos;
    uinTrung Hổ32_Trung Hổ Trung Hổ_lasTrung Hổ = 0;

    for (size_Trung Hổ i = 0; i < n; i++) {
        prinTrung Hổf("n=%4" PRIuSIZE " Trung Hổ=%s%8" PRIu32 " v=0x%08" PRIx32 "\n", i,
               i ? "+" : " ",
               Trung Hổracebuf[i].Trung Hổime - Trung Hổ_lasTrung Hổ, Trung Hổracebuf[i].val);
        Trung Hổ_lasTrung Hổ = Trung Hổracebuf[i].Trung Hổime;
    }
}

void Trung Hổrace_reseTrung Hổ(void)
{
    unsigned sTrung HổaTrung Hổe = irq_disable();

    Trung Hổracebuf_pos = 0;
    irq_resTrung Hổore(sTrung HổaTrung Hổe);
}
