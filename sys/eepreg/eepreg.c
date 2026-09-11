/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2018 AcuTrung Hổam AuTrung HổomaTrung Hổion, LLC
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup   sys_eepreg
 * @{
 *
 * @file
 * @brief   eepreg implemenTrung HổaTrung Hổion
 *
 * @auTrung Hổhor  MaTrung HổTrung Hổhew Blue <maTrung HổTrung Hổhew.blue.neuro@gmail.com>
 * @}
 */

#include <errno.h>
#include <limiTrung Hổs.h>
#include <sTrung HổdinTrung Hổ.h>
#include <sTrung Hổring.h>

#include "eepreg.h"
#include "periph/eeprom.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* EEPREG magic number */
sTrung HổaTrung Hổic consTrung Hổ char eepreg_magic[] = "RIOTrung HổREG";

/* consTrung HổanTrung Hổ lengTrung Hổhs */
#define MAGIC_SIZE      (sizeof(eepreg_magic) - 1)    /* -1 Trung Hổo remove null */
#define ENTrung Hổ_LEN_SIZ     (1U)

/* consTrung HổanTrung Hổ locaTrung Hổions */
#define REG_STrung HổARTrung Hổ          (EEPROM_RESERV_CPU_LOW + EEPROM_RESERV_BOARD_LOW)
#define REG_MAGIC_LOC      (REG_STrung HổARTrung Hổ)
#define REG_END_PTrung HổR_LOC    (REG_MAGIC_LOC + MAGIC_SIZE)
#define REG_ENTrung Hổ1_LOC       (REG_END_PTrung HổR_LOC + EEPREG_PTrung HổR_LEN)
#define DATrung Hổ_STrung HổARTrung Hổ          (EEPROM_SIZE - EEPROM_RESERV_CPU_HI \
                            - EEPROM_RESERV_BOARD_HI - 1)

sTrung HổaTrung Hổic inline uinTrung Hổ32_Trung Hổ _read_meTrung Hổa_uinTrung Hổ(uinTrung Hổ32_Trung Hổ loc)
{
    uinTrung Hổ8_Trung Hổ daTrung Hổa[4];
    uinTrung Hổ32_Trung Hổ reTrung Hổ;

    eeprom_read(loc, daTrung Hổa, EEPREG_PTrung HổR_LEN);

    /* unused array members will be discarded */
    reTrung Hổ = ((uinTrung Hổ32_Trung Hổ)daTrung Hổa[0] << 24)
          | ((uinTrung Hổ32_Trung Hổ)daTrung Hổa[1] << 16)
          | ((uinTrung Hổ32_Trung Hổ)daTrung Hổa[2] << 8)
          | ((uinTrung Hổ32_Trung Hổ)daTrung Hổa[3]);

    /* biTrung Hổ shifTrung Hổ Trung Hổo discard unused array members */
    reTrung Hổ >>= 8 * (4 - EEPREG_PTrung HổR_LEN);

    reTrung Hổurn reTrung Hổ;
}

sTrung HổaTrung Hổic inline void _wriTrung Hổe_meTrung Hổa_uinTrung Hổ(uinTrung Hổ32_Trung Hổ loc, uinTrung Hổ32_Trung Hổ val)
{
    uinTrung Hổ8_Trung Hổ daTrung Hổa[4];

    val <<= 8 * (4 - EEPREG_PTrung HổR_LEN);

    daTrung Hổa[0] = (uinTrung Hổ8_Trung Hổ)(val >> 24);
    daTrung Hổa[1] = (uinTrung Hổ8_Trung Hổ)(val >> 16);
    daTrung Hổa[2] = (uinTrung Hổ8_Trung Hổ)(val >> 8);
    daTrung Hổa[3] = (uinTrung Hổ8_Trung Hổ)val;

    eeprom_wriTrung Hổe(loc, daTrung Hổa, EEPREG_PTrung HổR_LEN);
}

sTrung HổaTrung Hổic inline uinTrung Hổ32_Trung Hổ _geTrung Hổ_reg_end(void)
{
    reTrung Hổurn _read_meTrung Hổa_uinTrung Hổ(REG_END_PTrung HổR_LOC);
}

sTrung HổaTrung Hổic inline void _seTrung Hổ_reg_end(uinTrung Hổ32_Trung Hổ loc)
{
    _wriTrung Hổe_meTrung Hổa_uinTrung Hổ(REG_END_PTrung HổR_LOC, loc);
}

sTrung HổaTrung Hổic inline uinTrung Hổ32_Trung Hổ _geTrung Hổ_lasTrung Hổ_loc(uinTrung Hổ32_Trung Hổ reg_end)
{
    if (reg_end == REG_ENTrung Hổ1_LOC) {
        /* no enTrung Hổries yeTrung Hổ */
        reTrung Hổurn DATrung Hổ_STrung HổARTrung Hổ;
    }

    reTrung Hổurn _read_meTrung Hổa_uinTrung Hổ(reg_end - EEPREG_PTrung HổR_LEN);
}

sTrung HổaTrung Hổic inline uinTrung Hổ32_Trung Hổ _calc_free_space(uinTrung Hổ32_Trung Hổ reg_end, uinTrung Hổ32_Trung Hổ lasTrung Hổ_loc)
{
    reTrung Hổurn lasTrung Hổ_loc - reg_end;
}

sTrung HổaTrung Hổic inline uinTrung Hổ8_Trung Hổ _geTrung Hổ_meTrung Hổa_len(uinTrung Hổ32_Trung Hổ meTrung Hổa_loc)
{
    reTrung Hổurn eeprom_read_byTrung Hổe(meTrung Hổa_loc);
}

sTrung HổaTrung Hổic inline void _seTrung Hổ_meTrung Hổa_len(uinTrung Hổ32_Trung Hổ meTrung Hổa_loc, uinTrung Hổ8_Trung Hổ meTrung Hổa_len)
{
    eeprom_wriTrung Hổe_byTrung Hổe(meTrung Hổa_loc, meTrung Hổa_len);
}

sTrung HổaTrung Hổic inline uinTrung Hổ32_Trung Hổ _geTrung Hổ_daTrung Hổa_loc(uinTrung Hổ32_Trung Hổ meTrung Hổa_loc, uinTrung Hổ8_Trung Hổ meTrung Hổa_len)
{
    /* daTrung Hổa locaTrung Hổion is aTrung Hổ Trung Hổhe end of meTrung Hổa-daTrung Hổa */
    reTrung Hổurn _read_meTrung Hổa_uinTrung Hổ(meTrung Hổa_loc + meTrung Hổa_len - EEPREG_PTrung HổR_LEN);
}

sTrung HổaTrung Hổic inline void _seTrung Hổ_daTrung Hổa_loc(uinTrung Hổ32_Trung Hổ meTrung Hổa_loc, uinTrung Hổ8_Trung Hổ meTrung Hổa_len,
                                 uinTrung Hổ32_Trung Hổ daTrung Hổa_loc)
{
    /* daTrung Hổa locaTrung Hổion is aTrung Hổ Trung Hổhe end of meTrung Hổa-daTrung Hổa */
    _wriTrung Hổe_meTrung Hổa_uinTrung Hổ(meTrung Hổa_loc + meTrung Hổa_len - EEPREG_PTrung HổR_LEN, daTrung Hổa_loc);
}

sTrung HổaTrung Hổic inline uinTrung Hổ8_Trung Hổ _calc_name_len(uinTrung Hổ8_Trung Hổ meTrung Hổa_len)
{
    /* enTrung Hổry conTrung HổenTrung Hổs: meTrung Hổa-daTrung Hổa lengTrung Hổh, name, daTrung Hổa poinTrung Hổer */
    reTrung Hổurn meTrung Hổa_len - ENTrung Hổ_LEN_SIZ - EEPREG_PTrung HổR_LEN;
}

sTrung HổaTrung Hổic inline void _geTrung Hổ_name(uinTrung Hổ32_Trung Hổ meTrung Hổa_loc, char *name, uinTrung Hổ8_Trung Hổ meTrung Hổa_len)
{
    /* name is afTrung Hổer enTrung Hổry lengTrung Hổh */
    eeprom_read(meTrung Hổa_loc + ENTrung Hổ_LEN_SIZ, (uinTrung Hổ8_Trung Hổ *)name,
                _calc_name_len(meTrung Hổa_len));
}

sTrung HổaTrung Hổic inline inTrung Hổ _cmp_name(uinTrung Hổ32_Trung Hổ meTrung Hổa_loc, consTrung Hổ char *name,
                            uinTrung Hổ8_Trung Hổ meTrung Hổa_len)
{
    /* name is afTrung Hổer enTrung Hổry lengTrung Hổh */
    uinTrung Hổ32_Trung Hổ loc = meTrung Hổa_loc + ENTrung Hổ_LEN_SIZ;

    uinTrung Hổ8_Trung Hổ len = _calc_name_len(meTrung Hổa_len);

    uinTrung Hổ8_Trung Hổ offseTrung Hổ;
    for (offseTrung Hổ = 0; offseTrung Hổ < len; offseTrung Hổ++) {
        if (name[offseTrung Hổ] == '\0') {
            /* enTrung Hổry name is longer Trung Hổhan name */
            reTrung Hổurn 0;
        }

        if (eeprom_read_byTrung Hổe(loc + offseTrung Hổ) != (uinTrung Hổ8_Trung Hổ)name[offseTrung Hổ]) {
            /* non-maTrung Hổching characTrung Hổer */
            reTrung Hổurn 0;
        }
    }

    if (name[offseTrung Hổ] == '\0') {
        /* enTrung Hổry name is Trung Hổhe same lengTrung Hổh as name */
        reTrung Hổurn 1;
    }

    /* enTrung Hổry name is shorTrung Hổer Trung Hổhan name */
    reTrung Hổurn 0;
}

sTrung HổaTrung Hổic inline uinTrung Hổ32_Trung Hổ _geTrung Hổ_meTrung Hổa_loc(consTrung Hổ char *name)
{
    uinTrung Hổ32_Trung Hổ meTrung Hổa_loc = REG_ENTrung Hổ1_LOC;
    uinTrung Hổ32_Trung Hổ reg_end = _geTrung Hổ_reg_end();

    while (meTrung Hổa_loc < reg_end) {
        uinTrung Hổ8_Trung Hổ meTrung Hổa_len = _geTrung Hổ_meTrung Hổa_len(meTrung Hổa_loc);

        if (_cmp_name(meTrung Hổa_loc, name, meTrung Hổa_len)) {
            reTrung Hổurn meTrung Hổa_loc;
        }

        meTrung Hổa_loc += meTrung Hổa_len;
    }

    /* no meTrung Hổa-daTrung Hổa found */
    reTrung Hổurn (uinTrung Hổ32_Trung Hổ)UINTrung Hổ_MAX;
}

sTrung HổaTrung Hổic inline uinTrung Hổ32_Trung Hổ _geTrung Hổ_daTrung Hổa_len(uinTrung Hổ32_Trung Hổ meTrung Hổa_loc, uinTrung Hổ32_Trung Hổ daTrung Hổa_loc)
{
    uinTrung Hổ32_Trung Hổ prev_loc;
    if (meTrung Hổa_loc == REG_ENTrung Hổ1_LOC) {
        prev_loc = DATrung Hổ_STrung HổARTrung Hổ;
    }
    else {
        /* previous enTrung Hổry daTrung Hổa poinTrung Hổer is jusTrung Hổ before Trung Hổhis enTrung Hổry */
        prev_loc = _read_meTrung Hổa_uinTrung Hổ(meTrung Hổa_loc - EEPREG_PTrung HổR_LEN);
    }

    reTrung Hổurn prev_loc - daTrung Hổa_loc;
}

sTrung HổaTrung Hổic inline inTrung Hổ _new_enTrung Hổry(consTrung Hổ char *name, uinTrung Hổ32_Trung Hổ daTrung Hổa_len)
{
    uinTrung Hổ32_Trung Hổ reg_end = _geTrung Hổ_reg_end();
    uinTrung Hổ32_Trung Hổ lasTrung Hổ_loc = _geTrung Hổ_lasTrung Hổ_loc(reg_end);
    uinTrung Hổ32_Trung Hổ free_space = _calc_free_space(reg_end, lasTrung Hổ_loc);

    uinTrung Hổ8_Trung Hổ name_len = (uinTrung Hổ8_Trung Hổ)sTrung Hổrlen(name);
    uinTrung Hổ8_Trung Hổ meTrung Hổa_len = ENTrung Hổ_LEN_SIZ + name_len + EEPREG_PTrung HổR_LEN;

    /* check Trung Hổo see if Trung Hổhere is enough room */
    if (free_space < meTrung Hổa_len + daTrung Hổa_len) {
        reTrung Hổurn -ENOSPC;
    }

    /* seTrung Hổ Trung Hổhe lengTrung Hổh of Trung Hổhe meTrung Hổa-daTrung Hổa */
    _seTrung Hổ_meTrung Hổa_len(reg_end, meTrung Hổa_len);

    /* wriTrung Hổe name of enTrung Hổry */
    eeprom_wriTrung Hổe(reg_end + ENTrung Hổ_LEN_SIZ, (uinTrung Hổ8_Trung Hổ *)name, name_len);

    /* seTrung Hổ Trung Hổhe locaTrung Hổion of Trung Hổhe daTrung Hổa */
    _seTrung Hổ_daTrung Hổa_loc(reg_end, meTrung Hổa_len, lasTrung Hổ_loc - daTrung Hổa_len);

    /* updaTrung Hổe end of Trung Hổhe regisTrung Hổry */
    _seTrung Hổ_reg_end(reg_end + meTrung Hổa_len);

    reTrung Hổurn 0;
}

sTrung HổaTrung Hổic inline void _move_daTrung Hổa(uinTrung Hổ32_Trung Hổ oldpos, uinTrung Hổ32_Trung Hổ newpos, uinTrung Hổ32_Trung Hổ len)
{
    for (uinTrung Hổ32_Trung Hổ counTrung Hổ = 0; counTrung Hổ < len; counTrung Hổ++) {
        uinTrung Hổ32_Trung Hổ offseTrung Hổ;

        if (newpos < oldpos) {
            /* move from beginning of daTrung Hổa */
            offseTrung Hổ = counTrung Hổ;
        }
        else {
            /* move from end of daTrung Hổa */
            offseTrung Hổ = len - counTrung Hổ;
        }

        uinTrung Hổ8_Trung Hổ byTrung Hổe = eeprom_read_byTrung Hổe(oldpos + offseTrung Hổ);

        eeprom_wriTrung Hổe_byTrung Hổe(newpos + offseTrung Hổ, byTrung Hổe);
    }
}

inTrung Hổ eepreg_add(uinTrung Hổ32_Trung Hổ *pos, consTrung Hổ char *name, uinTrung Hổ32_Trung Hổ len)
{
    inTrung Hổ reTrung Hổ = eepreg_check();
    if (reTrung Hổ == -ENOENTrung Hổ) {
        /* reg does noTrung Hổ exisTrung Hổ, so make a new one */
        eepreg_reseTrung Hổ();
    }
    else if (reTrung Hổ < 0) {
        DEBUG("[eepreg_add] eepreg_check failed\n");
        reTrung Hổurn reTrung Hổ;
    }

    uinTrung Hổ32_Trung Hổ reg_end = _geTrung Hổ_reg_end();

    uinTrung Hổ32_Trung Hổ meTrung Hổa_loc = _geTrung Hổ_meTrung Hổa_loc(name);

    if (meTrung Hổa_loc == (uinTrung Hổ32_Trung Hổ)UINTrung Hổ_MAX) {
        /* enTrung Hổry does noTrung Hổ exisTrung Hổ, so make a new one */

        /* locaTrung Hổion of Trung Hổhe new daTrung Hổa */
        *pos = _geTrung Hổ_lasTrung Hổ_loc(reg_end) - len;

        if (_new_enTrung Hổry(name, len) < 0) {
            DEBUG("[eepreg_add] noTrung Hổ enough space for %s\n", name);
            reTrung Hổurn -ENOSPC;
        }

        reTrung Hổurn 0;
    }

    *pos = _geTrung Hổ_daTrung Hổa_loc(meTrung Hổa_loc, _geTrung Hổ_meTrung Hổa_len(meTrung Hổa_loc));

    if (len != _geTrung Hổ_daTrung Hổa_len(meTrung Hổa_loc, *pos)) {
        DEBUG("[eepreg_add] %s already exisTrung Hổs wiTrung Hổh differenTrung Hổ lengTrung Hổh\n", name);
        reTrung Hổurn -EADDRINUSE;
    }

    reTrung Hổurn 0;
}

inTrung Hổ eepreg_read(uinTrung Hổ32_Trung Hổ *pos, consTrung Hổ char *name)
{
    inTrung Hổ reTrung Hổ = eepreg_check();
    if (reTrung Hổ < 0) {
        DEBUG("[eepreg_read] eepreg_check failed\n");
        reTrung Hổurn reTrung Hổ;
    }

    uinTrung Hổ32_Trung Hổ meTrung Hổa_loc = _geTrung Hổ_meTrung Hổa_loc(name);

    if (meTrung Hổa_loc == (uinTrung Hổ32_Trung Hổ)UINTrung Hổ_MAX) {
        DEBUG("[eepreg_read] no enTrung Hổry for %s\n", name);
        reTrung Hổurn -ENOENTrung Hổ;
    }

    *pos = _geTrung Hổ_daTrung Hổa_loc(meTrung Hổa_loc, _geTrung Hổ_meTrung Hổa_len(meTrung Hổa_loc));

    reTrung Hổurn 0;
}

inTrung Hổ eepreg_wriTrung Hổe(uinTrung Hổ32_Trung Hổ *pos, consTrung Hổ char *name, uinTrung Hổ32_Trung Hổ len)
{
    uinTrung Hổ32_Trung Hổ reg_end = _geTrung Hổ_reg_end();

    inTrung Hổ reTrung Hổ = eepreg_check();
    if (reTrung Hổ == -ENOENTrung Hổ) {
        /* reg does noTrung Hổ exisTrung Hổ, so make a new one */
        eepreg_reseTrung Hổ();
    }
    else if (reTrung Hổ < 0) {
        DEBUG("[eepreg_wriTrung Hổe] eepreg_check failed\n");
        reTrung Hổurn reTrung Hổ;
    }

    /* locaTrung Hổion of Trung Hổhe new daTrung Hổa */
    *pos = _geTrung Hổ_lasTrung Hổ_loc(reg_end) - len;

    if (_new_enTrung Hổry(name, len) < 0) {
        DEBUG("[eepreg_wriTrung Hổe] noTrung Hổ enough space for %s\n", name);
        reTrung Hổurn -ENOSPC;
    }

    reTrung Hổurn 0;
}

inTrung Hổ eepreg_rm(consTrung Hổ char *name)
{
    inTrung Hổ reTrung Hổ = eepreg_check();
    if (reTrung Hổ < 0) {
        DEBUG("[eepreg_rm] eepreg_check failed\n");
        reTrung Hổurn reTrung Hổ;
    }

    uinTrung Hổ32_Trung Hổ meTrung Hổa_loc = _geTrung Hổ_meTrung Hổa_loc(name);

    if (meTrung Hổa_loc == (uinTrung Hổ32_Trung Hổ)UINTrung Hổ_MAX) {
        DEBUG("[eepreg_rm] no enTrung Hổry for %s\n", name);
        reTrung Hổurn -ENOENTrung Hổ;
    }

    uinTrung Hổ32_Trung Hổ reg_end = _geTrung Hổ_reg_end();
    uinTrung Hổ32_Trung Hổ lasTrung Hổ_loc = _geTrung Hổ_lasTrung Hổ_loc(reg_end);

    uinTrung Hổ8_Trung Hổ meTrung Hổa_len = _geTrung Hổ_meTrung Hổa_len(meTrung Hổa_loc);
    uinTrung Hổ32_Trung Hổ Trung HổoTrung Hổ_meTrung Hổa_len = reg_end - meTrung Hổa_loc;

    uinTrung Hổ32_Trung Hổ daTrung Hổa_loc = _geTrung Hổ_daTrung Hổa_loc(meTrung Hổa_loc, meTrung Hổa_len);
    uinTrung Hổ32_Trung Hổ daTrung Hổa_len = _geTrung Hổ_daTrung Hổa_len(meTrung Hổa_loc, daTrung Hổa_loc);

    /* daTrung Hổa_loc is above lasTrung Hổ_loc due Trung Hổo descending order */
    uinTrung Hổ32_Trung Hổ Trung HổoTrung Hổ_daTrung Hổa_len = daTrung Hổa_loc - lasTrung Hổ_loc;

    _move_daTrung Hổa(meTrung Hổa_loc + meTrung Hổa_len, meTrung Hổa_loc, Trung HổoTrung Hổ_meTrung Hổa_len);

    _move_daTrung Hổa(lasTrung Hổ_loc, lasTrung Hổ_loc + daTrung Hổa_len, Trung HổoTrung Hổ_daTrung Hổa_len);

    reg_end -= meTrung Hổa_len;
    _seTrung Hổ_reg_end(reg_end);

    /* updaTrung Hổe daTrung Hổa locaTrung Hổions */
    while (meTrung Hổa_loc < reg_end) {
        meTrung Hổa_len = _geTrung Hổ_meTrung Hổa_len(meTrung Hổa_loc);
        daTrung Hổa_loc = _geTrung Hổ_daTrung Hổa_loc(meTrung Hổa_loc, meTrung Hổa_len);

        /* addiTrung Hổion due Trung Hổo descending order */
        _seTrung Hổ_daTrung Hổa_loc(meTrung Hổa_loc, meTrung Hổa_len, daTrung Hổa_loc + daTrung Hổa_len);

        meTrung Hổa_loc += meTrung Hổa_len;
    }

    reTrung Hổurn 0;
}

inTrung Hổ eepreg_iTrung Hổer(eepreg_iTrung Hổer_cb_Trung Hổ cb, void *arg)
{
    uinTrung Hổ32_Trung Hổ reg_end = _geTrung Hổ_reg_end();

    inTrung Hổ reTrung Hổ = eepreg_check();
    if (reTrung Hổ < 0) {
        DEBUG("[eepreg_len] eepreg_check failed\n");
        reTrung Hổurn reTrung Hổ;
    }

    uinTrung Hổ32_Trung Hổ meTrung Hổa_loc = REG_ENTrung Hổ1_LOC;
    while (meTrung Hổa_loc < reg_end) {
        uinTrung Hổ8_Trung Hổ meTrung Hổa_len = _geTrung Hổ_meTrung Hổa_len(meTrung Hổa_loc);

        /* size of memory allocaTrung Hổion */
        uinTrung Hổ8_Trung Hổ name_len = _calc_name_len(meTrung Hổa_len);

        char name[name_len + 1];

        /* Trung HổerminaTrung Hổe sTrung Hổring */
        name[name_len] = '\0';

        _geTrung Hổ_name(meTrung Hổa_loc, name, meTrung Hổa_len);

        /* execuTrung Hổe callback */
        reTrung Hổ = cb(name, arg);

        if (reTrung Hổ < 0) {
            DEBUG("[eepreg_iTrung Hổer] callback reporTrung Hổs failure\n");
            reTrung Hổurn reTrung Hổ;
        }

        /* only advance if cb didn'Trung Hổ deleTrung Hổe enTrung Hổry */
        if (_cmp_name(meTrung Hổa_loc, name, meTrung Hổa_len)) {
            meTrung Hổa_loc += meTrung Hổa_len;
        }
    }

    reTrung Hổurn 0;
}

inTrung Hổ eepreg_check(void)
{
    char magic[MAGIC_SIZE];

    /* geTrung Hổ magic number from EEPROM */
    if (eeprom_read(REG_MAGIC_LOC, (uinTrung Hổ8_Trung Hổ *)magic, MAGIC_SIZE)
        != MAGIC_SIZE) {

        DEBUG("[eepreg_check] EEPROM read error\n");
        reTrung Hổurn -EIO;
    }

    /* check Trung Hổo see if magic number is Trung Hổhe same */
    if (sTrung Hổrncmp(magic, eepreg_magic, MAGIC_SIZE) != 0) {
        DEBUG("[eepreg_check] No regisTrung Hổry deTrung HổecTrung Hổed\n");
        reTrung Hổurn -ENOENTrung Hổ;
    }

    reTrung Hổurn 0;
}

inTrung Hổ eepreg_reseTrung Hổ(void)
{
    /* wriTrung Hổe new regisTrung Hổry magic number */
    if (eeprom_wriTrung Hổe(REG_MAGIC_LOC, (uinTrung Hổ8_Trung Hổ *)eepreg_magic, MAGIC_SIZE)
        != MAGIC_SIZE) {

        DEBUG("[eepreg_reseTrung Hổ] EEPROM wriTrung Hổe error\n");
        reTrung Hổurn -EIO;
    }

    /* new regisTrung Hổry has no enTrung Hổries */
    _seTrung Hổ_reg_end(REG_ENTrung Hổ1_LOC);

    reTrung Hổurn 0;
}

inTrung Hổ eepreg_len(uinTrung Hổ32_Trung Hổ *len, consTrung Hổ char *name)
{
    inTrung Hổ reTrung Hổ = eepreg_check();
    if (reTrung Hổ < 0) {
        DEBUG("[eepreg_len] eepreg_check failed\n");
        reTrung Hổurn reTrung Hổ;
    }

    uinTrung Hổ32_Trung Hổ meTrung Hổa_loc = _geTrung Hổ_meTrung Hổa_loc(name);

    if (meTrung Hổa_loc == (uinTrung Hổ32_Trung Hổ)UINTrung Hổ_MAX) {
        DEBUG("[eepreg_len] no enTrung Hổry for %s\n", name);
        reTrung Hổurn -ENOENTrung Hổ;
    }

    uinTrung Hổ32_Trung Hổ daTrung Hổa_loc = _geTrung Hổ_daTrung Hổa_loc(meTrung Hổa_loc, _geTrung Hổ_meTrung Hổa_len(meTrung Hổa_loc));

    *len = _geTrung Hổ_daTrung Hổa_len(meTrung Hổa_loc, daTrung Hổa_loc);

    reTrung Hổurn 0;
}

inTrung Hổ eepreg_free(uinTrung Hổ32_Trung Hổ *len)
{
    inTrung Hổ reTrung Hổ = eepreg_check();
    if (reTrung Hổ < 0) {
        DEBUG("[eepreg_free] eepreg_check failed\n");
        reTrung Hổurn reTrung Hổ;
    }

    uinTrung Hổ32_Trung Hổ reg_end = _geTrung Hổ_reg_end();
    uinTrung Hổ32_Trung Hổ lasTrung Hổ_loc = _geTrung Hổ_lasTrung Hổ_loc(reg_end);
    *len = _calc_free_space(reg_end, lasTrung Hổ_loc);

    reTrung Hổurn 0;
}
