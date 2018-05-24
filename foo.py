from numpy import *
from pyqtgraph.Qt import QtGui, QtCore
import pyqtgraph as pg
import serial
import time
import struct

# Create object serial port
portName = "/dev/tty.usbmodem145121"                      # replace this port name by yours!
baudrate = 115200
ser = serial.Serial(portName,baudrate)

### START QtApp #####
app = QtGui.QApplication([])            # you MUST do this once (initialize things)
####################

pg.setConfigOptions(useOpenGL=True)
win = pg.GraphicsWindow(title="Signal from serial port") # creates a window
p = win.addPlot(title="Realtime plot")  # creates empty space for the plot in the window
curve = p.plot()                        # create an empty "plot" (a curve to plot)

windowWidth = 500                       # width of the window displaying the curve
N = 30
Xm = linspace(0,0,windowWidth*100)          # create array that will contain the relevant time series     
ptr = -windowWidth                      # set first x position

# Realtime data plot. Each time this function is called, the data display is updated
def update():
    global curve, ptr, Xm
#    for i in range(N):
#        values = ser.readline().replace("\r\n", "").split(" ")        # read line (single value) from the serial port
#        if len(values) != 1:
#            return
#        yarr.append(float(values[0]))
#        #print values

    yarr = []
    t0 = time.time()
    for i in range(8): #read 8 * 64
        values = bytearray(ser.read(64))
        if values[0] == 0x11 and values[1] == 0x22 and values[2] == 0x33 and values[3] == 0x44:
            pass
        else:
            print "bad"
            while True:
                if bytearray(ser.read(1))[0] == 0x11 and bytearray(ser.read(1))[0] == 0x22 and bytearray(ser.read(1))[0] == 0x33 and bytearray(ser.read(1))[0] == 0x44:
                    ser.read(60)
                    return
                print "loop..."
        yarr.extend(list(struct.unpack(">" + N*"H", values[4:])))
    
    NN = len(yarr)
    print NN / (time.time() - t0)

    #return
    Xm[:-NN] = Xm[NN:]                      # shift data in the temporal mean 1 sample left
    Xm[-NN:] = yarr             # vector containing the instantaneous values      
    ptr += NN                              # update x position for displaying the curve
    curve.setData(Xm)                     # set the curve with this data
    curve.setPos(ptr,0)                   # set x position in the graph to 0
    QtGui.QApplication.processEvents()    # you MUST process the plot now

### MAIN PROGRAM #####    
# this is a brutal infinite loop calling your realtime data plot
while True: update()

### END QtApp ####
pg.QtGui.QApplication.exec_() # you MUST put this at the end
##################
