#!/usr/bin/env python3
"""
Derive a key from a password using pbkdf2/sha256.
"""

import os
import hashlib
import textwrap

password = b"Passw0rd!"
salt = os.urandom(64)
iterations = 1000

def _inner_array(a):
    return ", ".join(f"0x{c:X}" for c in a)

def _to_array(name, a):
    return f"static const uint8_t {name}[] = {{{_inner_array(a)}}};"

def pretty_indent(s):
    initial_brace = " " * (1 + s.index("{"))
    wrapper = textwrap.TextWrapper(width=79, subsequent_indent=initial_brace)

    return "\n".join(wrapper.wrap(s))

def my_pbkdf(password, salt, iterations):
    """This is derived from the implementation in python's hashlib, heavily
    simplified. The C version is a translation of this code.
    """
    first = True

    # Fast inline HMAC implementation
    inner = hashlib.new('sha256')
    outer = hashlib.new('sha256')
    blocksize = inner.block_size

    if len(password) > blocksize:
        password = hashlib.new('sha256', password).digest()

    password = password + b'\x00' * (blocksize - len(password))

    password = bytes((x ^ 0x36) for x in password)
    inner.update(password)

    password = bytes((x ^ 0x36 ^ 0x5C) for x in password)
    outer.update(password)

    dklen = outer.digest_size

    rkey = b"\x00"*dklen

    for _ in range(iterations):
        icpy = inner.copy()
        ocpy = outer.copy()

        if first:
            icpy.update(salt)
            icpy.update(b'\x00\x00\x00\x01')
            first = False
        else:
            icpy.update(prev)

        prev = icpy.digest()
        ocpy.update(prev)
        prev = ocpy.digest()
        rkey = bytes(x^y for x, y in zip(rkey,prev))

    return rkey

key = hashlib.pbkdf2_hmac('sha256', password, salt, iterations)
key2 = my_pbkdf(password, salt, iterations)

print(f"#define PBKDF2_ITERS {iterations}")
print(pretty_indent(_to_array('salt', salt)))
print(pretty_indent(_to_array('key', key)))
print(pretty_indent(_to_array('key2', key2)))

# Check that my ugly version gives the same results as the official one.
print(key==key2)
print(len(key))
