import serial
import time
import math

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyACM1',
    baudrate=921600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

try:
    ser.isOpen()
except IOError:
  ser.close()
  ser.open()
  print ("port was already open, was closed and opened again!")


try:
    ser.isOpen()
except Exception as e:
    print("error open serial port: "+str(e))
    exit()

#coords = [[0,0,0], [16,1000,0], [8,1000,1000], [16,0,1000]]

i = 0

while True:
    z = int(i*16)
    x = int(512 * (1+math.cos(2 * math.pi * i))) # should be 0 to 1024
    y = int(512 * (1+math.sin(2 * math.pi * i))) # should be 0 to 1024

    packet = bytearray()
    packet.append(z)
    packet.append(x & 0x00FF)
    packet.append(x >> 8)
    packet.append(y & 0x00FF)
    packet.append(y >> 8)
    ser.write(packet)
    print("Sent: "+str(z)+","+str(x)+","+str(y))

    time.sleep(0.0002)

    i += 0.001
    if i > 1:
        i = 0
