#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdint.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>


int presence(uint8_t pin);

void writeBit(uint8_t pin, int b); 

void writeByte(uint8_t pin, int byte);

uint8_t readBit(uint8_t pin);

int readByte(uint8_t pin);

int convert(uint8_t pin);

uint8_t crc8(uint8_t *data, uint8_t len);

float getTemperature(uint8_t pin);
