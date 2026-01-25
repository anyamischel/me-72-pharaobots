import pigpio
import sys
import time
import traceback
from enum import Enum
from constants import *



class Motor:
    def __init__(self, io, pinA, pinB):
        self.io = io
        self.pinA = pinA
        self.pinB = pinB

        self.io.set_mode(self.pinA, pigpio.OUTPUT)
        self.io.set_mode(self.pinB, pigpio.OUTPUT)

        self.io.set_PWM_range(self.pinA, 255)
        self.io.set_PWM_range(self.pinB, 255)
    
        self.io.set_PWM_frequency(self.pinA, 1000)
        self.io.set_PWM_frequency(self.pinB, 1000)

        self.io.set_PWM_dutycycle(self.pinA, 0)
        self.io.set_PWM_dutycycle(self.pinB, 0)


    def setLevel(self, level):
        pwm = level * 255
        if level > 0: 
            self.io.set_PWM_dutycycle(self.pinB, pwm)
            self.io.set_PWM_dutycycle(self.pinA, 0)
        else:
            self.io.set_PWM_dutycycle(self.pinA, -pwm)
            self.io.set_PWM_dutycycle(self.pinB, 0)


    def stop(self):
        self.setLevel(0)

class TurnLevel(Enum):
    STRAIGHT = (0.72, 0.72)
    VEER_RIGHT =  (0.8, 0.7)
    STEER_RIGHT = (0.8,0.55)
    TURN_RIGHT =  (0.9,0.4)
    HOOK_RIGHT = (0.85,0)
    SPIN_RIGHT = (0.85,-0.85)
    VEER_LEFT =  (0.7, 0.85)
    STEER_LEFT = (0.55,0.8)
    TURN_LEFT =  (0.35,0.85)
    HOOK_LEFT = (0,0.85)
    SPIN_LEFT = (-0.85,0.85)

class DriveSystem:
    def __init__(self, io, PIN_MOTOR1_LEGA, PIN_MOTOR1_LEGB, PIN_MOTOR2_LEGA, PIN_MOTOR2_LEGB):
        self.motorL = Motor(io, PIN_MOTOR1_LEGA, PIN_MOTOR1_LEGB)
        self.motorR = Motor(io, PIN_MOTOR2_LEGA, PIN_MOTOR2_LEGB)
    def stop(self):
        self.motorL.stop()
        self.motorR.stop()

    def drive(self, dir: TurnLevel):
        mL_pwr, mR_pwr = dir.value
        self.motorL.setLevel(mL_pwr * LOWER_RATIO)
        self.motorR.setLevel(mR_pwr)

if __name__ == "__main__":
    io = pigpio.pi()

    driveSys = DriveSystem(io, PIN_MOTOR1_LEGA, PIN_MOTOR1_LEGB, PIN_MOTOR2_LEGA, PIN_MOTOR2_LEGB)
    try:
        for direction in TurnLevel:
            print(f"\n Driving: {direction.name} ")
            driveSys.drive(direction)
            time.sleep(4)
            driveSys.stop()
            input("Hit return to continue...\n")
        
    except BaseException as ex:
        print("Ending due to exception: %s" % repr(ex))
        traceback.print_exc()
    driveSys.stop()
    io.stop()