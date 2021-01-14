# SCPSS
Software Controlled Physical Sound Sources

This Git repo contains all the work pertaining to the sponsored Software Controlled Physical Sound Sources senior design project.

Some files included were used for testing and I would encourage to make use of these test files if there are any issues you are debugging.

The only files neccesary for using this project are the SCPSS.py module and the relay_firmware, servo_firmware, and ring_firmware arduino scripts.

## Set Up

### Setting up Arduino IDE with ESP32-S2

## Usage

### Controlling the SCPSS devices using the Python Module

#### Initializing a SCPSS device

1) Open the firmware code for the device you want to use. Look for the section of the code near the beginning of the file that defines the 
   WiFi credentials. Enter the credentials you want to use and upload the code to the esp32-s2.
   
2) Allow the board time to connect to WiFi. When the connection is established, the LCD will display the IP address and port for the TCP connection.
   You will need this information to connect to the device from your PC.
   
3) Initiate an instance of the SCPSS Class

```
#import the SCPSS object
import SCPSS from SCPSS

#initialize the SCPSS object
relay_device = SCPSS(<string IP>, <int port>)  
```

