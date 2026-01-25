import pigpio
import sys
import time
import traceback
from constants import PIN_IR_LEFT, PIN_IR_MIDDLE, PIN_IR_RIGHT



class IR:
    def __init__(self, io, pin):
        self.io = io
        self.io.set_mode(pin, pigpio.INPUT)
        self.pin = pin


    def read(self):
        return self.io.read(self.pin)

class LineSensor:
    def __init__(self, io, irL, irM, irR):
        self.irL = IR(io, irL)
        self.irM = IR(io, irM)
        self.irR = IR(io, irR)

    def read(self):
        return self.irL.read(), self.irM.read(), self.irR.read()

if __name__ == "__main__":
    io = pigpio.pi()
    if not io.connected:
        print("Unable to connection to pigpio daemon!")
        sys.exit(0)
    print("GPIO ready...")

    allIR = LineSensor(io, PIN_IR_LEFT, PIN_IR_MIDDLE, PIN_IR_RIGHT)

    try:
        while True:
        
            print(f"Reading: {allIR.read()}")
    except BaseException as ex:
        print("Ending due to exception: %s" % repr(ex))
        traceback.print_exc()
    io.stop()