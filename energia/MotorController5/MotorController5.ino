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
 * NOTE: One really nice feature of the Energia+MSP430 is it uses the physical pin numbers.
 */

#include <Wire.h>

#define LEFT_PWM_PIN        9
#define RIGHT_PWM_PIN      12

#define LEFT_DIR_PIN       10
#define RIGHT_DIR_PIN      11
                                    
#define I2C_ADDRESS        77       // ASCII 'M'


char left_current_dir  = 'F';
char right_current_dir = 'F';

volatile char cmd[4] = "";
volatile int i = 0;

void setup() {
    Serial.begin(9600);
    Serial.println("Setup start");
    
    pinMode(LEFT_PWM_PIN, OUTPUT);
    pinMode(RIGHT_PWM_PIN, OUTPUT);
    pinMode(LEFT_DIR_PIN, OUTPUT);
    pinMode(RIGHT_DIR_PIN, OUTPUT);
    
    set_speed("SB0");
    set_direction("DBF");
    
    BCSCTL1 = CALBC1_16MHZ;                   
    DCOCTL = CALDCO_16MHZ;
    
    Wire.begin(I2C_ADDRESS);
    Wire.onReceive(receive_event);
    
    Serial.println("Setup complete");
}


void loop() {
    delay(100);  // give it a little time to load the command bytes
}

void receive_event(int num_bytes) {
    while (Wire.available() > 0) {
        cmd[i++] = Wire.read();
        Serial.print("INFO: Read char from Wire: ");
        Serial.println(cmd[i-1]);
        switch(i) {
            case 1:
                if (!(cmd[0] == 'D' || cmd[0] == 'S')) {
                    Serial.print("ERROR: Invalid command: ");
                    Serial.println(cmd[0]);
                    i = 0;
                }
                break;
            case 2:
                if (!(cmd[1] == 'L' || cmd[1] == 'R' || cmd[1] == 'B')) {
                    Serial.print("ERROR: Invalid side: ");
                    Serial.println(cmd[1]);
                    i = 0;
                }
                break;
            case 3:
                Serial.print("INFO: Received command: ");
                Serial.println((char*)cmd);
                if (cmd[0] == 'D') {
                    set_direction((char*)cmd);
                } else if (cmd[0] == 'S') {
                    set_speed((char*)cmd);
                } else {
                    Serial.print("ERROR: Somehow we got to the end with an invalid command: ");
                    Serial.println(cmd[0]);
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
    if (cmd[0] != 'D') {
        Serial.print("ERROR: Invalid command: ");
        Serial.println(cmd[0]);
        return;
    }
    if (!(cmd[1] == 'L' || cmd[1] == 'R' || cmd[1] == 'B')) {
        Serial.print("ERROR: Invalid side: ");
        Serial.println(cmd[1]);
        return;
    }
    if (!(cmd[2] == 'F' || cmd[2] == 'R')) {
        Serial.print("ERROR: Invalid direction value: ");
        Serial.println(cmd[2]);
        return;
    }
    if (cmd[1] == 'L' || cmd[1] == 'B') {
        if (left_current_dir != cmd[2]) set_speed("SL0");
        Serial.print("INFO: Setting left direction: ");
        Serial.println(cmd[2] == 'F' ? "FORWARD" : "REVERSE");
        digitalWrite(LEFT_DIR_PIN, cmd[2] == 'F' ? LOW : HIGH);
        left_current_dir = cmd[2];
    }
    if (cmd[1] == 'R' || cmd[1] == 'B') {
        if (right_current_dir != cmd[2]) set_speed("SR0");
        Serial.print("INFO: Setting right direction: ");
        Serial.println(cmd[2] == 'F' ? "FORWARD" : "REVERSE");
        digitalWrite(RIGHT_DIR_PIN, cmd[2] == 'F' ? LOW : HIGH);
        right_current_dir = cmd[2];
    }
}


void set_speed(char* cmd) {
    if (cmd[0] != 'S') {
        Serial.print("ERROR: Invalid command: ");
        Serial.println(cmd[0]);
        return;
    }
    if (!(cmd[1] == 'L' || cmd[1] == 'R' || cmd[1] == 'B')) {
        Serial.print("ERROR: Invalid side: ");
        Serial.println(cmd[1]);
        return;
    }
    if (cmd[2] < 48 || cmd[2] > 58) {
        Serial.print("ERROR: Invalid speed value: ");
        Serial.println(cmd[2]);
        return;
    }
    
    int duty_cycle = 127 * (cmd[2] - 48) / 10;
    
    if (cmd[1] == 'L' || cmd[1] == 'B') {
        Serial.print("INFO: Setting left duty cycle: ");
        Serial.println(duty_cycle);
        analogWrite(LEFT_PWM_PIN, duty_cycle);
    }
    if (cmd[1] == 'R' || cmd[1] == 'B') {
        Serial.print("INFO: Setting right duty cycle: ");
        Serial.println(duty_cycle);
        analogWrite(RIGHT_PWM_PIN, duty_cycle);
    }
}

