/**
 * Incoming message format:
 *    { -- Begin Message: ASCII 123
 *    D -- Left Motor Direction: F or R
 *    V -- Left Motor Duty Cycle: CHAR 0-9 (0-90% in 10% increment)
 *    D -- Right Motor Direction: F or R
 *    V -- Right Motor Duty Cycle: CHAR 0-9 (0-90% in 10% increment)
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
    if (sSerial.available() > 5) {
        char ldir, rdir, ldc, rdc;
        if (sSerial.read() == 123) {
            ldir = sSerial.read();
            ldc  = sSerial.read();
            rdir = sSerial.read();
            rdc  = sSerial.read();
            if (sSerial.read() == 125) {
                if ((ldir == 'F' || ldir == 'R') && (rdir == 'F' || rdir == 'R')) {
                    set_dir(LEFT_DIR_PIN, ldir);
                    set_duty_cycle(LEFT_PWM_PIN, ldc);
                    set_dir(RIGHT_DIR_PIN, rdir);
                    set_duty_cycle(RIGHT_PWM_PIN, rdc);
                }
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

void set_duty_cycle(int pin, char dc) {
    // Convert CHAR '0'-'9' representing 0-90% to PWM analog value 0-255:
    //    CHAR '0' is ASCII 48 so subtract 48 from the ASCII value to get int 0-9
    //    Multiply by 10 and divide by 100 (a.k.a. divide by 10) to get 0-90% (0-0.9)
    //    Multiply by 255 to get analog value
    //    Keep value a float until it is assigned to the int variable
    int aval = ((dc - 48) / 10.0) * 255.0;
    if (aval < 0) aval = 0;
    else if (aval > 255) aval = 255;    
    analogWrite(pin, aval);
}

