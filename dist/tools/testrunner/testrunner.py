#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#               2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import click, glob, os, signal, sys, subprocess, json, copy
from pexpect import spawnu, TIMEOUT
from traceback import print_tb

import nodes as _nodes
import tests as _tests
import _thread

DEFAULT_TIMEOUT = 10

from util import listify, uniquify, deep_replace, dict_list_product, is_main_interrupted

class InvalidJSONException(Exception):
    pass

def merge(a, b, path=None):
    "merges b into a"
    if path is None: path = []
    for key in b:
        if key in a:
            if isinstance(a[key], dict) and isinstance(b[key], dict):
                merge(a[key], b[key], path + [str(key)])
            elif isinstance(a[key], set) and isinstance(b[key], set):
                a[key] = a[key] | b[key]
            elif isinstance(a[key], list) and isinstance(b[key], list):
                a[key] = uniquify(a[key] + b[key])
            elif a[key] == b[key]:
                pass # same leaf value
            else:
                raise Exception('Conflict at %s' % '.'.join(path + [str(key)]))
        else:
            a[key] = b[key]
    return a

def merge_override(a, b, path=None):
    "merges b into a. b overrides a"
    if path is None: path = []
    for key in b:
        if key in a:
            if isinstance(a[key], dict) and isinstance(b[key], dict):
                merge_override(a[key], b[key], path + [str(key)])
            elif isinstance(a[key], set) and isinstance(b[key], set):
                a[key] = a[key] | b[key]
            elif isinstance(a[key], list) and isinstance(b[key], list):
                a[key] = uniquify(a[key] + b[key])
            elif a[key] == b[key]:
                pass # same leaf value
            else:
                a[key] = b[key]
        else:
            a[key] = b[key]
    return a

def set_default(obj):
    if isinstance(obj, set):
        return list(obj)

def run(testfunc, timeout=DEFAULT_TIMEOUT, echo=True, traceback=False, extra_env=None):
    n = _nodes.RiotNode({"timeout": timeout, "env" : extra_env or {} })

    n.spawn()
    n.reset()

    try:
        testfunc(n.child)
    except TIMEOUT:
        print("Timeout in expect script")
        if traceback:
            print_tb(sys.exc_info()[2])
        return 1
    finally:
        print("")
        n.close()

    return 0

class IgnoredException(Exception):
    pass

def json_load(filename):
    def do_include(data):
        includes = listify(data.get("include"))
        for include in includes:
            _data = json_load(os.path.join(os.path.dirname(filename), include))[0]
            _data.pop("ignore", None)
            if "template" in _data:
                raise InvalidJSONException("template statement in included file currently not supported!")
            merge_override(_data, data)
            data = _data

        data.pop("include", None)
        return data

    with open(filename, 'r') as f:
        data = json.load(f)

    res = []
    template = data.get('template')

    if template:
        data.pop('template')
        result = []
        i = 0
        for repl in dict_list_product(template):
            _data = copy.deepcopy(data)
            _data = deep_replace(_data, repl)
            _data = do_include(_data)
            _data["template_instance"] = repl
            _data["template_instance_id"] = _data.get("template_instance_id", str(i))
            result.append(_data)
            i += 1
        return result
    else:
        data = do_include(data)
        return [data]

class NodeGroup(object):
    def __init__(s, filename, murdock):
        s.filename = filename
        s.nodes, s.murdock_queue = s.load_from_file()
        s.tests = []
        s.node_objs = None

        if murdock and not s.murdock_queue:
            print("testrunner: node group %s has no murdock queue set!" % filename,
                    file=sys.stderr)
            raise IgnoredException

    def load_from_file(s):
        node_data = json_load(s.filename)[0]
        if node_data.get("ignore", False):
            raise IgnoredException

        node_defaults = node_data.get("node_defaults", {})
        nodes = []
        for _node in node_data.get("nodes", []):
            node = copy.deepcopy(node_defaults)
            node['tags'] = set(listify(node.get('tags', [])))
            _node['tags'] = set(listify(_node.get('tags', [])))
            merge(node, _node)
            nodes.append(node)

        return nodes, node_data.get("murdock_queue")

    def load_all(paths, murdock):
        files = []
        for path in paths:
            if os.path.isdir(path):
                files.extend(glob.glob(path + "/**/*.json", recursive=True))
            elif os.path.isfile(path):
                files.append(path)

        node_groups = []
        for f in files:
            try:
                node_groups.append(NodeGroup(f,murdock))
            except IgnoredException:
                pass
        return node_groups

    def create_node_objs(s, test):
        node_objs = []
        for i, node in enumerate(test.match_nodegroup(s)):
            node = copy.deepcopy(node)
            merge(node, test.nodes[i])
            node_objs.append(_nodes.RiotNode(node))
        return node_objs

class Test(object):
    def __init__(s, data, filename):
        s.filename = filename
        s.node_groups = None
        s.test_class = None
        s.data = data

        if data.get("ignore", False):
            raise IgnoredException

        num_nodes = data.get("num_nodes", 1)
        node_data = data.get("nodes", [])

        nodes = []
        for i in range(0, num_nodes):
            node = copy.deepcopy(data.get("node_defaults", {}))
            node['tags'] = set(listify(node.get('tags', [])))
            if i < len(node_data):
                _node_data = node_data[i]
                _node_data['tags'] = set(listify(_node_data.get('tags', [])))
                merge(node, _node_data)

            nodes.append(node)

        s.nodes = nodes
        s.instance_id = data.get("template_instance_id", None)

    def load_all(paths):
        files = []
        for path in paths:
            if os.path.isdir(path):
                files.extend(glob.glob(path + "/**/*.json", recursive=True))
            elif os.path.isfile(path):
                files.append(path)

        tests = []
        for f in files:
            try:
                raw_data = listify(json_load(f))
                for entry in raw_data:
                    tests.append(Test(entry,f))
            except IgnoredException:
                pass
        return tests

    def match_nodegroup(s, node_group):
        used = set()
        result = []
        for needed in s.nodes:
            needed_tags = needed["tags"]
            for node in node_group.nodes:
                if node["name"] in used:
                    continue
                if set(needed_tags) & node["tags"] == set(needed_tags):
                    result.append(node)
                    used.add(node["name"])
                    break

        if len(result) == len(s.nodes):
            return result

def match_tests(tests, node_groups):
    possible = {}
    for test in tests:
        _possible = []
        for node_group in node_groups:
            if test.match_nodegroup(node_group):
                _possible.append(node_group)

        possible[test] = _possible

    return possible

def distribute(possible, _all=False):
    for test, node_groups in possible.items():
        if not node_groups:
            continue

        if _all:
            test.node_groups = node_groups
        else:
            test.node_groups = [sorted(node_groups, key=lambda x: len(x.tests))[0]]

        for node_group in test.node_groups:
            node_group.tests.append(test)

def check_results(results):
    if results["result"]:
        return False

    terms = listify(results.get("check_result", []))
    if not terms:
        return True
    else:
        print("testrunner: checking results")

        res = False
        _check_results = {}
        for term in terms:
            _res = eval(term, {}, results["results"])
            res = res or bool(_res)
            print("testrunner: '%s'==%s (%s)" % (term, res, bool(res)))
            _check_results[term] = _res

        print("testrunner: %s" % "passed" if res else "failed")
        results["check_result"] = _check_results
        return res

def run_tests(tests, outfile, instance):
    exit_code = 0
    results = []
    for test in tests:
        if test.instance_id != instance:
            continue
        if not test.node_groups:
            print("%s: no matching group found!" % test.filename, file=sys.stderr)
            continue
        for node_group in test.node_groups:
            print("- running test", test.filename, "on node group", node_group.filename, end="")

            try:
                print(" (instance %s: %s)" % ( test.instance_id, test.data.get("template_instance", {})))
            except KeyError:
                print("")

            test.test_class = _tests.map.get(test.data["test"])
            test_nodes = node_group.create_node_objs(test)

            test_obj = test.test_class(test_nodes, args=test.data.get("args", {}))

            # run test
            try:
                test_obj.safe_run()
            except KeyboardInterrupt as e:
                if not is_main_interrupted():
                    raise e
                else:
                    test_obj.result = "error"

            result = {}

            node_data = []
            for node in test_nodes:
                node_data.append(node.data)

            result["used_nodes"] = node_data
            result.update(test.data)
            result.update(test_obj.result_data or {})
            result["result"] = test_obj.result

            test_obj.result = test_obj.result or (not check_results(result))
            result["output"] = test_obj.output

            if test_obj.result:
                print("- TEST FAILED!")
                exit_code = 1
            else:
                print("- TEST SUCCESSFUL!")

            results.append(result)

        outfile.write(json.dumps(results, indent=4, sort_keys=True, default=set_default))

    return exit_code

def print_murdock_jobs(tests):
    for test in tests:
        if not test.node_groups:
            print("testrunner: %s: no matching group found!" % test.filename, file=sys.stderr)
            continue
        for node_group in test.node_groups:
            murdock_queue = node_group.murdock_queue

            instance_str = " %s" % test.instance_id if test.instance_id else ""

            print("./.murdock run_test %s%s ### { \"queue\" : \"%s\", \"max_retries\" : 0 }" \
                    % (test.filename, instance_str, murdock_queue))

@click.command()
@click.option("--nodes", multiple=True, type=click.Path())
@click.option("--tests", multiple=True, type=click.Path())
@click.option("--all", is_flag=True)
@click.option("--outfile", type=click.File("w"))
@click.option("--murdock", is_flag=True)
@click.option("--instance")
def main(nodes, tests, all, outfile, murdock, instance):
    nodes = list(nodes)
    tests = list(tests)

    _tests = []
    node_groups = []
    _tests = Test.load_all(tests)
    node_groups = NodeGroup.load_all(nodes, murdock)

    if not _tests:
        print("testrunner: don't know about any tests!", file=sys.stderr)
        sys.exit(1)

    if not node_groups:
        print("testrunner: don't know about any nodes!", file=sys.stderr)
        sys.exit(1)

    possible = match_tests(_tests, node_groups)
    distrib = distribute(possible, _all=all)

    if murdock:
        print_murdock_jobs(_tests)
        sys.exit(0)
    try:
        res = run_tests(_tests, outfile or sys.stdout, instance)
    except (KeyboardInterrupt, SystemExit) as e:
        print(e)
        res = 1

    sys.exit(res)

if __name__=="__main__":
    main(auto_envvar_prefix='TESTRUNNER')
