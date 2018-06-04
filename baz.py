from pyqtgraph.Qt import QtGui, QtCore
import pyqtgraph as pg
import serial
import time
import struct
import sys

class SerialConnection(QtCore.QThread):
    data_ready = QtCore.Signal(list)
    portName = "/dev/tty.usbmodem145141"
    baudrate = 115200
    ser = serial.Serial(portName, baudrate)

    def __init__(self):
        super(SerialConnection, self).__init__()

    def run(self):
        N = 30
        loops = 0
        while True:
            yarr = []
            repeat = False
            loops += 1
            t0 = time.time()
            values = bytearray(self.ser.read(64))
            if values[0] == 0x11 and values[1] == 0x22 and values[2] == 0x33 and values[3] == 0x44:
                pass
            else:
                print("bad")
                while True:
                    if bytearray(self.ser.read(1))[0] == 0x11 and bytearray(self.ser.read(1))[0] == 0x22 and bytearray(self.ser.read(1))[0] == 0x33 and bytearray(self.ser.read(1))[0] == 0x44:
                        self.ser.read(60)
                        repeat = True
                        break
                    print("loop...")
            if repeat:
                pass
            yarr.extend(list(struct.unpack("<" + N*"H", values[4:])))

            if loops % 10 == 0:
                self.data_ready.emit(yarr)

class RootWidget(QtGui.QWidget):
    def __init__(self, parent=None):
        super(RootWidget, self).__init__(parent)

        ## Create some widgets to be placed inside
        btn = QtGui.QPushButton('Stop/Start')
        text = QtGui.QLineEdit('enter text')
        listw = QtGui.QListWidget()
        plot = pg.PlotWidget()

        self.curve = plot.plot()
        self.curve.setData([1,4,3,2,8])

        ## Create a grid layout to manage the widgets size and position
        layout = QtGui.QGridLayout()
        self.setLayout(layout)

        ## Add widgets to the layout in their proper positions
        #layout.addWidget(btn, 0, 0)   # button goes in upper-left
        #layout.addWidget(text, 1, 0)   # text edit goes in middle-left
        #layout.addWidget(listw, 2, 0)  # list widget goes in bottom-left
        layout.addWidget(plot, 0, 1, 3, 1)  # plot goes on right side, spanning 3 rows

        self.windowWidth = 500
        self.N = 30
        self.Xm = [0] * (self.windowWidth * 100)
        self.ptr = -self.windowWidth

    def data_ready_handler(self, data):
        NN = len(data)
        self.Xm[:-NN] = self.Xm[NN:]
        self.Xm[-NN:] = data
        self.ptr += NN
        self.curve.setData(self.Xm)
        self.curve.setPos(self.ptr, 0)

        #print("Got data:", data)

if __name__ == '__main__':
    pg.setConfigOptions(useOpenGL=True)
    app = QtGui.QApplication([])

    w = RootWidget()
    w.show()

    serconn = SerialConnection()
    serconn.data_ready.connect(w.data_ready_handler)
    serconn.finished.connect(app.exit)
    serconn.start()

    ## Start the Qt event loop
    app.exec_()
