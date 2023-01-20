import serial
import time
import math

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyACM1',
    baudrate=115200,
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
packet = [0 for i in range(64)]
#print(packet)
packetidx = 0

i = 0
PACKET_LIMIT = 60

while True:
    z = int(i*10)
    x = int(512 * (1+math.cos(2 * math.pi * i))) # should be 0 to 1024
    y = int(512 * (1+math.sin(2 * math.pi * i))) # should be 0 to 1024

    packet[packetidx] = z & 0x00FF
    packet[packetidx+1] = x >> 8
    packet[packetidx+2] = x & 0x00FF
    packet[packetidx+3] = y >> 8
    packet[packetidx+4] = y & 0x00FF
    packetidx += 5
    if packetidx >= PACKET_LIMIT:
        ser.write(bytearray(packet[0:PACKET_LIMIT]))
        packetidx = 0
        print(bytearray(packet[0:PACKET_LIMIT]))
    #print("Sent: "+str(z)+","+str(x)+","+str(y))

    #print(packetidx)

    time.sleep(0.02)

    i += 0.005
    if i > 1:
        i = 0
