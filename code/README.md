# Snow Sensing Code

This folder contains the code required to operate, troubleshoot, and maintain a snow sensing station for stand alone data collection as well as for communication of collected data over radio and cellular telemetry networks. 

## Structure of the Code in this Repository

There are multiple folders in this directory, each with different purposes and types of code inside them. They are as follows:

* **[arduino_libraries](arduino_libraries)**: This folder contains the source code for the Arduino code libraries you will need for programming your Mayfly (which is an Arduino board). 
* **[base_station](base_station)**: This folder contains code for implementing a base station used for telemetering collected data from snow sensing stations that do not have cellular service. It is meant to compliment the [satellite snow sensing station sketches](mayfly_datalogger/telemetry) found in the ```mayfly_datalogger/telemetry``` folder.
* **[mayfly_datalogger](mayfly_datalogger)**: This folder contains the code needed for the deployment of a stand-alone snow sensing station or a station with telemetry. It contains the code for the Mayfly datalogger that will be located at the actual site where you are making snow measurements.
* **[utilities](utilities)**: This folder contains useful Mayfly datalogger sketches that can assist in troubleshooting sensors, downloading data, or assuring you have the proper drivers for your computer

The steps for implementing code for your snow sensing station are as follows:

1. [Install the Arduino Interactive Development Environment (IDE) using the directions below.](#1-install-the-arduino-interactive-development-environment-ide)
2. [Copy the required Arduino libraries to your computer](#2-copy-the-required-arduino-libraries-to-your-computer)
3. [Choose the Mayfly datalogger code you want to implement](#3-choose-the-mayfly-datalogger-code-you-want-to-implement)
4. [Copy the correct sketch to your computer, open it in the Arduino IDE, compile it, and send it to your Mayfly datalogger]() 
5. [Troubleshoot your Implementation](#5-troubleshoot-your-implementation)

## 1. Install the Arduino Interactive Development Environment (IDE)

Before you proceed, you will need to download the Arduino Interactive Development Environment (IDE). Do this by following the [instructions listed on the EnviroDIY website](https://www.envirodiy.org/mayfly/software/). 

**IMPORTANT: DO NOT complete the "Installing Libraries" section on that page.** The "code" directory in this GitHub repository contains the Arduino libraries you need. Just follow the steps on the EnviroDIY webpage up to the "Installing Libraries" section. 

## 2. Copy the Required Arduino Libraries to Your Computer

The ```arduino_libraries``` folder in this GitHub repository contains all the necessary libraries for the use of a Mayfly datalogger with the set of sensors selected for snow sensing stations. If you just get your libraries from the EnviroDIY website, you will miss critical source code for snow sensors that we have added and some edits that are necessary for the deployment of snow sensing stations. When you download and install the Arduino IDE, a folder will be made in the "Documents" directory of your computer. There should be a folder within that titled "libraries". Do the following:

* Copy the contents of the ```arduino_libraries``` folder from this GitHub repository into the ```Documents > Arduino > libraries``` folder on your computer. 

**NOTE:** Do not just click and copy the arduino_libraries folder itself, rather open that folder, select all the contents within that folder, copy all that content, then paste it inside the ```Documents > Arduino > libraries``` folder on your computer.

For additional information about the specific libraries used in the code for the snow sensing stations, see the [README file](arduino_libraries/README.md) within the "arduino_libraries" folder in this repository.

## 3. Choose the Mayfly Datalogger Code You Want to Implement

Once you have completed the soldering, construction, and wiring setup instructions, and you have installed the the Arduino IDE and copied the necessary Arduino libraries to your computer (instructions above), you are ready to program the datalogger for your snow sensing station. There are multiple Mayfly datalogger sketches provided in this repository. The specific sketch you choose depends on which of the following you are setting up:

* A stand-alone snow sensing station with no telemetry
* A stand-alone snow sensing station with cellular telemetry
* A satellite snow sensing station in a network with 900 MHz spread spectrum radio telemetry
* A radio repeater station in a network with 900 MHz spread spectrum radio telemetry
* A base station in a network with 900 MHz spread spectrum radio telemetry

### Mayfly Datalogger Code

Access the [Mayfly Datalogger Code](mayfly_datalogger/README.md) to configure and program your Mayfly datalogger. This folder contains code for setting up your datalogger as a stand-alone snow sensing station with no telemetry (data is recorded on a microSD card) and code for adding telemetry to your snow sensing station via LTE cellular or 900 MHz radio modules.

### Telemetry Base Station and Repeater Code

Access the [Base Station and Repeater Code](base_station/README.md) to configure and program a Mayfly datalogger as part of a network base station or as a radio repeater.

## 4. Copy the Correct Sketch to Your Computer

Once you know which code you will be using, do the following:

1. Copy the sketch from the GitHub repository to your computer
2. Open the sketch in the Arduino IDE
3. Modify the sketch as needed according to the documentation
4. Compile the sketch
5. Use the Arduino IDE to load the sketch onto your Mayfly datalogger

## 5. Troubleshoot your Implementation 

If you have issues with datalogger programming, you can consult the provided [utility code](utilities/README.md) to help with troubleshooting.

