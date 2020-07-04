# Folimetro

Folimetro is able to measure the air pressure into a galician bagpipe, connecting it instead of the ronqueta drone.

## Motivation

One of the most tricky things to do when playing the bagpipe is maintaining the level of the air pressure in the bag, in order to keep the sound of the drones.

The idea behind this project was to give support to the students and teachers of this instrument to control this problem.

## How it works?

This device consist of 9 leds who marks the pressure and 2 push buttons to displace the pressure value of the central led.

```
                                          PRESSURE VALUE (kPA)
          0.2  0.8  1.4  2.0  2.6  3.2  3.8  4.4  5.0  5.6  6.2  6.8  7.4  8.0  8.6  9.2  9.8
                               |    |    |    |    |    |    |    |    |    
                              -4   -3   -2   -1    0    1    2    3    4  
                                            NUMBER OF LED

```

## Project structure
* `docs` - various PDF datasheets for component used.
* `hardware` - Fritzing Schematic.
* `folimetro.ino` - Arduino project.

## Hardware Requirements

* Arduino Micro Pro (Leonardo)
* 11x Leds
* 10x 1k Ohm resistor
* 2x Push button
* 2x 10k Ohm resistor
* Pressure sensor (0 to 10kPA)

## Arduino Schematic
![Connection Scheme](figures/sketch.png)

## Reference
1.  Cassandre Balosso-Bardin, Camille Vauthrin, Patricio de la Cuadra, Ilya Franciosi, Khalil Ben Mansour, Lise Ochej. EuroRegio2016 (2016, June 13-15). *\<\<The musical arm of the bagpipe player: a cultural and mechanical approach>>*  [[Link]]( http://www.sea-acustica.es/fileadmin/Oporto16/73.pdf)
