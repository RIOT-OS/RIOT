/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup    sys_clif
 *
 * @{
 *
 * @file
 * @brief     Internal definitions for CoRE Link format module
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#define _P_SIZE(p)     (sizeof(p) - 1)

/**
 * @brief link format path initial character
 */
#define LF_PATH_BEGIN_C     '<'

/**
 * @brief link format path final character
 */
#define LF_PATH_END_C       '>'

/**
 * @brief link format link separator character
 */
#define LF_LINK_SEPARATOR_C ','

/**
 * @brief link format attribute separator character
 *
 */
#define LF_ATTR_SEPARATOR_C ';'

/**
 * @brief link format attribute value separator character
 */
#define LF_ATTR_VAL_SEPARATOR_C '='

/**
 * @name Anchor attribute
 * @{
 */
#define LF_ATTR_ANCHOR   "anchor"                /**< attr name */
#define LF_ATTR_ANCHOR_S _P_SIZE(LF_ATTR_ANCHOR) /**< attr name length */
/** @} */

/**
 * @name Relation type attribute
 * @{
 */
#define LF_ATTR_REL_TYPE   "rel"                     /**< attr name */
#define LF_ATTR_REL_TYPE_S _P_SIZE(LF_ATTR_REL_TYPE) /**< attr name length */
/** @} */

/**
 * @name Language attribute
 * @{
 */
#define LF_ATTR_LANG   "hreflang"            /**< attr name */
#define LF_ATTR_LANG_S _P_SIZE(LF_ATTR_LANG) /**< attr name length */
/** @} */

/**
 * @name Media attribute
 * @{
 */
#define LF_ATTR_MEDIA   "media"                /**< attr name */
#define LF_ATTR_MEDIA_S _P_SIZE(LF_ATTR_MEDIA) /**< attr name length */
/** @} */

/**
 * @name Title attribute
 * @{
 */
#define LF_ATTR_TITLE   "title"                /**< attr name */
#define LF_ATTR_TITLE_S _P_SIZE(LF_ATTR_TITLE) /**< attr name length */
/** @} */

/**
 * @name Title extended attribute
 * @{
 */
#define LF_ATTR_TITLE_EXT   "title*"                   /**< attr name */
#define LF_ATTR_TITLE_EXT_S _P_SIZE(LF_ATTR_TITLE_EXT) /**< attr name length */
/** @} */

/**
 * @name Type attribute
 * @{
 */
#define LF_ATTR_TYPE   "type"                /**< attr name */
#define LF_ATTR_TYPE_S _P_SIZE(LF_ATTR_TYPE) /**< attr name length */
/** @} */

/**
 * @name Resource type attribute
 * @{
 */
#define LF_ATTR_RES_TYPE   "rt"                      /**< attr name */
#define LF_ATTR_RES_TYPE_S _P_SIZE(LF_ATTR_RES_TYPE) /**< attr name length */
/** @} */

/**
 * @name Interface description attribute
 * @{
 */
#define LF_ATTR_IF_DESC   "if"                     /**< attr name */
#define LF_ATTR_IF_DESC_S _P_SIZE(LF_ATTR_IF_DESC) /**< attr name length */
/** @} */

/**
 * @name Size attribute
 * @{
 */
#define LF_ATTR_SIZE   "sz"                  /**< attr name */
#define LF_ATTR_SIZE_S _P_SIZE(LF_ATTR_SIZE) /**< attr name length */
/** @} */

/**
 * @name Content-format attribute
 * @{
 */
#define LF_ATTR_CT   "ct"                /**< attr name */
#define LF_ATTR_CT_S _P_SIZE(LF_ATTR_CT) /**< attr name length */
/** @} */

/**
 * @name Observable attribute
 * @{
 */
#define LF_ATTR_OBS   "obs"                /**< attr name */
#define LF_ATTR_OBS_S _P_SIZE(LF_ATTR_OBS) /**< attr name length */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
