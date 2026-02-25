import pigpio
import sys
import time
import traceback
from drive import DriveSystem, TurnLevel
from irSensor import LineSensor
from enum import Enum
from constants import *

ReadingMove = {
    (0, 0, 0): TurnLevel.SPIN_RIGHT, #turn right
    (1, 0, 0): TurnLevel.TURN_LEFT, #turn right 
    (1, 1, 0): TurnLevel.STEER_LEFT, # veer right
    (1, 1, 1): TurnLevel.STRAIGHT, #spin
    (0, 1, 1): TurnLevel.STEER_RIGHT, # veer left
    (0, 0, 1): TurnLevel.TURN_RIGHT, #turn left
    (1, 0, 1): TurnLevel.STRAIGHT, #straight
    (0, 1, 0): TurnLevel.STRAIGHT, #
}


class LineFollow():
    def __init__(self, LineSensor, DriveSystem):
        self.LineSensor = LineSensor
        self.DriveSystem = DriveSystem


    def run(self):
        while True:
            reading = ReadingMove.get(self.LineSensor.read(), TurnLevel.SPIN_RIGHT)
            self.DriveSystem.drive(reading)
            print(f"Reading: {self.LineSensor.read()}\n")
            print("Moving: %s\n", reading)


if __name__ == "__main__":
    io = pigpio.pi()

    allIR = LineSensor(io, PIN_IR_LEFT, PIN_IR_MIDDLE, PIN_IR_RIGHT)
    driveSys = DriveSystem(io, PIN_MOTOR1_LEGA, PIN_MOTOR1_LEGB, PIN_MOTOR2_LEGA, PIN_MOTOR2_LEGB)
    LF = LineFollow(allIR, driveSys)
    try:
        LF.run()
    except BaseException as ex:
        print("Ending due to exception: %s" % repr(ex))
        traceback.print_exc()
    driveSys.stop()
    io.stop()

    