#!/usr/bin/env python3

import os
import sys
import argparse
import pathlib
import posixpath
import io
import itertools
import mmap
import shutil
import configparser
import codecs
from functools import partial
from binascii import hexlify


def _relpath_p(path, start):
    return posixpath.relpath(pathlib.Path(os.path.abspath(path)).as_posix(),
                             pathlib.Path(os.path.abspath(start)).as_posix())


def mkconstfs(templates, files, root_path, mount_point, constfs_name):
    """Generate a C file containing a constant file system

    Return
    ------

    chunks: Iterator yielding fragments of the of the output file.
    """
    def _d(x):
        return codecs.decode(x, "unicode-escape")

    FILE_TEMPLATE = _d(templates['Files']['template'])

    _print_file_data = partial(print_file_data, _d(templates['Blob']['header']),
                               _d(templates['Blob']['footer']))

    filemap = {f: (_mkident(i), _relpath_p(f, root_path))
               for i, f in enumerate(files)}

    yield _d(templates['Main']['header'])
    yield from itertools.chain.from_iterable(
                _print_file_data(local_f, *f_data) for local_f, f_data in filemap.items())

    yield _d(templates['Files']['header'])

    yield from (FILE_TEMPLATE.format(target_name=relp,
                                     target_name_addroot=_addroot(relp),
                                     buff_name=ident)
                for ident, relp in sorted(filemap.values()))

    yield _d(templates['Files']['footer'])

    yield _d(templates['Main']['footer']).format(constfs_name=constfs_name,
                                                 mount_point=mount_point)


def _addroot(fname):
    return "/" + fname if not fname.startswith("/") else fname


def _mkident(k):
    return "_file{:02X}".format(k)


def print_file_data(header, footer, local_fname, varname, target_fname=""):
    """Convert a file into a static C array:

    Parameters
    ----------

    local_fname: real Path (where the file is on this machine's fs)
    target_fname: name that the file will have in the constfs.
    output_file: File-like object where the array will be written.

    Return
    ------

    chunks: Iterator yielding fragments of the of the output text.
    """

    yield header.format(fname=target_fname, varname=varname)

    def byte2s(b):
        return "0x{},".format(hexlify(b).decode('utf-8'))

    def chunk(iterable, blocksize):
        """Break a single iterable into chunks of maximum size 'blocksize'"""
        return (x for _, x in itertools.groupby(enumerate(iterable),
                                                lambda x: x[0]//blocksize))

    with open(local_fname, 'rb') as f, mmap.mmap(f.fileno(), 0,
                                                 access=mmap.ACCESS_READ
                                                 ) as bfile:
        yield from map(lambda x: x[1],
                       itertools.chain.from_iterable(
                            map(lambda l: itertools.chain(l, [(0, "\n")]),
                                chunk(map(byte2s, bfile), 16)
                                )
                        )
                       )

    yield footer


def main():
    this_script_dir = os.path.dirname(os.path.realpath(sys.argv[0]))
    default_templates = os.path.join(this_script_dir, 'default.ini')

    parser = argparse.ArgumentParser(
            description="Embed files into a constant file system. "
            "See %s to learn how to customize the generator." % default_templates)

    parser.add_argument("-m", '--mount', metavar="mountpoint",
                        help="Where to mount the resulting fs", default="/")

    parser.add_argument("-t", '--template', metavar="ini_file",
                        help="Configuration file containing template chunks",
                        type=argparse.FileType(),
                        default=default_templates)

    parser.add_argument("-o", '--output', metavar="output_file",
                        help="Write the output to a file instead of stdout. "
                             "The file is only written if the command is successful "
                             "(i.e. there is no partial output")

    parser.add_argument("-r", '--root', metavar="root_base_path",
                        type=pathlib.Path,
                        help="Paths on the constf will be generated for the real "
                        "path of the files by considering this path to be the root "
                        "By default the current directory (.) is used",
                        default=pathlib.Path())

    parser.add_argument("name", help="Name for the vfs_mount_t structure")

    parser.add_argument("files", nargs="+", type=pathlib.Path,
                        help="Files to be included.")

    ns = parser.parse_args()

    template = configparser.ConfigParser(comment_prefixes=())
    template.read_file(ns.template)

    f_chunks = mkconstfs(template, ns.files, ns.root, ns.mount, ns.name)

    if ns.output:
        tmp_out = io.StringIO()
    else:
        tmp_out = sys.stdout

    tmp_out.writelines(f_chunks)

    if ns.output:
        with open(ns.output, "w+") as f:
            tmp_out.seek(0)
            shutil.copyfileobj(tmp_out, f)

    return 0


if __name__ == "__main__":
    exit(main())
