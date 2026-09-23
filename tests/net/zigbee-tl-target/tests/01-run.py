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
ZLL_KEY = [
    0x9F, 0x55, 0x95, 0xF1, 0x02, 0x57, 0xC8, 0xA4,
    0x69, 0xCB, 0xF4, 0x2B, 0xC9, 0x3F, 0xEE, 0x31
]
zep_params = {
        "local_addr":  "127.0.0.1",
        "local_port":  60547,
        "remote_addr": "127.0.0.1",
        "remote_port": 36017,
    }

s = None
transaction_id = []
response_id = []
pan_id = []
riot_addr = None
zep_seq = 0

def get_enc_key():
    nonce = [transaction_id[3-i%4] for i in range(8)] + [response_id[3-i%4] for i in range(8)]
    # encrypt the network key
    c = AES.new(bytes(ZLL_KEY), AES.MODE_ECB)
    exchange_key = c.encrypt(bytes(nonce))
    c = AES.new(exchange_key, AES.MODE_ECB)
    return list(c.encrypt(bytes(KEY)))

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
                fcf_ackreq= riot_addr != None, \
                fcf_pending= False, \
                fcf_security= False, \
                fcf_frametype= 0x01, \
                fcf_srcaddrmode= 0x03, \
                fcf_framever= 0, \
                fcf_destaddrmode= (0x03 if riot_addr != None else 0x02)\
            ) \
            / Dot15d4Data(\
                dest_panid = 0xffff,\
                dest_addr = (riot_addr if riot_addr != None else 0xffff),\
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

def send_scan_req():
    rsp = get_generic_intrp() / ZigbeeClusterLibrary(\
        disable_default_response=1, \
        command_direction=0, \
        manufacturer_specific=0, \
        zcl_frametype=1, \
        transaction_sequence=1, \
        command_identifier=0)
    zcl_data = [transaction_id[0], transaction_id[1], transaction_id[2], transaction_id[3],
            #ZBinf|TLinf|
            0x06, 0x93
    ]
    rsp = rsp / Raw(load=bytes(zcl_data))
    zep_send(rsp)

def send_net_start_req():
    rsp = get_generic_intrp() / ZigbeeClusterLibrary(\
        disable_default_response=1, \
        command_direction=0, \
        manufacturer_specific=0, \
        zcl_frametype=1, \
        transaction_sequence=2, \
        command_identifier=0x10)
    zcl_data = [transaction_id[0], transaction_id[1], transaction_id[2], transaction_id[3],
            # extended pan        | master key
            0, 0, 0, 0, 0, 0, 0, 0, 0x04] + get_enc_key()
            #    Ch.|  PAN ID  |  Net Addr | GrID Start | GrID End  | A.R. start| A.R. end  |
    zcl_data += [25, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
            #GID start |  GID end  |   Initiator addr                              | Net addr |
            0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0x01, 0x00]
           
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
                dest_panid = pan_id,\
                dest_addr = 0x0002,\
                src_addr = 0x0001\
            ) \
            / ZigbeeNWK(\
                proto_version=2,\
                frametype=1,\
                flags = 0x1a,\
                destination = 0x0002,\
                source=0x0001,\
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

def send_rejoin_req():
    clear = ZigbeeNWKCommandPayload(\
            cmd_identifier=6,\
            allocate_address=1, \
            security_capability=0, \
            receiver_on_when_idle=1, \
            power_source=0, \
            device_type=0, \
            alternate_pan_coordinator=1 \
    )

    frame = get_generic_enc()
    frame[ZigbeeNWK] = encrypt(frame[ZigbeeNWK], raw(clear))
    zep_send(frame)

def testfunc(child):
    global riot_addr
    global response_id
    global pan_id
    send_scan_req()
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
            pkt[ZigbeeAppDataPayloadStub].data[0] == 0x19:
            # is data and touchlink, cluster specific
                pl = pkt[ZigbeeAppDataPayloadStub].data
                if pl[2] == 1:
                    #scan rsp
                    riot_addr = pkt[Dot15d4Data].src_addr
                    response_id = pkt[ZigbeeAppDataPayloadStub].data[12:16]
                    send_net_start_req()
                if pl[2] == 0x11:
                    # start net rsp
                    pan_id = int.from_bytes(pkt[ZigbeeAppDataPayloadStub].data[-2:], byteorder='little')
                    send_rejoin_req()
        if pkt.haslayer(ZigbeeSecurityHeader):
            pkt[ZigbeeNWK] = decrypt(pkt[ZigbeeNWK])
            if pkt.haslayer(ZigbeeNWKCommandPayload):
                if pkt[ZigbeeNWKCommandPayload].cmd_identifier == 0x07 and \
                pkt[ZigbeeNWKCommandPayload].rejoin_status == 0:
                    break
    return 0

if __name__ == "__main__":
    print(f"Ports: {zep_params['local_port']} {zep_params['remote_port']}")
    os.environ['TERMFLAGS'] = "-z [%s]:%d,[%s]:%d -c tap0" % (
            zep_params['local_addr'], zep_params['local_port'],
            zep_params['remote_addr'], zep_params['remote_port'])
    # setup scapy
    conf.dot15d4_protocol = 'zigbee'
    transaction_id = [0xbc, 0x70, 0x45, 0x78]
    s = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    s.bind(("127.0.0.1", zep_params['remote_port']))
    res = run(testfunc, timeout=1, echo=False, traceback=True)
    s.close()
    if (res == 0):
        print("Run tests successful")
    else:
        print("Run tests failed")
    sys.exit(res)
