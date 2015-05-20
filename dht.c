// Access from ARM Running Linux
//##define BCM2708_PERI_BASE        0x20000000
//#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include "gpio_lib.h"
#define MAXTIMINGS 100
#define PG04 SUNXI_GPG(4) //####### DATA-Pin select

int dhtpin = PG04;//####### DATA-Pin select

//#define DEBUG
#define DHT11 11
#define DHT22 22
#define AM2302 22

int readDHT(int type, int pin);

int main(int argc, char **argv)
{
  if(SETUP_OK!=sunxi_gpio_init()){
    printf("Error init gpio\n");
    return -1;
  }

  if (argc != 2) {
    printf("usage: %s [11|22|2302]#\n", argv[0]);
    //printf("example: %s 2302 4 - Read from an AM2302 connected to GPIO #4\n", argv[0]);
    return 2;
  }

  int type = 0;

  if (strcmp(argv[1], "11") == 0) type = DHT11;
  if (strcmp(argv[1], "22") == 0) type = DHT22;
  if (strcmp(argv[1], "2302") == 0) type = AM2302;
  if (type == 0) {
    printf("Select 11, 22, 2302 as type!\n");
    return 3;
  }
  //atoi(argv[2]);
  // if (dhtpin <= 0) {
  //	printf("Please select a valid GPIO pin #\n");
  //	return 3;
  //  }
  
  readDHT(type, dhtpin);
  return 0;
} // main

int bits[250], data[100];

int bitidx = 0;

int readDHT(int type, int pin) {
  int counter = 0;
  int laststate = HIGH;
  int j=0;

  // Set GPIO pin to output

 	sunxi_gpio_set_cfgpin(pin,OUTPUT);
  sunxi_gpio_output(pin,HIGH);
  usleep(500000);

  sunxi_gpio_output(pin,LOW);  
  usleep(18000);
  
  sunxi_gpio_output(pin,HIGH);
  usleep(18);

  sunxi_gpio_set_cfgpin(pin,INPUT);

  data[0] = data[1] = data[2] = data[3] = data[4] = 0;

  while (sunxi_gpio_input(pin) == 1) {
    printf(" wait for pin to drop");
    usleep(1);
  }

  // read data!

  int i;

  for (i=0; i< MAXTIMINGS; i++) {
    counter = 0;
    while (sunxi_gpio_input(pin) == laststate) {
      counter++;
      //usleep(1);
      if (counter == 1000)
        break;
    }

    laststate = sunxi_gpio_input(pin);

    if (counter == 1000) break;

    bits[bitidx++] = counter;

    if ((i>3) && (i%2 == 0)) {
    // shove each bit into the storage bytes
      data[j/8] <<= 1;
      if (counter > 200)
        data[j/8] |= 1;
      j++;
    }
  }

#ifdef DEBUG
  for ( i=3; i<bitidx; i+=2) {
    printf("bit %d: %d\n", i-3, bits[i]);
    printf("bit %d: %d (%d)\n", i-2, bits[i+1], bits[i+1] > 200);
  }
#endif


printf("Data (%d): 0x%x 0x%x 0x%x 0x%x 0x%x\n", j, data[0], data[1], data[2], data[3], data[4]);

if ((j >= 39) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
// yay!
  if (type == DHT11)
    printf("Temp = %d *C, Hum = %d \%\n", data[2], data[0]);
  if (type == DHT22) {
    float f, h;
    h = data[0] * 256 + data[1];
    h /= 10;
    f = (data[2] & 0x7F)* 256 + data[3];
    f /= 10.0;
    if (data[2] & 0x80)  f *= -1;
    //printf("Temp =  %%.1f *C, Hum = %%.1f \%%\n", f, h);
    printf("Temp = %2.1f *C, Hum = %3.1f \%%\n", f, h);
  }
  return 1;
}

return 0;
}

