from re import A
from socket import timeout
from tracemalloc import stop
import serial # import module for serial communcation
import pyautogui # import module for keyboard/mouse control
import io # import module to to use universal newline mode
import time
from numpy import asarray

# open Serial Port 3
ser = serial.Serial("COM3",9600, timeout=0.05)

#ser.baudrate = 9600 # baud rate
#ser.port = 'COM3'# set port
#ser.timeout = 0.1 # set timeout in seconds

#ser.open() # open serial communication
print('COM 3 Open: ', ser.is_open)
sio = io.TextIOWrapper(io.BufferedRWPair(ser, ser))
ser.flush() # wait until all data is written

pyautogui.FAILSAFE = False # set failsafe for manual exit
width, height = pyautogui.size() # set size of screen
pyautogui.moveTo(960, 540); # set mouse to middle

landing_gear = 0
flap_up = 0
flap_down = 0
stop = 0
len_gas = 0
xuong_gas = 0
alive = True

count = 0

#time.sleep(0.2)
while (alive): # kill switch not asserted
    line = sio.readline()
    data = line.split()
    data = asarray(data)  # chuyen char sang array
    print(data[:12])
    #time.sleep(0.05)

    # landing gear
    if int(data[0])!=0:
        pyautogui.press('g')
        landing_gear = int(data[0])

    # flap up
    if int(data[1])!=0:
        pyautogui.press('[')
        flap_up = int(data[1])

    # flaps down
    if int(data[2])!=0:
        pyautogui.press(']')
        flap_down = int(data[2])

    # stop
    if int(data[3])!=0:
        pyautogui.press('space')
        stop = int(data[3])

    # len gas
    if int(data[4])!=0:
        pyautogui.keyDown('pageup')
        len_gas = int(data[4])
    else: pyautogui.keyUp('pageup')
 
    # xuong gas
    if int(data[5])!=0:
        pyautogui.keyDown('pagedown')
        xuong_gas = int(data[5])
    else: pyautogui.keyUp('pagedown')

    # exit
    if int(data[6]):
        pyautogui.press('esc')
        ser.close()
        print('COM 3 Open: ', ser.is_open)
        alive = False

    # che do auto
    if int(data[9])!=0:
        pyautogui.moveTo(960, 540); # set mouse to middle
    else:
        # roll, pitch
        x = float(data[7])
        y = float(data[8]) 
        if x>90:
            x = 90
        if x<-90:
            x = -90
        if y>90:
            y = 90
        if y<-90:
            y = -90
        xn = y*10+900
        yn = x*6+540
        if(count == 0):
            pyautogui.moveTo(xn, yn)

    # quay trai
    if int(data[10])!=0:
        pyautogui.keyDown('shift')
        pyautogui.keyDown('left')
        pyautogui.keyUp('left')
        pyautogui.keyUp('shift') 

    # quay phai
    if int(data[11])!=0:
        pyautogui.keyDown('shift')
        pyautogui.keyDown('right')
        pyautogui.keyUp('right')
        pyautogui.keyUp('shift')