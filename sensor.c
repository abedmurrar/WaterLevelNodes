
#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// Used for the NRF24L01+
int msg[1];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

// For the Ultrasonic
const int trigPin = 3;
const int echoPin = 5;
long duration;
int distance;

void setup(void) {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  radio.begin();
  radio.openWritingPipe(pipe);
}


void loop(void){

  // read ultrasonic distance into the distance variable
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance= duration*0.034/2;
  
  // send the distance
  msg[0] = distance;
  int done = 0;
  // keep trying to send, terrible evergy use, needs fixing
  while (done == 0) {
    done = radio.write(msg, 1);
  }
  
  delay(1000);
}
