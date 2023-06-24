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
        self.auton_time = 15
        self.teleop_time = 135 + 6
        self.auton_init = False
        self.teleop_init = False

        self.testing = False
        self.from_button = False

        # Buttons
        self.enable_button = Button(self.screen, self.screen_width/2 - 50, self.screen_height/2 - 50, 100, 50, text='ENABLE', fontSize=20, margin=20, inactiveColour=(0,255,0), onClick=lambda: self.enable(), radius=20)
        self.disable_button = Button(self.screen, self.screen_width/2 - 50, self.screen_height/2 + 25, 100, 50, text='DISABLE', fontSize=20, margin=20, inactiveColour=(255,0,0), onClick=lambda: self.disable(), radius=20)
        self.state_text = TextBox(self.screen, self.screen_width/2 - 93, self.screen_height/2 - 200, 0, 0, fontSize=60, text=self.state, color=(255, 255, 255), inactiveColour=(255,255,255), borderColour=(0,0,0), borderThickness=0)
        self.time_text = TextBox(self.screen, self.screen_width/2 - 93, self.screen_height/2 - 100, 0, 0, fontSize=60, text=str(round(self.auton_time)), color=(255, 255, 255), inactiveColour=(255,255,255), borderColour=(0,0,0), borderThickness=0)
        self.switch_to_auton = Button(self.screen, self.screen_width/2 - 50, self.screen_height/2 + 100, 100, 50, text='AUTON', fontSize=20, margin=20, inactiveColour=(175, 0, 255), onClick=lambda: self.switch_state_a(), radius=20)
        self.switch_to_teleop = Button(self.screen, self.screen_width/2 - 50, self.screen_height/2 + 175, 100, 50, text='TELEOP', fontSize=20, margin=20, inactiveColour=(175, 0, 255), onClick=lambda: self.switch_state_t(), radius=20)

    def updateWidgets(self):
        events = pygame.event.get()
        for event in events:
            if event.type == pygame.QUIT:
                pygame.quit()
                self.running = False
                self.enabled = False
                quit()
        self.screen.fill((0, 64, 191))
        self.state_text.text = self.state

        if self.state == "AUTON":
            self.teleop_time = 135 + 6
            mins, secs = divmod(round(self.auton_time), 60)
            time_str = '{:02d}:{:02d}'.format(mins, secs)
            self.time_text.text = time_str
            if (round(self.auton_time) == 0):
                self.time_text.textColour = (255, 0, 0)
        elif self.state == "TELEOP":
            self.auton_time = 15
            if not self.from_button:
                mins, secs = divmod(round(self.teleop_time), 60)
                time_str = '{:02d}:{:02d}'.format(mins, secs)
                self.time_text.text = time_str
                if (round(self.teleop_time) == 0):
                    self.time_text.textColour = (255, 0, 0)
                elif (round(self.teleop_time) == 30):
                    self.time_text.textColour = (255, 255, 0)
                elif (round(self.teleop_time) == 135):
                    self.time_text.textColour = (0, 0, 0)
            else:
                self.time_text.textColour = (0, 0, 0)
                self.time_text.text = ""
        else:
            self.time_text.textColour = (0, 0, 0)
            self.time_text.text = ""

        pygame_widgets.update(events)
        pygame.display.update()

    def switch_state_a(self):
        self.state = "AUTON"
        self.from_button = True
        self.enabled = True

    def switch_state_t(self):
        self.state = "TELEOP"
        self.from_button = True
        self.enabled = True

    def enable(self):
        self.enabled = True

    def disable(self):
        self.enabled = False
        self.state = "STANDBY"

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
                self.updateWidgets()
            else:
                self.state = "AUTON"

    def autonInit(self):
        if self.enabled and self.auton_init == False:
            self.auton_running = True
            self.first_pitch = False
            self.auton_init = True

    def auton(self):
        if self.auton_running:
            # self.robot.receive()
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
            
            # quit logic

            if self.enabled == False:
                self.auton_running = False
                self.auton_time = 15

            if round(self.auton_time) == 0:
                self.auton_running = False
                self.enabled = False
                if not self.testing:
                    if self.from_button:
                        self.state = "STANDBY"
                        self.from_button = False
                    else:
                        self.state = "TELEOP"
                else:
                    self.state = "STANDBY"

            self.auton_time -= 0.2
            self.updateWidgets()

    def stop(self):
        self.robot.back_left_power = 0
        self.robot.front_left_power = 0
        self.robot.front_right_power = 0
        self.robot.back_right_power = 0
        # self.robot.send()

    def teleopInit(self):
        if self.enabled and self.teleop_init == False:
            self.running = True
            self.state = "TELEOP"
            self.teleop_init = True

    def teleop(self):
        if self.running:
            self.robot.receive()

            # get values from joystick
            linearX = (self.controller.get_axis(1) * 90) + 90
            linearY = (self.controller.get_axis(0) * 90) + 90
            angularZ = (self.controller.get_axis(3) * 90) + 90
            
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
            # if self.field_oriented:
            #     botHeading = math.radians(self.robot.yaw)
            #     rotX = linearX * math.cos(-botHeading) - linearY * math.sin(-botHeading)
            #     rotY = linearX * math.sin(-botHeading) + linearY * math.cos(-botHeading)

            #     denominator = max(abs(rotY) + abs(rotX) + abs(angularZ), 1)
            #     frontLeftPower = ((rotY + rotX + angularZ) / denominator) * 255
            #     backLeftPower = ((rotY - rotX + angularZ) / denominator) * 255
            #     frontRightPower = ((rotY - rotX - angularZ) / denominator) * 255
            #     backRightPower = ((rotY + rotX - angularZ) / denominator) * 255
            # else:
            #     denominator = max(abs(linearY) + abs(linearX) + abs(angularZ), 1)
            #     frontLeftPower = ((linearY + linearX + angularZ) / denominator) * 255
            #     backLeftPower = ((linearY - linearX + angularZ) / denominator) * 255
            #     frontRightPower = ((linearY - linearX - angularZ) / denominator) * 255
            #     backRightPower = ((linearY + linearX - angularZ) / denominator) * 255

            # self.robot.front_left_power = frontLeftPower
            # self.robot.back_left_power = backLeftPower
            # self.robot.front_right_power = frontRightPower
            # self.robot.back_right_power = backRightPower

            self.robot.four_bar = linearX
            self.robot.second_joint = angularZ

            print("sending" + str(linearX))

            self.robot.send()
            
            # quit logic

            if self.enabled == False:
                self.running = False
                self.state = "STANDBY"
                self.from_button = False
                self.teleop_time = 135 + 6

                if round(self.teleop_time) == 0:
                    if not self.from_button:
                        self.running = False
                        self.enabled = False
                        self.state = "STANDBY"
                
            if not self.from_button:
                self.teleop_time -= 0.2
            self.updateWidgets()

    def loop(self):
        while True:
            if self.state == "STANDBY":
                self.standby()
            elif self.state == "AUTON":
                self.autonInit()
                self.auton()
            elif self.state == "TELEOP":
                self.teleopInit()
                self.teleop()

            time.sleep(0.2)

main = Main()
main.loop()