// this library is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple

#ifndef _MAX6675_H_
#define _MAX6675_H_

void max6675_init(uint8_t SCLK, uint8_t CS, uint8_t MISO);
double max6675_readCelsius(void);
double max6675_readFahrenheit(void);
#endif

