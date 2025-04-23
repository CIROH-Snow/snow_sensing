# Snow Sensing Code

This folder contains the code required to operate, troubleshoot, and maintain a snow sensing station for stand alone data collection as well as for communication of collected data over radio and cellular telemetry networks.  

Before you proceed, you will need to download the Arduino Interactive Development Environment (IDE). Do this by following the instructions listed on the EnviroDIY website: [Installing IDE](https://www.envirodiy.org/mayfly/software/). Don't worry about "Installing Libraries" section. This directory contains the libraries you need. Just follow the steps on the page until "Installing Libraries." 

There are multiple folders in this directory, each with different purposes and types of code inside them. They are as follows:
- **[arduino_libraries](arduino_libraries)**: this folder contains all the source code you will need for programming your Mayfly (which is an Arduino board). There is no README for that folder, so how to use it is explained further down in this markdown file.
- **[base](base)**: this folder contains code for telemetering collected data from snow stations without cellular service. It is meant to compliment the [satellite sketches](mayfly_datalogger/telemetry) found in the mayfly_datalogger->telemetry folder.
- **[mayfly_datalogger](mayfly_datalogger)**: this folder contains the code needed for the deployment of a standalone or integrated snow station. It deals with the code for the Mayfly of the actual site where you are sampling measurements.
- **[utilities](utilities)**: this folder contains useful Mayfly sketches that can assist in troubleshooting sensors, downloading data, or assuring you have the proper drivers for your computer

**IMPORTANT:** the arduino_libraries folder contains all the necessary libraries for the use of a Mayfly in this project. If you just get your libraries from the EnviroDIY website, you will miss critical source code for sensors that have been added and some edits that are necessary for the deployment of these stations. When you download the Arduino IDE, a folder will be made in the "Documents" directory of your computer. There should be a folder within that titled "libraries." The contents of the arduino_libraries folder from this GitHub repository need to be copied over to Documents > Arduino > libraries. Do not just click and copy the arduino_libraries folder itself, rather open that folder, select all the contents within the first page of that folder, copy all that content, then paste it inside the appropriate  
Documents > Arduino > libraries folder.
