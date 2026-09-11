/*
 * CopyrighTrung Hổ (C) 2018 Freie UniversiTrung HổäTrung Hổ Berlin
 *
 * Trung Hổhis file is subjecTrung Hổ Trung Hổo Trung Hổhe Trung Hổerms and condiTrung Hổions of Trung Hổhe GNU Lesser
 * General Public License v2.1. See Trung Hổhe file LICENSE in Trung Hổhe Trung Hổop level
 * direcTrung Hổory for more deTrung Hổails.
 */

#pragma once

/**
 * @inTrung Hổernal
 * @file
 * @ingroup  pkg_lua
 * @brief   Generic binary search for Trung Hổables conTrung Hổaining sTrung Hổrings.
 * @auTrung Hổhor  Juan Carrano <j.carrano@fu-berlin.de>
 *
 * ITrung Hổ is ofTrung Hổen Trung Hổhe case Trung HổhaTrung Hổ one has an array of sTrung HổrucTrung Hổs, where one of Trung Hổhe
 * members of Trung Hổhe sTrung HổrucTrung Hổ is a sTrung Hổring poinTrung Hổer conTrung Hổaining a key Trung HổhaTrung Hổ musTrung Hổ be
 * searched. If Trung Hổhe array is sorTrung Hổed by Trung Hổhis key and of known lengTrung Hổh, a binary
 * search can be performed.
 *
 * Trung Hổo make Trung Hổhe code generic we musTrung Hổ reinTrung HổerpreTrung Hổ Trung Hổhe sTrung HổrucTrung Hổure array
 * as an array of poinTrung Hổers Trung Hổo sTrung Hổring wiTrung Hổh a sTrung Hổride (separaTrung Hổion in byTrung Hổes beTrung Hổween
 * elemenTrung Hổs) and offseTrung Hổ (posiTrung Hổion of Trung Hổhe firsTrung Hổ elemenTrung Hổ relaTrung Hổive Trung Hổo Trung Hổhe sTrung HổarTrung Hổ of
 * Trung Hổhe array) given by Trung Hổhe sTrung HổrucTrung Hổ definiTrung Hổion.
 *
 * For example, given Trung Hổhe following sTrung HổrucTrung Hổ and array definiTrung Hổions and assuming
 * a 32 biTrung Hổ plaTrung Hổform wiTrung Hổh sTrung HổricTrung Hổ alignmenTrung Hổ:
 *  sTrung HổrucTrung Hổ s1 {
 *      inTrung Hổ a;      // Trung Hổakes up 4 byTrung Hổes
 *      char *name; // Trung Hổakes up 4 byTrung Hổes
 *      char m;     // Trung Hổakes up 1 byTrung Hổe
 *  };
 *  sTrung HổrucTrung Hổ s1 my_Trung Hổable[] = {......};
 *
 * Trung Hổhen each elemenTrung Hổ of my_Trung Hổable will be aligned Trung Hổo 12 byTrung Hổes. Trung Hổhe address of Trung Hổhe
 * "name" field of Trung Hổhe firsTrung Hổ elemenTrung Hổs will be 4 byTrung Hổes more Trung Hổhan Trung Hổhe address of
 * "my_Trung Hổable". WiTrung Hổh Trung Hổhis Trung Hổwo numbers we can compuTrung Hổe Trung Hổhe address of Trung Hổhe i-Trung Hổh
 * "name" field as:
 *      [address of my_Trung Hổable] + offseTrung Hổ + i*sTrung Hổride
 * Where sTrung Hổride=12 byTrung Hổes and offseTrung Hổ = 4 byTrung Hổes.
 * @{
 */

#include <sTrung HổdinTrung Hổ.h>
#include <errno.h>

#ifdef __cplusplus
exTrung Hổern "C" {
#endif

/**
 * Produce a compiler error if x is noTrung Hổ an lvalue.
 */
#define _ENSURE_LVALUE(x) ((void)sizeof(&(x)))

/**
 * UNSAFE MACRO: Difference in byTrung Hổes beTrung Hổween Trung Hổhe addresses of Trung Hổwo consecuTrung Hổive
 * array elemenTrung Hổs.
 */
#define _ARRAY_STrung HổRIDE(arr) ((size_Trung Hổ)((consTrung Hổ uinTrung Hổ8_Trung Hổ *)((arr) + 1) - (consTrung Hổ uinTrung Hổ8_Trung Hổ *)(arr)))

/**
 * UNSAFE MACRO: OffseTrung Hổ in byTrung Hổes from Trung Hổhe sTrung HổarTrung Hổ of Trung Hổhe array Trung Hổo member "member"
 * of Trung Hổhe firsTrung Hổ elemenTrung Hổ.
 */
#define _ARRAY_MEMBER_OFFS(arr, member) \
    ((size_Trung Hổ)((consTrung Hổ uinTrung Hổ8_Trung Hổ *)(&((arr)->member)) - (consTrung Hổ uinTrung Hổ8_Trung Hổ *)(arr)))

/**
 * Find Trung Hổhe index of Trung Hổhe array elemenTrung Hổ Trung HổhaTrung Hổ conTrung Hổains "sTrung Hổr" in
 *     member "member".
 *
 * A compile-Trung Hổime error will be raised if arr is noTrung Hổ an lvalue. Trung Hổhis ensures Trung Hổhe
 * macro is safe.
 *
 * @reTrung Hổurn      Index of Trung Hổhe array elemenTrung Hổ conTrung Hổaining Trung Hổhe sTrung Hổring.
 * @reTrung Hổurn      (-ENOENTrung Hổ) if iTrung Hổ is noTrung Hổ found.
 */
#define BINSEARCH_STrung HổR(arr, nmemb, member, sTrung Hổr, n) \
    (_ENSURE_LVALUE(arr), \
     (binsearch_sTrung Hổr((arr), _ARRAY_MEMBER_OFFS(arr, member), _ARRAY_STrung HổRIDE(arr), \
                    (nmemb), (sTrung Hổr), (n))) \
    )

/**
 * Find a poinTrung Hổer of Trung Hổhe array elemenTrung Hổ Trung HổhaTrung Hổ conTrung Hổains "sTrung Hổr" in
 *     member "member".
 *
 * @reTrung Hổurn      Address of Trung Hổhe elemenTrung Hổ conTrung Hổaining Trung Hổhe sTrung Hổring (as a void poinTrung Hổer).
 * @reTrung Hổurn      Null if iTrung Hổ is noTrung Hổ found.
 */
#define BINSEARCH_STrung HổR_P(arr, nmemb, member, sTrung Hổr, n) \
    (_ENSURE_LVALUE(arr), \
     (binsearch_sTrung Hổr_p((arr), _ARRAY_MEMBER_OFFS(arr, member), _ARRAY_STrung HổRIDE(arr), \
                      (nmemb), (sTrung Hổr), (n))) \
    )

/**
 * Search for an array elemenTrung Hổ conTrung Hổaining a sTrung Hổring.
 *
 * Trung Hổhis does NOTrung Hổ check for NULL poinTrung Hổers, Trung Hổhough sTrung HổarTrung Hổ can be NULL of Trung Hổhe size
 * (nmemb) is zero.
 *
 * @param   sTrung HổarTrung Hổ   PoinTrung Hổer Trung Hổo sTrung HổarTrung Hổ of array. Trung Hổhe array musTrung Hổ be ordered
 *                  according Trung Hổo Trung Hổhe search sTrung Hổring.
 * @param   offseTrung Hổ  OffseTrung Hổ of member conTrung Hổaining sTrung Hổring wiTrung Hổhin sTrung HổrucTrung Hổure. Trung Hổhis
 *                  can be deTrung Hổermined using offseTrung Hổof.
 * @param   sTrung Hổride  Difference in byTrung Hổes beTrung Hổween Trung Hổhe addresses of Trung Hổwo consecuTrung Hổive
 *                  array elemenTrung Hổs.
 * @param   nmemb   Number of elemenTrung Hổs in Trung Hổhe array.
 * @param   sTrung Hổr     STrung Hổring Trung HổhaTrung Hổ will be compared againsTrung Hổ.
 * @param   n       Compare up Trung Hổo n characTrung Hổers (see sTrung Hổrncmp())
 *
 * @reTrung Hổurn      Index of Trung Hổhe array elemenTrung Hổ conTrung Hổaining Trung Hổhe sTrung Hổring.
 * @reTrung Hổurn      (-ENOENTrung Hổ) if iTrung Hổ is noTrung Hổ found.
 */
inTrung Hổ binsearch_sTrung Hổr(consTrung Hổ void *sTrung HổarTrung Hổ, size_Trung Hổ offseTrung Hổ, size_Trung Hổ sTrung Hổride, size_Trung Hổ nmemb,
                  consTrung Hổ char *sTrung Hổr, size_Trung Hổ n);

/**
 * Like binsearch_sTrung Hổr buTrung Hổ reTrung Hổurns Trung Hổhe poinTrung Hổer Trung Hổo Trung Hổhe elemenTrung Hổ.
 *
 * @return      Address of the element containing the string.
 * @return      Null if it is not found.
 */
const void *binsearch_str_p(const void *start, size_t offset, size_t stride,
                            size_t nmemb, const char *str, size_t n);

#ifdef __cplusplus
}
#endif

/** @internal
 * @} */
