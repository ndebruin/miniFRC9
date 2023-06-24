#!/usr/bin/python3

# used as my debug app

import pygame
import pygame_widgets
from pygame_widgets.button import Button
from pygame_widgets.textbox import TextBox
from pygame_widgets.progressbar import ProgressBar
import serial
# import flet as ft
import threading

from time import sleep
import datetime
from os import environ
from connection import Robot

import time
import math

from os import system, name

class Main():
    def __init__(self):    
        # robot constants
        self.track_width = 126/2

        pygame.init()
        pygame.joystick.init()
        self.controller = pygame.joystick.Joystick(0)

        self.screen = pygame.display.set_mode((1280, 720))
        pygame.display.set_caption("Driver Station")
        self.screen_width = 1280
        self.screen_height = 720

        self.robot = Robot()
        self.standingby = False
        self.running = False
        self.auton_running = False
        self.field_oriented = True
        self.enabled = False
        self.state = "STANDBY"

        # Buttons
        # self.enable_button = Button(self.screen, self.screen_width/2 - 50, self.screen_height/2 - 50, 100, 50, text='ENABLE', fontSize=20, margin=20, inactiveColour=(0,255,0), onClick=lambda: self.enable(), radius=20)
        self.disable_button = Button(self.screen, self.screen_width/2 - 50, self.screen_height/2 + 25, 100, 50, text='DISABLE', fontSize=20, margin=20, inactiveColour=(255,0,0), onClick=lambda: self.disable(), radius=20)
        self.state_text = TextBox(self.screen, self.screen_width/2 - 93, self.screen_height/2 - 200, 0, 0, fontSize=60, text=self.state, color=(255, 255, 255), inactiveColour=(255,255,255), borderColour=(0,0,0), borderThickness=0)
        self.switch_to_auton = Button(self.screen, self.screen_width/2 - 50, self.screen_height/2 + 100, 100, 50, text='AUTON', fontSize=20, margin=20, inactiveColour=(175, 0, 255), onClick=lambda: self.switch_state_a(), radius=20)
        self.switch_to_teleop = Button(self.screen, self.screen_width/2 - 50, self.screen_height/2 + 175, 100, 50, text='TELEOP', fontSize=20, margin=20, inactiveColour=(175, 0, 255), onClick=lambda: self.switch_state_t(), radius=20)

    def updateWidgets(self):
        events = pygame.event.get()
        for event in events:
            if event.type == pygame.QUIT:
                pygame.quit()
                self.running = False
                self.enabled = False
                self.robot.enabled = False
                quit()
        self.state_text.text = self.state

        pygame_widgets.update(events)
        pygame.display.update()

    def switch_state_a(self):
        self.state = "AUTON"
        self.enable()

    def switch_state_t(self):
        self.state = "TELEOP"
        self.enable()

    def enable(self):
        if self.state != "STANDBY":
            self.enabled = True
            self.robot.enabled = True
            self.screen.fill((0, 255, 0))
            self.updateWidgets()

    def disable(self):
        self.enabled = False
        self.robot.enabled = False
        self.stop()
        self.screen.fill((255, 0, 0))
        self.updateWidgets()

    def clear(self):
        # for windows
        if name == 'nt':
            _ = system('cls')

        # for mac and linux
        else:
            _ = system('clear')

    def standby(self):
        if self.state == "STANDBY":
            if not self.enabled:
                self.screen.fill((0, 64, 191))
                self.updateWidgets()

    def auton(self):
        power = 0.6 * 255
        pitch = self.robot.pitch

        self.robot.front_left_power = power
        self.robot.back_right_power = power
        self.robot.back_left_power = power
        self.robot.front_right_power = power

        if pitch > 15:
            self.first_pitch = True
        
        if self.first_pitch:
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
                
        self.updateWidgets()

    def stop(self):
        self.robot.back_left_power = 0
        self.robot.front_left_power = 0
        self.robot.front_right_power = 0
        self.robot.back_right_power = 0
        self.robot.send()

    def teleop(self):
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
        
        # kinematic equations
        if self.field_oriented:
            botHeading = math.radians(self.robot.yaw)
            rotX = linearX * math.cos(-botHeading) - linearY * math.sin(-botHeading)
            rotY = linearX * math.sin(-botHeading) + linearY * math.cos(-botHeading)

            denominator = max(abs(rotY) + abs(rotX) + abs(angularZ), 1)
            frontLeftPower = ((rotY + rotX + angularZ) / denominator) * 100
            backLeftPower = ((rotY - rotX + angularZ) / denominator) * 100
            frontRightPower = ((rotY - rotX - angularZ) / denominator) * 100
            backRightPower = ((rotY + rotX - angularZ) / denominator) * 100
        else:
            denominator = max(abs(linearY) + abs(linearX) + abs(angularZ), 1)
            frontLeftPower = ((linearY + linearX + angularZ) / denominator) * 100
            backLeftPower = ((linearY - linearX + angularZ) / denominator) * 100
            frontRightPower = ((linearY - linearX - angularZ) / denominator) * 100
            backRightPower = ((linearY + linearX - angularZ) / denominator) * 100

        self.robot.front_left_power = int(frontLeftPower)
        self.robot.back_left_power = int(backLeftPower)
        self.robot.front_right_power = int(frontRightPower)
        self.robot.back_right_power = int(backRightPower)

        self.robot.four_bar = linearX
        self.robot.second_joint = angularZ

        print("sending " + str(frontLeftPower) + " " + str(backLeftPower) + " " + str(frontRightPower) + " " + str(backRightPower))

        self.robot.send()
        
        # quit logic
        self.updateWidgets()

    def loop(self):
        while True:
            if self.state == "STANDBY":
                self.standby()
            elif self.state == "AUTON":
                if self.enabled:
                    self.auton()
            elif self.state == "TELEOP":
                if self.enabled:
                    self.teleop()
            time.sleep(0.2)

main = Main()
main.loop()