#!/usr/bin/python3
from connection import Robot
from time import sleep

robot = Robot()

robot.front_right_power = 100

robot.enabled = 1

sleep(5)
robot.send()

robot.front_right_power = 0
robot.enabled = 0
sleep(5)
robot.send()