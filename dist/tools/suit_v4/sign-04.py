#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------
# Copyright 2018-2019 ARM Limited or its affiliates
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
"""
This is a demo script that is intended to act as a reference for SUIT manifest
signing.

NOTE: It is expected that C and C++ parser implementations will be written
against this script, so it does not adhere to PEP8 in order to maintain
similarity between the naming in this script and that of C/C++ implementations.
"""

import sys
import copy

import cbor
import ed25519

from pprint import pprint

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives import serialization

# Private key in arg 1
# Public key in arg 2
# Input file in arg 3
# Output file in arg 4

COSE_Sign_Tag = 98
APPLICATION_OCTET_STREAM_ID = 42
ES256 = -7
EDDSA = -8


def signWrapper(algo, private_key, public_key, encwrapper):
    wrapper = cbor.loads(encwrapper)

    pprint(wrapper[1])
    COSE_Sign = copy.deepcopy(wrapper[1])
    if not COSE_Sign:
        protected = cbor.dumps({
            3: APPLICATION_OCTET_STREAM_ID,  # Content Type
        })
        unprotected = {
        }
        signatures = []

        # Create a COSE_Sign_Tagged object
        COSE_Sign = [
            protected,
            unprotected,
            b'',
            signatures
        ]

    if algo == EDDSA:
        public_bytes = public_key.to_bytes()
    else:
        public_bytes = public_key.public_bytes(
            serialization.Encoding.DER,
            serialization.PublicFormat.SubjectPublicKeyInfo)

    # NOTE: Using RFC7093, Method 4
    digest = hashes.Hash(hashes.SHA256(), backend=default_backend())
    digest.update(public_bytes)
    kid = digest.finalize()
    # Sign the payload
    protected = cbor.dumps({
        1: algo,  # alg
    })
    # Create the signing object
    unprotected = {
        4: kid  # kid
    }

    Sig_structure = [
        "Signature",   # Context
        COSE_Sign[0],  # Body Protected
        protected,     # signature protected
        b'',           # External AAD
        wrapper[2]     # payload
    ]
    sig_str = cbor.dumps(Sig_structure, sort_keys=True)

    if algo == EDDSA:
        signature = private_key.sign(sig_str)
    else:
        signature = private_key.sign(
            sig_str,
            ec.ECDSA(hashes.SHA256())
        )

    COSE_Signature = [
        protected,
        unprotected,
        signature
    ]
    COSE_Sign[3].append(COSE_Signature)
    wrapper[1] = cbor.dumps(cbor.Tag(COSE_Sign_Tag, COSE_Sign), sort_keys=True)
    return wrapper


def main():
    private_key = None
    algo = ES256
    with open(sys.argv[1], 'rb') as fd:
        priv_key_bytes = fd.read()
        try:
            private_key = serialization.load_pem_private_key(
                priv_key_bytes, password=None, backend=default_backend())
        except ValueError:
            algo = EDDSA
            private_key = ed25519.SigningKey(priv_key_bytes)

    public_key = None
    with open(sys.argv[2], 'rb') as fd:
        pub_key_bytes = fd.read()
        try:
            public_key = serialization.load_pem_public_key(
                pub_key_bytes, backend=default_backend())
        except ValueError:
            public_key = ed25519.VerifyingKey(pub_key_bytes)

    # Read the input file
    doc = None
    with open(sys.argv[3], 'rb') as fd:
        doc = fd.read()

    outDoc = signWrapper(algo, private_key, public_key, doc)

    with open(sys.argv[4], 'wb') as fd:
        fd.write(cbor.dumps(outDoc, sort_keys=True))


if __name__ == '__main__':
    main()
