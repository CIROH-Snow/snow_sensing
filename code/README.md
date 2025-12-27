# Snow Sensing Code

This folder contains the code required to operate, troubleshoot, and maintain a snow sensing station for stand alone data collection as well as for communication of collected data over radio and cellular telemetry networks.  

## IMPORTANT: DO THIS FIRST TO ENABLE PROGRAMMING SNOW SENSING STATIONS

Before you proceed, you will need to download the Arduino Interactive Development Environment (IDE). Do this by following the [instructions listed on the EnviroDIY website](https://www.envirodiy.org/mayfly/software/). Do not complete the "Installing Libraries" section on that page. The "code" directory in this GitHub repository contains the Arduino libraries you need. Just follow the steps on the EnviroDIY webpage up to the "Installing Libraries" section. 

**IMPORTANT NOTE:** The arduino_libraries folder in this GitHub repository contains all the necessary libraries for the use of a Mayfly datalogger with the set of sensors selected for snow sensing stations. If you just get your libraries from the EnviroDIY website, you will miss critical source code for sensors that have been added and some edits that are necessary for the deployment of snow sensing stations. When you download the Arduino IDE, a folder will be made in the "Documents" directory of your computer. There should be a folder within that titled "libraries". The contents of the arduino_libraries folder from this GitHub repository need to be copied over to the Documents > Arduino > libraries folder on your computer. Do not just click and copy the arduino_libraries folder itself, rather open that folder, select all the contents within that folder, copy all that content, then paste it inside the Documents > Arduino > libraries folder on your computer.

## Getting Started with Programming Your Snow Sensing Station

Once you have completed the soldering, construction, and wiring setup instructions, and you have installed the the Arduino IDE and copied the necessary Arduino libraries to your computer, you are ready to program the datalogger for your snow sensing station. 

### Mayfly Datalogger Code

[Access the Mayfly Datalogger Code](mayfly_datalogger/README.md) to set up and program your Arduino Datalogger. This folder contains code for setting up your datalogger as a stand-alone snow sensing station with no telemetry (data is recorded on a microSD card) and code for adding telemetry to your snow sensing station via LTE cellular or 900 MHz radio modules.

### Troubleshooting 

If you have issues with datalogger programming, you can consult the provided [utility code](utilities/README.md) to help with troubleshooting.

## Structure of the Code in this Repository

There are multiple folders in this directory, each with different purposes and types of code inside them. They are as follows:

* **[arduino_libraries](arduino_libraries)**: This folder contains all the source code you will need for programming your Mayfly (which is an Arduino board). There is no README for that folder, so how to use it is explained further down in this markdown file.
* **[base](base)**: This folder contains code for telemetering collected data from snow stations without cellular service. It is meant to compliment the [satellite sketches](mayfly_datalogger/telemetry) found in the mayfly_datalogger->telemetry folder.
* **[mayfly_datalogger](mayfly_datalogger)**: This folder contains the code needed for the deployment of a standalone or integrated snow station. It deals with the code for the Mayfly of the actual site where you are sampling measurements.
* **[utilities](utilities)**: This folder contains useful Mayfly sketches that can assist in troubleshooting sensors, downloading data, or assuring you have the proper drivers for your computer