/* I2C Dual Motor Controller
 * by Ron Smith
 * Copyright 2015 That Ain't Working, All Rights Reserved
 * 
 * COMMANDS:
 * "{D[L|R|B][F|R]}"    Sets direction for Left|Right|Both motors to Forward|Reverse
 * "{S[L|R|B][0-9]}"    Sets speed for Left|Right|Both motors to 0-9 (equates to duty cycle 0-90%)
 *
 * NOTE: changing direction always changes the speed to 0 to protect motor and other components.
 *
 */

#include <Wire.h>

#define LEFT_PWM_PIN        6       // Physical Pin 12
#define RIGHT_PWM_PIN      11       // Physical Pin 17

#define LEFT_DIR_PIN        8       // Physical Pin 14
#define RIGHT_DIR_PIN       9       // Physical Pin 15
                                    
#define I2C_ADDRESS        77       // ASCII 'M'


char left_current_dir  = 'F';
char right_current_dir = 'F';

void setup() {
    Serial.begin(9600);
    Serial.println("Setup start");
    
    pinMode(LEFT_PWM_PIN, OUTPUT);
    pinMode(RIGHT_PWM_PIN, OUTPUT);
    pinMode(LEFT_DIR_PIN, OUTPUT);
    pinMode(RIGHT_DIR_PIN, OUTPUT);
    
    set_speed('B', '0');
    set_direction('B', 'F');
    
    Wire.begin(I2C_ADDRESS);
    Wire.onReceive(receive_event);
    
    Serial.println("Setup complete");
}


void loop() {
    //delay(100);
}

void receive_event(int byte_count) {
    if (Wire.available() <5) {
        Serial.print("DEBUG: Received event with < 5 bytes: ");
        Serial.println(Wire.available());
        return;
    }
    char start = Wire.read();
    if (start != '{') {
        Serial.print("DEBUG: First byte not start cmd: ");
        Serial.println(start);
        return;
    }
    char cmd = Wire.read();
    if (!(cmd == 'D' || cmd == 'S')) {
        Serial.print("ERROR: Invalid command: ");
        Serial.println(cmd);
        return;
    }
    char side = Wire.read();
    if (!(side == 'L' || side == 'R' || side == 'B')) {
        Serial.print("ERROR: Invalid side: ");
        Serial.println(side);
        return;
    }
    char val = Wire.read();
    char term = Wire.read();
    if (term != '}') {
        Serial.print("WARNING: Invalid terminator: ");
        Serial.println(term);
        return;
    }
    if (cmd == 'D') {
        set_direction(side, val);
    } else {
        set_speed(side, val);
    }
}


void set_direction(char side, char val) {
    if (!(val == 'F' || val == 'R')) {
        Serial.print("ERROR: Invalid direction value: ");
        Serial.println(val);
        return;
    }
    if (side == 'L' || side == 'B') {
        if (left_current_dir != val) set_speed('L', '0');
        Serial.print("INFO: Setting left direction: ");
        Serial.println(val == 'F' ? "FORWARD" : "REVERSE");
        digitalWrite(LEFT_DIR_PIN, val == 'F' ? LOW : HIGH);
        left_current_dir = val;
    }
    if (side == 'R' || side == 'B') {
        if (right_current_dir != val) set_speed('R', '0');
        Serial.print("INFO: Setting right direction: ");
        Serial.println(val == 'F' ? "FORWARD" : "REVERSE");
        digitalWrite(RIGHT_DIR_PIN, val == 'F' ? LOW : HIGH);
        right_current_dir = val;
    }
}


void set_speed(char side, char val) {
    if (val < 48 && val > 58) {
        Serial.print("ERROR: Invalid speed value: ");
        Serial.println(val);
        return;
    }
    int duty_cycle = 127 * (val - 48) / 10;
    if (side == 'L' || side == 'B') {
        Serial.print("INFO: Setting left duty cycle: ");
        Serial.println(duty_cycle);
        analogWrite(LEFT_PWM_PIN, duty_cycle);
    }
    if (side == 'R' || side == 'B') {
        Serial.print("INFO: Setting right duty cycle: ");
        Serial.println(duty_cycle);
        analogWrite(RIGHT_PWM_PIN, duty_cycle);
    }
}

