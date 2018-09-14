#!/usr/bin/env python3

import argparse
import base64
import binascii
import calendar
import cbor
import hashlib
import os
import os.path
import time
import uuid

import ed25519
from pyasn1.type import univ
from pyasn1.type.namedtype import NamedType, NamedTypes
from pyasn1.codec.der.decoder import decode as der_decoder

TEXT_MANIFEST_DESC = 1
TEXT_PAYLOAD_DESC = 2
TEXT_VENDOR = 3
TEXT_MODEL = 4

CONDITION_VENDORID = 1
CONDITION_CLASSID = 2
CONDITION_DEVICEID = 3
CONDITION_BESTBEFORE = 4

DIRECTIVE_APPLY_IMMEDIATE = 1
DIRECTIVE_APPLY_AFTER = 2

PAYLOADFORMAT_BIN = 1
PAYLOADFORMAT_HEX = 2

DIGEST_TYPE_SHA256 = 1
DIGEST_TYPE_SHA384 = 2
DIGEST_TYPE_SHA512 = 3

PAYLOAD_DIGEST_RAW = 1
PAYLOAD_DIGEST_INSTALLED = 2
PAYLOAD_DIGEST_CIPHER = 3
PAYLOAD_DIGEST_PREIMAGE = 4

ASN1_ED25519_TYPE = '1.3.101.112'


class Identifier(univ.Sequence):
    componentType = NamedTypes(
        NamedType('type', univ.ObjectIdentifier())
    )


class PrivKeyContainer(univ.OctetString):
    pass


class EddsaPrivateKey(univ.Sequence):
    componentType = NamedTypes(
        NamedType('Version', univ.Integer()),
        NamedType('Type', Identifier()),
        NamedType('PrivateKey', univ.OctetString()),
    )


def _get_skey(skey_container):
    container = skey_container['PrivateKey']
    private_key, _ = der_decoder(container, asn1Spec=PrivKeyContainer)
    return bytes(private_key)


def _parse_privkey(skey):
    if skey.startswith(b"-----BEGIN PRIVATE KEY-----"):
        pem_input = skey.decode('ascii')
        skey = base64.b64decode(''.join(pem_input.splitlines()[1:-1]))
    keys_input, _ = der_decoder(skey, asn1Spec=EddsaPrivateKey())
    return _get_skey(keys_input)


def _format_sign1(payload, protected, unprotected, signature):
    return [
            cbor.dumps(protected),
            {},
            payload,
            signature
            ]


def _gen_signature(key, payload, protected):
    sig = [
            "Signature1",
            cbor.dumps(protected),
            b'',
            payload
            ]
    return key.sign(cbor.dumps(sig))


def _sign1(payload, kid, key):
    protected = {4: kid, 1: -8}
    signature = _gen_signature(key, payload, protected)
    return _format_sign1(payload, protected, {}, signature)


def _get_conditions(args):
    conds = []
    uuid_vendor = None
    if args.vendorname:
        uuid_vendor = uuid.uuid5(uuid.NAMESPACE_DNS, str(args.vendorname))
        print("Vendor ID: {}".format(uuid_vendor.hex))
        conds.append([CONDITION_VENDORID, uuid_vendor.bytes])
        if args.classname:
            uuid_class = uuid.uuid5(uuid_vendor, args.classname)
            conds.append([CONDITION_CLASSID, uuid_class.bytes])
            print("Class ID: {}".format(uuid_class.hex))
            if args.deviceid:
                conds.append([CONDITION_DEVICEID,
                              binascii.unhexlify(args.deviceid)])
    if args.valid_duration:

        timestamp = cbor.dumps(cbor.Tag(1, _get_timestamp() + args.valid_duration))
        conds.append([CONDITION_BESTBEFORE, timestamp])
    return conds


def _get_payloadinfo(args):
    return [
             [PAYLOADFORMAT_BIN],
             _get_bin_size(args.filename),
             None,
             [[1, args.uri]],
             [DIGEST_TYPE_SHA256],
             {1: _get_bin_hash(args.filename)},
             None
        ]


def _format_suit(args):
    suit = [
            args.version,
            None,
            _get_nonce(),
            _get_timestamp(),
            _get_conditions(args),
            None,
            None,
            None,
            None,
           ]
    payloadinfo = _get_payloadinfo(args)
    suit.append(payloadinfo)
    return suit


def _get_timestamp():
    return calendar.timegm(time.gmtime())


def _get_nonce():
    return os.urandom(8)


def _get_bin_size(filename):
    return os.path.getsize(filename)


def _get_bin_hash(filename):
    digest = None
    with open(filename, 'rb') as f:
        digest = hashlib.sha256(f.read()).digest()
    return digest


def _get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-u", "--uri", type=str, help="Full uri to the binary")
    parser.add_argument("-k", "--key", type=argparse.FileType('rb'),
                        help="File containing the ed25519 key")
    parser.add_argument("-r", "--raw", help="File is a raw ed25519 byte key",
            action="store_true")
    parser.add_argument("-e", "--vendorname", type=str,
                        help="Vendor")
    parser.add_argument("-c", "--classname", type=str,
                        help="Device class")
    parser.add_argument("-d", "--deviceid", type=str,
                        help="Device ID uuid")
    parser.add_argument("-V", "--version", type=int,
                        help="Firmware version")
    parser.add_argument("-b", "--valid-duration", type=int,
                        help="Valid duration in seconds")
    parser.add_argument("-o", "--output", type=argparse.FileType('wb'),
                        help="Signed manifest output file")
    parser.add_argument("file", type=argparse.FileType('rb'),
                        help="Input binary to create the manifest for")
    args = parser.parse_args()
    args.filename = args.file.name
    return args


def main():
    args = _get_args()
    print(args)
    key_data = args.key.read()
    skey = None
    if args.raw:
        skey = ed25519.SigningKey(key_data)
    else:
        skey = ed25519.SigningKey(_parse_privkey(key_data))
    suit = _format_suit(args)
    print("manifest generated, {} bytes long".format(len(cbor.dumps(suit))))
    sign = _sign1(cbor.dumps(suit), "test", skey)
    args.output.write(cbor.dumps(sign))
    return 0


if __name__ == "__main__":
    main()
