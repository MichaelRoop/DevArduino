// TemperatureProcessing.h

#ifndef _TEMPERATUREPROCESSING_h
#define _TEMPERATUREPROCESSING_h

#ifdef __cplusplus
extern "C" {
#endif

float TemperatureKelvin(int sensorValue);
float KelvinToCelcius(float kelvin);
float KelvinToFarenheit(float kelvin);

#ifdef __cplusplus
}
#endif

#endif

