#!/usr/bin/python3
# -*- coding: utf-8 -*-
#   Author: Andy Garcia
#   Date: 1/28/2017

import sys, os
from PyQt5.QtWidgets import (QWidget, QGridLayout, 
    QPushButton, QApplication, QCheckBox, QSlider, QStatusBar, QLabel)
from PyQt5 import (QtGui,QtCore,Qt)
from math import pi, cos, sqrt
import serial
sys.path.append(os.path.dirname(__file__) + "../XboxController/")
from XboxController import XboxController

from time import sleep

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
        self.xbox_connected = False
        self.bluetooth_connected = False
        self.motor_powers = [50,50,50] #note that 0 is full reverse, 50 is stop, and 100 is full forward
        self.left_motor_power = 50  #note that 0 is full reverse, 50 is stop, and 100 is full forward
        self.right_motor_power = 50 #note that 0 is full reverse, 50 is stop, and 100 is full forward
        self.z_motor_power = 50     #note that 0 is full reverse, 50 is stop, and 100 is full forward
        self.sendSliderTimer = QtCore.QTimer()
        self.sendSliderTimer.setInterval(300)
        self.sendSliderTimer.timeout.connect(self.sendSlider)
        self.calcMotorPower = QtCore.QTimer()
        self.calcMotorPower.setInterval(10)
        self.calcMotorPower.timeout.connect(self.calculate_motor_power)
        self.readSerialTimer = QtCore.QTimer()
        self.readSerialTimer.setInterval(100)
        self.readSerialTimer.timeout.connect(self.readSerial)    

    def initUI(self):
        
        grid = QGridLayout()
        self.setLayout(grid)
    
        delay = 500
        interval = 300

        stop_button = QPushButton(text = "Stop")
        stop_button.setAutoRepeat(1)
        stop_button.setAutoRepeatDelay(delay)
        stop_button.setAutoRepeatInterval(interval)
        stop_button.pressed.connect(self.stop)

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

        path1_button = QPushButton(text = "Path 1")
        path1_button.pressed.connect(self.send_path1)

        fire_button = QPushButton(text = "Fire")
        fire_button.pressed.connect(self.fire)

        self.bt_button = QCheckBox(text = "Connect to Drone")
        self.bt_button.pressed.connect(lambda: self.bt_handle('/dev/tty.HC-05-DevB'))

        xbox_button = QCheckBox(text = "Use Xbox Controller")
        xbox_button.setChecked(False)
        xbox_button.stateChanged.connect(lambda:self.setXboxSend(xbox_button))

        send_button = QCheckBox(text = "Send slider values")
        send_button.setChecked(False)
        send_button.stateChanged.connect(lambda:self.sendSliderHandle(send_button))

        self.sliders = []
        for x in range (3):
            self.sliders.append(QSlider(QtCore.Qt.Vertical))
            self.sliders[x].setMinimum(1)
            self.sliders[x].setMaximum(100)
            self.sliders[x].setValue(50)
            self.sliders[x].valueChanged.connect(lambda: self.setSliderValues(x))
            self.sliders[x].setInvertedAppearance(False)
        
       
        self.heading_label = QLabel()
        self.heading_label.setText("Heading:")
        
        self.heading = QLabel()
        self.heading.setText("Not set")


        grid.addWidget(forward_button,1,3)
        grid.addWidget(stop_button,2,3)
        grid.addWidget(right_button,2,4)
        grid.addWidget(left_button,2,2)
        grid.addWidget(reverse_button,3,3)
        grid.addWidget(sl_button,1,2)
        grid.addWidget(sr_button,1,4)
        grid.addWidget(self.bt_button,1,1)
        grid.addWidget(xbox_button,2,1)
        grid.addWidget(send_button,3,1)
        grid.addWidget(self.sliders[0],1,5,2,4)
        grid.addWidget(self.sliders[1],1,7,2,4)
        grid.addWidget(self.sliders[2],1,9,2,4)
        grid.addWidget(path1_button,4,1)
        grid.addWidget(fire_button,5,1)

        LEFT = QtCore.Qt.AlignLeft
        RIGHT = QtCore.Qt.AlignRight
        CENTER = QtCore.Qt.AlignHCenter
        JUSTIFY = QtCore.Qt.AlignJustify

        grid.addWidget(self.heading_label,6,2,alignment = RIGHT)
        grid.addWidget(self.heading,6,3,alignment = LEFT)


        self.move(300, 150)
        self.setWindowTitle('TaterTank Controller')
        self.show()


    def send_path1(self):
        try:
            self.ser.write(bytes([101,101,101,126]))
        except Exception as e:
            print ("Error")
            print (e)
    

    def readSerial(self):
        try:
            while (self.ser.in_waiting > 0):
                line = (self.ser.readline().decode("ascii"))
                if (line.find("Heading: ") != -1):
                    index = line.find("Heading: ") + len("Heading: ")
                    self.heading = "Not yet implemented"
                print ("Line: " + line)
        except Exception as e:
            print ("Error")
            print (e)

    def setSendSlider(self,button):
        if button.isChecked():
            pass

    def sendSlider(self):
        try:
            self.ser.reset_input_buffer()
            print ([self.sliders[0].value(),self.sliders[1].value(),self.sliders[2].value()])
            # print (bytes([self.sliders[0].value(),self.sliders[1].value(),self.sliders[2].value(),126]))
            self.ser.write(bytes([self.sliders[0].value(),self.sliders[1].value(),self.sliders[2].value(),126]))
        except Exception as e:
            print ("Uh oh. Well I caught it")
            print (e)

    def sendSliderHandle(self,button):
        if button.isChecked() == True:
            print ("Enabling Send Slider")
            try:
                self.sendSliderTimer.start(500)
                print ("Enabled Send Slider")
            except Exception as e:
                print ("Could not enable send slider timer: ")
                print (e)
        else:
            print ("Disabling Send Slider")
            self.sendSliderTimer.stop()

    def setSliderValues(self,motor):
        self.motor_powers[motor] = self.sliders[motor].value()
         
    def defaultStop(self,button):
        if (button.isChecked()):
            pass

    def printMotorPower(self):
        if (self.left_motor_power < 50 and self.right_motor_power < 50):
            print ("Reverse at {0}%".format(self.left_motor_power*2))
        elif (self.left_motor_power >50 and self.right_motor_power > 50):
            print ("Forward at {0}%".format(self.left_motor_power*2))
        else:
            print ("Stop")

   

    def bt_handle(self,port = '/dev/cu.HC-05-DevB'):
        if self.bt_button.isChecked() == False:
            try:
                self.ser = serial.Serial(port, baudrate = 9600,timeout = 0)
                self.bluetooth_connected = True
                self.readSerialTimer.start(500)
                print ("Connected on " + self.ser.name)
                self.ser.reset_input_buffer()
            except ValueError:
                self.ser = None
                print ("Could not connect to " + port + ": Value Error")
                self.bt_button.setCheckState(False)

            except serial.SerialException:
                self.ser = None
                print ("Could not connect to " + port + ": Device not found")
                self.bt_button.setCheckState(False)
            
            except:
                self.ser = None
                print ("Could not connect to " + port + ":Unknown error")
                self.bt_button.setCheckState(False)
        else:
            try:
                if (self.ser.is_open):
                    self.readSerialTimer.stop()
                    print ("Closing " + port)
                    self.ser.close()
                    self.bluetooth_connected = False
                    
                else:
                    print (port + " is not open")

            except Exception as e:
                print ("Invalid Port")
                print (e)

    def bt_close(self,port):
        try:
            if (port.is_open):
                print ("Closing " + port.name)
                port.close()
                self.bluetooth_connected = False
            else:
                print (port.name + " is not open")
        except:
            print ("Invalid Port")

    def swing_right(self):
        try:
            self.sliders[0].setValue(100)
            self.sliders[1].setValue(50)
            if (not self.sendSliderTimer.isActive()):
                self.ser.write(bytes([100,50,self.sliders[2].value(),126]))
                print (bytes([100,50,self.sliders[2].value(),126]))
            
        except:
            print ("Could not send swing right command")

    def swing_left(self):
        try:
            self.sliders[0].setValue(50)
            self.sliders[1].setValue(100)
            if (not self.sendSliderTimer.isActive()):
                self.ser.write(bytes([50,100,self.sliders[2].value(),126]))
                print (bytes([50,100,self.sliders[2].value(),126]))
            
        except:
            print ("Could not send swing left command")

    def stop(self):
        try:
            self.sliders[0].setValue(50)
            self.sliders[1].setValue(50)
            if (not self.sendSliderTimer.isActive()):
                self.ser.write(bytes([50,50,self.sliders[2].value(),126]))
                print (bytes([50,50,self.sliders[2].value(),126]))

        except:
            print ("Could not send stop command")

    def forward(self):
        try:
            self.sliders[0].setValue(100)
            self.sliders[1].setValue(100)
            if (not self.sendSliderTimer.isActive()):
                self.ser.write(bytes([100,100,self.sliders[2].value(),126]))
                print (bytes([100,100,self.sliders[2].value(),126]))
        except:
            print ("Could not send forward command")

    def reverse(self):
        try:
            self.sliders[0].setValue(1)
            self.sliders[1].setValue(1)
            if (not self.sendSliderTimer.isActive()):
                self.ser.write(bytes([1,1,self.sliders[2].value(),126]))
                print (bytes([1,1,self.sliders[2].value(),126]))
        except:
            print ("Could not send reverse command")

    def right(self):
        try:
            self.sliders[0].setValue(100)
            self.sliders[1].setValue(1)
            if (not self.sendSliderTimer.isActive()):
                self.ser.write(bytes([100,1,self.sliders[2].value(),126]))
                print (bytes([100,1,self.sliders[2].value(),126]))
        
        except:
            print ("Could not send right command")

    def left(self):
        try:
            self.sliders[0].setValue(1)
            self.sliders[1].setValue(100)
            if (not self.sendSliderTimer.isActive()):
                self.ser.write(bytes([1,100,self.sliders[2].value(),126]))
                print (bytes([1,100,self.sliders[2].value(),126]))
        except:
            print ("Could not send left command")

    def fire(self):
        try:
            self.ser.write(bytes([104,104,104,126]))
            print ("Firing")
            print (bytes([104,104,104,126]))
        except:
            print ("Could not send fire command")
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

    def calculate_motor_power(self):
        # send.append(int(self.xboxCont.LTHUMBY))
        # send.append(int(self.xboxCont.LTHUMBY
        left_motor = self.xboxCont.LTHUMBY
        right_motor = self.xboxCont.LTHUMBY
        z_motor = self.xboxCont.RTHUMBY
        trig = self.xboxCont.LTRIGGER
        if (trig >= 90):
            self.fire()
            while (self.xboxCont.LTRIGGER > 60):
                continue
        # if (self.xboxCont.LTHUMBY < 65 and self.xboxCont.LTHUMBY > 35 and self.xboxCont.LTHUMBX > 75):
        #     #check spin right
        #     left_motor = self.xboxCont.LTHUMBX
        #     right_motor = -self.xboxCont.LTHUMBX
        # elif (self.xboxCont.LTHUMBY < 65 and self.xboxCont.LTHUMBY > 35 and self.xboxCont.LTHUMBX < 35):
        #     #check spin left
        #     left_motor = -self.xboxCont.LTHUMBX
        #     right_motor = 100 - self.xboxCont.LTHUMBX
        if (self.xboxCont.LTHUMBX > 55):
            unit_x = (self.xboxCont.LTHUMBX - 50)/float(50)
            ratio = sqrt(1 - (unit_x * unit_x))
            right_motor = self.xboxCont.LTHUMBY * ratio
        elif (self.xboxCont.LTHUMBX < 45):
            unit_x = (self.xboxCont.LTHUMBX - 50)/float(50)
            ratio = sqrt(1 - (unit_x * unit_x))
            left_motor = self.xboxCont.LTHUMBY * ratio
        self.motor_powers = [left_motor,right_motor,z_motor]
        self.sliders[0].setValue(left_motor)
        self.sliders[1].setValue(right_motor)
        self.sliders[2].setValue(z_motor)
        #print (self.motor_powers)

        return self.motor_powers

            
            

    def setXboxSend(self,button):
        # motors = self.calculate_motor_power()
        # self.setSliderValues(motors)
        if button.isChecked() == True:
            try:
                #generic call back
                self.calcMotorPower.start()
                def controlCallBack(xboxControlId, value):
                    # print ("Control Id = {}, Value = {}".format(xboxControlId, value))
                    if (xboxControlId == 1):
                        self.y_value = value
                    if (xboxControlId == 0):
                        self.x_value = value
                    if (xboxControlId == 3):
                        self.z_value = value

                    
                #setup xbox controller, set out the deadzone and scale, also invert the Y Axis (for some reason in Pygame negative is up - wierd! 
                self.xboxCont = XboxController(controlCallBack, deadzone = 10, scale = 50, invertYAxis = True)
                self.xboxCont.start()
                print ("Xbox 360 Controller Connected")

            except Exception as e:
                print (e)
                button.setChecked(False)
        else:
            try:
                self.xboxCont.stop()
                self.xboxCont = None
                self.calcMotorPower.stop()
                print ("Xbox 360 Controller Disconnected")
            except:
                pass

    def multikey(self):
        if (contains(self.keys,[QtCore.Qt.Key_W,QtCore.Qt.Key_A])):
            self.swing_left()
        elif (contains(self.keys,[QtCore.Qt.Key_W,QtCore.Qt.Key_D])):
            self.swing_right()
        elif (contains(self.keys,QtCore.Qt.Key_Space)):
            self.stop()
        elif (contains(self.keys,QtCore.Qt.Key_W)):
            self.forward()
        elif (contains(self.keys,QtCore.Qt.Key_S)):
            self.reverse()
        elif (contains(self.keys,QtCore.Qt.Key_A)):
            self.left()
        elif (contains(self.keys,QtCore.Qt.Key_D)):
            self.right()
        elif (contains(self.keys,QtCore.Qt.Key_Space)):
            try:
                send = []
                #send.append(int(self.xboxCont.left_y))
                send.append(int(self.xboxCont.LTHUMBY))
                send.append(int(self.xboxCont.LTHUMBY))
                send.append(int(self.xboxCont.LTHUMBY))
                send.append(126)
                self.ser.write(bytes(send))
                print ("Sent {0}".format(send))
            except Exception as e:
                print ("Could not send xbox command")
                print (type(e))
                print(e.args)
                print (e)
        else:
            print ("Unknown Key: " + str(self.keys))

    def closeEvent(self,event):
        try:
            self.xboxCont.stop()
        except:
            pass
        event.accept()

if __name__ == '__main__':
    
    app = QApplication(sys.argv)
    win = MainWindow()
    sys.exit(app.exec_())