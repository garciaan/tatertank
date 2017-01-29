#!/usr/bin/python3
# -*- coding: utf-8 -*-
#   Author: Andy Garcia
#   Date: 1/28/2017

import sys
from PyQt5.QtWidgets import (QWidget, QGridLayout, 
    QPushButton, QApplication)
from PyQt5 import (QtGui,QtCore)
import serial

B = b'\x00'
Y = b'\x01'
Select = b'\x02'
Start = b'\x03'
Up = b'\x04'
Down = b'\x05'
Left = b'\x06'
Right = b'\x07'
A = b'\x08'
X = b'\x09'
L = b'\x0A'
R = b'\x0B'
SwingRight = b'\x0C'
SwingLeft = b'\x0D'

def contains(main_list,items):
    try:
        if type(items) is int:
            items = [items]
        if type(items) is QtCore.Qt.Key:
            items = [items]
        return all(x in main_list for x in items)
    except:
        print ("\nException")
        return False


class MainWindow(QWidget):
    
    def __init__(self):
        super().__init__()
        self.ser = None
        self.keys = []
        self.initUI()

    def initUI(self):
        
        grid = QGridLayout()
        self.setLayout(grid)
    
        delay = 50
        interval = 60
        forward_button = QPushButton(text = "Forward")
        forward_button.setAutoRepeat(1)
        forward_button.setAutoRepeatDelay(delay)
        forward_button.setAutoRepeatInterval(interval)
        forward_button.pressed.connect(self.forward)

        reverse_button = QPushButton(text = "Reverse")
        reverse_button.setAutoRepeat(1)
        reverse_button.setAutoRepeatDelay(delay)
        reverse_button.setAutoRepeatInterval(interval)
        reverse_button.pressed.connect(self.reverse)

        right_button = QPushButton(text = "Right")
        right_button.setAutoRepeat(1)
        right_button.setAutoRepeatDelay(delay)
        right_button.setAutoRepeatInterval(interval)
        right_button.pressed.connect(self.right)

        left_button = QPushButton(text = "Left")
        left_button.setAutoRepeat(1)
        left_button.setAutoRepeatDelay(delay)
        left_button.setAutoRepeatInterval(interval)
        left_button.pressed.connect(self.left)

        sl_button = QPushButton(text = "Swing Left")
        sl_button.setAutoRepeat(1)
        sl_button.setAutoRepeatDelay(delay)
        sl_button.setAutoRepeatInterval(interval)
        sl_button.pressed.connect(self.swing_left)

        sr_button = QPushButton(text = "Swing Right")
        sr_button.setAutoRepeat(1)
        sr_button.setAutoRepeatDelay(delay)
        sr_button.setAutoRepeatInterval(interval)
        sr_button.pressed.connect(self.swing_right)

        bt_button = QPushButton(text = "Connect to Tank")
        bt_button.pressed.connect(self.bt_connect)

        bt_close_button = QPushButton(text = "Disconnect Tank")
        bt_close_button.pressed.connect(lambda: self.bt_close(self.ser))

        grid.addWidget(forward_button,1,3)
        grid.addWidget(right_button,2,4)
        grid.addWidget(left_button,2,2)
        grid.addWidget(reverse_button,3,3)
        grid.addWidget(sl_button,1,2)
        grid.addWidget(sr_button,1,4)
        grid.addWidget(bt_button,1,1)
        grid.addWidget(bt_close_button,3,1)

        
        self.move(300, 150)
        self.setWindowTitle('TaterTank Controller')
        self.setWindowIcon(QtGui.QIcon('tank.png'))
        self.show()

    def bt_connect(self,port = '/dev/cu.HC-05-DevB'):
        try:
            self.ser = serial.Serial(port)
            print ("Connected on " + self.ser.name)
        except ValueError:
            self.ser = None
            print ("Could not connect to " + port + ": Value Error")

        except serial.SerialException:
            self.ser = None
            print ("Could not connect to " + port + ": Device not found")
        
        except:
            self.ser = None
            print ("Could not connect to " + port + ":Unknown error")

    def bt_close(self,port):
        try:
            if (port.is_open):
                print ("Closing " + port.name)
                port.close()
            else:
                print (port.name + " is not open")
        except:
            print ("Invalid Port")

    def swing_right(self):
        try:
            print("Swing Right")
            self.ser.write(SwingRight)
        except:
            print ("Could not send swing right command")

    def swing_left(self):
        try:
            print("Swing Left")
            self.ser.write(SwingLeft)
        except:
            print ("Could not send swing left command")

    def forward(self):
        try:
            print ("Forward")
            self.ser.write(Up)
        except:
            print ("Could not send forward command")

    def reverse(self):
        try:
            print ("Reverse")
            self.ser.write(Down)
        except:
            print ("Could not send reverse command")

    def right(self):
        try:
            print ("Right")
            self.ser.write(Right)
        except:
            print ("Could not send right command")

    def left(self):
        try:
            print ("Left")
            self.ser.write(Left)
        except:
            print ("Coult not send left command")

    def keyPressEvent(self,event):
        if type(event) == QtGui.QKeyEvent:
            if (not contains(self.keys,event.key())):
                self.keys.append(event.key())
            self.multikey()
            event.accept()

    def keyReleaseEvent(self,event):
        if type(event) == QtGui.QKeyEvent:
            try:
                self.keys.remove(event.key())
            except:
                pass
    
    def multikey(self):
        if (contains(self.keys,[QtCore.Qt.Key_W,QtCore.Qt.Key_A])):
            self.swing_left()
        elif (contains(self.keys,[QtCore.Qt.Key_W,QtCore.Qt.Key_D])):
            self.swing_right()
        elif (contains(self.keys,QtCore.Qt.Key_W)):
            self.forward()
        elif (contains(self.keys,QtCore.Qt.Key_S)):
            self.reverse()
        elif (contains(self.keys,QtCore.Qt.Key_A)):
            self.left()
        elif (contains(self.keys,QtCore.Qt.Key_D)):
            self.right()
        else:
            print ("Unknown Key: " + str(self.keys))

if __name__ == '__main__':
    
    app = QApplication(sys.argv)
    win = MainWindow()
    sys.exit(app.exec_())