#!/usr/bin/python3
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
import cbor2 as cbor
import json
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
