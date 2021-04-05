// 
// 
// 

#include "TemperatureProcessing.h"

//float TemperatureKelvin(int sensorValue) {
//	// KY-013 analog temperature sensor
//	// https://arduinomodules.info/ky-013-analog-temperature-sensor-module/
//	float R1 = 10000; // value of R1 on board
//	float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  // steinhart-hart coeficients for thermistor
//	float R2 = R1 * (1023.0 / (float)sensorValue - 1.0);			 // calculate resistance on thermistor
//	float logR2 = log(R2);
//	return (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2)); // temperature in Kelvin
//}
//
//
//// Convert Kelvin to Celcius
//float KelvinToCelcius(float kelvin) {
//	return kelvin - 273.15;
//}
//
//
//// Convert Kelvin to Fahrenheit
//float KelvinToFarenheit(float kelvin) {
//	return (KelvinToCelcius(kelvin) * 9.0) / 5.0 + 32.0;
//}


//TemperatureProcessor::TemperatureProcessor() {
//	this.R1 = 10000;
//	// steinhart-hart coeficients for thermistor
//	this.c1 = 0.001129148;
//	this.c2 = 0.000234125;
//	this.c3 = 0.0000000876741;
//	this.kalvin = 0;
//}
//
//TemperatureProcessor::~TemperatureProcessor()
//{
//}
//
//void TemperatureProcessor::ProcessRaw(int sensorValue) {
//}
//
//
//float TemperatureProcessor::TemperatureKelvin() {
//	return 0;
//}
//
//float TemperatureProcessor::KelvinToCelcius() {
//	return 0;
//}
//
//float TemperatureProcessor::KelvinToFarenheit() {
//	return 0;
//}
//





