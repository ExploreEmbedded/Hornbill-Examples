// this library is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple


#include <stdint.h>
#include <stdlib.h>
#include "max6675.h"
#include "driver/gpio.h"
#include "rom\gpio.h"
#include "rom/ets_sys.h"

static uint8_t max6675_spiread(void);
uint8_t sclk_pin,cs_pin,miso_pin;

void max6675_init(uint8_t SCLK, uint8_t CS, uint8_t MISO) {
  sclk_pin = SCLK;
  cs_pin = CS;
  miso_pin = MISO;

  gpio_pad_select_gpio(sclk_pin);
  gpio_pad_select_gpio(cs_pin);
  gpio_pad_select_gpio(miso_pin);
  //define pin direction
  gpio_set_direction(sclk_pin, GPIO_MODE_OUTPUT); 
  gpio_set_direction(cs_pin, GPIO_MODE_OUTPUT);
  gpio_set_direction(miso_pin, GPIO_MODE_INPUT);
 
  ets_delay_us(100);
  gpio_set_level(cs_pin, 1);
}

double max6675_readCelsius(void) {

  uint16_t v;

  gpio_set_level(cs_pin, 0);
  ets_delay_us(10);

  v = max6675_spiread();
  v <<= 8;
  v |= max6675_spiread();

  gpio_set_level(cs_pin, 1);

  if (v & 0x4) {
    // uh oh, no thermocouple attached!
    return 0; 
    //return -100;
  }

  v >>= 3;

  return v*0.25;
}

double max6675_readFahrenheit(void) {
  return max6675_readCelsius() * 9.0/5.0 + 32;
}

static uint8_t max6675_spiread(void) { 
  int i;
  uint8_t d = 0;
  

  for (i=7; i>=0; i--)
  {
    gpio_set_level(sclk_pin, 0);

    ets_delay_us(100);
    if (gpio_get_level(miso_pin)) 
	{
      //set the bit to 0 no matter what
      d |= (1 << i);
    }

    gpio_set_level(sclk_pin, 1);
    ets_delay_us(100);
  }

  return d;
}
