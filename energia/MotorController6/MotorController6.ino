/****************************************************************
 * MotorController6
 * by Ron Smith
 * Copyright 2015 That Ain't Working, All Rights Reserved
 ****************************************************************/

#include <Wire.h>

#define LEFT_PWM_PIN        P2_1       // Physical Pin 9
#define RIGHT_PWM_PIN       P2_2       // Physical Pin 10

#define LEFT_DIR_PIN        P1_5       // Physical Pin 7
#define RIGHT_DIR_PIN       P2_0       // Physical Pin 98

#define I2C_ADDRESS         77         // ASCII 'M'  0x4d

#define CMD buf[0]
#define WCH buf[1]
#define VAL buf[2]

char left_current_dir  = 'F';
char right_current_dir = 'F';

volatile char cmd[4] = "";

/****************************************************************
 *
 ****************************************************************/
void setup() {
  Serial.begin(9600);           // start serial for output
  Serial.println("Setup begin");
  
  pinMode(LEFT_PWM_PIN, OUTPUT);
  pinMode(RIGHT_PWM_PIN, OUTPUT);
  pinMode(LEFT_DIR_PIN, OUTPUT);
  pinMode(RIGHT_DIR_PIN, OUTPUT);
  
  set_speed("SB0");
  set_direction("DBF");

  memset((void*)cmd, 0, 4);
  
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  
  Serial.println("Setup complete");
}


/****************************************************************
 *
 ****************************************************************/
void receiveEvent(int howMany) {
  int i = 0;
  char c;
  while(Wire.available() > 0) {
    c = Wire.read();
    if (i < 4) {
      cmd[i++] = c;
    }
  }
}


/****************************************************************
 *
 ****************************************************************/
void set_direction(char* buf) {
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


/****************************************************************
 *
 ****************************************************************/
void set_speed(char* buf) {
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
    
    int duty_cycle = 255 * (VAL - 48) / 10;
    
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


/****************************************************************
 *
 ****************************************************************/
void loop() {
  char buf[4] = "";
  if (cmd[0]) {
    // gotta get the data out of the cmd buffer before the next interrupt if possible
    memcpy((void*)buf, (void*)cmd, 4);
    memset((void*)cmd, 0, 4);
    
    Serial.print("Received command: ");
    Serial.println((char*)buf);
    
    if (CMD == 'D') {
      set_direction((char*)buf);
    } else if (CMD == 'S') {
      set_speed((char*)buf);
    } else {
      Serial.print("Invalid command");
      Serial.println((char*)buf);
    }
  } else {
    delay(100);
  }
}

