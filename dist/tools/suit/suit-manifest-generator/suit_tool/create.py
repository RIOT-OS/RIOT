# -*- coding: utf-8 -*-

# SPDX-FileCopyrightText: 2019 ARM Limited or its affiliates
# SPDX-License-Identifier: Apache-2.0

from suit_tool.compile import compile_manifest
import json
import cbor2 as cbor
import itertools
import textwrap
from collections import OrderedDict

def main(options):
    m = json.loads(options.input_file.read(), object_pairs_hook=OrderedDict)

    nm = compile_manifest(options, m)
    if m.get('severable') or (hasattr(options, 'severable') and options.severable):
        nm = nm.to_severable('sha256')
    output = {
        'suit' : lambda x: cbor.dumps(x.to_suit(), canonical=True),
        'suit-debug' : lambda x: '\n'.join(itertools.chain.from_iterable(
            map(textwrap.wrap, x.to_debug('').split('\n'))
        )).encode('utf-8'),
        'json' : lambda x : json.dumps(x.to_json(), indent=2).encode('utf-8')
    }.get(options.format)(nm)
    options.output_file.write(output)

    return 0
