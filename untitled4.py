from PyQt5 import QtWidgets, uic, QtCore
from PyQt5.QtSerialPort import QSerialPort, QSerialPortInfo

from pyqtgraph import PlotWidget
import pyqtgraph as pg
import sys
import numpy as np
import time

c=299792458
pg.setConfigOption('background', 'w')
pg.setConfigOption('foreground', 'k')
pen1=pg.mkPen('b', width=1.5)

app = QtWidgets.QApplication([])
ui = uic.loadUi('control.ui')
ui.setWindowTitle('Spectrometer_GUI')

serial = QSerialPort()
serial.setBaudRate(115200)

portList = []



ports = QSerialPortInfo().availablePorts()
for port in ports:
    portList.append(port.portName())
ui.port_list.addItems(portList)
   
listX = []
listY = []



'''
sample=np.linspace(0,100,100000)
s=np.interp(sample,n/(c*100),ft)
s_ref=np.interp(sample,n_ref/(c*100),ft_ref)
'''
def on_read():

    #ra = serial.readAll()
    #print(ra)
    while serial.canReadLine():
        rx=serial.readLine()
        rxs = str(rx,'utf-8').strip()

        data = rxs.split(',')
        global listY,listX
        
        if (len(listX) == 1000):
            ft=abs(np.fft.rfft(listY,norm='ortho'))
            n=np.fft.rfftfreq(len(listX),abs(listX[-1]-listX[0])/len(listX))
            peakA=max(listY)
            peakP=listX[listY.index(peakA)]
            ui.peak_ampl.setText(f'{peakA:.2f}')
            ui.peak_pos.setText(f'{peakP:.2f}')

            ui.spectrum_w.clear()
            ui.spectrum_w.plot(n,ft,pen=pen1)
            ui.interf_w.clear()
            ui.interf_w.plot(listX, listY, pen=pen1)
            listX=[]
            listY=[]
        
        listX.append(int(data[0]))
        listY.append(int(data[1])*100./2**23)
        '''
        if (len(listX) < 1000):
            listX.append(int(data[0]))
            listY.append(int(data[1])*2.048/2**23)
        else:
            for i in range(len(listX)-1):
                listX[i]=listX[i+1]
                listY[i]=listY[i+1]
            listX[-1]=int(data[0])
            listY[-1]=int(data[1])*2.048/2**23
        '''   
    

    
    
    '''
    if (int(data[1]) == 1000):
        listX1 = listX
        listY1 = listY
        ui.interf_w.clear()
        ui.interf_w.plot(listX1, listY1, pen=pen1)
        listX = []
        listY = []
    else:
        listX.append(int(data[0]))
        listY.append(int(data[1]))
    
    if (len(listX) < 500):
        listX.append(int(data[0]))
        listY.append(int(data[1]))
    else:
        for i in range(len(listX)-1):
            listX[i]=listX[i+1]
            listY[i]=listY[i+1]
        listX[-1]=int(data[0])
        listY[-1]=int(data[1])
    ui.interf_w.clear()
    ui.interf_w.plot(listX, listY, pen=pen1)
'''  
    

    #print(t2)
        
    
    

def com(checked):
    if (checked == True):
        serial.setPortName(ui.port_list.currentText())
        serial.open(QtCore.QIODevice.ReadWrite)
    else:
        serial.close()

serial.readyRead.connect(on_read)


ui.open_b.toggled.connect(com)




x = np.arange(1000)
y = np.random.normal(size=1000)

ui.spectrum_w.plot(x, y, pen=pen1)

ui.show()
app.exec()
#print('puk')
serial.close()
