### Utilties Code

This folder contains various software that will help in the troubleshooting and deployment of these snow sensing stations.  
Summary of each folder:

- **[mayflydriver](mayflydriver)**: this folder contains the driver for your computer to talk to the Mayfly datalogger board. Most likely you will not need this code, as your computer should automatically download the driver itself, but in case you need it, it is here. The driver in this folder is for Windows. If you need the driver for another platform, consult the EnviroDIY website.
- **[measure_amps](measure_amps)**: this folder contains an Arduino sketch that can be used to log electrical current demands across a power supply line using an Adafruit INA260 sensor. This can be useful for precise measurement of power demand and in sizing of batteries.
- **[sd_readfile](sd_readfile)**: this folder contains an Arduino sketch that will allow a user to read data to the Arduino IDE serial monitor from a microSD card.
- **[test_modular_sensors](test_modular_sensors)**: this folder contains multiple sketches that show how each sensor is used individually in modular sensors and is mostly here for troubleshooting the modular sensors library.
- **[test_sensors](test_sensors)**: this folder contains sketches that test each sensor for functionality without using the modular sensors library. You can troubleshoot individual sensors using the sketches in this folder.

A more in-depth explanation of the code and how to use it is given in the comments of each sketch except for the mayflydriver folder. That folder is meant to be a standalone directory that you can refer your computer to if it needs to manually download the driver.  
Some folders contain more README files that may help to make sense of the contents in that directory if multiple files exist within it.
