#!/usr/bin/env python3
#
# Copyright (c) 2021 Max Thomas
# This file is part of DSiWifi and is distributed under the MIT license.
# See dsiwifi_license.txt for terms of use.
#
# Basic RPC Client
# This file is provided to allow benchmarking TCP speeds
# and to provide a simple working example for LwIP.
# RPC commands are all unimplemented in dsiwifi but are
# provided as an example of how this RPC interface might
# be extended.
#
# NetBIOS is used in order to find an active server on the
# network to communicate with.

import socket
import time
import struct
import os
import sys
from nmb.NetBIOS import NetBIOS # pysmb
import netifaces # netifaces

HOST = None
PORT = 8336

RPC_CMD_NOP               = (0)
RPC_CMD_CREATEFILE        = (1)
RPC_CMD_WRITEFILE         = (2)
RPC_CMD_REBOOT            = (15)
RPC_CMD_MAX               = (16)

def smb_query_name(name):
    for interface in netifaces.interfaces():
        if "docker" in interface:
            continue

        ips = netifaces.ifaddresses(interface)
        
        # Only IPv4
        if 2 not in ips:
            continue

        for idk in ips[2]:
            if 'broadcast' not in idk:
                continue

            broadcast_ip = idk['broadcast']
            
            n = NetBIOS(broadcast=True, listen_port=0)
            ip = n.queryName(name, ip=broadcast_ip, timeout=1)
            if ip is not None and ip is not []:
                return ip[0]
    return None

def millis():
    return round(time.time() * 1000)

def round_up(x, n): 
    return ( ((x) + (n)-1)  & (~((n)-1)) ) 

def rpc_simple_cmd(cmd_id):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))

        rpc_send_cmd(s, cmd_id)

        s.close()

def rpc_send_cmd(s, cmd_id, data=None):
    payload = None
    
    if data is not None:
        payload = struct.pack("<04sLL", b'SLTC', len(data) + 4, cmd_id) + data
    else:
        payload = struct.pack("<04sLLL", b'SLTC', 8, cmd_id, 0)
    
    s.settimeout(1)
    s.sendall(payload)
    s.settimeout(0.001)
    
    return len(payload)

def rpc_find_3ds():
    global HOST
    host_ip = smb_query_name("WhoNeedTheyPzy8")
    if host_ip is not None:
        HOST = host_ip
        print ("Found 3DS: " + HOST)

def rpc_nop():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))

        rpc_send_cmd(s, 0)

        s.close()

def benchmark():
    ms_start = millis()

    to_send = 0x200000
    bulk_size = 0x40000

    to_send = (to_send // bulk_size) * bulk_size

    sent_bytes = 0

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        
        j = 0
        for i in range(to_send // bulk_size):
            payload = struct.pack("<04sLL", b'SLTC', bulk_size, 0) + (b'a' * (bulk_size-4))
            
            sent_bytes += len(payload)

            while True:
                try:
                    s.settimeout(10)
                    s.sendall(payload)
                    s.settimeout(60) # TODO why doesn't LwIP tell me the TCP window is full?
                    data = s.recv(8)
                    break
                except KeyboardInterrupt:
                    exit(0)
                except:
                    a="a"
            
            print (hex(i * bulk_size))
            
            j += 1
            if j >= 10:
                #data = s.recv(5*10)
                j = 0

            #print('Received', repr(data))
        #s.recv(5*(to_send // bulk_size))
        print ('Done')
        s.close()

    ms_end = millis()

    ms_total = ms_end - ms_start

    bytes_per_sec = sent_bytes / (ms_total / 1000)

    print(int(bytes_per_sec / 1024), "KiB/s")

def create_remote_file(fpath, trunc):
    ms_start = millis()
    
    sent_bytes = 0
    
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))

        payload = struct.pack("<4sLL64sL", b'SLTC', 4+64+4, RPC_CMD_CREATEFILE, fpath.encode("utf-8"), 1 if trunc else 0)
        
        sent_bytes = len(payload)
        
        s.settimeout(10)
        s.sendall(payload)
        s.settimeout(0.001)

        s.close()
    
    ms_total = millis() - ms_start
    bytes_per_sec = sent_bytes / (ms_total / 1000)
    
    rate_str = str(int(bytes_per_sec / 1024)) + " KiB/s";
    print ("Sent (" + rate_str + ")")

def copy_file_to_remote(host_path, remote_path):
    sd_path = remote_path.encode("utf-8")
    f_size = os.path.getsize(host_path)
    f = open(host_path, "rb")
    
    create_remote_file(remote_path, True)
    
    bulk_size = 0x8000#0x5AC
    
    ms_start = millis()
    
    sent_bytes = 0
    
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))

        sent = 0
        while (sent < f_size):
            print (hex(sent),"/",hex(f_size))
            contents_len = bulk_size - 4 - 64 - 8
            contents = f.read(contents_len-4)
            contents_real_len = len(contents)
            contents_len_round = round_up(len(contents), 4)
            if contents_real_len < contents_len_round:
                contents += b'\x00' * (contents_len_round-len(contents))

            payload = struct.pack("<4sLL64sLL", b'SLTC', 4+64+4+4+len(contents), RPC_CMD_WRITEFILE, sd_path, sent, contents_real_len) + contents
            
            sent_bytes += len(payload)

            while True:
                try:
                    s.settimeout(10)
                    s.sendall(payload)
                    s.settimeout(60) # TODO why doesn't LwIP tell me the TCP window is full?
                    data = s.recv(8)
                    break
                except KeyboardInterrupt:
                    exit(0)
                except:
                    a="a"
            
            sent += len(contents)

        s.close()
    
    ms_total = millis() - ms_start
    bytes_per_sec = sent_bytes / (ms_total / 1000)
    
    rate_str = str(int(bytes_per_sec / 1024)) + " KiB/s";
    print ("Sent (" + rate_str + ")")
    
    rpc_nop()
    rpc_nop()

if len(sys.argv) < 2:
    print("Usage: " + sys.argv[0] + " [benchmark | upload]")
    exit(0)

# Search for 3DS using NetBIOS
while HOST == None:
    rpc_find_3ds()

for arg in sys.argv[1:]:
    if arg.lower() == "benchmark":
        benchmark()
    elif arg.lower() == "upload":
        copy_file_to_remote("payload.bin", "sdmc:/payload.bin")
    elif arg.lower() == "dsiup":
        #copy_file_to_remote("BOOT.NDS", "sdmc:/BOOT.NDS")
        #copy_file_to_remote("main.srldr", "sdmc:/_nds/TWiLightMenu/main.srldr")
        copy_file_to_remote("test_app.nds", "sdmc:/_nds/TWiLightMenu/main.srldr")
    elif arg.lower() == "reboot":
        rpc_simple_cmd(RPC_CMD_REBOOT)
    else:
        print("Unknown argument `" + arg + "`.")
        break;
#rpc_nop()

