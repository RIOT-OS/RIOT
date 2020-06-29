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

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.asymmetric import ed25519
from cryptography.hazmat.primitives.asymmetric import utils as asymmetric_utils
from cryptography.hazmat.primitives import serialization as ks

from suit_tool.manifest import COSE_Sign1, COSEList, SUITDigest,\
                               SUITEnvelope, SUITBytes, SUITBWrapField, \
                               COSETaggedAuth
import logging
import binascii
LOG = logging.getLogger(__name__)

def get_cose_es_bytes(options, private_key, sig_val):
    ASN1_signature = private_key.sign(sig_val, ec.ECDSA(hashes.SHA256()))
    r,s = asymmetric_utils.decode_dss_signature(ASN1_signature)
    ssize = private_key.key_size
    signature_bytes = r.to_bytes(ssize//8, byteorder='big') + s.to_bytes(ssize//8, byteorder='big')
    return signature_bytes

def get_cose_ed25519_bytes(options, private_key, sig_val):
    return private_key.sign(sig_val)

def get_hsslms_bytes(options, private_key, sig_val):
    sig = private_key.sign(sig_val)
    key_file_name = options.private_key.name
    options.private_key.close()
    with open(key_file_name, 'wb') as fd:
        fd.write(private_key.serialize())
    return sig

def main(options):
    # Read the manifest wrapper
    wrapper = cbor.loads(options.manifest.read())

    private_key = None
    digest = None
    private_key_buffer = options.private_key.read()
    try:
        private_key = ks.load_pem_private_key(private_key_buffer, password=None, backend=default_backend())
        if isinstance(private_key, ec.EllipticCurvePrivateKey):
            options.key_type = 'ES{}'.format(private_key.key_size)
        elif isinstance(private_key, ed25519.Ed25519PrivateKey):
            options.key_type = 'EdDSA'
        else:
            LOG.critical('Unrecognized key: {}'.format(type(private_key).__name__))
            return 1
        digest = {
            'ES256' : hashes.Hash(hashes.SHA256(), backend=default_backend()),
            'ES384' : hashes.Hash(hashes.SHA384(), backend=default_backend()),
            'ES512' : hashes.Hash(hashes.SHA512(), backend=default_backend()),
            'EdDSA' : hashes.Hash(hashes.SHA256(), backend=default_backend()),
        }.get(options.key_type)
    except:
        LOG.critical('Non-library key type not implemented')
        return 1

    digest.update(cbor.dumps(wrapper[SUITEnvelope.fields['manifest'].suit_key]))

    cose_signature = COSE_Sign1().from_json({
        'protected' : {
            'alg' : options.key_type
        },
        'unprotected' : {},
        'payload' : {
            'algorithm-id' : 'sha256',
            'digest-bytes' : digest.finalize()
        }
    })

    Sig_structure = cbor.dumps([
        "Signature1",
        cose_signature.protected.to_suit(),
        b'',
        cose_signature.payload.to_suit(),
    ], canonical = True)
    LOG.debug('Signing: {}'.format(binascii.b2a_hex(Sig_structure).decode('utf-8')))

    signature_bytes = {
        'ES256' : get_cose_es_bytes,
        'ES384' : get_cose_es_bytes,
        'ES512' : get_cose_es_bytes,
        'EdDSA' : get_cose_ed25519_bytes,
        'HSS-LMS' : get_hsslms_bytes,
    }.get(options.key_type)(options, private_key, Sig_structure)

    cose_signature.signature = SUITBytes().from_suit(signature_bytes)

    auth = SUITBWrapField(COSEList)().from_suit(wrapper[SUITEnvelope.fields['auth'].suit_key])
    auth.v.append(auth.v.field.obj().from_json({
        'COSE_Sign1_Tagged' : cose_signature.to_json()
    }))
    wrapper[SUITEnvelope.fields['auth'].suit_key] = auth.to_suit()

    options.output_file.write(cbor.dumps(wrapper, canonical=True))
    return 0
