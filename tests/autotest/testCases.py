#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

from defaultAutoTest import DefaultBuildStrategy
from defaultAutoTest import DefaultTestStrategy

tests = [
        ('bitarithm_timings', 'native', DefaultBuildStrategy, None),
        ('bloom_bytes', 'native', DefaultBuildStrategy, DefaultTestStrategy),
        ('coap', 'native', DefaultBuildStrategy, None),
        ('fmt_print', 'native', DefaultBuildStrategy, DefaultTestStrategy),
        ]
