from scapy.config import conf
conf.load_layers.append('sixlowpan')
from scapy.all import *

class Riot(Packet):
    name = "RIOT native packet"
    fields_desc = [ LenField("len", None),
                    ShortField("dst", 0),
                    ShortField("src", 0) ]

