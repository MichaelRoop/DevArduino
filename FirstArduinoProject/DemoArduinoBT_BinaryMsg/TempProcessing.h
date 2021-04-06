// TempProcessing.h

#ifndef _TEMPPROCESSING_h
#define _TEMPPROCESSING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class TemperatureProcessor
{
public:
	TemperatureProcessor();
	~TemperatureProcessor();

	void ProcessRaw(int sensorValue);
	float Kelvin();
	float Celcius();
	float Farenheit();


private:
	// value of R1 on board
	float R1;
	// steinhart-hart coeficients for thermistor
	float c1;
	float c2;
	float c3;
	float kelvin;
};


#endif
