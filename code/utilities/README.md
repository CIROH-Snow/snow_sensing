### Utilties Code

This folder contains various software that will help in the troubleshooting and deployment of these snow stations.  
Summary of each folder:

- mayflydriver: this folder contains the driver for your computer to talk to the Mayfly board. Most likely you will not need this code, as your computer should automatically download the driver itself, but incase you need it, it is here.
- measure_amps: this folder contains an Arduino sketch that can be used to log amperage demands across a line using an Adafruit INA260 sensor. This can be useful for precise sizing of batteries.
- sd_readfile: this folder contains an Arduino sketch that will allow a user to read data to the Arduino IDE serial monitor from an microSD card.
- test_modular_sensors: this folder contains multiple sketches that show how each sensor is used individually in modular sensors and is mostly here for troubleshooting the modular sensors library
- test_sensors: this folder contains sketches that test each sensor for functionality without using the modular sensors library. You can troubleshoot sensors here.

A more in-depth explanation of the code and how to use it is given in the comments of each sketch except for the mayflydriver folder. That folder is meant to be a standalone directory that you can refer your computer to if it needs to manually download the driver.  
Some folders contain more README files that may help to make sense of the contents in that directory if multiple files exist within it.
