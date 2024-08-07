import serial
import time

ser = serial.Serial("COM3")
print(ser.name)

looping = True

time.sleep(6)

ser.flush()
ser.write(b'MOV,25.0,25.0\n')
time.sleep(0.5)

while (looping):
    out_msg = input(">> ")
    ser.write(out_msg.encode())
    time.sleep(1)

    msg = b''
    while ser.in_waiting > 0:
        msg += ser.read(1)

    print(str(msg))
    time.sleep(1)

ser.close()