# Adding Telemetry to Your Snow Sensing Station

For telemetering snow data from a snow station to a Hydrologic Information System (HIS), there are two possible scenarios:

1. The location of your snow station has cellular data service 
2. You have no cellular service at your station location

## Using Cellular Data for Telemetry

If the snow station is located in an area with cellular data service, and you wish to telemeter data, you will be using the [lte_hydroserver](lte_hydroserver) sketch. This sketch is setup to make posts to [HydroServer](http://hydroserver.org), which is a generalized HIS that can be used for sensor data management.

Other servers exist that the Mayfly datalogger and the Modular Sensors code library are capable of posting to thanks to the development of source code by the Stroud Water Research Center, such as [Monitor My Watershed](https://monitormywatershed.org/), [ThingSpeak](https://thingspeak.mathworks.com/), [DreamHost](https://www.dreamhost.com/), and [Ubidiots](https://ubidots.com/).

### Setting up a user account

You will need to properly set up an account on the server website and use the appropriate sketch depending on the publisher you would like to use. The ModularSensors library has included example sketches for publishing to ThingSpeak and Monitor My Watershed [Publishers Examples](../../arduino_libraries/EnviroDIY_ModularSensors/examples).

Note that these examples were created by EnviroDIY, and the sketch contains code for other sensors that are not used in the example LTE sketch in this repository folder. If you wish to publish to one of these servers rather than HydroServer, you will need to adjust their code to include all the sensors we've laid out. A testing instance of HydroServer is available at [https://playground.hydroserver.org](https://playground.hydroserver.org).

## Using 900 MHz Radio Telemetry

If your snow sensing station will be located where there is no cellular data service, and you would still like to publish to a HIS, then you will be using one of the two "satellite..." sketches. In this setup, a satellite station is just a snow station that gives its data to a different data logger to publish rather than publishing the data itself. The data logger that receives the data and publishes it to a HIS is referred to as a "base station". Any number of satellite snow sensing stations can be set up to give their data to a base station. 

The base station will need to be installed in a location where there is cellular data service, such as up on a ridge top, or it needs to be co-located with an already existing monitoring station that has its own way of connecting to a HIS. A more thorough breakdown of how this works and code for setting up this network of satellite-base stations can be found in the [base folder](../../base).

The two satellite sketches found in this folder contain the code for a station that makes measurements and feeds those measurements to a base station. The two versions, varCode and varUUID, refer to the information the base station needs to know to properly post the data it receives from the satellite snow station to the HIS. 

* "varCode" refers to the variable code given to each observed variable in the sketch and is typically a pre-programmed default string in the ModularSensors source code unless explicitly declared in the Arduino sketch. For example, "snowDepth" or "airTemp" are potential variable codes that a base station could use to post data to an HIS. 
* "UUID" is the other option and uses a Universally Unique Identifier (UUID) to identify each observed variable. The UUID is also programmed into the sketch and is used in the request to POST data to the a HIS like HydoServer.

 
