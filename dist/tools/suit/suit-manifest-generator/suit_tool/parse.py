#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# SPDX-FileCopyrightText: 2019 ARM Limited or its affiliates
# SPDX-License-Identifier: Apache-2.0

import itertools
import textwrap

from suit_tool.manifest import SUITEnvelope

def main(options):
    # Read the manifest wrapper
    decoded_cbor_wrapper = cbor.loads(options.manifest.read())
    # print(decoded_cbor_wrapper)
    wrapper = SUITEnvelope().from_suit(decoded_cbor_wrapper)
    if options.json:
        print (json.dumps(wrapper.to_json(),indent=2))
    else:
        print ('\n'.join(itertools.chain.from_iterable(
            [textwrap.wrap(t, 70) for t in wrapper.to_debug('').split('\n')]
        )))
    return 0
