#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>

#define GATEWAY_ADDR 9

const byte numSlaves = 2;
const byte slaveAddress[numSlaves][5] = {
    {'R','x','A','A','B'},
    {'R','x','A','A','C'}
};
RF24 radio(9, 10);

int dataToSend[1];
int ackData[2] = {-1, -1};

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000;

// struct for I2C communication
struct Reading{
    float id;
    float measurement;
};

void setup() {
    Wire.begin(); // join i2c bus (address optional for master)
    Serial.begin(9600);
    
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    
    radio.enableAckPayload();
    
    radio.setRetries(3,5);
    radio.openWritingPipe(slaveAddress);
}

void loop() {
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        
        for (byte n = 0; n < numSlaves; n++){
            
            radio.openWritingPipe(slaveAddress[n]);
            bool rslt;
            dataToSend[0] = 1;
            rslt = radio.write( &dataToSend, sizeof(dataToSend) );
            
            if (rslt) {
                if ( radio.isAckPayloadAvailable() ) {
                    radio.read(&ackData, sizeof(ackData));
                    
                    // Create I2C structure //
                    Reading r;
                    r.id = float(ackData[0]);
                    r.measurement = float(ackData[1]);
                    
                    // Display on Serial Monitor for debugging //
                    Serial.print(r.id);
                    Serial.print(" ");
                    Serial.println(r.measurement);
                    
                    // send I2C //
                    sendReading(r);
                }
                else {
                    Serial.println("  Acknowledge but no data ");
                }
            }
            else {
                Serial.println("  Tx failed");
            }
            
            prevMillis = millis();
        }
    }
}




void sendReading(Reading r){
    Wire.beginTransmission(GATEWAY_ADDR); // transmit to device #8
    Wire.write((byte *) &r, sizeof(r));
    Wire.endTransmission();    // stop transmitting
}
