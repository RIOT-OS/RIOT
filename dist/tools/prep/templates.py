#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# Copyright (C) 2015  Hauke Petersen <dev@haukepetersen.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from datetime import date

def copyright( cr ):
    return "Copyright (C) %i %s" % (date.today().year, cr)

mainheader = """/*
 * %s
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    %s_%s %s
 * @ingroup     %s
 * @brief       TODO: describe the module briefly
 *
 * TODO: give an elaborate description here
 *
 * @{
 *
 * @file
 * @brief       TODO: describe whats in this header
 *
 * @author      %s <%s>
 */

#ifndef %s_H
#define %s_H

/* TODO. add your includes here */

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: add your types, vars and function defs here */

#ifdef __cplusplus
}
#endif

#endif /* %s_H */
/** @} */
"""

subheader = """/*
 * %s
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     %s_%s
 * @{
 *
 * @file
 * @brief       TODO: describe whats in this header
 *
 * @author      %s <%s>
 */

#ifndef %s_H
#define %s_H

/* TODO. add your includes here */

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: add your types, vars and function defs here */

#ifdef __cplusplus
}
#endif

#endif /* %s_H */
/** @} */
"""

cfile = """/*
 * %s
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     %s_%s
 * @{
 *
 * @file
 * @brief       TODO: describe whats in this source file
 *
 * @author      %s <%s>
 *
 * @}
 */

#include "%s.h"
/* TODO: more includes here */

/* TODO: your code comes here */
"""

dep_entry = """
ifneq (,$(filter %s,$(USEMODULE)))
  %s
endif
"""

inc_entry = """ifneq (,$(filter %s,$(USEMODULE)))
    USEMODULE_INCLUDES += $(RIOTBASE)/%s
endif

"""
