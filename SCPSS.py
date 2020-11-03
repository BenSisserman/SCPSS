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
        
        # The allows to create time stamps for when messages are sent, but without using values as large as system time
        self.startTime = getMilliSecs()

        # initialze the time difference between device and host
        self.setTimeDifference()


    # send a string with M
    def sendStringWithTime(self, msg):
        self.mySocket.send(bytes(str(msg).lower() + 'E' + str(self.getTime()) + 'E','utf-8'))

    def sendTime(self):
        self.mySocket.send(bytes(str(self.getTime()) + 'E','utf-8'))

    def closePort(self):
        self.mySocket.close()

    # TO DO
    def turnOn(self):
        pass

    def turnOff(self):
        pass

    def timer(self):
        pass

    def setLatency(self):
        pass
    
    def setWiFi(self,SSID,PASS):
        pass

    def getTime(self):
        return getMilliSecs() - self.startTime

    # Function called during initialization for esp32s2 to calculate time difference between machines
    def setTimeDifference(self):
        pass
        # send small msg with timestamp 10 times to the esp 32
        # for i in range(10):
        #     self.sendTime()
        
        # esp32 will store minimum of those 10 - may need to increase this value
        # now recieve timestamp from esp32 10 times
        
        # how recieve from server?
        
        # for each calcualte difference with current time
        # send the minimum value back