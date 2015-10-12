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
#define LEFT_DIR_PIN        7       // Physical Pin 13
#define RIGHT_PWM_PIN      11       // Physical Pin 17
#define RIGHT_DIR_PIN      10       // Physical Pin 16
                                    
#define I2C_ADDRESS        77       // ASCII 'M'  0x4d

char left_current_dir  = 'F';
char right_current_dir = 'F';

volatile char cmd[4] = "";
volatile int i = 0;

#define CMD    cmd[0]
#define WCH    cmd[1]
#define VAL    cmd[2]

void setup() {
    Serial.begin(9600);
    Serial.println("Setup start");
    
    pinMode(LEFT_PWM_PIN, OUTPUT);
    pinMode(RIGHT_PWM_PIN, OUTPUT);
    pinMode(LEFT_DIR_PIN, OUTPUT);
    pinMode(RIGHT_DIR_PIN, OUTPUT);
    
    set_speed("SB0");
    set_direction("DBF");
    
    Wire.begin(I2C_ADDRESS);
    Wire.onReceive(receive_event);
    
    Serial.println("Setup complete");
}


void loop() {
    delay(100);
}

void receive_event(int num_bytes) {
    while (Wire.available() > 0) {
        cmd[i++] = Wire.read();
        Serial.print("INFO: Read char from Wire: ");
        Serial.println(cmd[i-1]);
        switch(i) {
            case 1:
                if (!(CMD == 'D' || CMD == 'S')) {
                    Serial.print("ERROR: Invalid command: ");
                    Serial.println(CMD);
                    i = 0;
                }
                break;
            case 2:
                if (!(WCH == 'L' || WCH == 'R' || WCH == 'B')) {
                    Serial.print("ERROR: Invalid side: ");
                    Serial.println(WCH);
                    i = 0;
                }
                break;
            case 3:
                Serial.print("INFO: Received command: ");
                Serial.println((char*)cmd);
                if (CMD == 'D') {
                    set_direction((char*)cmd);
                } else if (CMD == 'S') {
                    set_speed((char*)cmd);
                } else {
                    Serial.print("ERROR: Somehow we got to the end with an invalid command: ");
                    Serial.println(CMD);
                }
                i = 0;
                break;
            default:
                Serial.print("ERROR: value of i should never be ");
                Serial.println(i);
                cmd[3] = 0;
                i = 0;
                break;
        }
    }
}


void set_direction(char* cmd) {
    if (CMD != 'D') {
        Serial.print("ERROR: Invalid command: ");
        Serial.println(CMD);
        return;
    }
    if (!(WCH == 'L' || WCH == 'R' || WCH == 'B')) {
        Serial.print("ERROR: Invalid side: ");
        Serial.println(WCH);
        return;
    }
    if (!(VAL == 'F' || VAL == 'R')) {
        Serial.print("ERROR: Invalid direction value: ");
        Serial.println(VAL);
        return;
    }
    if (WCH == 'L' || WCH == 'B') {
        if (left_current_dir != VAL) set_speed("SL0");
        Serial.print("INFO: Setting left direction: ");
        Serial.println(VAL == 'F' ? "FORWARD" : "REVERSE");
        digitalWrite(LEFT_DIR_PIN, (VAL == 'F' ? LOW : HIGH));
        left_current_dir = VAL;
    }
    if (WCH == 'R' || WCH == 'B') {
        if (right_current_dir != VAL) set_speed("SR0");
        Serial.print("INFO: Setting right direction: ");
        Serial.println(VAL == 'F' ? "FORWARD" : "REVERSE");
        digitalWrite(RIGHT_DIR_PIN, (VAL == 'F' ? LOW : HIGH));
        right_current_dir = VAL;
    }
}


void set_speed(char* cmd) {
    if (CMD != 'S') {
        Serial.print("ERROR: Invalid command: ");
        Serial.println(CMD);
        return;
    }
    if (!(WCH == 'L' || WCH == 'R' || WCH == 'B')) {
        Serial.print("ERROR: Invalid side: ");
        Serial.println(WCH);
        return;
    }
    if (VAL < 48 || VAL > 58) {
        Serial.print("ERROR: Invalid speed value: ");
        Serial.println(VAL);
        return;
    }
    
    int duty_cycle = 127 * (VAL - 48) / 10;
    
    if (WCH == 'L' || WCH == 'B') {
        Serial.print("INFO: Setting left duty cycle: ");
        Serial.println(duty_cycle);
        analogWrite(LEFT_PWM_PIN, duty_cycle);
    }
    if (WCH == 'R' || WCH == 'B') {
        Serial.print("INFO: Setting right duty cycle: ");
        Serial.println(duty_cycle);
        analogWrite(RIGHT_PWM_PIN, duty_cycle);
    }
}

