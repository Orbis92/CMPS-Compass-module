/*
 * CMPS12.h
 *
 *  Created on: 11.03.2018
 *      Author: chris
 *
 */
#ifndef CMPS12_H_
#define CMPS12_H_

#include "Wire.h"

#define CMPS12_ADDRESS 0x60

class CMPS12 {

	private:
		char roll, pitch;
		float angle;
		float temp;

	public:
		CMPS12();
		bool updateCompass();
		bool begin();
		bool autoCal(bool en = true);		

		float getHeading();
		char getPitch();
		char getRoll();
		char getTemp();
		unsigned char getCal();

		char* getCompassRose(unsigned short length = 10);
};

#endif /* CMPS12_H_ */
