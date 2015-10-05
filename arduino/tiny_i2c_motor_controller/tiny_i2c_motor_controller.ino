/* ATtiny85 as an I2C Motor Controller
 * by Ron Smith
 * Copyright 2015 That Ain't Working, All Rights Reserved
 * 
 * SETUP:
 * Pin 1 = (RESET)                            Pin 8 = VCC (2.7-5.5V)
 * Pin 2 = (D3)(A3) I2C Address Jumper        Pin 7 = (D2)(A1) I2C SCL
 * Pin 3 = (D4)(A2) PWM to motor              Pin 6 = (D1) Direction Control to Relay
 * Pin 4 = GND                                Pin 5 = (D0) I2C SDA
 *
 * COMMANDS:
 * "DF"    Sets direction forward
 * "DR"    Sets direction reverse
 * "S0"    Sets speed. Second byte can be ASCII 0-9 which represents 0-90% duty cycle.
 * NOTE: changing direction always changes the speed to 0 to protect motor and other components.
 *
 * REMEMBER: pull pin 2 high for the left motor, and low for right motor.  This will set the
 *           I2C address to 76 (ASCII L) or 82 (ASCII R) respectively. Perhaps a jumper...
 * 
 * Credit and thanks to BroHogan for his TinyWire libraries.
 * Credit and thanks to Don Blake for his usiTwiSlave code. 
 */

#include <TinyWireS.h>

#define PWM_PIN     4            // Physical Pin 3
#define DIR_PIN     1            // Physical Pin 6
#define JUMPER_PIN  3            // Physical pin 2

#define LEFT_ADDR   76           // Left I2C address
#define RIGHT_ADDR  82           // Right I2C address


char current_dir = 'F';


void setup() {
    pinMode(PWM_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(JUMPER_PIN, INPUT);
    
    set_speed('0');
    set_direction('F');
    
    int jumper = (digitalRead(JUMPER_PIN) == HIGH);
    
    TinyWireS.begin(jumper ? LEFT_ADDR : RIGHT_ADDR);
}


void loop() {
    if (TinyWireS.available() > 1) {
        char cmd = TinyWireS.receive();
        char val = TinyWireS.receive();
        switch (cmd) {
            case 'D':
                set_direction(val);
                break;
            case 'S':
                set_speed(val);
                break;
        }
    }
}


void set_direction(char val) {
    if (current_dir != val) set_speed('0');
    switch (val) {
        case 'F':
            digitalWrite(DIR_PIN, LOW);
            current_dir = 'F';
            break;
        case 'R':
            digitalWrite(DIR_PIN, HIGH);
            current_dir = 'R';
            break;
    }
}


void set_speed(char val) {
    if (val >= 48 && val <= 58) {
        analogWrite(PWM_PIN, 127 * (48 - val) / 10);
    }
}

