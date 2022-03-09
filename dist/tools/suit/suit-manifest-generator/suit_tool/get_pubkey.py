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
import binascii

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.asymmetric import ec, ed25519
from cryptography.hazmat.primitives.asymmetric import utils as asymmetric_utils
from cryptography.hazmat.primitives import serialization as ks


def to_uecc_pubkey(pk):
    if not isinstance(pk, ec.EllipticCurvePrivateKey):
        raise Exception('Private key of type {} is not supported'.format(type(pk)))
    public_numbers = pk.public_key().public_numbers()
    x = public_numbers.x
    y = public_numbers.y
    uecc_bytes = x.to_bytes(
        (x.bit_length() + 7) // 8, byteorder='big'
    ) + y.to_bytes(
        (y.bit_length() + 7) // 8, byteorder='big'
    )
    uecc_c_def = ['const uint8_t public_key[] = {'] + textwrap.wrap(
        ', '.join(['{:0=#4x}'.format(x) for x in uecc_bytes]),
        76
    )
    return '\n    '.join(uecc_c_def) + '\n};\n'


def to_header(pk):
    if isinstance(pk, ec.EllipticCurvePrivateKey):
        return to_uecc_pubkey(pk)
    if isinstance(pk, ed25519.Ed25519PrivateKey):
        public_bytes = pk.public_key().public_bytes(ks.Encoding.Raw,
                                                      ks.PublicFormat.Raw)
        public_c_def = ['const uint8_t public_key[] = {'] + textwrap.wrap(
            ', '.join(['{:0=#4x}'.format(x) for x in public_bytes]),
            76
        )
        return str.encode('\n    '.join(public_c_def) + '\n};\n')


OutputFormaters = {
    'uecc' : to_uecc_pubkey,
    'header': to_header,
    'pem' : lambda pk: pk.public_key().public_bytes(ks.Encoding.PEM, ks.PublicFormat.SubjectPublicKeyInfo),
    'der' : lambda pk: pk.public_key().public_bytes(ks.Encoding.DER, ks.PublicFormat.SubjectPublicKeyInfo),
    'hsslms' : lambda pk: pk.publicKey().serialize(),
    'c-hsslms' : lambda pk: ('\n    '.join(['const uint8_t hsslms_public_key[] = {'] + textwrap.wrap(
        ', '.join(['{:0=#4x}'.format(x) for x in pk.publicKey().serialize()]),
        76
    )) + '\n};\n').encode('utf-8')
}


def main(options):
    private_key = None
    # This test is here because the cryptography module doesn't know about hss-lms keys
    if options.output_format in ('pem', 'der', 'uecc', 'header'):
        private_key = ks.load_pem_private_key(
            options.private_key.read(),
            password=None,
            backend=default_backend()
        )

    odata = OutputFormaters.get(options.output_format)(private_key)

    try:
        odata = odata.decode('utf-8')
    except:
        odata = binascii.b2a_hex(odata).decode('utf-8')

    odata = '\n'.join(
        [line for lines in [textwrap.wrap(line, 80)
            for line in odata.split('\n')] for line in lines]
    ) + '\n'
    options.output_file.write(odata)

    return 0
