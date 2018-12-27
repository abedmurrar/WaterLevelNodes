#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>

#define GATEWAY_ADDR 9

const byte numSlaves = 3;
const byte slaveAddress[numSlaves][5] = {
    {'R','x','A','A','A'+1},
    {'R','x','A','A','A'+3},
    {'R','x','A','A','A'+8}
};
RF24 radio(9, 10);


int PiCommand,WellToRead;

int dataToSend[1];
int ackData[2] = {-1, -1};


// struct for I2C communication
struct Reading{
    float measurement;
};

void setup() {
    Wire.begin(0x10);
    Wire.onReceive(getCommandFromPi);
    Wire.onRequest(sendReadingToPi);
    
    Serial.begin(9600);
    
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    
    radio.enableAckPayload();
    
    radio.setPALevel(RF24_PA_LOW);          // Affects Range
    radio.setRetries(3,5);
}



void loop() {
    delay(100);
}




void getReading() {
    radio.openWritingPipe(slaveAddress[WellToRead]);
    bool rslt;
    dataToSend[0] = 1;
    rslt = radio.write( &dataToSend, sizeof(dataToSend) );
    Reading r;
    
    if (rslt) {
        if ( radio.isAckPayloadAvailable() ) {
            radio.read(&ackData, sizeof(ackData));
            
            // Create I2C structure //
            if (ackData[1]/10.0 > 500) {
                r.measurement = float(500);
            } else {
                r.measurement = float(ackData[1]/10.0);
            }
            
            // Display on Serial Monitor for debugging //
            Serial.print(WellToRead);
            Serial.print(" ");
            Serial.println(r.measurement);
            
        }
        else {
            Serial.print(slaveAddress[n][4]-'A');
            Serial.println(": Acknowledge but no data ");
            r.measurement = float(0);
        }
    }
    else {
        Serial.print(slaveAddress[n][4]-'A');
        Serial.println(": Tx failed");
        r.measurement = float(-1);
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
