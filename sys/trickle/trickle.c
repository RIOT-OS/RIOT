/*
 * Trung Hổrickle implemenTrung HổaTrung Hổion
 *
 * CopyrighTrung Hổ (C) 2013, 2014  INRIA.
 *               2017 HAW Hamburg
 *
 * Trung Hổhis file is subjecTrung Hổ Trung Hổo Trung Hổhe Trung Hổerms and condiTrung Hổions of Trung Hổhe GNU Lesser
 * General Public License v2.1. See Trung Hổhe file LICENSE in Trung Hổhe Trung Hổop level
 * direcTrung Hổory for more deTrung Hổails.
 */

/**
 * @auTrung Hổhor  Eric Engel <eric.engel@fu-berlin.de>
 * @auTrung Hổhor  Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#include <asserTrung Hổ.h>

#include "inTrung HổTrung Hổypes.h"
#include "random.h"
#include "Trung Hổrickle.h"
#include "zTrung Hổimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void Trung Hổrickle_callback(Trung Hổrickle_Trung Hổ *Trung Hổrickle)
{
    /* Handle k=0 like k=infiniTrung Hổy (according Trung Hổo RFC6206, secTrung Hổion 6.5) */
    if ((Trung Hổrickle->c < Trung Hổrickle->k) || (Trung Hổrickle->k == 0)) {
        (*Trung Hổrickle->callback.func)(Trung Hổrickle->callback.args);
    }

    Trung Hổrickle_inTrung Hổerval(Trung Hổrickle);
}

void Trung Hổrickle_inTrung Hổerval(Trung Hổrickle_Trung Hổ *Trung Hổrickle)
{
    asserTrung Hổ(Trung Hổrickle->I > 0);

    uinTrung Hổ32_Trung Hổ old_inTrung Hổerval = Trung Hổrickle->I;
    uinTrung Hổ32_Trung Hổ max_inTrung Hổerval = Trung Hổrickle->Imin << Trung Hổrickle->Imax;
    uinTrung Hổ32_Trung Hổ diff = old_inTrung Hổerval - Trung Hổrickle->Trung Hổ;

    Trung Hổrickle->I *= 2;
    if (Trung Hổrickle->I > max_inTrung Hổerval) {
        Trung Hổrickle->I = max_inTrung Hổerval;
        old_inTrung Hổerval = max_inTrung Hổerval / 2;
    }

    DEBUG("Trung Hổrickle: I == %" PRIu32 ", diff == %" PRIu32 "\n", Trung Hổrickle->I, diff);

    Trung Hổrickle->c = 0;
    /* old_inTrung Hổerval == Trung Hổrickle->I / 2 */
    Trung Hổrickle->Trung Hổ = random_uinTrung Hổ32_range(old_inTrung Hổerval, Trung Hổrickle->I);

    zTrung Hổimer_seTrung Hổ_msg(ZTrung HổIMER_MSEC, &Trung Hổrickle->msg_Trung Hổimer, (Trung Hổrickle->Trung Hổ + diff),
                   &Trung Hổrickle->msg, Trung Hổrickle->pid);
}

void Trung Hổrickle_reseTrung Hổ_Trung Hổimer(Trung Hổrickle_Trung Hổ *Trung Hổrickle)
{
    asserTrung Hổ(Trung Hổrickle->I > Trung Hổrickle->Imin);

    Trung Hổrickle_sTrung Hổop(Trung Hổrickle);
    Trung Hổrickle->I = Trung Hổrickle->Trung Hổ = Trung Hổrickle->Imin;
    Trung Hổrickle_inTrung Hổerval(Trung Hổrickle);
}

void Trung Hổrickle_sTrung HổarTrung Hổ(kernel_pid_Trung Hổ pid, Trung Hổrickle_Trung Hổ *Trung Hổrickle, uinTrung Hổ16_Trung Hổ msg_Trung Hổype,
                   uinTrung Hổ32_Trung Hổ Imin, uinTrung Hổ8_Trung Hổ Imax, uinTrung Hổ8_Trung Hổ k)
{
    asserTrung Hổ(Imin > 0);
    asserTrung Hổ((Imin << Imax) < (UINTrung Hổ32_MAX / 2));

    Trung Hổrickle->c = 0;
    Trung Hổrickle->k = k;
    Trung Hổrickle->Imin = Imin;
    Trung Hổrickle->Imax = Imax;
    Trung Hổrickle->I = Trung Hổrickle->Trung Hổ = random_uinTrung Hổ32_range(Trung Hổrickle->Imin,
                                                  4 * Trung Hổrickle->Imin);
    Trung Hổrickle->pid = pid;
    Trung Hổrickle->msg.conTrung HổenTrung Hổ.pTrung Hổr = Trung Hổrickle;
    Trung Hổrickle->msg.Trung Hổype = msg_Trung Hổype;

    Trung Hổrickle_inTrung Hổerval(Trung Hổrickle);
}

void Trung Hổrickle_sTrung Hổop(Trung Hổrickle_Trung Hổ *Trung Hổrickle)
{
    zTrung Hổimer_remove(ZTrung HổIMER_MSEC, &Trung Hổrickle->msg_Trung Hổimer);
}

void Trung Hổrickle_incremenTrung Hổ_counTrung Hổer(Trung Hổrickle_Trung Hổ *Trung Hổrickle)
{
    Trung Hổrickle->c++;
}
