import io, sys
from contextlib import redirect_stdout, redirect_stderr
from threading import Thread
from util import print_exception

class NotEnoughNodesException(Exception):
    pass

class TestAbortedException(Exception):
    pass

test_aborted = False

class BaseTest(object):
    name = "base test class"
    def __init__(s, nodes, args=None, min_nodes=1):
        if len(nodes) < min_nodes:
            raise NotEnoughNodesException

        s.result = "unknown"
        s.result_data = {}
        s.args = args
        s.nodes = nodes
        s.exception = None

    def set_results(s, data):
        s.result_data.update(data)
        return s.result_data

    def safe_run(s):
        buf = io.StringIO()
        try:
            with redirect_stdout(buf), redirect_stderr(buf):
                s.launch_nodes()
                s.run()
        except Exception as e:
            print_exception()
            s.result = "aborted"
        finally:
            print("closing nodes")
            s.close_nodes()
            s.output = buf.getvalue()
            buf.close()

        return s.result

    def launch_nodes(s):
        threads = []
        for node in s.nodes:
            t = Thread(target=s.launch_node, args=(node,))
            t.start()
            threads.append(t)

        for t in threads:
            t.join()

    def close_nodes(s):
        for node in s.nodes:
            try:
                node.child.close()
            except Exception as e:
                print("Error closing node:", e)

    def launch_node(s, node):
        node.launch()
