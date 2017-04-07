/*
  Emon.h - Library for openenergymonitor
  Created by Trystan Lea, April 27 2010
  GNU GPL
  modified to use up to 12 bits ADC resolution (ex. Arduino Due)
  by boredman@boredomprojects.net 26.12.2013
  Low Pass filter for offset removal replaces HP filter 1/1/2015 - RW
*/

#ifndef _CurrentSensor_h_
#define _CurrentSensor_h_



// to enable 12-bit ADC resolution on Arduino Due,
// include the following line in main sketch inside setup() function:
//  analogReadResolution(ADC_BITS);
// otherwise will default to 10 bits, as in regular Arduino-based boards.

#define ADC_BITS    12


#define ADC_COUNTS  (1<<ADC_BITS)

typedef struct
{
    double realPower;
    double apparentPower;
    double powerFactor;
    double Vrms;
    double Irms;
}currentSensor_values_t;


void currentSensor_voltagePin(unsigned int _inPinV, double _VCAL, double _PHASECAL);
void currentSensor_currentPin(unsigned int _inPinI, double _ICAL);


void currentSensor_calcVI(unsigned int crossings, unsigned int timeout);
double currentSensor_calcIrms(unsigned int NUMBER_OF_SAMPLES);
void currentSensor_printAll();


#endif
