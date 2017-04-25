from threading import Barrier, Lock, Thread

from util import listify

import tests.shell

class Network(object):
    def __init__(s, network, numnodes=2):
        s.barrier = Barrier(numnodes)
        s.lock = Lock()
        s.nodes = {}
        s.network = network

    def get_addr(s, child, nodenum):
        res = []
        interfaces = tests.shell.get_interfaces(child)
        for ifnum, addrs in interfaces:
            res.append(addrs)

        with s.lock:
            s.nodes[nodenum] = res

        return interfaces

    def setup(s, child, nodenum):
        interfaces = s.get_addr(child, nodenum)

        s.barrier.wait()

        mynetwork = s.network[nodenum]

        # handle addresses
        addrs = listify(mynetwork.get('addrs', []))
        if addrs and (type(addrs[0]) != list):
            addrs = [addrs]

        for i, addrlist in enumerate(addrs):
            ifnum, _unused = interfaces[i]
            for addr in addrlist:
                print("node %s: add address %s to interface %s" % (nodenum, addr, ifnum))
                child.sendline("ifconfig %s add %s" % (ifnum, addr))

        # handle channel settings
        channels = listify(mynetwork.get('channel', []))
        for i, channel in enumerate(channels):
            ifnum, _unused = interfaces[i]
            print("node %s: set channel %s on interface %s" % (nodenum, channel, ifnum))
            child.sendline("ifconfig %s set channel %s" % (ifnum, channel))

        _locals = { "link_local" : s.link_local }

        for _route in listify(mynetwork.get("routes", [])):
            route = { "dst" : "::", "dev" : interfaces[0][0] }
            route.update(_route)
            for key, val in route.items():
                if val.startswith("%"):
                    route[key] = eval(val[1:], {}, _locals)

            print("node %s setting route %s via %s dev %s" % (nodenum, route["dst"], route["via"], route["dev"]))
            child.sendline("fibroute add %s via %s dev %s" % (route["dst"], route["via"], route["dev"]))

        s.barrier.wait()

    def link_local(s, nodenum, ifnum=0, addrnum=0):
        return tests.shell.find_addrs(s.nodes[nodenum][ifnum], "fe80::")[addrnum]

    def setup_all(s, nodes):
        threads = []
        for i, node in enumerate(nodes):
            t = Thread(target=s.setup, args=(node.child, i))
            t.start()
            threads.append(t)

        for t in threads:
            t.join()
