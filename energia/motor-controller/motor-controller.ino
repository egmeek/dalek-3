// I2C Slave Motor Controller
// by Ron Smith
// Copyright ©2015 That Ain't Working, All Rights Reserved

#include <Wire.h>

#define I2C_ADDR      77

#define LEFT_DIR_PIN  4
#define RIGHT_DIR_PIN 3

#define LEFT_PWM_PIN  0
#define RIGHT_PWM_PIN 1


void setup() {
    Wire.begin(I2C_ADDR);
    Wire.onReceive(receiveEvent);
    Serial.begin(9600);
}

void loop() {
    delay(100);
}

// NOTE: Since there is only one PWM pin available when using I2C, 
//       maybe there should be a separate motor controller for each motor.
//       Then we could make separate commands for direction and speed.
//       When direction is changed, it would always set the speed to 0
//
//       It's kind of a waste on a 20-pin MCU but in the future, we could add 
//       a monitor of the actuall wheel speed, temp sensor on the motor, etc.

void receiveEvent(int bytes) {
    if (Wire.available() > 5) {
        char v, cmd[4];
        int i = 0;
        while (Wire.available() > 0) {
            v = Wire.read();
            switch (v) {
               case 123:    // {
                    i = 0;
                    break;
               case 125:    // }
                    if (i == 4) update_speed_and_direction(cmd);
                    else Serial.println("Did not receive 4 cmd chars before closing brace");
                    i = 0;
                    break;
               default:
                    if (i < 4) {
                        cmd[i++] = v;
                    } else {
                        Serial.println("Too many cmd chars before closing brace");
                        i = 0;
                    }
                    break;
            }
        }
    }
}

void update_speed_and_direction(char* cmd) {
    if ((cmd[0] == 'F' || cmd[0] == 'R') && 
        (cmd[1] >= '0' && cmd[1] <= '9') &&
        (cmd[2] == 'F' || cmd[2] == 'R') && 
        (cmd[3] >= '0' && cmd[3] <= '9')) {
            set_dir(LEFT_DIR_PIN, ldir);
            set_duty_cycle(LEFT_PWM_PIN, ldc);
            set_dir(RIGHT_DIR_PIN, rdir);
            set_duty_cycle(RIGHT_PWM_PIN, rdc);
    }
}
    
    
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}

