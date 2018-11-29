

// Well Nodes //

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define NODEID 2

const int trigPin = 3;
const int echoPin = 5;
int WakeUpFlag;

long duration;
int distance;

const byte thisSlaveAddress[5] = {'R','x','A','A','A'+NODEID};

RF24 radio(9, 10);

int dataReceived[1];
int ackData[2] = {NODEID, 0};

void setup() {
    power_adc_disable();
    
    Serial.begin(9600);
    
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, thisSlaveAddress);
    
    radio.enableAckPayload();
    radio.writeAckPayload(1, &ackData, sizeof(ackData));
    radio.maskIRQ(1,1,0);
    radio.setPALevel(RF24_PA_LOW);          // Affects Range
    
    radio.startListening();
    WakeUpFlag = 0;
    attachInterrupt(0, wakeUp, FALLING);
}




void loop() {
    if (WakeUpFlag == 1) {
        if ( radio.available() ) {
            radio.read( &dataReceived, sizeof(dataReceived) );
            if (dataReceived[0] == 1) {
                ackData[0] = NODEID;
                ackData[1] = getUltrasonic();
                Serial.println(ackData[1]);
                radio.writeAckPayload(1, &ackData, sizeof(ackData));
            }
        }
    }
    WakeUpFlag = 0;
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    
    noInterrupts ();
    attachInterrupt (0, wakeUp, FALLING);
    EIFR = bit (INTF0);
    
    interrupts ();
    sleep_cpu ();
}




int getUltrasonic() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration*0.034/2;
    return distance;
}



void wakeUp() {
    WakeUpFlag = 1;
    sleep_disable();
    detachInterrupt (0);
}
