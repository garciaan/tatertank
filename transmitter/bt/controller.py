import serial
import Tkinter as tk
import os
import time

pressed = False

B = chr(0)
Y = chr(1)
Select = chr(2)
Start = chr(3)
Up = chr(4)
Down = chr(5)
Left = chr(6)
Right = chr(7)
A = chr(8)
X = chr(9)
L = chr(10)
R = chr(11)
ser = serial.Serial('/dev/cu.HC-05-DevB')
print ("Connected on " + ser.name)

def keyup(e):
	print 'up', e.char

def keydown(e):
	print 'down', e.char

def forwardA(event):
    print "Forward started"

def forward(event):
    print "Forward"
    t_end = time.time() + 2
    while time.time() < t_end:
        ser.write(Up)
        time.sleep(0.060)
    print "Stop"


def reverse(event):
    print 'Reverse'
    t_end = time.time() + 2
    while time.time() < t_end:
        ser.write(Down)
        time.sleep(0.060)
    print 'Stop'

def right(event):
    print "Right"
    t_end = time.time() + 2
    while time.time() < t_end:
        ser.write(Right)
        time.sleep(0.060)
    print "Stop"

def left(event):
    print "Left"
    t_end = time.time() + 2
    while time.time() < t_end:
        ser.write(Left)
        time.sleep(0.05)
    print "Stop"

def stop(event):
    global pressed
    pressed = False
    print "Stop"

def set_buttons(root):
    upButton = tk.Button(root,text = "Forward")
    upButton.pack(padx = 5, pady = 5)
    upButton.bind('<ButtonPress-1>',forward)
    #upButton.bind('<ButtonRelease-1>',forwardA)
    downButton = tk.Button(root,text = "Reverse");
    downButton.pack(padx = 5, pady = 5);
    downButton.bind('<ButtonPress-1>',reverse)

    leftButton = tk.Button(root,text = "Left")
    leftButton.pack(side = tk.LEFT,padx = 5, pady = 5)
    leftButton.bind("<ButtonPress-1>",left)

    rightButton = tk.Button(root, text = "Right")
    rightButton.pack(side = tk.RIGHT,padx = 5, pady = 5)
    rightButton.bind("<ButtonPress-1>",right)

def main():
    
    
    root = tk.Tk()
    frame = tk.Frame(root)
    frame.bind("<KeyPress>",keydown)
    frame.bind("<KeyRelease>",keyup)
    frame.pack()
    frame.focus_set()
    root.title("Tater Tank Bluetooth Controller")
    set_buttons(root)
    root.mainloop()
    

main()
ser.close()
