'''
Author : Ben Sisserman
Date : 10/18/2020
Description  :  This python module is meant to be used by a PC to intialize and send commands to SCPSS devices. 

    Currently each instance of SCPSS establishes communication with exactly one device. May revisit this design for managing
    mulitple devices later on.

NOTE : all IPs are strings and all port are ints, this matters for using both the socket library and decoding on devices
'''

import socket 
import time

getMilliSecs = lambda : int(round(time.time() * 1000))

class SCPSS:
    # defines the user's socket for recieving data from devices
    def __init__(self, deviceIP, devicePort):
        self.deviceIP = deviceIP
        self.devicePort = devicePort

        # create a TCP/IP socket, and connect to device. 
        self.mySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.mySocket.connect((deviceIP,devicePort))

    # send a string with M
    def cmd(self, msg):
        self.mySocket.send(bytes('M' + str(msg).lower() + 'T' + str(getMilliSecs()) + 'E','utf-8'))

    def close(self):
        self.mySocket.close()
