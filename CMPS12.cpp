#include "CMPS12.h"

CMPS12::CMPS12() {
	
}

bool CMPS12::begin() {
     Wire.begin();
     return true;
}


bool CMPS12::updateCompass() {
	unsigned short angle16;

	Wire.beginTransmission(CMPS12_ADDRESS);  //starts communication with CMPS12
	Wire.write(2);                     	    //Sends the register we wish to start reading from
	Wire.endTransmission();

	// Request 4 bytes from the CMPS12
	// both bytes of the 16 bit bearing, ////pitch and roll -> 4
	Wire.requestFrom(CMPS12_ADDRESS, 2);

	while (Wire.available() < 2);       	// Wait for all bytes to come back
	// Read back the 4 bytes
	angle16  = Wire.read();                 // Calculate 16 bit angle
	angle16 <<= 8;
	angle16 += Wire.read();
	//pitch    = Wire.read();
	//roll     = Wire.read();

	if (angle16 <= 3600) {			//crude check 0-3599
		angle = ((float)angle16)/10.0;
		return true;
	}
	else return false;
}

float CMPS12::getHeading() {
	return angle;
}

char CMPS12::getPitch() {
	return pitch;
}

char CMPS12::getRoll() {
	return roll;
}

float getTemp() {
	unsigned short temp16;

	Wire.beginTransmission(CMPS12_ADDRESS);
	Wire.write(18);							//temp value start in Reg 18
	Wire.endTransmission();

	Wire.requestFrom(CMPS12_ADDRESS, 2);
	while (Wire.available() < 2);
	temp16  = Wire.read();                 // Calculate 16 bit temp
	temp16 <<= 8;
	temp16 += Wire.read();

	return temp16;
}

bool autoCal(bool en) {
	Wire.beginTransmission(CMPS12_ADDRESS);  //starts communication with CMPS12
	Wire.write(0x98);                     	    
	delay(20);
	Wire.write(0x95);                     	    
	delay(20);
	Wire.write(0x99);                     	    
	delay(20);
	byte data;
	if (en) data = 0b10010111; //full auto cal, auto save
	else    data = 0b10000000;
	Wire.write(data);
	Wire.endTransmission();
}

unsigned char CMPS12::getCal() {
	// Get Cal Status
	Wire.beginTransmission(CMPS12_ADDRESS);
	Wire.write(30);							//Cal value is in Reg 30
	Wire.endTransmission();

	Wire.requestFrom(CMPS12_ADDRESS, 1);
	while (Wire.available() < 1);
	unsigned char cal = Wire.read();

	return cal & 0b00000011;
}

char* CMPS12:: getCompassRose(unsigned short visibleLength) {
  // -2 for extra side symbol if centered: [W]--N--E and "StringEnd" in the visible String
  unsigned short size = 2 * (visibleLength - 2);
  double phi = 360.0 / size;

  // " + phi/2 " to center the display of eg. East from an angle of 87.0 to 92.99
  // instead of 90.0 to 95.99 deg   (angle +/-2deg instead of angle +4deg)
  unsigned short start = (short)((angle + phi/2) / phi);
  unsigned short stop  = (short)(start + size / 2);

  //             west  = 0;
  unsigned short north = (short)size * 0.25;
  unsigned short east  = (short)size * 0.50;
  unsigned short south = (short)size * 0.75;

  char rose[size];			//360deg String
  memset(rose, '-', size);      //fill "rose" with '-'
  rose[0]     = 'W';
  rose[north] = 'N';
  rose[east]  = 'E';
  rose[south] = 'S';

  // Visible String
  char out[visibleLength];
  unsigned short j = 0, k = 0;
  for (short i = start; i <= stop; i++) {
    j = i;

    //Wrap around if the stop is bigger than size
    if (j >= size) {
      j = j - size;
    }

    out[k] = rose[j];
    k++;
  }
  out[k] = '\0';		//End of string

  return out;
}
