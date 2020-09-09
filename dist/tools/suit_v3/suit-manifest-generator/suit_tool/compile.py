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
import binascii
import copy

import logging

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes


from suit_tool.manifest import SUITComponentId, SUITCommon, SUITSequence, \
                     SUITCommand, \
                     SUITWrapper, SUITTryEach

LOG = logging.getLogger(__name__)

def runable_id(c):
    id = c['install-id']
    if c.get('loadable'):
        id = c['load-id']
    return id

def hash_file(fname, alg):
    imgsize = 0
    digest = hashes.Hash(alg, backend=default_backend())
    with open(fname, 'rb') as fd:
        def read_in_chunks():
            while True:
                data = fd.read(1024)
                if not data:
                    break
                yield data
        for chunk in read_in_chunks():
            imgsize += len(chunk)
            digest.update(chunk)
    return digest, imgsize


def mkCommand(cid, name, arg):
    if hasattr(arg, 'to_json'):
        jarg = arg.to_json()
    else:
        jarg = arg
    return SUITCommand().from_json({
        'component-id' : cid.to_json(),
        'command-id' :  name,
        'command-arg' : jarg
    })

def check_eq(ids, choices):
    eq = {}
    neq = {}

    check = lambda x: x[:-1]==x[1:]
    get = lambda k, l: [d.get(k) for d in l]
    eq = { k: ids[k] for k in ids if any([k in c for c in choices]) and check(get(k, choices)) }
    check = lambda x: not x[:-1]==x[1:]
    neq = { k: ids[k] for k in ids if any([k in c for c in choices]) and check(get(k, choices)) }
    return eq, neq


def make_sequence(cid, choices, seq, params, cmds, pcid_key=None, param_drctv='directive-set-parameters'):
    eqcmds, neqcmds = check_eq(cmds, choices)
    eqparams, neqparams = check_eq(params, choices)
    if not pcid_key:
        pcid = cid
    else:
        pcid = SUITComponentId().from_json(choices[0][pcid_key])
    params = {}
    for param, pcmd in eqparams.items():
        k,v = pcmd(pcid, choices[0])
        params[k] = v
    if len(params):
        seq.append(mkCommand(pcid, param_drctv, params))
    TryEachCmd = SUITTryEach()
    for c in choices:
        TECseq = SUITSequence()
        for item, cmd in neqcmds.items():
            TECseq.append(cmd(cid, c))
        params = {}
        for param, pcmd in neqparams.items():
            k,v = pcmd(cid, c)
            params[k] = v
        if len(params):
            TECseq.append(mkCommand(pcid, param_drctv, params))
        if len(TECseq.items):
            TryEachCmd.append(TECseq)
    if len(TryEachCmd.items):
        seq.append(mkCommand(cid, 'directive-try-each', TryEachCmd))
    # Finally, and equal commands
    for item, cmd in eqcmds.items():
        seq.append(cmd(cid, choices[0]))
    return seq

def compile_manifest(options, m):
    m = copy.deepcopy(m)
    m['components'] += options.components
    # Compile list of All Component IDs
    ids = set([
        SUITComponentId().from_json(id) for comp_ids in [
            [c[f] for f in [
                'install-id', 'download-id', 'load-id'
            ] if f in c] for c in m['components']
        ] for id in comp_ids
    ])
    cid_data = {}
    for c in m['components']:
        if not 'install-id' in c:
            LOG.critical('install-id required for all components')
            raise Exception('No install-id')

        cid = SUITComponentId().from_json(c['install-id'])
        if not cid in cid_data:
            cid_data[cid] = [c]
        else:
            cid_data[cid].append(c)

    for id, choices in cid_data.items():
        for c in choices:
            if 'file' in c:
                digest, imgsize = hash_file(c['file'], hashes.SHA256())
                c['install-digest'] = {
                    'algorithm-id' : 'sha256',
                    'digest-bytes' : binascii.b2a_hex(digest.finalize())
                }
                c['install-size'] = imgsize

    if not any(c.get('vendor-id', None) for c in m['components']):
        LOG.critical('A vendor-id is required for at least one component')
        raise Exception('No Vendor ID')

    if not any(c.get('class-id', None) for c in m['components'] if 'vendor-id' in c):
        LOG.critical('A class-id is required for at least one component that also has a vendor-id')
        raise Exception('No Class ID')

    # Construct common sequence
    CommonCmds = {
        'offset': lambda cid, data: mkCommand(cid, 'condition-component-offset', data['offset'])
    }
    CommonParams = {
        'install-digest': lambda cid, data: ('image-digest', data['install-digest']),
        'install-size': lambda cid, data: ('image-size', data['install-size']),
    }
    CommonSeq = SUITSequence()
    for cid, choices in cid_data.items():
        if any(['vendor-id' in c for c in choices]):
            CommonSeq.append(mkCommand(cid, 'condition-vendor-identifier',
                [c['vendor-id'] for c in choices if 'vendor-id' in c][0]))
        if any(['vendor-id' in c for c in choices]):
            CommonSeq.append(mkCommand(cid, 'condition-class-identifier',
                [c['class-id'] for c in choices if 'class-id' in c][0]))
        CommonSeq = make_sequence(cid, choices, CommonSeq, CommonParams,
            CommonCmds, param_drctv='directive-override-parameters')

    InstSeq = SUITSequence()
    FetchSeq = SUITSequence()
    for cid, choices in cid_data.items():
        if any([c.get('install-on-download', True) and 'uri' in c for c in choices]):
            InstParams = {
                'uri' : lambda cid, data: ('uri', data['uri']),
            }
            if any(['compression-info' in c and not c.get('decompress-on-load', False) for c in choices]):
                InstParams['compression-info'] = lambda cid, data: data.get('compression-info')
            InstCmds = {
                'offset': lambda cid, data: mkCommand(
                    cid, 'condition-component-offset', data['offset'])
            }
            InstSeq = make_sequence(cid, choices, InstSeq, InstParams, InstCmds)
            InstSeq.append(mkCommand(cid, 'directive-fetch', None))
            InstSeq.append(mkCommand(cid, 'condition-image-match', None))

        elif any(['uri' in c for c in choices]):
            FetchParams = {
                'uri' : lambda cid, data: ('uri', data['uri']),
                'download-digest' : lambda cid, data : (
                    'image-digest', data.get('download-digest', data['install-digest']))
            }
            if any(['compression-info' in c and not c.get('decompress-on-load', False) for c in choices]):
                FetchParams['compression-info'] = lambda cid, data: data.get('compression-info')

            FetchCmds = {
                'offset': lambda cid, data: mkCommand(
                    cid, 'condition-component-offset', data['offset']),
                'fetch' : lambda cid, data: mkCommand(
                    data.get('download-id', cid.to_json()), 'directive-fetch', None),
                'match' : lambda cid, data: mkCommand(
                    data.get('download-id', cid.to_json()), 'condition-image-match', None)
            }
            FetchSeq = make_sequence(cid, choices, FetchSeq, FetchParams, FetchCmds, 'download-id')

            InstParams = {
                'download-id' : lambda cid, data : ('source-component', data['download-id'])
            }
            InstCmds = {
            }
            InstSeq = make_sequence(cid, choices, InstSeq, InstParams, InstCmds)
            InstSeq.append(mkCommand(cid, 'directive-copy', None))
            InstSeq.append(mkCommand(cid, 'condition-image-match', None))

    # TODO: Dependencies
    # If there are dependencies
        # Construct dependency resolution step

    ValidateSeq = SUITSequence()
    RunSeq = SUITSequence()
    LoadSeq = SUITSequence()
    # If any component is marked bootable
    for cid, choices in cid_data.items():
        if any([c.get('bootable', False) for c in choices]):
            # TODO: Dependencies
            # If there are dependencies
                # Verify dependencies
                # Process dependencies
            ValidateSeq.append(mkCommand(cid, 'condition-image-match', None))

        if any(['loadable' in c for c in choices]):
            # Generate image load section
            LoadParams = {
                'install-id' : lambda cid, data : ('source-component', c['install-id']),
                'load-digest' : ('image-digest', c.get('load-digest', c['install-digest'])),
                'load-size' : ('image-size', c.get('load-size', c['install-size']))
            }
            if 'compression-info' in c and c.get('decompress-on-load', False):
                LoadParams['compression-info'] = lambda cid, data: ('compression-info', c['compression-info'])
            LoadCmds = {
                # Move each loadable component
            }
            load_id = SUITComponentId().from_json(choices[0]['load-id'])
            LoadSeq = make_sequence(load_id, choices, ValidateSeq, LoadParams, LoadCmds)
            LoadSeq.append(mkCommand(load_id, 'directive-copy', None))
            LoadSeq.append(mkCommand(load_id, 'condition-image-match', None))

        # Generate image invocation section
        bootable_components = [x for x in m['components'] if x.get('bootable')]
        if len(bootable_components) == 1:
            c = bootable_components[0]
            RunSeq.append(SUITCommand().from_json({
                'component-id' : runable_id(c),
                'command-id' : 'directive-run',
                'command-arg' : None
            }))
        else:
            t = []
            for c in bootable_components:
                pass
                # TODO: conditions
                # t.append(
                #
                # )
    #TODO: Text
    common = SUITCommon().from_json({
        'components': [id.to_json() for id in ids],
        'common-sequence': CommonSeq.to_json(),
    })

    jmanifest = {
        'manifest-version' : m['manifest-version'],
        'manifest-sequence-number' : m['manifest-sequence-number'],
        'common' : common.to_json()
    }

    jmanifest.update({k:v for k,v in {
            'payload-fetch' : FetchSeq.to_json(),
            'install' : InstSeq.to_json(),
            'validate' : ValidateSeq.to_json(),
            'run' : RunSeq.to_json(),
            'load' : LoadSeq.to_json()
    }.items() if v})

    wrapped_manifest = SUITWrapper().from_json({'manifest' : jmanifest})
    return wrapped_manifest
