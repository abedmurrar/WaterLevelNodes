#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

int msg[1];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup(void){
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
}

void loop(void){
  if (radio.available()){
    int done = 0;    
    while (done == 0){
      done = radio.read(msg, 1);     

      // display recieved data, mapping is done for my water container
      Serial.print("Water Level: "); 
      Serial.print(-5*(msg[0]-17));
      Serial.println("%");
      delay(10);
    }
  }
  
  delay(1000);
}