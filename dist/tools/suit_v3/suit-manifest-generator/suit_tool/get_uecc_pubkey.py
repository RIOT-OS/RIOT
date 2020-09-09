#!/usr/bin/python3
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------
# Copyright 2020 ARM Limited or its affiliates
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
import textwrap

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization as ks

def main(options):
    private_key = ks.load_pem_private_key(
        options.private_key.read(),
        password=None,
        backend=default_backend()
    )
    #public_numbers = private_key.public_key().public_numbers()
    #x = public_numbers.x
    #y = public_numbers.y
    #uecc_bytes = x.to_bytes(
    #    (x.bit_length() + 7) // 8, byteorder='big'
    #) + y.to_bytes(
    #    (y.bit_length() + 7) // 8, byteorder='big'
    #)
    #uecc_c_def = ['const uint8_t public_key[] = {'] + textwrap.wrap(
    #    ', '.join(['{:0=#4x}'.format(x) for x in uecc_bytes]),
    #    76
    #)
    public_bytes = private_key.public_key().public_bytes(
        encoding=ks.Encoding.Raw,
        format=ks.PublicFormat.Raw
    )

    c_def = ['const uint8_t public_key[] = {'] + textwrap.wrap(
        ', '.join(['{:0=#4x}'.format(x) for x in public_bytes]),
        76
    )
    print('\n    '.join(c_def) + '\n};')
    return 0
