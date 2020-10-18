'''
Author : Ben Sisserman
Date : 10/18/2020
Description  :  This python module is meant to be used by a PC to intialize and send commands to SCPSS devices. 

NOTE : all IPs are strings and all port are ints, this matters for using both the socket library and decoding on devices
'''

import socket 

class SCPSS:
    # defines the users socket for recieving data from devices
    def __init__(self, userIP, userPort):
        self.userIP = userIP
        self.userPort = userPort
        self.mySocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.mySocket.bind((self.userIP,self.userPort))
        self.initMsg = bytes(self.userIP + " " + str(self.userPort), 'utf-8') # convert string to bytes array 

    # initiates a device using its IP and port, should be displayed on LCD display
    def initDevice(self, deviceIP, devicePort): 
        self.deviceSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.deviceIP = deviceIP
        self.devicePort = devicePort
        # send user socket
        self.deviceSocket.sendto(self.initMsg, (self.deviceIP, self.devicePort))
        #### this part gets tricky, need to wait on reply from device

