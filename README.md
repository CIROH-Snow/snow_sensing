# Snow Sensing 

This repository contains hardware and software designs for low(er) cost snow sensing stations. These next-generation snow monitoring stations enable collection of near real-time, physiographically representative water and energy balance observations in montane headwater catchments. These stations provide hardware for developing robust, low-cost, low-power snow monitoring networks in complex terrain to enhance the spatial representation of catchment snow-water-equivalent (SWE). They are meant to complement existing snow monitoring infrastructure (e.g., NRCS SNOTEL stations) and enable establishment of sensing locations that represent different combinations of aspect, slope, vegetation, and elevation. Site instrumentation is intended to capture key snow-energy-balance processes, including: snow depth, short- and long-wave radiation, soil moisture, and air temperature. 

## Hardware

This folder contains designs and specifications for sensors, dataloggers, communication peripherals, and components required for building a snow sensing station. It is suggested that you follow the guides in the hardware directory first, effectively constructing a station before programming it using the information found in the code directory.

## Code

This folder contains the code used to operate the snow sensing stations, including data collection, data storage, and data communication. It also includes necessary libraries, guidance on programming XBee modules, helpful sketches, and more. Programming the snow stations is done after the hardware has been set up, so go through the hardware directory before attempting to program your dataloggers, unless a step specifically tells you to program something as part of the hardware implementation (such as programming the soil sensors before burying them). In that case, the instructions will explicitly tell you where to find the code necessary and what steps to take before continuing the hardware setup.

## Funding and Acknowledgments

Funding for this project was provided by the United States Geological Survey (USGS) awarded through the Cooperative Institute for Research to Operations in Hydrology (CIROH) via the NOAA Cooperative Agreement with The University of Alabama (NA22NWS4320003). Collaborators include Utah State University, University of Utah, and University of Vermont. Additional funding and support have been provided by the Utah Water Research laboratory at Utah State University.

<img src="/assets/CIROH_logo.png" alt="CIROH Logo" width="15%"/>


