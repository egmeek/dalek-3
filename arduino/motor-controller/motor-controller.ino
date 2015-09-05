/**
 * Incoming message format:
 *    { -- Begin Message: ASCII 123
 *    D -- Left Motor Direction: F or R
 *    V -- Left Motor Duty Cycle: ASCII 0-255
 *    D -- Right Motor Direction: F or R
 *    V -- Right Motor Duty Cycle: ASCII 0-255
 *    } -- End Message: ASCII 125
 */

#include <SoftwareSerial.h>

#define RX_PIN 2          // ATTiny85 pin 7
#define TX_PIN 5          // ATTiny85 pin 1 -- we won't be transmitting anyway

#define LEFT_DIR_PIN  4   // ATTiny85 pin 3
#define RIGHT_DIR_PIN 3   // ATTiny85 pin 2

#define LEFT_PWM_PIN  0   // ATTiny85 pin 5
#define RIGHT_PWM_PIN 1   // ATTiny85 pin 5

SoftwareSerial sSerial(RX_PIN, TX_PIN);

void setup() {
    // set the data rate for the SoftwareSerial port
    sSerial.begin(9600);
    
    pinMode(LEFT_DIR_PIN, OUTPUT);
    pinMode(RIGHT_DIR_PIN, OUTPUT);
    pinMode(LEFT_PWM_PIN, OUTPUT);
    pinMode(RIGHT_PWM_PIN, OUTPUT);
    
    set_dir(LEFT_DIR_PIN, 'F');
    set_duty_cycle(LEFT_PWM_PIN, 0);
    set_dir(RIGHT_DIR_PIN, 'F');
    set_duty_cycle(RIGHT_PWM_PIN, 0);
}

void loop() {
    if (sSerial.available() > 4) {
        char ldir, rdir;
        int ldc, rdc;
        if (sSerial.read() == 123) {
            ldir = sSerial.read();
            ldc = (int)sSerial.read();
            rdir = sSerial.read();
            rdc = (int)sSerial.read();
            if ((ldir == 'F' || ldir == 'R') && (rdir == 'F' || rdir == 'R')) {
                set_dir(LEFT_DIR_PIN, ldir);
                set_duty_cycle(LEFT_PWM_PIN, ldc);
                set_dir(RIGHT_DIR_PIN, rdir);
                set_duty_cycle(RIGHT_PWM_PIN, rdc);
            }
        }
    }
}

void set_dir(int pin, char dir) {
    switch (dir) {
        case 'F':
            digitalWrite(pin, LOW);
            break;
        case 'R':
            digitalWrite(pin, HIGH);
            break;
    }
}

void set_duty_cycle(int pin, int dc) {
    analogWrite(pin, dc);
}

