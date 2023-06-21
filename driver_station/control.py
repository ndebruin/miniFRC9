#!/usr/bin/python3

# used as my debug app

import pygame
import serial

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


try: 
    ser = serial.Serial('/dev/rfcomm0', 115200)
except serial.SerialException:
    print("Port cannot be opened")
    quit()

enabled = True

running = True

initialDisable = True

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
    linearX *= -1.0
    linearY *= 1.0
    angularZ *= -1.0
    
    # kinematic equations
    
    # enable/disable logic
    enabled = controller.get_button(4)
    
    # print(str(linearX) + " " + str(linearY) + " " + str(angularZ))
    
    # send data if enabled
    if(enabled):
        # send values
        data = "{:.3f},{:.3f}\n".format(linearX, angularZ)
        print(data)
        
        
        ser.write(data.encode())
        
        # set screen color
        screen.fill(enabledCol)
        pygame.display.flip()
        
        initialDisable = True
        
    else:
        # send all 0s (technically)
        if initialDisable:
            ser.write("0,0,0\n".encode())
            initialDisable = False
        
        # set screen color
        screen.fill(disabledCol)
        pygame.display.flip()
    
    # quit logic
    for event in pygame.event.get():
        
        if event.type == pygame.QUIT:
            running = False



controller.quit()
pygame.joystick.quit()
