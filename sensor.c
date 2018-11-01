#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#define NODEID 2                        // Give a node ID

RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E5LL;   // Change the last byte for each node
const int trigPin = 3;
const int echoPin = 5;
long duration;
int distance=5;
int msg[2];

void setup(void){
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    Serial.begin(9600);
    radio.begin();
    radio.openWritingPipe(pipe);
}


void loop(void){
    // Ultrasonic reading //
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance= duration*0.034/2;
    
    // construct message //
    msg[0] = NODEID;
    msg[1] = distance;
    
    // send id and distance //
    int done = 0;
    while (done == 0) {
        done = radio.write(msg, sizeof(msg));
    }
    
    
    delay(1000);
    
}
