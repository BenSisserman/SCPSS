'''
Author : Ben Sisserman
Date : 10/18/2020
Description  :  This python module is meant to be used by a PC to intialize and send commands to SCPSS devices. 
'''

import socket 

class SCPSS:
    def __init__(self, IP, PORT):
        self.IP = IP
        self.PORT = PORT
        self.socket = socket.socket(socket.AF_I)