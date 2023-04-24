import serial

#Identificação da porta COM:
def check_port():
    for i in range(10): 
        try:
            return serial.Serial(('COM'+str(i)), 115200) 
        except serial.SerialException:
            pass
port = check_port()

while(True):
    port.write(bytes(input().rstrip('\n'), 'utf-8'))