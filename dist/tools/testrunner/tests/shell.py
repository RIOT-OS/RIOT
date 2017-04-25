import re
import threading

from .base import BaseTest
from .network import Network

re_iface = re.compile(r"Iface\s+(\d)")
re_addr = re.compile(r"\s+inet6 addr: (\S+) .*$")

def find_addr(_list, prefix):
    for addr in _list:
        if addr.startswith(prefix):
            return addr

def find_addrs(_list, prefix):
    result = []
    for addr in _list:
        if addr.startswith(prefix):
            result.append(addr)

    return result

def get_interfaces(child):
    print("getting node interfaces / addresses")
    child.sendline("ifconfig")
    result = []
    ifnum = None
    addrs = []

    while True:
        res = child.expect([re_iface, re_addr, "> "])
        if res==0:
            print("got iface")
            if ifnum:
                result.append((ifnum, addrs))
                addrs = []
            ifnum = child.match.group(1)
        elif res==1:
            print("got address")
            addrs.append(child.match.group(1).split("/")[0])
        else:
            if ifnum != None:
                result.append((ifnum, addrs))
            break

    return result

def get_link_local_addr(child):
    ifaces = get_interfaces(child)
    if not ifaces:
        print("cannot get interface addresses")
        return

    ifnum, addrs = ifaces[0]
    return find_addr(addrs, "fe80::")

class ShellTest(BaseTest):
    def __init__(s, nodes, args=None, min_nodes=1):
        args = args or {}

        super().__init__(nodes, args, min_nodes)

        network = args.get("network")
        if network:
            s.network = Network(network, len(nodes))
        else:
            s.network = None

        s.setup_barrier = threading.Barrier(len(nodes))

    def launch_node(s, node):
        super().launch_node(node)
        child = node.child

        if s.args.get("shell", None) != False:
            child.expect("> ")

        if s.network:
            for node_num, _node in enumerate(s.nodes):
                if node == _node:
                    s.network.setup(child, node_num)

        s.setup_barrier.wait()
