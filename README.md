# dht-sensor
DHT-sensor driver for Cubietruck

Development resources: demo codes, schematic, datasheets, etc., can be found here:
http://www.wvshare.com/product/Temperature-Humidity-Sensor.htm

Before compile you need to specify the "DATA"-pin of sensor in dht.c near comment with label "DATA-Pin select"

##Compile
1. gcc dht.c -c
2. gcc gpio_lib.c -c
3. gcc dht.o gpio_lib.o -o dht

##Usage
# ./dht 11

##Output example
Data (40): 0x20 0x0 0x17 0x0 0x37
Temp = 23 *C, Hum = 32 %