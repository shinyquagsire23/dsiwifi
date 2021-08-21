#!/usr/bin/env python3

import socket

HOST = '10.42.0.207'
PORT = 1234

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b'Hello, world')
    data = s.recv(256)

print('Received', repr(data))
