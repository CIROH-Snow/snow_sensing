### Integrations

This directory contains the code needed for integrating a snow station with an existing network of Campbell Scientific dataloggers. The basic setup consists of one or more "satellite" stations that sample the measurements, a "central" station (sometimes referred to as the "host" in some of the code in this repository) that aggregates the data that the satellite stations collect, and "repeater" stations that help relay information from the satellite station to the central station when they don't have enough signal strength to do it directly. 

Folder summaries
- central_station: while there are two dataloggers physically located where the central station is (one Mayfly and one CR800), this folder specifically just has the Mayfly's code for aggregating data
- cr800: this folder contains the code for a Campbell Scientific CR800 that integrates with an existing network and receives data from the satellite stations through the central station's Mayfly serially
- repeater: this folder contains some simple Mayfly code for a repeater station if you need one. Note that a repeater station is not inherently necessary for a functioning network, but may be needed depending on where you set up your satellite stations relative to the central station
