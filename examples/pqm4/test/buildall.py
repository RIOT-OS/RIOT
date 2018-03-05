#!/bin/env python3
import os
import subprocess

kems = [
    'frodo640-cshake',
    'kindi256342',
    'kyber768',
    'newhope1024cca',
    'ntruhrss701',
    'saber',
    'sikep751',
    'sntrup4591761'
]


signs = [
    'dilithium',
    'qTesla-I',
    'qTesla-III-size',
    'qTesla-III-speed',
    'sphincs-shake256-128s'
]

makeflags = ['BOARD=native']


def build(flags, name):
    # Clean build dir
    if os.path.exists('./bin'):
        subprocess.call(['/bin/rm', '-rf', './bin'])
    # Build
    cmd = ['/bin/make'] + flags
    print(cmd)
    with open(name + '.log', 'w') as out:
        subprocess.call(cmd, env=dict(os.environ, CFLAGS="-DNATIVE_AUTO_EXIT"), stdout=out, stderr=out)


for kem in kems:
    flags = makeflags + ["PQ_KEM=" + kem, 'all', 'term']
    build(flags, kem.replace('/', '_'))

for sign in signs:
    flags = makeflags + ["PQ_SIGN=" + sign, 'all', 'term']
    build(flags, sign.replace('/', '_'))
