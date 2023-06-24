#!/usr/bin/python3
from connection import Robot
from time import sleep

robot = Robot()

angle = int(input("help"))

robot.four_bar = angle
robot.second_joint = angle

sleep(1)
robot.send()