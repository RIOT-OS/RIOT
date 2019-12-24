#!/usr/bin/env python3
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
import json
import cbor
import binascii
import uuid
import os

from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives import serialization

COSE_ALG = 1
COSE_Sign_Tag = 98
APPLICATION_OCTET_STREAM_ID = 42
ES256 = -7
EDDSA = -8

SUIT_Authentication_Wrapper = 1
SUIT_Manifest = 2
SUIT_Dependency_Resolution = 7
SUIT_Payload_Fetch = 8
SUIT_Install = 9
SUIT_Text = 13
SUIT_Coswid = 14

SUIT_Manifest_Version = 1
SUIT_Manifest_Sequence_Number = 2
SUIT_Dependencies = 3
SUIT_Components = 4
SUIT_Dependency_Components = 5
SUIT_Common = 6
SUIT_Dependency_Resolution = 7
SUIT_Payload_Fetch = 8
SUIT_Install = 9
SUIT_Validate = 10
SUIT_Load = 11
SUIT_Run = 12
SUIT_Text = 13
SUIT_Coswid = 14

SUIT_Dependency_Digest = 1
SUIT_Dependency_Prefix = 2

SUIT_Component_Identifier = 1
SUIT_Component_Size = 2
SUIT_Component_Digest = 3

SUIT_Component_Dependency_Index = 2

SUIT_Condition_Vendor_Identifier = 1
SUIT_Condition_Class_Identifier = 2
SUIT_Condition_Device_Identifier = 3
SUIT_Condition_Image_Match = 4
SUIT_Condition_Image_Not_Match = 5
SUIT_Condition_Use_Before = 6
SUIT_Condition_Minimum_Battery = 7
SUIT_Condition_Update_Authorised = 8
SUIT_Condition_Version = 9
SUIT_Condition_Component_Offset = 10

SUIT_Directive_Set_Component_Index = 11
SUIT_Directive_Set_Manifest_Index = 12
SUIT_Directive_Run_Sequence = 13
SUIT_Directive_Run_Sequence_Conditional = 14
SUIT_Directive_Process_Dependency = 15
SUIT_Directive_Set_Parameters = 16
SUIT_Directive_Override_Parameters = 19
SUIT_Directive_Fetch = 20
SUIT_Directive_Copy = 21
SUIT_Directive_Run = 22
SUIT_Directive_Wait = 23

SUIT_Parameter_Strict_Order = 1
SUIT_Parameter_Coerce_Condition_Failure = 2
SUIT_Parameter_Vendor_ID = 3
SUIT_Parameter_Class_ID = 4
SUIT_Parameter_Device_ID = 5
SUIT_Parameter_URI_List = 6
SUIT_Parameter_Encryption_Info = 7
SUIT_Parameter_Compression_Info = 8
SUIT_Parameter_Unpack_Info = 9
SUIT_Parameter_Source_Component = 10
SUIT_Parameter_Image_Digest = 11
SUIT_Parameter_Image_Size = 12

SUIT_Compression_Algorithm = 1

def obj2bytes(o):
    if isinstance(o, int):
        l = []
        while o:
            l.append(o&0xff)
            o = o >> 8
        return bytes(l)
    if isinstance(o, str):
        return o.encode('utf-8')
    if isinstance(o, bytes):
        return o
    return b''

def make_SUIT_Components(unused, components):
    comps = []
    for component in components:
        c = {
            SUIT_Component_Identifier : [obj2bytes(x) for x in component["id"]]
        }
        if "digest" in component:
            c[SUIT_Component_Digest] = [1, binascii.a2b_hex(component["digest"])]
        if "size" in component:
            c[SUIT_Component_Size] = component["size"]
        comps.append(c)
    return (SUIT_Components, comps)

def make_SUIT_Compression_Info(info):
    algorithms = {
        'gzip' : 1,
        'bzip2' : 2,
        'deflate' : 3,
        'lz4' : 4,
        'lzma' : 7,
    }
    cinfo = {
        SUIT_Compression_Algorithm :algorithms[info['algorithm']]
    }

def make_SUIT_Set_Parameters(parameters):
    set_parameters = {}
    SUIT_Parameters_Keys = {
        # SUIT_Parameter_Strict_Order = 1
        # SUIT_Parameter_Coerce_Condition_Failure = 2
        # SUIT_Parameter_Vendor_ID = 3
        # SUIT_Parameter_Class_ID = 4
        # SUIT_Parameter_Device_ID = 5
        # SUIT_Parameter_URI_List = 6
        'uris' : lambda x: (SUIT_Parameter_URI_List, cbor.dumps(x)),
        # SUIT_Parameter_Encryption_Info = 7
        # SUIT_Parameter_Compression_Info = 8
        'compression-info': lambda x : (
            SUIT_Parameter_Compression_Info,
            cbor.dumps(make_SUIT_Compression_Info(x))
        ),
        # SUIT_Parameter_Unpack_Info = 9
        'source-index' : lambda x :(SUIT_Parameter_Source_Component, int(x)),
        'image-digest' : lambda x :(SUIT_Parameter_Image_Digest, cbor.dumps(x, sort_keys=True)),
        'image-size'   : lambda x :(SUIT_Parameter_Image_Size, int(x)),
    }
    for p in parameters:
        if p in SUIT_Parameters_Keys:
            k, v = SUIT_Parameters_Keys[p](parameters[p])
            set_parameters[k] = v
        else:
            raise Exception('ERROR: {} not found!'.format(p))

    return (SUIT_Directive_Set_Parameters, set_parameters)

def make_SUIT_Sequence(seq_name, sequence):
    seq = []
    SUIT_Sequence_Keys = {
        "condition-vendor-id"       : lambda x : (SUIT_Condition_Vendor_Identifier, uuid.UUID(x).bytes),
        "condition-class-id"        : lambda x : (SUIT_Condition_Class_Identifier, uuid.UUID(x).bytes),
        "condition-device-id"       : lambda x : (SUIT_Condition_Device_Identifier, uuid.UUID(x).bytes),
        "condition-image"           : lambda x : (SUIT_Condition_Image_Match, None),
        "condition-not-image"       : lambda x : (SUIT_Condition_Image_Not_Match, None),
        # SUIT_Condition_Use_Before = 6
        # SUIT_Condition_Minimum_Battery = 7
        # SUIT_Condition_Update_Authorised = 8
        # SUIT_Condition_Version = 9
        "condition-component-offset" : lambda x: (SUIT_Condition_Component_Offset, int(x)),
        #
        "directive-set-component"   : lambda x : (SUIT_Directive_Set_Component_Index, x),
        # SUIT_Directive_Set_Manifest_Index = 12
        # SUIT_Directive_Run_Sequence = 13
        # SUIT_Directive_Run_Sequence_Conditional = 14
        "directive-run-conditional" : lambda x : (
            SUIT_Directive_Run_Sequence_Conditional,
            cbor.dumps(make_SUIT_Sequence("conditional-sequence", x), sort_keys = True)
        ),
        # SUIT_Directive_Process_Dependency = 15
        # SUIT_Directive_Set_Parameters = 16
        "directive-set-var"         : make_SUIT_Set_Parameters,
        # SUIT_Directive_Override_Parameters = 19
        "directive-fetch"           : lambda x : (SUIT_Directive_Fetch, None),
        "directive-copy"            : lambda x : (SUIT_Directive_Copy, None),
        "directive-run"             : lambda x : (SUIT_Directive_Run, None),
        # SUIT_Directive_Wait = 23
    }
    for command in sequence:
        com_dict = {}
        for c in command:
            if c in SUIT_Sequence_Keys:
                k, v = SUIT_Sequence_Keys[c](command[c])
                com_dict[k] = v
            else:
                raise Exception('ERROR: {} not found!'.format(c))
        seq.append(com_dict)
    return seq

def make_SUIT_Manifest(info):
    # print(info)
    SUIT_Manifest_Keys = {
        "structure-version" : lambda y, x: (SUIT_Manifest_Version, x),
        "sequence-number"   : lambda y, x: (SUIT_Manifest_Sequence_Number, x),
        # SUIT_Dependencies = 3
        "components"        : make_SUIT_Components,
        # SUIT_Dependency_Components = 5
        "common"            : lambda y, x: (SUIT_Common, cbor.dumps(make_SUIT_Sequence(y, x), sort_keys=True)),
        # SUIT_Dependency_Resolution = 7
        # SUIT_Payload_Fetch = 8
        "apply-image"       : lambda y, x: (SUIT_Install, cbor.dumps(make_SUIT_Sequence(y, x), sort_keys=True)),
        "system-verification": lambda y, x: (SUIT_Validate, cbor.dumps(make_SUIT_Sequence(y, x), sort_keys=True)),
        "load-image"        : lambda y, x: (SUIT_Load, cbor.dumps(make_SUIT_Sequence(y, x), sort_keys=True)),
        "run-image"         : lambda y, x: (SUIT_Run, cbor.dumps(make_SUIT_Sequence(y, x), sort_keys=True)),
        # SUIT_Text = 13
        # SUIT_Coswid = 14
    }
    manifest = {}
    for field in info:
        if field in SUIT_Manifest_Keys:
            k, v = SUIT_Manifest_Keys[field](field, info[field])
            manifest[k] = v
        else:
            raise Exception('ERROR: {} not found!'.format(field))

    # print ('suit-manifest: {}'.format(manifest))
    return manifest

def make_SUIT_Outer_Wrapper(info):
    Outer_Wrapper = {
        SUIT_Authentication_Wrapper : None,
        SUIT_Manifest               : cbor.dumps(make_SUIT_Manifest(info), sort_keys = True)
    }
    # print('Outer_Wrapper: {}'.format(Outer_Wrapper))
    return Outer_Wrapper


def make_SUIT_Components(unused, components):
    comps = []
    for component in components:
        c = {
            SUIT_Component_Identifier : [obj2bytes(x) for x in component["id"]]
        }
        if "digest" in component:
            c[SUIT_Component_Digest] = [1, binascii.a2b_hex(component["digest"])]
        if "size" in component:
            c[SUIT_Component_Size] = component["size"]
        comps.append(c)
    return (SUIT_Components, comps)

# Expected input format:
# {
#     "digest-type" : "str",
#     "structure-version" : 1,
#     "sequence-number" : 2,
#     "components": [
#         {
#             "component-id":[bytes()],
#             "bootable" : bool(),
#             "images" : [
#                 {
#                     "conditions" : [
#                         {"current-digest": bytes()},
#                         {"target-offset" : int()},
#                         {"target-id": [bytes()]},
#                     ],
#                     "digest": bytes(),
#                     "size" : int(),
#                     "uri" : str(),
#                 }
#             ]
#             "conditions" : [
#                 {"current-digest": bytes()},
#                 {"vendor-id" : bytes()},
#                 {"class-id" : bytes()},
#                 {"device-id" : bytes()},
#             ]
#         }
#     ],
#     "conditions" : [
#         {"vendor-id" : bytes()},
#         {"class-id" : bytes()},
#         {"device-id" : bytes()},
#     ]
# }
def digest_str_to_id(s):
    return {
        'sha-256' : 1,
        'sha-256-128' : 2,
        'sha-256-120' : 3,
        'sha-256-96' : 4,
        'sha-256-64' : 5,
        'sha-256-32' : 6,
        'sha-384' : 7,
        'sha-512' : 8,
        'sha3-224' : 9,
        'sha3-256' : 10,
        'sha3-384' : 11,
        'sha3-512' : 12,
    }.get(s, 1)

def compile_to_suit(suit_info):
    digest_id = digest_str_to_id(suit_info.get('digest-type', 'sha-256'))
    suit_manifest_desc = {
        'structure-version':int(suit_info.get('structure-version', 1)),
        'sequence-number':int(suit_info['sequence-number']),
    }
    #TODO: Dependencies
    # Components
    components = []
    #TODO: dependency components
    common = []
    dependency_fetch = None
    #TODO: Image Fetch when not in streaming mode
    fetch_image = None
    apply_image = []
    # System Verification
    #TODO: Dependencies
    system_verification = [
        {"directive-set-component": True},
        {"condition-image": None},
    ]
    #TODO: Load Image
    load_image = None
    run_image = []

    for con in suit_info.get('conditions', []):
        common.append(con)
    # for each component
    for i, comp in enumerate(suit_info['components']):
        comp_info = {
            'id' : comp['id']
        }
        components.append(comp_info)
        if len(comp['images']) == 1:
            set_comp = {"directive-set-component": i}
            set_params = {
                "directive-set-var" : {
                    "image-size" : int(comp['images'][0]['size']),
                    "image-digest" : [digest_id, bytes(comp['images'][0]['digest'])]
                }
            }
            common.append(set_comp)
            common.append(set_params)
            set_params = {
                "directive-set-var" : {
                    "uris" : [[0, str(comp['images'][0]['uri'])]]
                }
            }
            apply_image.append(set_comp)
            apply_image.append(set_params)
        else:
            for image in comp['images']:
                set_comp = {"directive-set-component": i}
                set_params = {
                    "directive-set-var" : {
                        "image-size" : int(image['size']),
                        "image-digest" : [digest_id, bytes(image['digest'])]
                    }
                }
                conditional_seq = [set_comp] + image.get('conditions',[])[:] + [set_params]
                conditional_set_params = {
                    'directive-run-conditional': conditional_seq
                }
                common.append(conditional_set_params)
                set_params = {
                    "directive-set-var" : {
                        "uris" : [[0, str(image['uri'])]]
                    }
                }
                conditional_seq = [set_comp] + image.get('conditions',[])[:] + [set_params]
                conditional_set_params = {
                    'directive-run-conditional': conditional_seq
                }

                apply_image.append(conditional_set_params)
        if comp.get('bootable', False):
            run_image.append({'directive-set-component' : i})
            run_image.append({'directive-run':None})

        apply_image.append({"directive-set-component": True})
        apply_image.append({"directive-fetch": None})

    suit_manifest_desc.update({
        "components"        : components,
        "common"            : common,
        "apply-image"       : apply_image,
        "system-verification": system_verification,
        "run-image"         : run_image,
    })

    print(suit_manifest_desc)

    return cbor.dumps(make_SUIT_Outer_Wrapper(suit_manifest_desc), sort_keys=True)
