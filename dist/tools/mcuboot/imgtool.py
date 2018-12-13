#! /usr/bin/env python3

import argparse
from imgtool import keys
from imgtool import image
from imgtool import version
import sys

def gen_rsa2048(args):
    keys.RSA2048.generate().export_private(args.key)
def gen_ecdsa_p256(args):
    keys.ecdsa256p1.generate().export_private(args.key)
def gen_ecdsa_p224(args):
    print("TODO: p-224 not yet implemented")

keygens = {
        'rsa-2048': gen_rsa2048,
        'ecdsa-p256': gen_ecdsa_p256,
        'ecdsa-p224': gen_ecdsa_p224, }

def do_keygen(args):
    if args.type not in keygens:
        msg = "Unexpected key type: {}".format(args.type)
        raise argparse.ArgumentTypeError(msg)
    keygens[args.type](args)

def do_getpub(args):
    key = keys.load(args.key)
    key.emit_c()

def do_sign(args):
    align = args.align
    if args.rsa_pkcs1_15:
        keys.sign_rsa_pss = False
    img = image.Image.load(args.infile, version=args.version,
            header_size=args.header_size,
            included_header=args.included_header,
            pad=args.pad)
    key = keys.load(args.key) if args.key else None
    img.sign(key)

    if args.pad:
        img.pad_to(args.pad, args.align)

    img.save(args.outfile)

subcmds = {
        'keygen': do_keygen,
        'getpub': do_getpub,
        'sign': do_sign, }

def alignment_value(text):
    value = int(text)
    if value not in [1, 2, 4, 8]:
        msg = "{} must be one of 1, 2, 4 or 8".format(value)
        raise argparse.ArgumentTypeError(msg)
    return value

def intparse(text):
    """Parse a command line argument as an integer.

    Accepts 0x and other prefixes to allow other bases to be used."""
    return int(text, 0)

def args():
    parser = argparse.ArgumentParser()
    subs = parser.add_subparsers(help='subcommand help', dest='subcmd')

    keygenp = subs.add_parser('keygen', help='Generate pub/private keypair')
    keygenp.add_argument('-k', '--key', metavar='filename', required=True)
    keygenp.add_argument('-t', '--type', metavar='type',
            choices=['rsa-2048', 'ecdsa-p224', 'ecdsa-p256'],
            required=True)

    getpub = subs.add_parser('getpub', help='Get public key from keypair')
    getpub.add_argument('-k', '--key', metavar='filename', required=True)

    sign = subs.add_parser('sign', help='Sign an image with a private key')
    sign.add_argument('-k', '--key', metavar='filename')
    sign.add_argument("--align", type=alignment_value, required=True)
    sign.add_argument("-v", "--version", type=version.decode_version, required=True)
    sign.add_argument("-H", "--header-size", type=intparse, required=True)
    sign.add_argument("--included-header", default=False, action='store_true',
            help='Image has gap for header')
    sign.add_argument("--pad", type=intparse,
            help='Pad image to this many bytes, adding trailer magic')
    sign.add_argument("--rsa-pkcs1-15", help='Use old PKCS#1 v1.5 signature algorithm',
            default=False, action='store_true')
    sign.add_argument("infile")
    sign.add_argument("outfile")

    args = parser.parse_args()
    if args.subcmd is None:
        print('Must specify a subcommand', file=sys.stderr)
        sys.exit(1)

    subcmds[args.subcmd](args)

if __name__ == '__main__':
    args()
