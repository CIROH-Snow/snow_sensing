## Mayfly Datalogger Code

This folder contains the final code that needs to be uploaded to a Mayfly datalogger upon deployment.  
Note that there are lines of code that you will need to edit for your specific implementation. This will likely  
include the sampling interval, sonar height, etc. These are noted in the sketch.

#### Hardware needed to successfully program the hardware for a snow sensing station (i.e., Mayfly Datalogger, XBee radios, soil sensors, etc.):
- Mayfly datalogger: [EnviroDIY Link](https://www.envirodiy.org/product/envirodiy-mayfly-data-logger/)
- CR1220 battery: [DigiKey Link](https://www.digikey.com/en/products/detail/panasonic-bsg/CR1220/269740?s=N4IgTCBcDaIA4AYDMSC0A7AJiAugXyA)
- USB to USB-C cable (make sure it has a data line on it and that it isn't just a charging cable): [Amazon Link](https://www.amazon.com/Anker-2-Pack-Premium-Samsung-Galaxy/dp/B07DD5YHMH/ref=sr_1_1?crid=OCEUG0LMDYLP&dib=eyJ2IjoiMSJ9.IcOZhxxaDPccd7D_9PJSez4TC7ZeslNm1EJdKPeQneHBEF-uoIV7LasPMxWyuM_Vya40K-iyPyMg6v_H45wy6mzKXxt6s3OYqWP5zhy1B9J-1LUpHezs29_rckwloWXBiXYf8MJ05P_svLPunlYzUe7gQfveNh-Zn7VBKaGt_9iWLG-n9virw4ACfWX6lJk2vqfw9e2OuA637VG6T4SehBXUF63MhLmMbi_0Qzeq_wo.LhyREBSKszocFqOQPSyZ6cSP8CXedu0OlJd6lyo4osc&dib_tag=se&keywords=usb+to+usb+c+cable&qid=1731527951&sprefix=usb+to%2Caps%2C163&sr=8-1)
- USB to mini USB cable (or whatever the expolorer board USB port is; likewise make sure it isn't just a charging cable but that it has data transfer capabillities): [Amazon Link](https://www.amazon.com/Amazon-Basics-Charging-Transfer-Gold-Plated/dp/B00NH11N5A/ref=sr_1_1?crid=1Z0FIFVVCXG0W&dib=eyJ2IjoiMSJ9.shJPkvHWsKPPj2XvYAPBmNL50UHskMqo16gwTKuFBExp4vatt4_0_judiSkXn9R6tbDUb6a3kQqJNT5YGJOsxUhNdQeKjHa2TSXLJMGJsOYR2U7iZSGto64mWcN8Ry-DeZc0ZJN7BWq3frLdTGhZROKR3tVtsDp0j_9M-VZgzCQyn5KyGpybJtvbnLpKkE27-dP30L9B274XFoM36szpTkp7nA3GOI8wVNV5Ls5fB40.ROudX9YX1I-bUxWiohZX6eGvnMZE8h9sToyxZ6urNi4&dib_tag=se&keywords=usb+to+mini+usb&qid=1731528058&sprefix=usb+to+mini+usb%2Caps%2C140&sr=8-1)
- EnviroDIY grove to 3.5 mm stereo jack converter: [EnviroDIY Link](https://www.envirodiy.org/product/envirodiy-grove-to-3-5mm-stereo-jack-pack-of-2/) 
- Sparkfun Bee explorer board: [SparkFun Link](https://www.sparkfun.com/products/11812)
- MicroSD card: [Amazon Link](https://www.amazon.com/PNY-Elite-X-microSDXC-Memory-3-Pack/dp/B08T6QSBPJ/ref=sxts_b2b_sx_reorder_acb_customer?content-id=amzn1.sym.f63a3b0b-3a29-4a8e-8430-073528fe007f%3Aamzn1.sym.f63a3b0b-3a29-4a8e-8430-073528fe007f&crid=3FKIWN7XO0G1T&cv_ct_cx=microsd&dib=eyJ2IjoiMSJ9.CDXdRBTTK5D7gOhcosDwDLRO1-udc6y3OmsZ_IS6n-M._me_VA9t2B5s1D-KL36yualKHhDRDZhm8OcrubA44I4&dib_tag=se&keywords=microsd&pd_rd_i=B08T6QSBPJ&pd_rd_r=899f26f9-a4e7-49c0-8751-6ac8cb32a7d3&pd_rd_w=V2sb6&pd_rd_wg=wIW2M&pf_rd_p=f63a3b0b-3a29-4a8e-8430-073528fe007f&pf_rd_r=2MNHMDSCGZH55X2MG5D5&qid=1731528178&sbo=RZvfv%2F%2FHxDF%2BO5021pAnSA%3D%3D&sprefix=microsd%2Caps%2C237&sr=1-1-9f062ed5-8905-4cb9-ad7c-6ce62808241a)
- EnviroDIY microSD vertical card adapter (optional and comes with Mayfly datalogger purchase): [EnviroDIY Link](https://www.envirodiy.org/product/envirodiy-vertical-microsd-card-adapter-pack-of-5/)

### Hardware Setup Steps
**IMPORTANT: You need to complete the following steps to set up the hardware before you edit and upload the Arduino Mayfly sketch found in this folder for logging data.**   

1. Set the real-time clock (RTC) on the Mayfly
	1. Insert your coin battery with the positive side up into the battery slot found on the right side of the board below the two white LiPo connectors.
	2. Open the Arduino IDE and connect your Mayfly to your laptop.
	3. Under the "File" tab, navigate to the sketch needed for adjusting the RTC (note that you may have to scroll down a little in the Examples dropdown to the "Examples from custom libraries" subsection): Examples > EnviroDIY_DS3231 > adjust
	4. Line 11 is where you adjust the datetime, following the parameters described in the comments above the line. Make sure you set this to the UTC time. The final sketch we upload to the Mayfly will have a parameter for adjusting to your timezone.
	5. Once you have that ready, turn on your Mayfly and upload the sketch.
	6. Check that you properly adjusted the RTC by turning on your serial monitor in the Arduino IDE (setting the baud rate to whatever was set in the setup function on line 16).
	7. From here, **DO NOT POWER DOWN YOUR MAYFLY**. It will reset your Mayfly's RTC back to whatever you originally had in the program when you power it backup and the Mayfly runs the setup function again.
	8. Upload a different example sketch while your Mayfly is still running just so you do not have to worry about it again.
	9. Navigate to Examples > EnvrioDIY_DS3231 > now
	10. Upload the sketch
	11. You may now power off and disconnect the Mayfly without it reseting the RTC
2. Make sure you have the pyranometer and pyrgeometer calibration factors on-hand. You will need them when you adjust the Arduino code on your Mayfly after these steps are complete.
3. If you are using the MaxBotix sonar sensor for snow depth, you will need to measure the height of the sensor.
	1. You can manually measure this, or you can use the "test_maxbotix" file found in snow_sensing > code > utilities > test_sensors. This will read out the sensor height. Get a height in millimeters, and write it down so you can add it to your sketch you'll upload upon deployment.
	2. It is suggested that you manually measure the height of the sensor, as it loses accuracy without a snow surface to measure
4. If you are using the Meter Teros 12 soil sensors, you will need to set their addresses within the sensors themselves. They operate on SDI-12 protocol which requires unique addresses for each sensor
	1. Take one of your sensors and connect it to a stereo-jack to grove terminal converter, the same converter that you will use for your station deployment
	2. Connect the grove end to one of the Mayfly's SDI-12 ports using a grove cable. There are two ports on the board. It does not matter which one you connect it to.
	3. Open the Arduino IDE, and navigate through the examples: File > Examples > SDI-12 > b_address_change
	4. Turn on your Mayfly and upload the b_address_change sketch
	5. Open your serial monitor and adjust its baud rate to match what is defined for SERIAL_BAUD near the beginning of the sketch
	6. Follow the serial monitor's prompts for changing the addresses, giving it a singular character for the address (it can be a number, uppercase letter, or lowercase letter) and pressing enter to send the character to the Mayfly
	7. Take a Sharpie and identify the sensor itself with the address so you know which sensor is which when you go to install
	8. You may consider having the following setup: sensor 'a' for a depth of 2", sensor 'b' for a depth of 8", and sensor 'c' for a depth of 20"
5. If you will be using the XBee S3B modules to develop a network of stations, you need to program those modules.
	1. Download the XCTU software for interfacing with the XBee modules
		1. Click on the link: https://www.digi.com/products/embedded-systems/digi-xbee/digi-xbee-tools/xctu
		2. Click on the button: "Visit Support to Download XCTU"
		3. Scroll down to "Product Resources"
		4. Under "Resources and Utilities" select the XCTU link for your operating system and follow the instructions
	2. You will want an explorer board from sparkfun in order to easily interface with XCTU (https://www.sparkfun.com/products/11812)
		1. Note that there are other explorer boards you can get, but make sure you can obtain the necessary cables for powering and using serial communication
	3. Attach your XBee S3B to the explorer making sure to match the board's geometry to the white outlines on the explorer
	4. Attach the board to your computer. If you get a different explorer board, note that you may need to supply power externally, such as with a barrel jack and wall plug.
	5. Open the XCTU software
	6. In the top left, select the button showing a module with a magnifying glass over it to discover the radio module
	7. Select the port the radio is connected to
	8. Press Next
	9. Make sure the following settings are set:
		1. Baud Rate: 9600
		2. Data Bits: 8
		3. Parity: None
		4. Stop Bits: 1
		5. Flow Control: None
	10. Press Finish
	11. A list of discovered devices should appear. If this does not work, hit cancel and try to add the radio module in the home page by selecting the button with the + sign and using the same parameters
	12. Select your device and press "Add selected devices"
	13. You should be taken back to the home page where your device is listed on the left
	14. Click on the module from that list (it should highlight orange when selecting it), avoiding the buttons on the right side
	15. This will open up all the radio settings.
	16. Under MAC/PHY settings, set Network ID (ID) to something significant to you that all radios will be on. Note that if two radios' network IDs do not match, they will not be able to communicate.
	17. Under MAC/PHY settings, set Unicast Mac Retries (RR) to F
	18. Under Network settings, set Mesh Unicast Retries (MR) to 5
	19. Under Addressing settings, set the Node Identifier (NI) to something meaningful such as the name of the station the module will be placed on
	20. Under Addressing settings, make sure that the Transmit Options (TO) is set to C0 (DigiMesh)
	21. Under Serial Interfacing options, set API Enable (AP) to API Mode Without Escapes
	22. Under Sleep Commands, set Sleep Mode (SM) to Async. Pin Sleep
	23. Save these changes by pressing the "Write" button at the top
	24. Your XBee is programmed, so clear it from the list on the left by clicking the module's "X" button
	25. Unplug the cable from your computer or explorer before removing the XBee from the explorer
6. Set the switched power jumper headers on the Mayfly
	1. At the top of each grove port on the Mayfly, there is a little, black header with four pins that identifies how much voltage is supplied over the switched power to that grove terminal
	2. The voltage supplied to each terminal is set by moving the jumper that connects one of the three outer pins to each headers' center pin
	3. Using a very fine, sturdy point, such as a very small flathead screwdriver, carefully make the following adjustments:
		1. Above the D5-6 grove terminal, move the jumper to 5v
		2. Above one of the SDI-12 grove terminals (it does not matter which), move the jumper to 12v
		3. Above the other SDI-12 grove terminal, move the jumper to 3v (it may already be there by default)
		4. Above the D10-11 grove terminal, move the jumper to 5v
		5. Above the Aux Analog grove terminals, move the jumper to 3v (it may already be there by default)
7. Attach the MicroSD card
	1. There are two options for attaching the MicroSD card. You may use the socket found on the left-hand side
	2. You may also use the adapter that likely came with your purchase of the Mayfly. Simply insert the 8 prongs of this adapter into the 8-pin header above the SD socket, matching the white outline on the Mayfly
8. There are some optional and some required soldering configurations that need to take place for setting up a station. Please consult the [soldering documentation](https://github.com/CIROH-Snow/snow_sensing/blob/main/hardware/soldering_guide.md) found in the "hardware" folder of this repository
9. All you need is to assemble the mounting hardware and wire the station. Constructing the station and wiring can be an intensive process and is difficult to describe without visual aids. Consult the [construction](https://github.com/CIROH-Snow/snow_sensing/blob/main/hardware/construction_guide.md) and [wiring](https://github.com/CIROH-Snow/snow_sensing/blob/main/hardware/wiring_guide.md) guides found in the "hardware" folder of this repository.

### Uploading the Final Mayfly Sketch for Logging Data

Once your station is constructed and you have completed the above steps, you are ready to upload the sketch found in this folder titled "satellite_station."  It is titled that way because this sketch's programming allows for it to be setup in a network of sites that "orbit" a central or host station that aggregates all the satellite snow sites' data. In our application, we then use the host station to communicates data from all sites to a CR800 datalogger from Campbell Scientific. If you are just doing a stand-alone site that will not be using this feature, you can still use this sketch, just follow comments in the code to properly disable radio communications.

The code is well-commented, and will walk you through what everything means and where you may need to make adjustments.
