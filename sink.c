#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Wire.h>

#define GATEWAY_ADDR 9
int msg[2];
RF24 radio(9,10);


// Add all node addresses
const uint64_t pipe1 = 0xE8E8F0F0E1LL;
const uint64_t pipe2 = 0xE8E8F0F0E5LL;


// struct for I2C communication
struct Reading{
    float id;
    float measurement;
};



void setup() {
    Wire.begin(); // join i2c bus (address optional for master)
    Serial.begin(9600);
    
    radio.begin();
    radio.openReadingPipe(1,pipe1);
    radio.openReadingPipe(2,pipe2);
    radio.startListening();
}


void loop() {
    // Recieve message //
    if (radio.available()){
        int done = 0;
        while (done == 0){
            done = radio.read(msg, sizeof(msg));
            delay(10);
        }
    }
    
    
    // Create I2C structure //
    Reading r;
    r.id = float(msg[0]);
    r.measurement = float(msg[1]);
    
    // Display on Serial Monitor for debugging //
    Serial.print(r.id);
    Serial.print(" ");
    Serial.println(r.measurement);
    
    // send I2C //
    sendReading(r);
    delay(2000);
}


void sendReading(Reading r){
    Wire.beginTransmission(GATEWAY_ADDR); // transmit to device #8
    Wire.write((byte *) &r, sizeof(r));
    Wire.endTransmission();    // stop transmitting
}
