# -*- coding: utf-8 -*-
# vim:fenc=utf-8

# SPDX-FileCopyrightText: 2020 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

def split_prefix(route):
    comp = route.split("/")
    return comp[0], int(comp[1])
