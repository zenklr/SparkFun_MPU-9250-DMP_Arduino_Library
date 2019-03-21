/******************************************************************************
arduino_mpu9250_i2c.cpp - MPU-9250 Digital Motion Processor Arduino Library 
Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library

This library implements motion processing functions of Invensense's MPU-9250.
It is based on their Emedded MotionDriver 6.12 library.
	https://www.invensense.com/developers/software-downloads/

Development environment specifics:
Arduino IDE 1.6.12
SparkFun 9DoF Razor IMU M0

Supported Platforms:
- ATSAMD21 (Arduino Zero, SparkFun SAMD21 Breakouts)
******************************************************************************/
#include "arduino_mpu9250_i2c.h"
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#define READ_FLAG 0x80
#define NOT_SPI -1
#define SPI_DATA_RATE 1000000 // 1MHz is the max speed of the MPU-9250
#define SPI_MODE SPI_MODE3

#define _csPin 10

// TODO Read SPI parameters from one place
// TODO implement select and deselect as a function

int arduino_i2c_write(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
	Wire.beginTransmission(slave_addr);
	Wire.write(reg_addr);
	for (unsigned char i = 0; i < length; i++)
	{
		Wire.write(data[i]);
	}
	Wire.endTransmission(true);
	
	return 0;
}

int arduino_i2c_read(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
	Wire.beginTransmission(slave_addr);
	Wire.write(reg_addr);
	Wire.endTransmission(false);
	Wire.requestFrom(slave_addr, length);
	for (unsigned char i = 0; i < length; i++)
	{
		data[i] = Wire.read();
	}
	
	return 0;
}

/*
	SPI Implementation
*/

int arduino_spi_write(unsigned char slave_addr, unsigned char reg_addr, 
						unsigned char length, unsigned char * data)
{
	SPI.beginTransaction(SPISettings(SPI_DATA_RATE, MSBFIRST, SPI_MODE));

	//select();
	digitalWrite(_csPin, LOW);

	SPI.transfer(reg_addr);
	for( int i = 0; i < length; i++)
	{
		SPI.transfer(data[i]);
	}

	/*
	Serial.print("lenght: ");
	Serial.println(length);
	Serial.print("writing: ");
	for (int i=0; i < length; i++)
	{
		Serial.print("0x");
		Serial.print(data[i], HEX);
		Serial.print(" ");
	}
	Serial.print(" to register: 0x");
	Serial.println(reg_addr, HEX);
	Serial.flush();
	*/

	//deselect();
	digitalWrite(_csPin, HIGH);

	SPI.endTransaction();

	return 0;
}

int arduino_spi_read(unsigned char slave_addr, unsigned char reg_addr,
                       unsigned char length, unsigned char * data)
{
	SPI.beginTransaction(SPISettings(SPI_DATA_RATE, MSBFIRST, SPI_MODE));

	//select();
	digitalWrite(_csPin, LOW);

	SPI.transfer(reg_addr | READ_FLAG);
	for (unsigned char i = 0; i < length; i++)
	{
		data[i] = SPI.transfer(0x00);
	}
	SPI.endTransaction();
	
	/*
	Serial.print("reading: ");
	for (int i=0; i < length; i++)
	{
		Serial.print("0x");
		Serial.print(data[i], HEX);
		Serial.print(" ");
	}
	Serial.print(" from register: 0x");
	Serial.println(reg_addr, HEX);
	Serial.flush();
	*/

  	//deselect();
	digitalWrite(_csPin, HIGH);
	
  	delayMicroseconds(50);
  	return 0;
}