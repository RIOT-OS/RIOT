# -*- coding: utf-8 -*-

# SPDX-FileCopyrightText: 2019 ARM Limited or its affiliates
# SPDX-License-Identifier: Apache-2.0

import cbor2 as cbor
import itertools
import textwrap
from collections import OrderedDict

from suit_tool.manifest import SUITEnvelope
def main(options):
    # Read the manifest wrapper
    envelope = cbor.loads(options.manifest.read())

    if hasattr(options, 'all'):
        options.elements = SUITEnvelope.severable_fields

    for e in options.elements:
        eid = SUITEnvelope.fields[e].suit_key
        if eid in envelope:
            del(envelope[eid])

    output = cbor.dumps(envelope, canonical=True)
    options.output_file.write(output)

    return 0
