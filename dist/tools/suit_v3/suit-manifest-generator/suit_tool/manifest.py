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
import collections
import binascii
import cbor
import copy
import uuid
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes

ManifestKey = collections.namedtuple(
    'ManifestKey',
    [
        'json_key',
        'suit_key',
        'obj'
    ]
)
def to_bytes(s):
    try:
        return binascii.a2b_hex(s)
    except:
        try:
            return binascii.a2b_base64(s)
        except:
            if isinstance(s,str):
                return s.encode('utf-8')
            elif isinstance(s,bytes):
                return s
            else:
                return str(s).encode('utf-8')

class SUITCommonInformation:
    def __init__(self):
        self.component_ids = []
        self.current_index = 0
        self.indent_size = 4
    def component_id_to_index(self, cid):
        id = -1
        for i, c in enumerate(self.component_ids):
            if c == cid and i >= 0:
                id = i
        return id

suitCommonInfo = SUITCommonInformation()
one_indent = '    '

class SUITInt:
    def from_json(self, v):
        self.v = int(v)
        return self
    def to_json(self):
        return self.v
    def from_suit(self, v):
        self.v = int(v)
        return self
    def to_suit(self):
        return self.v
    def to_debug(self, indent):
        return str(self.v)

class SUITPosInt(SUITInt):
    def from_json(self, v):
        _v = int(v)
        if _v < 0:
            raise Exception('Positive Integers must be >= 0')
        self.v = _v
        return self
    def from_suit(self, v):
        return self.from_json(v)

class SUITManifestDict:
    def mkfields(d):
        # rd = {}
        return {k: ManifestKey(*v) for k,v in d.items()}

    def __init__(self):
        pass
    def from_json(self, data):
        for k, f in self.fields.items():
            v = data.get(f.json_key, None)
            setattr(self, k, f.obj().from_json(v) if v is not None else None)
        return self

    def to_json(self):
        j = {}
        for k, f in self.fields.items():
            v = getattr(self, k)
            if v:
                j[f.json_key] = v.to_json()
        return j

    def from_suit(self, data):
        for k, f in self.fields.items():
            v = data.get(f.suit_key, None)
            d = f.obj().from_suit(v) if v is not None else None
            setattr(self, k, d)
        return self

    def to_suit(self):
        sd = {}
        for k, f in self.fields.items():
            v = getattr(self, k)
            if v:
                sd[f.suit_key] = v.to_suit()
        return sd
    def to_debug(self, indent):
        s = '{'
        newindent = indent + one_indent

        for k, f in self.fields.items():
            v = getattr(self, k)
            if v:
                s += '\n{ind}/ {jk} / {sk}:'.format(ind=newindent, jk=f.json_key, sk=f.suit_key)
                s += v.to_debug(newindent) + ','
        s += '\n' + indent + '}'
        return s


class SUITManifestNamedList(SUITManifestDict):
    def from_suit(self, data):
        for k, f in self.fields.items():
            setattr(self, k, f.obj().from_suit(data[f.suit_key]))
        return self

    def to_suit(self):
        sd = [None] * (max([f.suit_key for k, f in self.fields.items()]) + 1)
        for k, f in self.fields.items():
            v = getattr(self, k)
            if v:
                sd[f.suit_key] = v.to_suit()
        return sd
    def to_debug(self, indent):
        newindent = indent + one_indent
        items = []
        for k, f in self.fields.items():
            v = getattr(self, k)
            if v:
                items.append('/ ' + f.json_key + ' / ' + v.to_debug(newindent))
        s = '[\n{newindent}{items}\n{indent}]'.format(
            newindent=newindent,
            indent=indent,
            items=',\n{newindent}'.format(newindent=newindent).join(items)
        )
        return s

class SUITKeyMap:
    def mkKeyMaps(m):
        return {v:k for k,v in m.items()}, m
    def to_json(self):
        return self.rkeymap[self.v]
    def from_json(self, d):
        self.v = self.keymap[d]
        return self
    def to_suit(self):
        return self.v
    def from_suit(self, d):
        self.v = self.keymap[self.rkeymap[d]]
        return self
    def to_debug(self, indent):
        s = str(self.v) + ' / ' + self.to_json() + ' /'
        return s

def SUITBWrapField(c):
    class SUITBWrapper:
        def to_suit(self):
            return cbor.dumps(self.v.to_suit(), sort_keys=True)
        def from_suit(self, d):
            self.v = c().from_suit(cbor.loads(d))
            return self
        def to_json(self):
            return self.v.to_json()
        def from_json(self, d):
            self.v = c().from_json(d)
            return self
        def to_debug(self, indent):
            s = 'h\''
            s += binascii.b2a_hex(self.to_suit()).decode('utf-8')
            s += '\' / '
            s += self.v.to_debug(indent)
            s += ' /'
            return s

    return SUITBWrapper

class SUITManifestArray:
    def __init__(self):
        self.items=[]
    def __eq__(self, rhs):
        if len(self.items) != len(rhs.items):
            return False
        for a,b in zip(self.items, rhs.items):
            if not a == b:
                return False
        return True

    def from_json(self, data):
        self.items = []
        for d in data:
            self.items.append(self.field.obj().from_json(d))
        return self

    def to_json(self):
        j = []
        for i in self.items:
            j.append(i.to_json())
        return j

    def from_suit(self, data):
        self.items = []
        for d in data:
            self.items.append(self.field.obj().from_suit(d))
        return self

    def to_suit(self):
        l = []
        for i in self.items:
            l.append(i.to_suit())
        return l

    def append(self, element):
        if not isinstance(element, self.field.obj):
            raise Exception('element {} is not a {}'.format(element, self.field.obj))
        self.items.append(element)

    def to_debug(self, indent):
        newindent = indent + one_indent
        s = '[\n'
        s += ' ,\n'.join([newindent + v.to_debug(newindent) for v in self.items])
        s += '\n' + indent + ']'
        return s
class SUITBytes:
    def to_json(self):
        return binascii.b2a_hex(self.v).decode('utf-8')
    def from_json(self, d):
        self.v = to_bytes(d)
        return self
    def from_suit(self, d):
        self.v = bytes(d)
        return self
    def to_suit(self):
        return self.v
    def to_debug(self, indent):
        return 'h\'' + self.to_json() + '\''
    def __eq__(self, rhs):
        return self.v == rhs.v

class SUITUUID(SUITBytes):
    def from_json(self, d):
        self.v = uuid.UUID(d).bytes
        return self
    def from_suit(self, d):
        self.v = uuid.UUID(bytes=d).bytes
        return self
    def to_debug(self, indent):
        return 'h\'' + self.to_json() + '\' / ' + str(uuid.UUID(bytes=self.v)) + ' /'


class SUITRaw:
    def to_json(self):
        return self.v
    def from_json(self, d):
        self.v = d
        return self
    def to_suit(self):
        return self.v
    def from_suit(self, d):
        self.v = d
        return self
    def to_debug(self, indent):
        return str(self.v)

class SUITNil:
    def to_json(self):
        return None
    def from_json(self, d):
        if d is not None:
            raise Exception('Expected Nil')
        return self
    def to_suit(self):
        return None
    def from_suit(self, d):
        if d is not None:
            raise Exception('Expected Nil')
        return self
    def to_debug(self, indent):
        return 'F6 / nil /'

class SUITTStr(SUITRaw):
    def from_json(self, d):
        self.v = str(d)
        return self
    def from_suit(self, d):
        self.v = str(d)
        return self
    def to_debug(self, indent):
        return '\''+ str(self.v) + '\''

class SUITComponentId(SUITManifestArray):
    field = collections.namedtuple('ArrayElement', 'obj')(obj=SUITBytes)
    def to_debug(self, indent):
        newindent = indent + one_indent
        s = '[' + ''.join([v.to_debug(newindent) for v in self.items]) + ']'
        return s
    def __hash__(self):
        return hash(tuple([i.v for i in self.items]))

class SUITComponentIndex(SUITComponentId):
    def to_suit(self):
        return suitCommonInfo.component_id_to_index(self)
    def from_suit(self, d):
        return super(SUITComponentIndex, self).from_suit(
            suitCommonInfo.component_ids[d].to_suit()
        )
    def to_debug(self, indent):
        newindent = indent + one_indent
        s = '{suit} / [{dbg}] /'.format(
            suit=self.to_suit(),
            dbg=''.join([v.to_debug(newindent) for v in self.items])
        )
        return s


class SUITComponents(SUITManifestArray):
    field = collections.namedtuple('ArrayElement', 'obj')(obj=SUITComponentId)

    def from_suit(self, data):
        super(SUITComponents, self).from_suit(data)
        suitCommonInfo.component_ids = self.items
        return self

    def from_json(self, j):
        super(SUITComponents, self).from_json(j)
        suitCommonInfo.component_ids = self.items
        return self

class SUITDigestAlgo(SUITKeyMap):
    rkeymap, keymap = SUITKeyMap.mkKeyMaps({
        'sha224' : 1,
        'sha256' : 2,
        'sha384' : 3,
        'sha512' : 4
    })

class SUITDigest(SUITManifestNamedList):
    fields = SUITManifestNamedList.mkfields({
        'algo' : ('algorithm-id', 0, SUITDigestAlgo),
        'digest' : ('digest-bytes', 1, SUITBytes)
    })

class SUITCompressionInfo(SUITKeyMap):
    rkeymap, keymap = SUITKeyMap.mkKeyMaps({
        'gzip' : 1,
        'bzip2' : 2,
        'deflate' : 3,
        'lz4' : 4,
        'lzma' : 7
    })

class SUITParameters(SUITManifestDict):
    fields = SUITManifestDict.mkfields({
        'digest' : ('image-digest', 11, SUITDigest),
        'size' : ('image-size', 12, SUITPosInt),
        'uri' : ('uri', 6, SUITTStr),
        'src' : ('source-component', 10, SUITComponentIndex),
        'compress' : ('compression-info', 8, SUITCompressionInfo)
    })
    def from_json(self, j):
        return super(SUITParameters, self).from_json(j)

class SUITTryEach(SUITManifestArray):
    pass

def SUITCommandContainer(jkey, skey, argtype):
    class SUITCmd(SUITCommand):
        json_key = jkey
        suit_key = skey
        def __init__(self):
            pass
        def to_suit(self):
            return [self.suit_key, self.arg.to_suit()]
        def to_json(self):
            if self.json_key == 'directive-set-component-index':
                return {}
            else:
                return {
                    'command-id' : self.json_key,
                    'command-arg' : self.arg.to_json(),
                    'component-id' : self.cid.to_json()
                }
        def from_json(self, j):
            if j['command-id'] != self.json_key:
                raise Except('JSON Key mismatch error')
            if self.json_key != 'directive-set-component-index':
                self.cid = SUITComponentId().from_json(j['component-id'])
            self.arg = argtype().from_json(j['command-arg'])
            return self
        def from_suit(self, s):
            if s[0] != self.suit_key:
                raise Except('SUIT Key mismatch error')
            if self.json_key == 'directive-set-component-index':
                suitCommonInfo.current_index = s[1]
            else:
                self.cid = suitCommonInfo.component_ids[suitCommonInfo.current_index]
            self.arg = argtype().from_suit(s[1])
            return self
        def to_debug(self, indent):
            s = '/ {} / {},'.format(self.json_key, self.suit_key)
            s += self.arg.to_debug(indent)
            return s
    return SUITCmd


class SUITCommand:
    def from_json(self, j):
        return self.jcommands[j['command-id']]().from_json(j)
    def from_suit(self, s):
        return self.scommands[s[0]]().from_suit(s)

SUITCommand.commands = [
    SUITCommandContainer('condition-vendor-identifier',    1,  SUITUUID),
    SUITCommandContainer('condition-class-identifier',     2,  SUITUUID),
    SUITCommandContainer('condition-image-match',          3,  SUITNil),
    SUITCommandContainer('condition-use-before',           4,  SUITRaw),
    SUITCommandContainer('condition-component-offset',     5,  SUITRaw),
    SUITCommandContainer('condition-custom',               6,  SUITRaw),
    SUITCommandContainer('condition-device-identifier',    24, SUITRaw),
    SUITCommandContainer('condition-image-not-match',      25, SUITRaw),
    SUITCommandContainer('condition-minimum-battery',      26, SUITRaw),
    SUITCommandContainer('condition-update-authorised',    27, SUITRaw),
    SUITCommandContainer('condition-version',              28, SUITRaw),
    SUITCommandContainer('directive-set-component-index',  12, SUITPosInt),
    SUITCommandContainer('directive-set-dependency-index', 13, SUITRaw),
    SUITCommandContainer('directive-abort',                14, SUITRaw),
    SUITCommandContainer('directive-try-each',             15, SUITTryEach),
    SUITCommandContainer('directive-process-dependency',   18, SUITRaw),
    SUITCommandContainer('directive-set-parameters',       19, SUITParameters),
    SUITCommandContainer('directive-override-parameters',  20, SUITParameters),
    SUITCommandContainer('directive-fetch',                21, SUITNil),
    SUITCommandContainer('directive-copy',                 22, SUITRaw),
    SUITCommandContainer('directive-run',                  23, SUITRaw),
    SUITCommandContainer('directive-wait',                 29, SUITRaw),
    SUITCommandContainer('directive-run-sequence',         30, SUITRaw),
    SUITCommandContainer('directive-run-with-arguments',   31, SUITRaw),
    SUITCommandContainer('directive-swap',                 32, SUITRaw),
]
SUITCommand.jcommands = { c.json_key : c for c in SUITCommand.commands}
SUITCommand.scommands = { c.suit_key : c for c in SUITCommand.commands}


class SUITSequence(SUITManifestArray):
    field = collections.namedtuple('ArrayElement', 'obj')(obj=SUITCommand)
    def to_suit(self):
        suit_l = []
        suitCommonInfo.current_index = 0 if len(suitCommonInfo.component_ids) == 1 else None
        for i in self.items:
            if i.json_key == 'directive-set-component-index':
                suitCommonInfo.current_index = i.arg.v
            else:
                cidx = suitCommonInfo.component_id_to_index(i.cid)
                if cidx != suitCommonInfo.current_index:
                    # Change component
                    cswitch = SUITCommand().from_json({
                        'command-id' : 'directive-set-component-index',
                        'command-arg' : cidx
                    })
                    suitCommonInfo.current_index = cidx
                    suit_l += cswitch.to_suit()
            suit_l += i.to_suit()
        return suit_l
    def to_debug(self, indent):
        return super(SUITSequence, SUITSequence().from_suit(self.to_suit())).to_debug(indent)
    def from_suit(self, s):
        self.items = [SUITCommand().from_suit(i) for i in zip(*[iter(s)]*2)]
        return self

SUITTryEach.field = collections.namedtuple('ArrayElement', 'obj')(obj=SUITSequence)

class SUITSequenceComponentReset(SUITSequence):
    def to_suit(self):
        suitCommonInfo.current_index = None
        return super(SUITSequenceComponentReset, self).to_suit()

def SUITMakeSeverableField(c):
    class SUITSeverableField:
        objtype = SUITBWrapField(c)
        def from_json(self, data):
            if 'algorithm-id' in data:
                self.v = SUITDigest().from_json(data)
            else:
                self.v = self.objtype().from_json(data)
            return self
        def from_suit(self, data):
            if isinstance(data, list):
                self.v = SUITDigest().from_suit(data)
            else:
                self.v = self.objtype().from_suit(data)
            return self
        def to_json(self):
            return self.v.to_json()
        def to_suit(self):
            return self.v.to_suit()
        def to_debug(self, indent):
            return self.v.to_debug(indent)
    return SUITSeverableField
# class SUITSequenceOrDigest()

class SUITCommon(SUITManifestDict):
    fields = SUITManifestNamedList.mkfields({
        # 'dependencies' : ('dependencies', 1, SUITBWrapField(SUITDependencies)),
        'components' : ('components', 2, SUITBWrapField(SUITComponents)),
        # 'dependency_components' : ('dependency-components', 3, SUITBWrapField(SUITDependencies)),
        'common_sequence' : ('common-sequence', 4, SUITBWrapField(SUITSequenceComponentReset)),
    })


class SUITManifest(SUITManifestDict):
    fields = SUITManifestDict.mkfields({
        'version' : ('manifest-version', 1, SUITPosInt),
        'sequence' : ('manifest-sequence-number', 2, SUITPosInt),
        'common' : ('common', 3, SUITBWrapField(SUITCommon)),
        'deres' : ('dependency-resolution', 7, SUITMakeSeverableField(SUITSequenceComponentReset)),
        'fetch' : ('payload-fetch', 8, SUITMakeSeverableField(SUITSequenceComponentReset)),
        'install' : ('install', 9, SUITMakeSeverableField(SUITSequenceComponentReset)),
        'validate' : ('validate', 10, SUITBWrapField(SUITSequenceComponentReset)),
        'load' : ('load', 11, SUITBWrapField(SUITSequenceComponentReset)),
        'run' : ('run', 12, SUITBWrapField(SUITSequenceComponentReset)),
    })

class COSE_Algorithms(SUITKeyMap):
    rkeymap, keymap = SUITKeyMap.mkKeyMaps({
        'ES256' : -7,
        'ES384' : -35,
        'ES512' : -36,
        'EdDSA' : -8,
        'HSS-LMS' : -46,
    })

class COSE_CritList(SUITManifestArray):
    field = collections.namedtuple('ArrayElement', 'obj')(obj=SUITInt)

class COSE_header_map(SUITManifestDict):
    fields = SUITManifestDict.mkfields({
        # 1: algorithm Identifier
        'alg' : ('alg', 1, COSE_Algorithms),
        # 2: list of critical headers (criticality)
        # 3: content type
        # 4: key id
        'kid' : ('kid', 4, SUITBytes),
        # 5: IV
        # 6: partial IV
        # 7: counter signature(s)
    })

class COSE_Sign:
    pass
class COSE_Sign1(SUITManifestNamedList):
    fields = SUITManifestDict.mkfields({
        'protected' : ('protected', 0, SUITBWrapField(COSE_header_map)),
        'unprotected' : ('unprotected', 1, COSE_header_map),
        'payload' : ('payload', 2, SUITBWrapField(SUITDigest)),
        'signature' : ('signature', 3, SUITBytes)
    })
class COSE_Mac:
    pass
class COSE_Mac0:
    pass

class COSETagChoice(SUITManifestDict):
    def to_suit(self):
        for k, f in self.fields.items():
            v = getattr(self, k, None)
            if v:
                return cbor.Tag(tag=f.suit_key, value=v.to_suit())
        return None

    def from_suit(self, data):
        for k, f in self.fields.items():
            if data.tag == f.suit_key:
                v = data.value
                d = f.obj().from_suit(v) if v is not None else None
                setattr(self, k, d)
        return self


    def to_debug(self, indent):
        s = ''
        for k, f in self.fields.items():
            if hasattr(self, k):
                v = getattr(self, k)
                newindent = indent + one_indent
                s = '{tag}({value})'.format(tag=f.suit_key, value=v.to_debug(newindent))
        return s

class COSETaggedAuth(COSETagChoice):
    fields = SUITManifestDict.mkfields({
        'cose_sign' : ('COSE_Sign_Tagged', 98, COSE_Sign),
        'cose_sign1' : ('COSE_Sign1_Tagged', 18, COSE_Sign1),
        'cose_mac' : ('COSE_Mac_Tagged', 97, COSE_Mac),
        'cose_mac0' : ('COSE_Mac0_Tagged', 17, COSE_Mac0)
    })

class COSEList(SUITManifestArray):
    field = collections.namedtuple('ArrayElement', 'obj')(obj=COSETaggedAuth)
    def from_suit(self, data):
        return super(COSEList, self).from_suit(data)

class SUITWrapper(SUITManifestDict):
    fields = SUITManifestDict.mkfields({
        'auth' : ('authentication-wrapper', 2, SUITBWrapField(COSEList)),
        'manifest' : ('manifest', 3, SUITBWrapField(SUITManifest)),
        'deres': ('dependency-resolution', 7, SUITBWrapField(SUITSequence)),
        'fetch': ('payload-fetch', 8, SUITBWrapField(SUITSequence)),
        'install': ('install', 9, SUITBWrapField(SUITSequence)),
        'validate': ('validate', 10, SUITBWrapField(SUITSequence)),
        'load': ('load', 11, SUITBWrapField(SUITSequence)),
        'run': ('run', 12, SUITBWrapField(SUITSequence)),
        # 'text': ('text', 13, SUITBWrapField(SUITSequence)),
    })
    severable_fields = {'deres', 'fetch', 'install'} #, 'text'}
    digest_algorithms = {
        'sha224' : hashes.SHA224,
        'sha256' : hashes.SHA256,
        'sha384' : hashes.SHA384,
        'sha512' : hashes.SHA512
    }
    def to_severable(self, digest_alg):
        sev = copy.deepcopy(self)
        for k in sev.severable_fields:
            f = sev.manifest.v.fields[k]
            if not hasattr(sev.manifest.v, k):
                continue
            v = getattr(sev.manifest.v, k)
            if v is None:
                continue
            cbor_field = cbor.dumps(v.to_suit(), sort_keys=True)
            digest = hashes.Hash(digest_algorithms.get(digest_alg)(), backend=default_backend())
            digest.update(cbor_field)
            field_digest = SUITDigest().from_json({
                'algorithm-id' : digest_alg,
                'digest-bytes' : digest.finalize()
            })
            cbor_digest = cbor.dumps(field_digest.to_suit(), sort_keys=True)
            if len(cbor_digest) < len(cbor_field):
                setattr(sev.manifest.v, k, field_digest)
                setattr(sev,k,v)
        return sev
    def from_severable(self):
        raise Exception('From Severable unimplemented')
        nsev = copy.deepcopy(self)
        for k in nsev.severable_fields:
            f = nsev.fields[k]
            if not hasattr(nsev, k):
                continue
            v = getattr(nsev, k)
            if v is None:
                continue
            # Verify digest
            cbor_field = cbor.dumps(v.to_suit(), sort_keys=True)
            digest = hashes.Hash(hashes.SHA256(), backend=default_backend())
            digest.update(cbor_field)
            actual_digest = digest.finalize()
            field_digest = getattr(sev.nsev.v, k)
            expected_digest = field_digest.to_suit()[1]
            if digest != expected_digest:
                raise Exception('Field Digest mismatch: For {}, expected: {}, got {}'.format(
                    f.json_key, expected_digest, actual_digest
                ))
            setattr(nsev.manifest.v, k, v)
            delattr(nsev, k)
        return nsev
