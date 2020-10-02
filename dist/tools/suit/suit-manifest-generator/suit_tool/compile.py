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
import binascii
import copy
import collections
import json
import cbor2 as cbor
import sys
import textwrap
import itertools

import logging

from collections import OrderedDict

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes


from suit_tool.manifest import SUITComponentId, SUITCommon, SUITSequence, \
                     suitCommonInfo, SUITCommand, SUITManifest, \
                     SUITEnvelope, SUITTryEach, SUITBWrapField, SUITText, \
                     SUITDigest, SUITDependencies, SUITDependency

import suit_tool.create
import suit_tool.sign

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
        TECseq = TryEachCmd.field.obj().from_json([])
        params = {}
        for param, pcmd in neqparams.items():
            k,v = pcmd(cid, c)
            params[k] = v
        dep_params = {}
        TECseq_cmds = []
        for item, cmd in neqcmds.items():
            ncmd = cmd(cid, c)
            for dp in ncmd.dep_params:
                if dp in params:
                    dep_params[dp] = params[dp]
                    del params[dp]
            TECseq_cmds.append(ncmd)

        if len(dep_params):
            TECseq.v.append(mkCommand(pcid, param_drctv, dep_params))

        for cmd in TECseq_cmds:
            TECseq.v.append(cmd)

        if len(params):
            TECseq.v.append(mkCommand(pcid, param_drctv, params))
        if hasattr(TECseq, "v") and len(TECseq.v.items):
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
    # There is no ordered set, so use ordered dict instead
    ids = OrderedDict.fromkeys([
        SUITComponentId().from_json(id) for comp_ids in [
            [c[f] for f in [
                'install-id', 'download-id', 'load-id'
            ] if f in c] for c in m['components']
        ] for id in comp_ids
    ])
    cid_data = OrderedDict()
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
                    'digest-bytes' : digest.finalize()
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
        'offset': lambda cid, data: mkCommand(cid, 'condition-component-offset', None),
        'vendor-id': lambda cid, data: mkCommand(cid, 'condition-vendor-identifier', None),
        'class-id': lambda cid, data: mkCommand(cid, 'condition-class-identifier', None),
    }
    CommonParams = {
        'install-digest': lambda cid, data: ('image-digest', data['install-digest']),
        'install-size': lambda cid, data: ('image-size', data['install-size']),
        'vendor-id' : lambda cid, data: ('vendor-id', data['vendor-id']),
        'class-id' : lambda cid, data: ('class-id', data['class-id']),
        'offset' : lambda cid, data: ('offset', data['offset'])
    }
    # print('Common')
    CommonSeq = SUITSequence()
    for cid, choices in cid_data.items():
        CommonSeq = make_sequence(cid, choices, CommonSeq, CommonParams,
            CommonCmds, param_drctv='directive-override-parameters')


    # print('Dependencies')
    # If there are dependencies
    DepSeq = SUITSequence()
    Dependencies = SUITDependencies()
    DepRequiredSequences = { k:[] for k in ['deres', 'fetch', 'install', 'validate', 'run', 'load']}
    if 'dependencies' in m:
        for dep in m['dependencies']:
            # Prepare dependency if necessary
            if "src-file" in dep:
                # Create
                with open(dep['src-file']) as input_fd:
                    with open(dep['file']+'.tmp','wb') as output_fd:
                        create_opts = type('',(object,),{
                            # 'input_file':  open(dep['src-file']),
                            # 'output_file': open(dep['file']+'.tmp','wb'),
                            'input_file':  input_fd,
                            'output_file': output_fd,
                            'format' : 'suit',
                            'components': [],
                            'log_level': options.log_level
                        })()
                        rc = suit_tool.create.main(create_opts)
                        if rc:
                            sys.exit(rc)
                # Sign
                with open(dep['file']+'.tmp','rb') as manifest_fd:
                    with open(dep['file'],'wb') as output_fd:
                        with open(dep['key-file'], 'rb') as private_key_fd:
                            sign_opts = type('',(object,),{
                                'manifest':    manifest_fd,
                                'output_file': output_fd,
                                'private_key': private_key_fd,
                                'log_level': options.log_level
                            })()
                            rc = suit_tool.sign.main(sign_opts)
                            if rc:
                                sys.exit(rc)
            # Compute the dependency digest
            digest = hashes.Hash(hashes.SHA256(), backend=default_backend())
            mfst = {}
            with open(dep['file'],'rb') as dep_fd:
                dep_envelope = cbor.loads(dep_fd.read())
                cmfst = dep_envelope[SUITEnvelope.fields['manifest'].suit_key]
                digest.update(cbor.dumps(cmfst))
                mfst = cbor.loads(cmfst)
            did = SUITDigest().from_json({
                    'algorithm-id' : 'sha256',
                    'digest-bytes' : binascii.b2a_hex(digest.finalize())
                })

            Dependencies.append(SUITDependency().from_json({
                'dependency-digest' : did.to_json()
            }))
            # Construct dependency resolution step
            if 'uri' in dep:
                DepSeq.append(mkCommand(did, 'directive-set-parameters', {
                    'uri' : dep['uri']
                }))
                DepSeq.append(mkCommand(did, 'directive-fetch', None))
                DepSeq.append(mkCommand(did, 'condition-image-match', None))
            for k, l in DepRequiredSequences.items():
                if SUITManifest.fields[k].suit_key in mfst:
                    l.append(mkCommand(did, 'directive-process-dependency', None))

    InstSeq = SUITSequence()
    FetchSeq = SUITSequence()
    # print('Install/Fetch')
    for cid, choices in cid_data.items():
        if any([c.get('install-on-download', True) and 'uri' in c for c in choices]):
            InstParams = {
                'uri' : lambda cid, data: ('uri', data['uri']),
                'offset' : lambda cid, data: ('offset', data['offset']),
            }
            if any(['compression-info' in c and not c.get('decompress-on-load', False) for c in choices]):
                InstParams['compression-info'] = lambda cid, data: data.get('compression-info')
            InstCmds = {
                'offset': lambda cid, data: mkCommand(
                    cid, 'condition-component-offset', None)
            }
            InstSeq = make_sequence(cid, choices, InstSeq, InstParams, InstCmds)
            for cmd in DepRequiredSequences['install']:
                InstSeq.append(cmd)
            InstSeq.append(mkCommand(cid, 'directive-fetch', None))
            InstSeq.append(mkCommand(cid, 'condition-image-match', None))

        elif any(['uri' in c for c in choices]):
            FetchParams = {
                'uri' : lambda cid, data: ('uri', data['uri']),
                'download-digest' : lambda cid, data : (
                    'image-digest', data.get('download-digest', data['install-digest'])),
                'offset' : lambda cid, data: ('offset', data['offset']),
            }
            if any(['compression-info' in c and not c.get('decompress-on-load', False) for c in choices]):
                FetchParams['compression-info'] = lambda cid, data: data.get('compression-info')

            FetchCmds = {
                'offset': lambda cid, data: mkCommand(
                    cid, 'condition-component-offset', data['offset']),
                # 'fetch' : lambda cid, data: mkCommand(
                #     data.get('download-id', cid.to_json()), 'directive-fetch', None),
                # 'match' : lambda cid, data: mkCommand(
                #     data.get('download-id', cid.to_json()), 'condition-image-match', None)
            }
            did = SUITComponentId().from_json([c['download-id'] for c in choices if 'download-id' in c][0])
            FetchSeq = make_sequence(cid, choices, FetchSeq, FetchParams, FetchCmds, 'download-id')
            for cmd in DepRequiredSequences['fetch']:
                FetchSeq.append(cmd)

            FetchSeq.append(mkCommand(did, 'directive-fetch', None))
            FetchSeq.append(mkCommand(did, 'condition-image-match', None))

            InstParams = {
                'download-id' : lambda cid, data : ('source-component', data['download-id'])
            }
            InstCmds = {
            }
            InstSeq = make_sequence(cid, choices, InstSeq, InstParams, InstCmds)
            for cmd in DepRequiredSequences['install']:
                InstSeq.append(cmd)
            InstSeq.append(mkCommand(cid, 'directive-copy', None))
            InstSeq.append(mkCommand(cid, 'condition-image-match', None))


    ValidateSeq = SUITSequence()
    RunSeq = SUITSequence()
    LoadSeq = SUITSequence()
    # print('Validate/Load/Run')
    # If any component is marked bootable
    for cid, choices in cid_data.items():
        ValidateCmds = {
            # 'install-digest' : lambda cid, data : mkCommand(cid, 'condition-image-match', None)
        }
        ValidateParams = {
        }
        ValidateSeq = make_sequence(cid, choices, ValidateSeq, ValidateParams, ValidateCmds)
        for cmd in DepRequiredSequences['validate']:
            ValidateSeq.append(cmd)
        ValidateSeq.append(mkCommand(cid, 'condition-image-match', None))
        # if any([c.get('bootable', False) for c in choices]):
        # TODO: Dependencies
        # If there are dependencies
            # Verify dependencies
            # Process dependencies


        if any(['loadable' in c for c in choices]):
            # Generate image load section
            LoadParams = {
                'install-id'  : lambda cid, data : ('source-component', c['install-id']),
                'load-digest' : lambda cid, data : ('image-digest', c.get('load-digest', c['install-digest'])),
                'load-size'   : lambda cid, data : ('image-size', c.get('load-size', c['install-size'])),
            }
            if 'compression-info' in c and c.get('decompress-on-load', False):
                LoadParams['compression-info'] = lambda cid, data: ('compression-info', c['compression-info'])
            LoadCmds = {
                # Move each loadable component
            }
            load_id = SUITComponentId().from_json(choices[0]['load-id'])
            LoadSeq = make_sequence(load_id, choices, LoadSeq, LoadParams, LoadCmds)
            for cmd in DepRequiredSequences['load']:
                LoadSeq.append(cmd)
            LoadSeq.append(mkCommand(load_id, 'directive-copy', None))
            LoadSeq.append(mkCommand(load_id, 'condition-image-match', None))

        # Generate image invocation section
        bootable_components = [x for x in m['components'] if x.get('bootable')]
        if len(bootable_components) == 1:
            c = bootable_components[0]
            for cmd in DepRequiredSequences['run']:
                RunSeq.append(cmd)
            RunSeq.append(SUITCommand().from_json({
                'component-id' : runable_id(c),
                'command-id' : 'directive-run',
                'command-arg' : None
            }))
        else:
            for c in bootable_components:
                pass
                # TODO: conditions
                # t.append(
                #
                # )
    #TODO: Text
    # print('Common')
    common = SUITCommon().from_json({
        'components': [id.to_json() for id in ids.keys()],
        'common-sequence': CommonSeq.to_json(),
    })
    if len(Dependencies.items):
        common.dependencies = Dependencies

    # print('manifest')
    jmanifest = {
        'manifest-version' : m['manifest-version'],
        'manifest-sequence-number' : m['manifest-sequence-number'],
        'common' : common.to_json()
    }

    # for k,v in {'deres':DepSeq, 'fetch': FetchSeq, 'install':InstSeq, 'validate':ValidateSeq, 'run':RunSeq, 'load':LoadSeq}.items():
    #     # print('sequence:{}'.format(k))
    #     v.to_json()

    jmanifest.update({k:v for k,v in {
            'deres' : DepSeq.to_json(),
            'fetch' : FetchSeq.to_json(),
            'install' : InstSeq.to_json(),
            'validate' : ValidateSeq.to_json(),
            'run' : RunSeq.to_json(),
            'load' : LoadSeq.to_json()
    }.items() if v})

    mtext = {}
    for k in ['manifest-description', 'update-description']:
        if k in m:
            mtext[k] = m[k]
    for c in m['components']:
        ctext = {}
        cfields = [
            'vendor-name',
            'model-name',
            'vendor-domain',
            'model-info',
            'component-description',
            'component-version',
            'version-required',
        ]
        for k in cfields:
            if k in c:
                ctext[k] = c[k]
        if len(ctext):
            cid = SUITComponentId().from_json(c['install-id']).to_suit()
            mtext[cid] = ctext

    jenvelope = {
        'authentication-wrapper' : [],
        'manifest' : jmanifest
    }

    if len(mtext):
        text = SUITText().from_json(mtext)
        digest_alg = m.get('digest-algorithm', 'sha256')
        suit_text = cbor.dumps(text.to_suit(), canonical=True)
        digest = hashes.Hash(SUITEnvelope.digest_algorithms.get(digest_alg)(), backend=default_backend())
        digest.update(suit_text)

        jenvelope['manifest'].update({'text' : {
            'algorithm-id' : digest_alg,
            'digest-bytes' : digest.finalize()
        }})
        jenvelope.update({'text' : mtext})

    # print('building envelope')
    wrapped_manifest = SUITEnvelope().from_json(jenvelope)

    return wrapped_manifest
