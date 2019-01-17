#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>

#define GATEWAY_ADDR 9

const byte numSlaves = 3;
const byte slaveAddress[numSlaves][5] = {
    {'R','x','A','A','A'+1},
    {'R','x','A','A','A'+2},
    {'R','x','A','A','A'+3}
};
RF24 radio(9, 10);

int PiCommand,WellToRead,i;

int dataToSend[1];
unsigned long ackData[2] = {-1, -1};


// struct for I2C communication
struct Reading{
    float id;
    float measurement;
};

Reading r;


void setup() {
    Wire.begin(0x10);
    Wire.onReceive(getCommandFromPi);
    Wire.onRequest(sendReadingToPi);
    
    Serial.begin(9600);
    
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    
    radio.enableAckPayload();
    
    radio.setPALevel(RF24_PA_LOW);          // Affects Range
    radio.setRetries(5,15);
}



void loop() {
    delay(100);
}




void getReading() {
    for (byte n = 0; n < numSlaves; n++){
        i++;
        radio.openWritingPipe(slaveAddress[n]);
        bool rslt;
        dataToSend[0] = 1;
        rslt = radio.write( &dataToSend, sizeof(dataToSend) );
        
        if (rslt) {
            if ( radio.isAckPayloadAvailable() ) {
                
                radio.read(&ackData, sizeof(ackData));
                
                // Create I2C structure //
                r.id = float(ackData[0]);
                if (ackData[1]/10.0 > 500) {
                    r.measurement = float(500);
                } else {
                    r.measurement = float(ackData[1]/10.0);
                }
                
                // Display on Serial Monitor for debugging //
                Serial.print(r.id);
                Serial.print(": ");
                Serial.println(r.measurement);
                
            }
            else {
                Serial.print(slaveAddress[n][4]-'A');
                Serial.println(": Acknowledge but no data ");
                r.id = float(slaveAddress[n][4]-'A');
                r.measurement = float(0);
            }
        }
        else {
            Serial.print(slaveAddress[n][4]-'A');
            Serial.println(": Tx failed");
            r.id = float(slaveAddress[n][4]-'A');
            r.measurement = float(-1);
        }
    }
}




void getCommandFromPi(int numBytes) {
    if (numBytes > 1) {
        PiCommand = Wire.read();
        WellToRead = Wire.read();
        getReading();
    }
}



void sendReadingToPi() {
    Wire.write((byte *) &r.measurement, sizeof(r.measurement));
}
