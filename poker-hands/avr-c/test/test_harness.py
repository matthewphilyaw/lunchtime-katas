import serial
import struct

cards = b'\x02\x23\x35\x14\x1e'

ser = serial.Serial('/dev/ttyUSB0', 115200)
while True:
    buf = ser.readline() 
    print(buf)
    if buf == bytes('rdy\n', 'ascii'):
        for c in cards:
            ca = bytearray(1)
            ca[0] = c
            ser.write(ca)
            print('0x{0:02x}'.format(c))

        buf = ser.readline()
        print(buf)
        if buf != bytes('wt\n', 'ascii'):
            print("error")
            break

        for b in ser.readline():
            if b == 10: 
                continue
            print('0x{0:02x}'.format(b))

        buf = ser.readline()
        print(buf)
        if buf == bytes('ok\n', 'ascii'):
            break
        
