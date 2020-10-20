'''
Author : Ben Sisserman
Date : 10/18/2020
Description  :  This python module is meant to be used by a PC to intialize and send commands to SCPSS devices. 

    Currently each instance of SCPSS establishes communication with exactly one device. May revisit this design for managing
    mulitple devices later on.

NOTE : all IPs are strings and all port are ints, this matters for using both the socket library and decoding on devices
'''

import socket 

TIMEOUT_VAL = 1
DEFAULT_PORT = 0

class SCPSS:
    # defines the user's socket for recieving data from devices
    def __init__(self, userIP, deviceIP, devicePort):
        self.userIP = userIP
        self.deviceIP = deviceIP
        self.devicePort = devicePort

        # create socket, get any open port by using port 0
        self.mySocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.mySocket.bind((self.userIP,DEFAULT_PORT))
        self.mySocket.settimeout(TIMEOUT_VAL)
        self.userPort = self.mySocket.getsockname()[1]

        # send initial msg until a response is given from the device, retry 5 times
        retry = 0
        while True:
            self.mySocket.sendto(b"Hello", (self.deviceIP,self.devicePort))
            try:
                data, addr = self.mySocket.recvfrom(1024)
            except socket.timeout:
                print("Timed out: try #", retry)
                retry += 1
                if retry > 5:
                    print("Unable to establish communication. Ensure device is on and displaying IP and Port.")
                    self.mySocket.close()
                    break


    def close(self):
        self.mySocket.close()
