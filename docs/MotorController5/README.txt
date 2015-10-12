12345678901234567890123456789012345678901234567890123456789012345678901234567890
This controller should work and would be idea since it would be 3.3V, but for 
some reason the MSP430 will not communicate with the RPi. From what I have found
on the interwebs, it would appear to be a mismatch in the I2C speed. In a lame
attempt to preserve power without me asking it to, the MSP430 seems to run at a
lower clock speed with causes the funky USCI communications module to run I2C
at some non-standard speed.  I tried several things I read online to get it to
run at 100Hz most of which required setting some magic variable to some magic
constant value. Either the magic didn't work or there is some other problem.
I got tired of screwing with it and switched back to the Arduino.

This seems to be a reoccuring theme with the MSP430. Most of the community (and
there for the example code) for the MSP430 and LaunchPad use the TI IDEs instead
of Energia. These IDEs A) only run under windows and B) require a more indepth 
knowledge of how to program these chips at a lower level with all the magic 
variables. Maybe, when I have time, I can learn all the magic. Until then it
seems like the MSP430 platform is out of my reach, which is very disappointing.

