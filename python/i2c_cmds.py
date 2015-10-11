# i2c_cmds.py
# coding: UTF-8
# A file for importing some basic I2C commands for the dalek
# Author: Ron Smith
# Copyright ©2015 That Ain't Working, All Rights Reserved

from smbus import SMBus


MOTOR_CONTROLLER_ADDRESS = 77


bus = SMBus(1)


def direction(d):
    bus.write_i2c_block_data(MOTOR_CONTROLLER_ADDRESS, ord('{'),
                             [ord(b) for b in 'DB%s}' % d])


def speed(n):
    bus.write_i2c_block_data(MOTOR_CONTROLLER_ADDRESS, ord('{'),
                             [ord(b) for b in 'SB%d}' % n])


