# coding: UTF-8

# i2c_cmds.py
# A file for importing some basic I2C commands for the dalek
# Author: Ron Smith
# Copyright ©2015 That Ain't Working, All Rights Reserved

from smbus import SMBus


MOTOR_CONTROLLER_ADDRESS = 0x4d	 # aka 77 or 'M'

WHICH = 'LRB'  # Left, Right or Both
DIR = 'FR'  # Forward or Backward


bus = SMBus(1)


def direction(w, d):
    if w[0] in WHICH and d[0] in DIR:
        bus.write_i2c_block_data(MOTOR_CONTROLLER_ADDRESS, ord('D'), [ord(w[0]), ord(d[0])])


def speed(w, n):
    if w[0] in WHICH and n >= 0 and n <= 10:
        bus.write_i2c_block_data(MOTOR_CONTROLLER_ADDRESS, ord('S'), [ord(w[0]), 48+n])
    else:
        raise Exception("Invalid values")


