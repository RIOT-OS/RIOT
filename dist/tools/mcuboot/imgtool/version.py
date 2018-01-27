"""
Semi Semantic Versioning

Implements a subset of semantic versioning that is supportable by the image header.
"""

import argparse
from collections import namedtuple
import re

SemiSemVersion = namedtuple('SemiSemVersion', ['major', 'minor', 'revision', 'build'])

version_re = re.compile(r"""^([1-9]\d*|0)(\.([1-9]\d*|0)(\.([1-9]\d*|0)(\+([1-9]\d*|0))?)?)?$""")
def decode_version(text):
    """Decode the version string, which should be of the form maj.min.rev+build"""
    m = version_re.match(text)
    # print("decode:", text, m.groups())
    if m:
        result = SemiSemVersion(
                int(m.group(1)) if m.group(1) else 0,
                int(m.group(3)) if m.group(3) else 0,
                int(m.group(5)) if m.group(5) else 0,
                int(m.group(7)) if m.group(7) else 0)
        return result
    else:
        msg = "Invalid version number, should be maj.min.rev+build with later parts optional"
        raise argparse.ArgumentTypeError(msg)

if __name__ == '__main__':
    print(decode_version("1.2"))
    print(decode_version("1.0"))
    print(decode_version("0.0.2+75"))
    print(decode_version("0.0.0+00"))
