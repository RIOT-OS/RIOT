# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------
# Copyright 2019-2020 ARM Limited or its affiliates
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
import sys, argparse, os
from suit_tool import __version__
from suit_tool import keygen
from suit_tool import get_pubkey
import json
import re

def str_to_component(s):
    types = {
        'file' : ('file', lambda x : str(x.strip('"'))),
        # 'desc' : ('component-description', lambda x : str(x.strip('"'))),
        'inst' : ('install-id', lambda x : [ str(y) for y in eval(x) ]),
        'uri' : ('uri', lambda x : str(x.strip('"')))
    }
    d = {types[k][0]:types[k][1](v) for k,v in [ re.split(r'=',e, maxsplit=1) for e in re.split(r''',\s*(?=["']?[a-zA-Z0-9_-]+["']?=)''', s)]}
    return d

class MainArgumentParser(object):

    def __init__(self):
        self.parser = self._make_parser()

    def _make_parser(self):
        parser = argparse.ArgumentParser(description = 'Create or transform a manifest.'
        ' Use {} [command] -h for help on each command.'.format(sys.argv[0]))

        # Add all top-level commands
        parser.add_argument('-l', '--log-level', choices=['debug','info','warning','exception'], default='info',
            help='Set the verbosity level of console output.')
        parser.add_argument('--version', action='version', version=__version__,
            help='display the version'
        )
        subparsers = parser.add_subparsers(dest="action")
        subparsers.required = True
        create_parser = subparsers.add_parser('create', help='Create a new manifest')

        # create_parser.add_argument('-v', '--manifest-version', choices=['1'], default='1')
        create_parser.add_argument('-i', '--input-file', metavar='FILE', type=argparse.FileType('r'),
            help='An input file describing the update. The file must be formatted as JSON. The overal structure is described in README.')
        create_parser.add_argument('-o', '--output-file', metavar='FILE', type=argparse.FileType('wb'), required=True)
        create_parser.add_argument('-f', '--format', metavar='FMT', choices=['suit', 'suit-debug', 'json'], default='suit')
        create_parser.add_argument('-s', '--severable', action='store_true', help='Convert large elements to severable fields.')
        create_parser.add_argument('-c', '--add-component', action='append', type=str_to_component, dest='components', default=[])

        sign_parser = subparsers.add_parser('sign', help='Sign a manifest')

        sign_parser.add_argument('-m', '--manifest', metavar='FILE', type=argparse.FileType('rb'), required=True)
        sign_parser.add_argument('-k', '--private-key', metavar='FILE', type=argparse.FileType('rb'), required=True)
        sign_parser.add_argument('-i', '--key-id', metavar='ID', type=str)
        sign_parser.add_argument('-o', '--output-file', metavar='FILE', type=argparse.FileType('wb'), required=True)

        parse_parser = subparsers.add_parser('parse', help='Parse a manifest')

        parse_parser.add_argument('-m', '--manifest', metavar='FILE', type=argparse.FileType('rb'), required=True)
        parse_parser.add_argument('-j', '--json-output', default=False, action='store_true', dest='json')

        get_pubkey_parser = subparsers.add_parser('pubkey', help='Get the public key for a supplied private key.')

        get_pubkey_parser.add_argument('-k', '--private-key', metavar='FILE', type=argparse.FileType('rb'), required=True)
        get_pubkey_parser.add_argument('-f', '--output-format', choices=get_pubkey.OutputFormaters.keys(), default='pem')
        get_pubkey_parser.add_argument('-o', '--output-file', metavar='FILE', type=argparse.FileType('wb'), default=sys.stdout)

        keygen_parser = subparsers.add_parser('keygen', help='Create a signing key. Not for production use')

        keygen_parser.add_argument('-t', '--type', choices=keygen.KeyGenerators.keys(),
            default='secp256r1', help='The type of the key to generate')
        keygen_parser.add_argument('-o', '--output-file', metavar='FILE', type=argparse.FileType('wb'), default=sys.stdout)
        keygen_parser.add_argument('-f', '--output-format', choices=keygen.OutputFormaters.keys(), default='pem')
        keygen_parser.add_argument('-l', '--levels', help='The number of hss-lms levels', type=int, default=2)

        sever_parser = subparsers.add_parser('sever', help='Remove one or more severable elements from the manifest, if present.')
        sever_parser.add_argument('-m', '--manifest', metavar='FILE', type=argparse.FileType('rb'), required=True)
        sever_parser.add_argument('-o', '--output-file', metavar='FILE', type=argparse.FileType('wb'), required=True)
        sever_parser.add_argument('-e', '--element', action='append', type=str, dest='elements', default=[])
        sever_parser.add_argument('-a', '--all', action='store_true', default=False)

        return parser


    def parse_args(self, args=None):
        self.options = self.parser.parse_args(args)

        return self
