# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------
# Copyright 2019 ARM Limited or its affiliates
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ----------------------------------------------------------------------------
from suit_tool.compile import compile_manifest
import json
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
