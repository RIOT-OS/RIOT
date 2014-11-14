#!/usr/bin/env python2

import os, binascii
import ConfigParser
import re
from collections import namedtuple
from bidict import bidict
from random import randint

from scapy.config import conf
conf.load_layers.append('sixlowpan')
from scapy.all import *

RiotEndpoint = namedtuple('RiotEndpoint', 'hwaddr ipv6 port')
IP6Endpoint  = namedtuple('IP6Endpoint', 'ipv6 port')

# import the RIOT native packet layer
from riot import Riot
# bind it to the right ethernet type
bind_layers(Ether, Riot, type=0x1234)
# next layer in RIOT native packets
bind_layers(Riot, Dot15d4FCS)

# Parse IEEE 802.15.4 payload as SixLoWPAN
split_layers(Dot15d4Data, ZigbeeNWK)
bind_layers(Dot15d4Data, SixLoWPAN)

ETHER_BROADCAST = "ff:ff:ff:ff:ff:ff"

from zope.interface import implementer
from zope.interface import implements, Interface
from twisted.pair.tuntap import TuntapPort
from twisted.pair.ethernet import IEthernetProtocol
from twisted.internet.protocol import DatagramProtocol, Factory
from twisted.internet import reactor, protocol

# spits out the sequence number for IEEE 802.15.4 data frames
# between 0 - 255 and initialised to a random number.
def seqnum():
    if "seq" not in seqnum.__dict__:
        seqnum.seq = randint(1, 255)
    else:
        seqnum.seq += 1
        seqnum.seq %= 256
    return seqnum.seq

def read_configuration():
    # read configuration file
    configparser = ConfigParser.SafeConfigParser()
    configparser.read('marz.config')

    config = dict()

    config['interface'] = configparser.get('marz', 'interface')
    config['hwaddr'] = configparser.getint('marz', 'hwaddr')
    config['panid'] = configparser.getint('marz', 'panid')
    # generate our own ipv6 address from hardware address
    #config['ipv6_address'] = "fe80::ff:fe00:%x" % (config['hwaddr'])
    config['ipv6_address'] = "ff02::1:0:ff00:%x" % (config['hwaddr'])

    config['UDP6'] = dict()
    config['TCP6'] = dict()
    for section in configparser.sections():
        m = re.match(r"^(TCP6|UDP6):(\d+)$", section)
        if m:
            config[m.group(1)][int(m.group(2))] = \
                RiotEndpoint(configparser.getint(m.group(0), 'recipient'),
                             "fe80::ff:fe00:%x" % configparser.getint(m.group(0), 'recipient'),
                             configparser.getint(m.group(0), 'port'))
    return config

def get_tap_mac(tap_name, tap):
    from fcntl import ioctl
    import ctypes

    # net/if.h: #define IFHWADDRLEN 6
    IFHWADDRLEN = 6
    # net/if.h: #define IF_NAMESIZE 16
    IF_NAMESIZE = 16
    # linux/sockios.h: #define SIOCGIFHWADDR 0x8927
    SIOCGIFHWADDR = 0x8927

    # bits/socket.h: sockaddr struct { ... }
    class sockaddr(ctypes.Structure):
        _fields_ = [("sa_family", ctypes.c_short),
                    ("sa_data", ctypes.c_char * 14)]

    # net/if.h: struct ifreq { ... }
    class ifru(ctypes.Union):
        _fields_ = [("flags", ctypes.c_short),
                    ("hwaddr", sockaddr)]

    class ifreq(ctypes.Structure):
        _fields_ = [("name", ctypes.c_char * IF_NAMESIZE),
                    ("ifru", ifru)]

    # get device MAC
    ifr = ifreq()
    ifr.name = tap_name
    ret = ioctl(tap, SIOCGIFHWADDR, ifr, 1)
    if ret != 0:
        raise IOError
    
    mac =  ':'.join(x.encode('hex') for x in ifr.ifru.hwaddr.sa_data[0:IFHWADDRLEN])

    return mac

class RiotProtocol(protocol.AbstractDatagramProtocol):
    implements(IEthernetProtocol)
    fragments = dict()
    
    def datagramReceived(self, frame, partial=0):
        riot = Ether(frame)[Riot]
        dot15d4 = riot[Dot15d4Data]
        # here should the defragmentation take place but it isn't here yet.
        if (LoWPANFragmentationFirst in dot15d4):
            first_frag = dot15d4[LoWPANFragmentationFirst]
            print "Found SixLoWPAN fragmentation header with tag %s and size %i" % \
                (first_frag.datagramTag, first_frag.datagramSize)
            self.fragments[(first_frag.datagramTag, first_frag.datagramSize)] = (first_frag.datagramSize - len(first_frag.load), first_frag.load)
            #print self.fragments
            return
        if (LoWPANFragmentationSubsequent in dot15d4):
            segment = dot15d4[LoWPANFragmentationSubsequent]
            print "Found SixLoWPAN fragmentation segment with tag %s, size %i and offset %i." % \
                (segment.datagramTag, segment.datagramSize, segment.datagramOffset*8)
            if (segment.datagramTag, segment.datagramSize) in self.fragments:
                print "Found corresponding first fragment"
                (rem, s) = self.fragments[(segment.datagramTag, segment.datagramSize)]
                rem -= len(segment.load)
                s = s + segment.load
                self.fragments[(segment.datagramTag, segment.datagramSize)] = (rem, s)
                #print self.fragments
                if rem <= 0:
                    # Ok, there is no way right now to put the
                    # assembled payload back into a recorded Dot15d4
                    # instance right now. That would be the right way.
                    # For now, we grab the UDP pkt right from offset
                    # 16 (ipv6.dest) + 1 (next header) + 2 (IPHC) = 19
                    if rem < 0:
                        dirty_udp = UDP(s[19:rem])
                    else:
                        dirty_udp = UDP(s[19:])
                    #interact(mydict={ "s": s, "dirty_udp": dirty_udp }, mybanner="In Fragmentation")
                    self._processUDP(dirty_udp)
            return
        ipv6 = dot15d4[IPv6]
        # we assume this implicitly
        assert riot.src == dot15d4.src_addr
        # check if we are the receiver on the...
        # ...RIOT level
        if ((riot.dst == config['hwaddr']) or
            (riot.dst == 0)):
            # ...IEEE 802.15.4 level
            if ((dot15d4.dest_addr == config['hwaddr']) or
                (dot15d4.dest_addr == 65535)):
                # ...IPv6 level
                if ipv6.dst == config['ipv6_address']:
                    self._processIPv6(ipv6)
                else:
                    print "dropped packet with ipv6 destination %s" % (ipv6.dst)
            else:
                print "dropped packet with IEEE 802.15.4 destination address %d" % (dot15d4.dest_addr)
        else:
            print "dropped packet with RIOT native destination %d" % (riot.dst)

    def _processIPv6(self, ipv6):
        if UDP in ipv6:
            self._processUDP(ipv6[UDP])
        else:
            print "dropped packet not of type UDP"

    def _processUDP(self, udp):
        (sender, endpoint) = self.udp_mgr.getConnection_out(udp.dport)

        if sender is not None:
            print "[TAP] Received %d data bytes on port %d: Relaying through %d to %s" % \
                (len(udp.load), udp.dport, sender.transport.port, endpoint)

            sender.transport.write(udp.payload.build(), endpoint)

    def register_udp(self, udp_mgr):
        self.udp_mgr = udp_mgr

class UDP6Forwarder(DatagramProtocol):
    def __init__(self, udp_mgr):
        self.udp_mgr = udp_mgr

    def datagramReceived(self, data, addr):
        (out_port, destination) = self.udp_mgr.getConnection_in(self.transport.port, IP6Endpoint(*addr))
        print "[UDP6 %d] Received %d data bytes from %s: Relaying through %d to %s" % \
            (self.transport.port, len(data), addr, out_port, destination)
        pkts = self._transform(data, out_port, destination)
        for pkt in pkts:
            self.udp_mgr.riot.transport.write(pkt)

    def _transform(self, data, out_port, destination):
        pkts = list()
        ipv6_pkt = IPv6(src=config['ipv6_address'], dst=destination.ipv6)\
                   /UDP(sport=out_port, dport=destination.port)/data
        
        # fragmentation threshold is 96 Bytes (hardwired somewhere in scapy)
        for pkt in fragmentate(ipv6_pkt, 0):
            packet = Ether(src=config['tap_mac'], dst=ETHER_BROADCAST)\
                     /Riot(src=config['hwaddr'], dst=0)\
                     /Dot15d4FCS(fcf_srcaddrmode="Short", fcf_panidcompress=True, seqnum = seqnum())\
                     /Dot15d4Data(src_addr=config['hwaddr'], dest_addr=0xffff, dest_panid=config['panid'])\
                     /pkt
            pkts.append(packet.build())
        return pkts

class UDPConnectionManager:
    def __init__(self, routes, riot):
        self.riot = riot
        riot.register_udp(self)
        self.routes = routes
        self.ports = [port for port in self.routes.keys()]
        self.sockets = dict()
        self.connections = bidict()

    def getConnection_in(self, port, ip6_endpoint):
        if (port, ip6_endpoint) not in self.connections:
            print "make new connection"
            assert port in self.ports
            self.connections[(port, ip6_endpoint)] = (ip6_endpoint.port, self.routes[port])

        return self.connections[(port, ip6_endpoint)]

    def getConnection_out(self, port):
        for (vport, endpoint) in ~self.connections:
            if vport == port:
                outgoing = self.connections[:(vport, endpoint)]
                # return the UDP6Forwarder instance responsible for this port
                # and the IP6Endpoint where the should be sent to
                return (self.sockets[outgoing[0]], outgoing[1])
        # reached this? Ohoh.
        print "No connection found for virtual port %d" % (port)
        return (None, None)

    def listen(self):
        for port in self.ports:
            self.sockets[port] = UDP6Forwarder(self)
            reactor.listenUDP(port, self.sockets[port], interface='::1')


# START
config = read_configuration()


riot = RiotProtocol()
udp = UDPConnectionManager(config['UDP6'], riot)

udp.listen()
print "Listening on UDP ports: %s" % (udp.ports)
tap = TuntapPort(interface=config['interface'], proto=riot, reactor=reactor)
tap.startListening()
config['tap_mac'] = get_tap_mac(config['interface'], tap.fileno())
print "Listening on tap interface %s with MAC address %s" % (config['interface'], config['tap_mac'])

reactor.run()
