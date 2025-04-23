### Integrations

This directory contains the code needed for telemetring data from a snow station that does not have cellular connection to a Hydrologic Information System (HIS). Here a base station (sometimes referred to as the "host" in some of the code in this repository) aggregates data from any number of snow stations that don't have cell service, named satellite stations from here on out. This is done by using 900 MHz spreadspectrum radios to get data from a satellite snow station where there is no cell service to a base station where there is. A base station could also be an existing weather station that has its own method of connecting to an HIS.

The following graphic illustrates the role of a base station in getting data published to an HIS when a snow station is setup in an area without cell service.
![satellite-base relationship](base_figures/satellite-base-his.png)

Folder summaries
- **[central_station](central_station)**: while there are two dataloggers physically located where the central station is (one Mayfly and one Campbell Scientific datalogger), this folder specifically contains just the Mayfly's code for aggregating data.
- **[cr800](cr800)**: this folder contains the code for a Campbell Scientific CR800 that integrates with an existing network and receives data from the satellite stations through the central station's Mayfly serially. While we are using a CR800 datalogger for this station, several of Campbell Scientific's newer dataloggers could be used with similar code (e.g., CR6, CR1000, CR310, CR350, etc.).
- **[repeater](repeater)**: this folder contains some simple Mayfly code for a repeater station if you need one. Note that a repeater station is not inherently necessary for a functioning network, but may be needed depending on distances between satellite stations and relative to the central station.

Refer to step 5 of the Mayfly datalogger [readme file](https://github.com/CIROH-Snow/snow_sensing/tree/main/code/mayfly_datalogger) in this repository for setup instructions for XBee radio modules if you are using them.
