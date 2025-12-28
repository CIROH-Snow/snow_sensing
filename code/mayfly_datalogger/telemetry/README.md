# Adding Telemetry to Your Snow Sensing Station

For telemetering snow data from a snow station to a Hydrologic Information System (HIS), there are two possible scenarios:

1. The location of your snow station has cellular data service 
2. You have no cellular service at your station location

## Using Cellular Data for Telemetry

If the snow station is located in an area with cellular data service, and you wish to telemeter data directly from the snow sensing station, you will be using the [lte_hydroserver](lte_hydroserver) sketch. This sketch is set up to send data to an instance of the [HydroServer](http://hydroserver.org) HIS via HTTP POST requests. HydroServer is a generalized HIS that can be used for sensor data management.

Other servers exist that the Mayfly datalogger and the Modular Sensors code library are capable of posting to thanks to the development of source code by the Stroud Water Research Center, such as [Monitor My Watershed](https://monitormywatershed.org/), [ThingSpeak](https://thingspeak.mathworks.com/), [DreamHost](https://www.dreamhost.com/), and [Ubidiots](https://ubidots.com/).

**NOTE:** If you have many snow sensing stations that all are within cellular data range, you can send data from every station to a HIS using a cellular data connection for each station. However, to reduce the cost of your cellular data plan, you might consider a mesh radio network for your stations using 900 MHz spread spectrum radios to aggregate your data on a single Base station that then sends data to a HIS using a single cellular data connection. This may reduce the cost of your cellular data plan. 

### Setting up a user account

You will need to properly set up an account on the server website and use the appropriate sketch depending on the publisher you would like to use. The ModularSensors library has included example sketches for publishing to ThingSpeak and Monitor My Watershed [Publishers Examples](../../arduino_libraries/EnviroDIY_ModularSensors/examples).

Note that these examples were created by EnviroDIY, and the sketch contains code for other sensors that are not used in the example LTE sketch in this repository folder. If you wish to publish to one of these servers rather than HydroServer, you will need to adjust their code to include all the sensors we've laid out. 

A testing instance of HydroServer is available at [https://playground.hydroserver.org](https://playground.hydroserver.org) where you can set up a user account, create metadata for a monitoring station, and add datastreams to the monitoring station to receive data from your snow sensing station.
HydroServer's documentation available at [http://hydroserver.org](http://hydroserver.org) provides directions for setting up a monitoring station and creating datastreams into which you can load data from your snow sensing station.

## Using 900 MHz Radio Telemetry

If your snow sensing station will be located where there is no cellular data service, and you would still like to publish to a HIS, then you will be using one of the two "satellite..." sketches. In this setup, a satellite station is just a snow station that gives its data to a different datalogger to publish rather than publishing the data itself. The datalogger that receives the data and publishes it to a HIS is referred to as a "base station". Any number of satellite snow sensing stations can be set up to give their data to a base station. 

The base station will need to be installed in a location where there is cellular data service, such as up on a ridge top, or it needs to be co-located with an already existing monitoring station that has its own way of connecting to a HIS. A more thorough breakdown of how this works and code for setting up this network of satellite-base stations can be found in the [base_station folder](../../base_station).

The two satellite sketches found in this folder contain the code for a station that makes measurements and feeds those measurements to a base station. The two versions, varCode and varUUID, refer to the information the base station needs to know to properly post the data it receives from the satellite snow station to the HIS. 

* [satellite_varCode](satellite_varCode): In this sketch, "varCode" refers to the variable code given to each observed variable in the sketch and is typically a pre-programmed default string in the ModularSensors source code unless explicitly declared in the Arduino sketch. For example, "snowDepth" or "airTemp" are potential variable codes that a base station could use to post data to an HIS. This sketch would be used for satellite stations within a network where the base station Internet-connected datalogger is a Campbell Scientific datalogger in an existing telemetry network.
* [satellite_varUUID](satellite_varUUID): In this sketch, "UUID" is the other option and uses a Universally Unique Identifier (UUID) to identify each observed variable. The UUID is also programmed into the sketch and is used in the request to POST data to a HIS like HydoServer. This sketch would be used for satellite stations within a network where the base station Internet-connected datalogger is a Mayfly datalogger that POSTs data directly to HydroServer using a cellular data connection. 
