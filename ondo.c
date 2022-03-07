#include "ondo.h"

//Initialization Pulse
//Every transaction with the a 1-wire device starts with an initialization handshake
int presence(uint8_t pin){      
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        delayMicroseconds(480);
        pinMode(pin, INPUT);
        delayMicroseconds(70);
        uint8_t b = digitalRead(pin);
        delayMicroseconds(410);
        return b;
}

//send one bit to the thermometer sensor
void writeBit(uint8_t pin, int b){  
        int delay1, delay2;
        if (b == 1) {
                delay1 = 6;
                delay2 = 64;
        } else {
                delay1 = 80;
                delay2 = 10;
        }
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        delayMicroseconds(delay1);
        pinMode(pin, INPUT);
        delayMicroseconds(delay2);
}

//send bits to the thermometer sensor by one byte
void writeByte(uint8_t pin, int byte) {
    int i;
    for (i = 0; i < 8; i++) {
        if (byte & 1) {
            writeBit(pin,1);
        } else {
            writeBit(pin,0);
        }
        byte = byte >> 1;
    }
}

//read one bit from the scratchpad of the thermometer sensor
uint8_t readBit(uint8_t pin){
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        delayMicroseconds(6);
        pinMode(pin, INPUT);
        delayMicroseconds(8);
        uint8_t b = digitalRead(pin);
        delayMicroseconds(55);
        return b;
}

//read bits from the scratchpad of the thermometer sensor by one byte
int readByte(uint8_t pin){
    int byte = 0;
    int i;
    for (i = 0; i < 8; i++) {
        byte = byte | readBit(pin) << i;
    };
    return byte;
}

//Starts the DS18B20 making a temperature measurement
int convert(uint8_t pin) {
    int i;
    writeByte(pin, 0x44);   //Send a Convert command 0x44
    /*
    Convert conmmand initiates a single temperature conversion. 
    Following the conversion, the resulting thermal data is stored 
    in the 2-byte temperature register in the scratch-pad memory and 
    the DS18B20 returns to its low-power idle state
    */
    for (i = 0; i < 1000; i++) {
        delayMicroseconds(100000);
        if (readBit(pin) == 1)break;
    }
    return i;
}

//Check CRC for errors
uint8_t crc8(uint8_t *data, uint8_t len) {
    uint8_t i;
    uint8_t j;
    uint8_t temp;
    uint8_t databyte;
    
    uint8_t crc = 0;
    for (i = 0; i < len; i++) {
        databyte = data[i];
        for (j = 0; j < 8; j++) {
            temp = (crc ^ databyte) & 0x01;
            crc >>= 1;
            if (temp)
                crc ^= 0x8C;
            databyte >>= 1;
        }
    }
    return crc;
}


//A Function to get the temperature
float getTemperature(uint8_t pin) {
    if (presence(pin) == 1) return -1000;
    writeByte(pin, 0xCC);   //Send Skip ROM 0xCC command
    /*
    Skip ROM 0xCC command to address all devices on the bus simultaneously
    without sending out any ROM code information
    */
    convert(pin);
    presence(pin);
    writeByte(pin, 0xCC);
    writeByte(pin, 0xBE);   //Send Read Scratch Pad 0xBE command
    int i;
    uint8_t data[9];
    for (i = 0; i < 9; i++) {
        data[i] = readByte(pin);
    }
    uint8_t crc = crc8(data, 9);
    if(crc!=0) return -2000;
    int t1 = data[0];   
    int t2 = data[1];
    /* 
    Get the first two bytes which are the least and most 
    significant bytes of the 11-bit temperature 
    reading as a 16-bit, 2-complement integer
    */
    int16_t temp1 = (t2 << 8 | t1);
    float temp = (float) temp1 / 16;    //convert the temperature to a scaled floating point value
    return temp;
}

