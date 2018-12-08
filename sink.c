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

int i = 0;

int dataToSend[1];
int ackData[2] = {-1, -1};

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 2000;

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
    
    radio.setPALevel(RF24_PA_LOW);          // Affects Range
    radio.setRetries(3,5);
    radio.openWritingPipe(slaveAddress);
}

void loop() {
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        
        for (byte n = 0; n < numSlaves; n++){
            i++;
            radio.openWritingPipe(slaveAddress[n]);
            bool rslt;
            dataToSend[0] = 1;
            rslt = radio.write( &dataToSend, sizeof(dataToSend) );
            
            
            Reading r;
            
            if (rslt) {
                if ( radio.isAckPayloadAvailable() ) {
                    radio.read(&ackData, sizeof(ackData));
                    
                    // Create I2C structure //
                    r.id = float(ackData[0]);
                    if (ackData[1] > 500) {
                        r.measurement = float(500);
                    } else {
                        r.measurement = float(ackData[1]);
                    }
                    
                    // Display on Serial Monitor for debugging //
                    Serial.print(i);
                    Serial.print(" -> ");
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
            
            // send I2C //
            sendReading(r);
            
            prevMillis = millis();
        }
    }
}




void sendReading(Reading r){
    Wire.beginTransmission(GATEWAY_ADDR); // transmit to device #8
    Wire.write((byte *) &r, sizeof(r));
    Wire.endTransmission();    // stop transmitting
}
