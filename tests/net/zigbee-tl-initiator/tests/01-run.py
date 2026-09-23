#!/usr/bin/env python3

# Copyright (C) 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import socket
import time
from testrunner import run
from scapy.all import *
from scapy.layers.zigbee import ZigbeeNWK
from Crypto.Cipher import AES

IEEE802154_FRAME_LEN_MAX = 127
ZEP_DATA_HEADER_SIZE = 32
FCS_LEN = 2
RCVBUF_LEN = IEEE802154_FRAME_LEN_MAX + ZEP_DATA_HEADER_SIZE + FCS_LEN
KEY = [
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
]

zep_params = {
        "local_addr":  "127.0.0.1",
        "local_port":  60547,
        "remote_addr": "127.0.0.1",
        "remote_port": 36017,
    }

s = None
transaction_id = []
riot_addr = []
zep_seq = 0

def zep_send(data):
    global zep_seq
    tmp = raw(ZEP2(\
            ver=2,\
            type=1, \
            channel=25, \
            device=0x86e0, \
            lqi_mode=1, \
            lqi_val=0xff, \
            timestamp=time.time(), \
            seq=zep_seq, \
            length=len(data) \
        ) / data)
    zep_seq += 1
    s.sendto(tmp, ("127.0.0.1", zep_params['local_port']))

def get_generic_intrp():
    return Dot15d4FCS(\
                fcf_panidcompress= False, \
                fcf_ackreq= True, \
                fcf_pending= False, \
                fcf_security= False, \
                fcf_frametype= 0x01, \
                fcf_srcaddrmode= 0x03, \
                fcf_framever= 0, \
                fcf_destaddrmode= 0x03\
            ) \
            / Dot15d4Data(\
                dest_panid = 0xffff,\
                dest_addr = riot_addr,\
                src_panid = 0x1234,\
                src_addr = 0xdeadbeefdeadbeef\
            ) \
            / ZigbeeNWKStub(\
                proto_version = 0x2,\
                frametype = 0x3\
            ) \
            / ZigbeeAppDataPayloadStub(\
                delivery_mode = 0,\
                frametype = 3,\
                cluster = 0x1000,\
                profile = 0xc05e,\
            )

def send_scan_rsp():
    rsp = get_generic_intrp() / ZigbeeClusterLibrary(\
        disable_default_response=1, \
        command_direction=1, \
        manufacturer_specific=0, \
        zcl_frametype=1, \
        transaction_sequence=1, \
        command_identifier=1)
    zcl_data = [transaction_id[0], transaction_id[1], transaction_id[2], transaction_id[3],
            #rssi|ZBinf|TLinf|Key bitmask|     response id      |
            0x0a, 0x05, 0x80, 0x12, 0x00, 0x12, 0x34, 0x56, 0x78, 
            #|              extended pan                  |net update|
            0xde, 0xad, 0xde, 0xad, 0xbe, 0xef, 0xbe, 0xef, 0x00,
            0x0f, 0x12, 0x34, 0x02, 0x00, 0x01, 0x00,
            0x0b, 0x04, 0x01, 0x0c, 0x01, 0x01, 0x00
    ]
    rsp = rsp / Raw(load=bytes(zcl_data))
    zep_send(rsp)

def send_net_start_rsp():
    rsp = get_generic_intrp() / ZigbeeClusterLibrary(\
        disable_default_response=1, \
        command_direction=1, \
        manufacturer_specific=0, \
        zcl_frametype=1, \
        transaction_sequence=2, \
        command_identifier=0x11)
    zcl_data = [transaction_id[0], transaction_id[1], transaction_id[2], transaction_id[3],
            #suc|                    exp pan                     | upd | ch. |  pan id  |
            0x00, 0xde, 0xad, 0xde, 0xad, 0xbe, 0xef, 0xbe, 0xef, 0x00, 0x19, 0x12, 0x34]
    rsp = rsp / Raw(load=bytes(zcl_data))
    zep_send(rsp)

def get_generic_enc():
    return Dot15d4FCS(\
                fcf_panidcompress= True, \
                fcf_ackreq= True, \
                fcf_pending= False, \
                fcf_security= False, \
                fcf_frametype= 0x01, \
                fcf_srcaddrmode= 0x02, \
                fcf_framever= 0, \
                fcf_destaddrmode= 0x02\
            ) \
            / Dot15d4Data(\
                dest_panid = 0x3412,\
                dest_addr = 0x0001,\
                src_addr = 0x0002\
            ) \
            / ZigbeeNWK(\
                proto_version=2,\
                frametype=1,\
                flags = 0x1a,\
                destination = 0x0001,\
                source=0x0002,\
                radius=1,\
                seqnum = 2,\
                ext_dst=riot_addr,\
                ext_src=0xdeadbeefdeadbeef\
            ) \
            / ZigbeeSecurityHeader(\
                extended_nonce=1,\
                key_type=1,\
                nwk_seclevel=5,\
                fc=0,\
                source=0xdeadbeefdeadbeef,\
                key_seqnum=0)

def decrypt(f):
    f[ZigbeeSecurityHeader].nwk_seclevel = 5
    f[ZigbeeSecurityHeader].post_dissect(None)
    nonce = bytearray([f[ZigbeeSecurityHeader].source >> (8*i) & 0xff for i in range(8)])
    nonce.extend([f[ZigbeeSecurityHeader].fc >> (8*i) & 0xff for i in range(4)])
    nonce.append(0x2d)
    c = AES.new(bytes(KEY), AES.MODE_CCM, nonce=nonce, mac_len=4)
    c.update(raw(f)[:(len(f[ZigbeeNWK])-len(f[ZigbeeSecurityHeader].data)-4)])
    f[ZigbeeNWK].flags -= 0x0002
    tmp = raw(f)[:len(f[ZigbeeNWK])-len(f[ZigbeeSecurityHeader])]
    tmp += c.decrypt_and_verify(raw(f[ZigbeeSecurityHeader].data), raw(f[ZigbeeSecurityHeader].mic))
    ret = ZigbeeNWK()
    ret.dissect(tmp)
    return ret

def encrypt(f, cl):
    nonce = bytearray([f[ZigbeeSecurityHeader].source >> (8*i) & 0xff for i in range(8)])
    nonce.extend([f[ZigbeeSecurityHeader].fc >> (8*i) & 0xff for i in range(4)])
    nonce.append(0x2d)
    c = AES.new(bytes(KEY), AES.MODE_CCM,nonce=nonce, mac_len=4)
    c.update(raw(f)[:len(f[ZigbeeNWK])])
    cipher, tag = c.encrypt_and_digest(bytes(cl))
    f[ZigbeeSecurityHeader].data = cipher
    f[ZigbeeSecurityHeader].mic = tag
    f[ZigbeeSecurityHeader].nwk_seclevel = 0
    return f

def send_rejoin_rsp():
    clear = ZigbeeNWKCommandPayload(\
            cmd_identifier=7,\
            network_address = 0x0002,\
            rejoin_status= 0)
    frame = get_generic_enc()
    frame[ZigbeeNWK] = encrypt(frame[ZigbeeNWK], raw(clear))
    zep_send(frame)

def testfunc(child):
    global transaction_id
    global riot_addr
    child.sendline("tl_scan")
    while True:
        data, addr = s.recvfrom(RCVBUF_LEN)
        data = bytearray(data)
        # hotfix for swapped interpretation by scapy
        data[7] = 0x00
        pkt = ZEP2()
        pkt.dissect(data)
        if pkt[Dot15d4].fcf_ackreq:
            zep_send(Dot15d4FCS(
                fcf_pending= True, \
                fcf_frametype= 0x02, \
                fcf_srcaddrmode= 0x00, \
                fcf_destaddrmode= 0x00, seqnum= pkt[Dot15d4].seqnum)/Dot15d4Ack())
        if pkt.haslayer(ZigbeeAppDataPayloadStub):
            if pkt[ZigbeeAppDataPayloadStub].cluster == 0x1000 and \
            pkt[ZigbeeAppDataPayloadStub].profile == 0xc05e and \
            pkt[ZigbeeAppDataPayloadStub].data[0] == 0x11:
            # is data and touchlink, cluster specific
                pl = pkt[ZigbeeAppDataPayloadStub].data
                if pl[2] == 0:
                    #scan req
                    transaction_id = [pl[i+3] for i in range(4)]
                    riot_addr = pkt[Dot15d4Data].src_addr
                    send_scan_rsp()
                if pl[2] == 0x10:
                    # start net req
                    send_net_start_rsp()
        if pkt.haslayer(ZigbeeSecurityHeader):
            pkt[ZigbeeNWK] = decrypt(pkt[ZigbeeNWK])
            if pkt.haslayer(ZigbeeNWKCommandPayload):
                if pkt[ZigbeeNWKCommandPayload].cmd_identifier == 6:
                    send_rejoin_rsp()
            if pkt.haslayer(ZDPDeviceAnnce):
                # device has joined correctly and is proud to talk to us
                break
    return 0

if __name__ == "__main__":
    print(f"Ports: {zep_params['local_port']} {zep_params['remote_port']}")
    os.environ['TERMFLAGS'] = "-z [%s]:%d,[%s]:%d -c tap0" % (
            zep_params['local_addr'], zep_params['local_port'],
            zep_params['remote_addr'], zep_params['remote_port'])
    # setup scapy
    conf.dot15d4_protocol = 'zigbee'
    s = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    s.bind(("127.0.0.1", zep_params['remote_port']))
    res = run(testfunc, timeout=1, echo=True, traceback=True)
    s.close()
    if (res == 0):
        print("Run tests successful")
    else:
        print("Run tests failed")
    sys.exit(res)
