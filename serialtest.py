import serial
import time

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

coords = [[0,0,0], [16,1000,0], [8,1000,1000], [16,0,1000]]

i = 0

while True:
    packet = bytearray()
    packet.append(coords[i][0])
    packet.append(coords[i][1] & 0x00FF)
    packet.append(coords[i][1] >> 8)
    packet.append(coords[i][2] & 0x00FF)
    packet.append(coords[i][2] >> 8)
    ser.write(packet)
    print("Sent: "+str(coords[i]))

    time.sleep(1)

    i += 1
    if i == 4:
        i = 0
