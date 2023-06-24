#!/usr/bin/python3

# used as my debug app

import pygame
import serial
from connection import Robot

from time import sleep
from os import environ

# robot constants
track_width = 126/2


# environ["SDL_VIDEO_ALLOW_SCREENSAVER"] = "1"
# environ["SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS"] = "1"
# environ["SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR"] = "0"

pygame.joystick.init()

controller = pygame.joystick.Joystick(0)

screen = pygame.display.set_mode((300,300))

pygame.display.init()

pygame.display.flip()

enabledCol = (0,255,0)
disabledCol = (255,0,0)


robot = Robot()

enabled = True

running = True

initialDisable = True

intake = 0

while running:
    
    # get values from joystick
    linearX = controller.get_axis(1)
    linearY = controller.get_axis(0)
    angularZ = controller.get_axis(3)
    
    # end detection
    if(linearX > 0.994):linearX = 1
    if(linearY > 0.994):linearY = 1
    if(angularZ > 0.994):angularZ = 1
    # deadzoning
    if(abs(linearX) < 0.01):linearX = 0
    if(abs(linearY) < 0.01):linearY = 0
    if(abs(angularZ) < 0.01):angularZ = 0
    
    # # scaling factors
    # linearX *= 50.0
    # angularZ *= 1.0
    
    # axis inversion (if needed)
    linearX *= 1.0
    linearY *= 1.0
    angularZ *= 1.0
    
    # kinematic equations

    # enable/disable logic
    enabled = controller.get_button(4)
    
    denominator = max(abs(linearY) + abs(linearX) + abs(angularZ), 1)
    frontLeftPower = ((linearY + linearX + angularZ) / denominator) * 100
    backLeftPower = ((linearY - linearX + angularZ) / denominator) * 100
    frontRightPower = ((linearY - linearX - angularZ) / denominator) * 100
    backRightPower = ((linearY + linearX - angularZ) / denominator) * 100
    
    robot.front_left_power = int(frontLeftPower)
    robot.front_right_power = int(frontRightPower)
    robot.back_left_power = int(backLeftPower)
    robot.back_right_power = int(backRightPower)
    
    
    # print(str(linearX) + " " + str(linearY) + " " + str(angularZ))
    # send data if enabled
    if(enabled):
        
        robot.enabled = True
        
        print(robot.send())
        
        
        
        
        # set screen color
        screen.fill(enabledCol)
        pygame.display.flip()
        
        initialDisable = True
        
    elif(not enabled):
        # send all 0s (technically)
        
        robot.enabled = False
        
        robot.send()
        
        # set screen color
        screen.fill(disabledCol)
        pygame.display.flip()
    
    # quit logic
    for event in pygame.event.get():
        
        if event.type == pygame.QUIT:
            running = False



controller.quit()
pygame.joystick.quit()
