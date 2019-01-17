// Well Nodes //

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define NODEID 2

const int trigPin = 5;
const int echoPin = 6;
const int ultrasonicGND = 7;
float duration;
float distance;
int WakeUpFlag;

const byte thisSlaveAddress[5] = {'R','x','A','A','A'+NODEID};

RF24 radio(3, 4);

int dataReceived[1];
unsigned long ackData[2] = {NODEID, 0};

void setup() {
    pinMode(trigPin,OUTPUT);
    pinMode(echoPin,OUTPUT);
    pinMode(ultrasonicGND,OUTPUT);
    digitalWrite(ultrasonicGND,1);
    digitalWrite(trigPin,1);
    digitalWrite(echoPin,1);
    
    power_adc_disable();
    
    //Serial.begin(9600);
    
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
                ackData[1] = getUltrasonic(15);
                //Serial.println(ackData[1]);
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




int getUltrasonic(int N) {
    pinMode(trigPin,OUTPUT);
    pinMode(echoPin,INPUT);
    pinMode(ultrasonicGND,OUTPUT);
    digitalWrite(ultrasonicGND,0);
    digitalWrite(trigPin,0);
    
    delay(450);
    int i;
    int avg=0;
    for (i=0;i<N;i++) {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH, 30000);
        if (duration == 0) duration = 30000;
        distance = duration*0.34/2;
        avg += distance;
        delay(25);
    }
    avg = avg / N;
    
    pinMode(trigPin,OUTPUT);
    pinMode(echoPin,OUTPUT);
    pinMode(ultrasonicGND,OUTPUT);
    digitalWrite(ultrasonicGND,1);
    digitalWrite(trigPin,1);
    digitalWrite(echoPin,1);
    
    
    return avg;
}



void wakeUp() {
    WakeUpFlag = 1;
    sleep_disable();
    detachInterrupt (0);
}
