#!/usr/bin/python3

# used as my debug app

import pygame
import serial
import flet as ft

from time import sleep
from os import environ
from connection import Robot

import time
import math

from os import system, name

class Main():
    def __init__(self):    
        # robot constants
        self.track_width = 126/2
        pygame.joystick.init()
        self.controller = pygame.joystick.Joystick(0)

        self.robot = Robot()

        self.startTime = 0

        self.standingby = False

        self.initialDisable = True

        self.running = True

        self.field_oriented = True

        self.enabled = False

    def driveInches(self, inches):
        power = 0.7 * 255

        if inches < 0:
            power *= -1

    def clear(self):
        # for windows
        if name == 'nt':
            _ = system('cls')

        # for mac and linux
        else:
            _ = system('clear')

    def autonInit(self):
        while self.enabled == False:
            print("Waiting for Enable")
            self.clear()

    def auton(self):
        self.startTime = time.time()

        first_pitch = False

        while time.time() - self.startTime > 15:
            self.robot.receive()
            power = 0.6 * 255
            pitch = self.robot.pitch

            self.robot.front_left_power = power
            self.robot.back_right_power = power
            self.robot.back_left_power = power
            self.robot.front_right_power = power

            if pitch > 15:
                first_pitch = True
            
            if first_pitch:
                power = 0.4 * 255

                if pitch > 3:
                    self.robot.front_left_power = power
                    self.robot.back_right_power = power
                    self.robot.back_left_power = power
                    self.robot.front_right_power = power
                elif pitch < -3:
                    self.robot.front_left_power = -power
                    self.robot.back_right_power = -power
                    self.robot.back_left_power = -power
                    self.robot.front_right_power = -power
                else:
                    self.robot.front_left_power = 0
                    self.robot.back_right_power = 0
                    self.robot.back_left_power = 0
                    self.robot.front_right_power = 0



    def teleopInit(self):
        self.running = True
        while self.enabled == False:
            print("Waiting for Enable")
            self.clear()

    def teleop(self):
        self.startTime = time.time()
        while self.running:
            # get values from joystick
            linearX = self.controller.get_axis(1)
            linearY = self.controller.get_axis(0)
            angularZ = self.controller.get_axis(3)
            
            # end detection
            if(linearX > 0.994): linearX = 1
            if(linearY > 0.994): linearY = 1
            if(angularZ > 0.994): angularZ = 1
                
            # deadzoning
            if(abs(linearX) < 0.01): linearX = 0
            if(abs(linearY) < 0.01): linearY = 0
            if(abs(angularZ) < 0.01): angularZ = 0
            
            # # scaling factors
            # linearX *= 50.0
            # angularZ *= 1.0
            
            # axis inversion (if needed)
            linearX *= -1.0
            linearY *= 1.0
            angularZ *= -1.0
            
            # kinematic equations
            if self.field_oriented:
                botHeading = math.radians(self.robot.yaw)
                rotX = linearX * math.cos(-botHeading) - linearY * math.sin(-botHeading)
                rotY = linearX * math.sin(-botHeading) + linearY * math.cos(-botHeading)

                denominator = max(abs(rotY) + abs(rotX) + abs(angularZ), 1)
                frontLeftPower = ((rotY + rotX + angularZ) / denominator) * 255
                backLeftPower = ((rotY - rotX + angularZ) / denominator) * 255
                frontRightPower = ((rotY - rotX - angularZ) / denominator) * 255
                backRightPower = ((rotY + rotX - angularZ) / denominator) * 255
            else:
                denominator = max(abs(linearY) + abs(linearX) + abs(angularZ), 1)
                frontLeftPower = ((linearY + linearX + angularZ) / denominator) * 255
                backLeftPower = ((linearY - linearX + angularZ) / denominator) * 255
                frontRightPower = ((linearY - linearX - angularZ) / denominator) * 255
                backRightPower = ((linearY + linearX - angularZ) / denominator) * 255

            self.robot.front_left_power = frontLeftPower
            self.robot.back_left_power = backLeftPower
            self.robot.front_right_power = frontRightPower
            self.robot.back_right_power = backRightPower

            self.robot.send()
            
            # quit logic
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False

            if self.enabled == False:
                self.running = False

            if time.time() - self.startTime >= 135:
                self.running = False
        
            time.sleep(0.5)



        self.controller.quit()
        pygame.joystick.quit()


main = Main()
main.autonInit()
main.auton()
main.teleopInit()
main.teleop()
