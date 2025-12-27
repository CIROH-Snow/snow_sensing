# Example Code for Base Station Mayfly Datalogger

This folder contains two example Arduino sketches for a Base station Mayfly, which is in charge of aggregating all data from every satellite snow sensing station in a network and pushing that data to an Internet-connected datalogger. They are as follows:

* **[push_to_cr800](push_to_cr800):** Use this sketch to push data from your Base Mayfly to a Campbell Scientific CR800 datalogger acting as the Internet-connected datalogger for your Base station. 
* **[push_to_mayfly_lte](push_to_mayfly_lte):** Use this sketch if you want to push data from your Base Mayfly to a separate Mayfly datalogger acting as the Internet-connected datalogger for your Base station.

![satellite-base-his](../base_figures/satellite-base-his.png)

In both sketches, the Base Mayfly follows a communication protocol with each Satellite Mayfly datalogger to collect all the necessary data from a snow sensing station. The Base mayfly then compiles the data into a structured string that is passed on to the Internet-connected datalogger at the Base station. This communication protocol is shown in the following figure.

![satellite-base-comms-protocol](../base_figures/satellite-base-comms-protocol.png)

The Base Mayfly collects a timestamp for the most recent measurement and then requests a variable identifier (either the varCode or varUUID) and associated measurement value for each variable measured at the satellite station. The compiled data string takes that information and formats it into the following structure:

```@variableIdentifier=variableMeasurement;```

An "@" sign indicates the start of a new data point followed by the identifier for the observed variable. An "=" sign indicates that the actual measurement value is following, and then the data point is terminated by a ";".

An example of this could be: 

```@snowDepth=652;```

In this example, the Base Mayfly requested a varCode and received "snowDepth" and then it requested snowDepth's measurement, which was 652, as in 652 mm. All the data points for a satellite station's measurements can be "stacked" in this format.

For example: 

```@timestamp=2025-02-04 11:30:00-7;@snowDepth=652;@airTemp=1.4;```

This format is used to compile all the data into one string that can be easily parsed by any data logger over serial connection - i.e., by the downstream, Internet-connected datalogger.

The reason there are two different sketches is that the [Internet-connected dataloggers](../internet-connected-datalogger) given in this repository's examples have different ways of parsing the data, and they don't necessarily require the same information to be able to post to their respective HIS. The [Campbell Scientific implementation](../internet-connected-datalogger/cr800), for example, has to have things organized into data tables, so each data table is associated with a station, and therefore the Base Mayfly needs to include in its compiled string the station name.

For example: 

```@station=marshes;@timestamp=2025-02-04 11:30:00-7;@snowDepth=652;@airTemp=1.4;```

In the [Mayfly LTE implementation](../internet-connected-datalogger/mayfly_lte), however, the Internet-connected datalogger is publishing data to the HydroServer HIS, which only cares about the Universally Unique Identifier (UUID) for each observed variable (HydroServer treats each observed variable at each satellite station as a separate datastream and each datastream has a UUID), so a station name is not necessary to report. The two Base Mayfly sketches also differ in how they handle letting the Internet-connected data logger know when all the compiled data string for one station has ended.
