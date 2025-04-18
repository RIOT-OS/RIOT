#!/usr/bin/env python3
"""
Command line utility to generate compile_commands.json for RIOT applications
"""
import argparse
import json
import os
import re
import shlex
import subprocess
import sys

REGEX_VERSION = re.compile(r"\ngcc version ([^ ]+)")
REGEX_INCLUDES = r"^#include <\.\.\.> search starts here:$((?:\n|\r|.)*?)^End of search list\.$"
REGEX_INCLUDES = re.compile(REGEX_INCLUDES, re.MULTILINE)


def detect_includes_and_version_gcc(compiler):
    """
    Runs the given compiler with -v -E on an no-op compilation unit and parses
    the built-in include search directories and the GCC version from the output

    :param compiler: name / path of the compiler to run
    :type compiler: str

    :return: (list_of_include_paths, version)
    :rtype: tuple
    """
    try:
        process_env = dict(os.environ)
        process_env["LC_MESSAGES"] = "C"
        with subprocess.Popen([compiler, "-v", "-E", "-"],
                              stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE, env=process_env) as proc:
            inputdata = b"typedef int dont_be_pedantic;"
            _, stderrdata = proc.communicate(input=inputdata)
    except FileNotFoundError:
        msg = f"Compiler {compiler} not found, not adding system include paths\n"
        sys.stderr.write(msg)
        return ([], "")

    stderrdata = stderrdata.decode("utf-8")
    version = REGEX_VERSION.search(stderrdata).group(1)
    tmp = [os.path.abspath(p) for p in REGEX_INCLUDES.search(stderrdata).group(1).split()]

    # the include path containing newlib.h must come prior to the gcc headers
    # in the include list to avoid mischief from happening
    newlib_path = ""
    includes = []
    for path in tmp:
        if os.path.exists(os.path.join(path, "newlib.h")):
            newlib_path = path
            includes.append(path)
            break

    for path in tmp:
        if path != newlib_path:
            includes.append(path)

    return (includes, version)


def detect_libstdcxx_includes(compiler, includes, version):
    """
    Tries to detect the g++ libstdc++ built-in include search directories using
    black magic and adds them to the list given in includes

    :param compiler: Name or path of the compiler
    :type compiler: str
    :param includes: List of include directories
    :type includes: list of str
    :param version: Version of g++
    :type version: str
    """
    for path in includes:
        cxx_lib = os.path.join(path, "c++", version)
        if os.path.exists(cxx_lib):
            includes.append(cxx_lib)
            triple = os.path.basename(compiler)[0:-4]
            cxx_extra = os.path.join(cxx_lib, triple)
            if os.path.exists(cxx_extra):
                includes.append(cxx_extra)
            break


def detect_built_in_includes(compiler, args):
    """
    Tries to detect the built-in include search directories of the given
    compiler

    :param compiler: Name or path of the compiler
    :type compiler: str
    :param args: Command line arguments

    :return: List of built-in include directories
    :rtype: list of str
    """
    if compiler.endswith('-gcc'):
        includes, version = detect_includes_and_version_gcc(compiler)
    elif compiler.endswith('-g++'):
        includes, version = detect_includes_and_version_gcc(compiler)
        if args.add_libstdcxx_includes:
            detect_libstdcxx_includes(compiler, includes, version)
    elif compiler in ('clang', 'clang++', 'gcc', 'g++'):
        # clang / clang++ doesn't have any magic include search dirs built in,
        # so we don't need to detect them.
        # for host gcc/g++ we don't need to detect magic include dirs either.
        includes = []
    else:
        msg = f"Warning: Cannot detect default include search paths for {compiler}\n"
        sys.stderr.write(msg)
        includes = []

    return includes


class CompilationDetails:  # pylint: disable=too-few-public-methods,too-many-instance-attributes
    """
    Representation of the compilation details stored by RIOT's build system.

    :param path: Path of the module to read compilation details from
    :type path: str
    """
    def __init__(self, path):  # pylint: disable=too-many-branches
        with open(os.path.join(path, "compile_cmds.txt"), "r") as file:
            for line in file.read().splitlines():
                if line.startswith("SRC: "):
                    self.src_c = line.lstrip("SRC: ").split()
                elif line.startswith("SRC_NO_LTO: "):
                    self.src_c_no_lto = line.lstrip("SRC_NO_LTO: ").split()
                elif line.startswith("SRCXX: "):
                    self.src_cxx = line.lstrip("SRCXX: ").split()
                elif line.startswith("CURDIR: "):
                    self.dir = line.lstrip("CURDIR: ").strip()
                elif line.startswith("CFLAGS: "):
                    self.cflags = shlex.split(line.lstrip("CFLAGS: "))
                elif line.startswith("LTOFLAGS: "):
                    self.ltoflags = shlex.split(line.lstrip("LTOFLAGS: "))
                elif line.startswith("INCLUDES: "):
                    self.includes = shlex.split(line.lstrip("INCLUDES: "))
                elif line.startswith("CXXFLAGS: "):
                    self.cxxflags = shlex.split(line.lstrip("CXXFLAGS: "))
                elif line.startswith("CXXINCLUDES: "):
                    self.cxxincludes = shlex.split(line.lstrip("CXXINCLUDES: "))
                elif line.startswith("CC: "):
                    self.cc = line.lstrip("CC: ").strip()  # pylint: disable=invalid-name
                elif line.startswith("CXX: "):
                    self.cxx = line.lstrip("CXX: ").strip()
                elif line.startswith("TARGET_ARCH: "):
                    self.target_arch = line.lstrip("TARGET_ARCH: ").strip()
                elif line.startswith("TARGET_ARCH_LLVM: "):
                    self.target_arch_llvm = line.lstrip("TARGET_ARCH_LLVM: ").strip()


class State:  # pylint: disable=too-few-public-methods
    """
    Entity to store the current programs state
    """
    def __init__(self):
        self.def_includes = dict()
        self.is_first = True


def get_built_in_include_flags(compiler, state, args):
    """
    Get built-in include search directories as parameter list.

    :param compiler: Name or path of the compiler to get the include search
                     dirs from
    :type compiler: str
    :param state: state of the program
    :param args: command line arguments

    :return: The -isystem <...> compiler flags for the built-in include search
             dirs as list
    :rtype: list
    """

    result = []
    if compiler not in state.def_includes:
        state.def_includes[compiler] = detect_built_in_includes(compiler, args)

    for include in state.def_includes[compiler]:
        result.append('-isystem')
        result.append(include)

    return result


def write_compile_command(state, compiler, src, flags, cdetails, path):
    """
    Write the compile command for the given source file with the given
    parameters to stdout

    :param state: state of the program
    :param compiler: the C/C++ compiler used
    :type compiler: str
    :param src: the file to compiler
    :type src: str
    :param flags: flags used for compiler invocation
    :type flags: list of str
    :param cetails: compilation details
    :type cdetails: CompilationDetails
    :param path: the output path
    "type path: str
    """
    if state.is_first:
        state.is_first = False
    else:
        sys.stdout.write(",\n")
    obj = os.path.splitext(src)[0] + ".o"
    arguments = [compiler, '-DRIOT_FILE_RELATIVE="' + os.path.join(cdetails.dir, src) + '"',
                 '-DRIOT_FILE_NOPATH="' + src + '"']
    arguments += flags
    if '-c' in arguments:
        # bindgen is unhappy with multiple -c (that would be created by the -c
        # added later) and even with the -c showing up anywhere between other
        # arguments.
        assert arguments.count('-c') == 1, "Spurious duplicate -c arguments"
        arguments.remove('-c')
    arguments += ['-MQ', obj, '-MD', '-MP', '-c', '-o', obj, src]
    entry = {
        'arguments': arguments,
        'directory': cdetails.dir,
        'file': os.path.join(cdetails.dir, src),
        'output': os.path.join(path, obj)
    }
    sys.stdout.write(json.dumps(entry, indent=2))


def generate_module_compile_commands(path, state, args):
    """
    Generate section of compile_commands.json for the module in path and write
    it to stdout.

    :param path: path of the module's bin folder to emit the
                 compile_commands.json chunk for
    :type path: str
    :param state: state of the program
    :param args: command line arguments
    """
    cdetails = CompilationDetails(path)

    for flag in args.filter_out:
        try:
            cdetails.cflags.remove(flag)
        except ValueError:
            pass
        try:
            cdetails.cxxflags.remove(flag)
        except ValueError:
            pass

    if args.clangd:
        cdetails.cflags.append('-Wno-unknown-warning-option')

    c_extra_includes = []
    cxx_extra_includes = []

    if args.add_built_in_includes:
        c_extra_includes = get_built_in_include_flags(cdetails.cc, state, args)
        cxx_extra_includes = get_built_in_include_flags(cdetails.cxx, state, args)

    if args.clangd:
        if cdetails.target_arch_llvm:
            cdetails.cflags += ['-target', cdetails.target_arch_llvm]
            cdetails.cxxflags += ['-target', cdetails.target_arch_llvm]
        elif cdetails.target_arch:
            cdetails.cflags += ['-target', cdetails.target_arch]
            cdetails.cxxflags += ['-target', cdetails.target_arch]

    for src in cdetails.src_c:
        compiler = 'clang' if args.clangd else cdetails.cc
        flags = cdetails.cflags + cdetails.ltoflags + cdetails.includes + c_extra_includes
        write_compile_command(state, compiler, src, flags, cdetails, path)

    for src in cdetails.src_c_no_lto:
        compiler = 'clang' if args.clangd else cdetails.cc
        flags = cdetails.cflags + cdetails.includes + c_extra_includes
        write_compile_command(state, compiler, src, flags, cdetails, path)

    for src in cdetails.src_cxx:
        compiler = 'clang++' if args.clangd else cdetails.cxx
        flags = cdetails.cxxflags + cdetails.cxxincludes + cdetails.includes + cxx_extra_includes
        write_compile_command(state, compiler, src, flags, cdetails, path)


def generate_compile_commands(args):
    """
    Generate the compile_commands.json content and write them to stdout

    :param args: command line arguments
    """
    state = State()
    sys.stdout.write("[\n")

    for module in os.scandir(args.path):
        if module.is_dir() and os.path.isfile(os.path.join(module.path, 'compile_cmds.txt')):
            generate_module_compile_commands(module.path, state, args)
    sys.stdout.write("\n]\n")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate compile_commands.json for RIOT apps')
    parser.add_argument('path', metavar='PATH', type=str,
                        help='Bin path, usually \'<APP>/bin/<BOARD>\'')
    parser.add_argument('--add-built-in-includes', default=False, action='store_const', const=True,
                        help='Explicitly add built in include search directories with -I<PATH> ' +
                             'options')
    parser.add_argument('--add-libstdcxx-includes', default=False, action='store_const', const=True,
                        help='Explicitly add libstdc++ include search directories with -I<PATH> ' +
                             'options')
    parser.add_argument('--filter-out', type=str, default=[], action='append',
                        help='Drop the given flag, if present (repeatable)')
    parser.add_argument('--clangd', default=False, action='store_const', const=True,
                        help='Shorthand for --add-built-in-includes --add-libstdxx-includes ' +
                             'and some CFLAG adjustments throughy --filter-out, and ignores ' +
                             'unknown warning flags')
    _args = parser.parse_args()
    if _args.clangd:
        _args.add_built_in_includes = True
        _args.add_libstdcxx_includes = True
        flags = [
            '-mno-thumb-interwork',
            # Only even included for versions of GCC that
            # support it
            '-misa-spec=2.2',
            '-malign-data=natural',
            # Only supported starting with clang 11
            '-msmall-data-limit=8',
            # not supported by clang, see
            # https://gcc.gnu.org/onlinedocs/gcc-10.2.0/gcc/Xtensa-Options.html
            '-mtext-section-literals',
            '-fstrict-volatile-bitfields',
            # it's called -mlong-calls in LLVM, but we don't need it for clangd
            # as we do not generate code anyway
            '-mlongcalls',
            # GCC specific diagnostics: Tell GCC address space starts at 0
            '--param=min-pagesize=0',
            # -fanalyzer is GCC only
            '-fanalyzer',
        ]
        _args.filter_out.extend(flags)
    generate_compile_commands(_args)
