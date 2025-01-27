### Wiring Guide

The steps in this guide are meant to be completed after the construction is done. Follow the instructions in the ![Construction Guide markdown file](construction_guide.md) and the ![Soldering Guide markdown file](soldering_guide.md) before continuing here. Note that this setup assumes a Solperk charge controller ([Link](https://www.amazon.com/SOLPERK-Controller-Waterproof-Intelligent-Regulator/dp/B0946MWYPK?ref_=ast_sto_dp)) is being used. Your implementation may look a little different if using a different charge controller, but the principles are the same. Also note that in these instructions a blue radio module and small whip antenna is placed on the Bee header of the Mayfly being used. Any Bee module is not inherently necessary for wiring a standalone station, but may be used if you will be integrating some form of wireless data transmission.

Necessary tools and hardware:
- Wire stripper
- Screw driver kit
- Stereo jack converters: [EnviroDIY Link](https://www.envirodiy.org/product/envirodiy-grove-to-3-5mm-stereo-jack-pack-of-5/)
- Soldered ADC development board
- 2 I2C Qwiic cables (preferably one long and one short): [Amazon Link](https://www.amazon.com/gp/product/B08HQ1VSVL/ref=ppx_yo_dt_b_asin_title_o09_s00?ie=UTF8&psc=1)
- 2 male-to-male jumper cables: [Amazon Link](https://www.amazon.com/Elegoo-EL-CP-004-Multicolored-Breadboard-arduino/dp/B01EV70C78/ref=sr_1_10?crid=219Y06Z2J6AAA&keywords=jumper%2Bcables%2Bsolder&qid=1699429168&sprefix=jumper%2Bcables%2Bsolder%2Caps%2C135&sr=8-10&th=1)
- Package of 3-port lever wire connectors: [Lowe's Link](https://www.lowes.com/pd/IDEAL-Lever-Wire-Connectors-3-Ports-Clear-10-Pack/5014013101)
- Red primary wire: [Lowe's Link](https://www.lowes.com/pd/Southwire-24-ft-16-AWG-Stranded-Red-GPT-Primary-Wire/1001833938)
- Black primary wire: [Lowe's Link](https://www.lowes.com/pd/Southwire-24-ft-16-AWG-Stranded-Black-GPT-Primary-Wire/1001833882)
- Alligator clips (these should come with the purchase of a Solperk charge controller, but if you are not using this regulator, you will need to purchase some): [Amazon Link](https://www.amazon.com/dp/B0BG7RCC11/ref=sspa_dk_detail_3?psc=1&pd_rd_i=B0BG7RCC11&pd_rd_w=QWtkO&content-id=amzn1.sym.8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_p=8c2f9165-8e93-42a1-8313-73d3809141a2&pf_rd_r=GVQ4VSGMPZ5580B94H6R&pd_rd_wg=RYreD&pd_rd_r=7bf77edd-0de5-486a-8d0c-8c2176a66427&sp_csd=d2lkZ2V0TmFtZT1zcF9kZXRhaWw)
- Grove screw terminal: [Seeed Link](https://www.seeedstudio.com/Grove-Screw-Terminal.html) 

1. Take the two alligator clips and cut their wires about five inches from the their ends. This is to help the wire get through the enclosure cable glands.
![Cut the wires of the alligator clips](wiring_images/IMG_9259.JPG)
2. Strip enough of the wire sheath to stick into a 3-lever wire connector by opening the lever to permit entry and then snapping the lever down to clamp onto the wire. You may need to twist the end of the wire to make it sturdy enough to get in.
![Attach the lever wire connectors to the alligator clips](wiring_images/IMG_9260.JPG)
3. Clamp the alligator clips onto the battery nodes, matching the colors.
![Attach the alligator clips to the battery](wiring_images/IMG_9261.JPG)
4. Take the soldered ADC development board and a small Qwiic cable.
![Qwiic cable and ADC board](wiring_images/wiring7.jpg)
5. Connec the two boards with the Qwiic cable.
![Connec the ADCs together](wiring_images/wiring8.jpg)
6. Address the ADCs by taking two jumper cables and connect one board's address pin (ADDR) to its power supply pin (VIN) and the other board's address pin (ADDR) to the serial data line (SDA). It does not matter which board is which, as long as they are different.
![Address the ADCs](wiring_images/wiring11.jpg)
7. Cut the ends off the Solperk charge controller wires leaving as much wire as possible.
![Clip the charge controller wires](wiring_images/IMG_9262.JPG)
8. Pull apart the coupled wires and strip and twist the ends.
![Strip the ends of the charge controller wires](wiring_images/IMG_9263.JPG)
9. Stick each wire in its own 3-lever wire connector. It does not matter which port you use.
![Add lever wire connectors to the charge controller wires](wiring_images/IMG_9264.JPG)
10. Cut a length of black primary wire that can reach from the charge controller to the battery. Strip the ends of this wire.
![Cut a lenght of black primary wire](wiring_images/IMG_9266.JPG)
11. Cut a length of red primary wire that can reach from the charge controller to the battery. Strip the ends of this wire.
![Cut a lenght of red primary wire](wiring_images/IMG_9265.JPG)
12. Attach these wires to their respective colors of the charge controller's battery wires using the already placed lever connectors. The battery wires are indicated by a battery icon and should the middle pair of red and black wires.
![Attach the battery wires to the charge controller](wiring_images/IMG_9268.JPG)
13. Run these wires through the cable glands over to the battery enclosure. Connect the black wire to the negative battery terminal, also indicated with the color black, and connect the red wire to the positive terminal, indicated with red.
![Connect the battery wires to the battery alligator clips](wiring_images/IMG_9267.JPG)
14. Strip the ends of the buck converter and attach them to one of the free ports on the charge controllers load output wires which are the pair of red and black wires on the far right. Match the colors.
![Attach the buck converter to the load](wiring_images/IMG_9271.JPG)
15. Cut and strip two small pieces of primary wire. One needs to reach from the positive (red) charge controller load lever port to the latching power relay and the other from the power relay to the bottom 3-to-9 lever port connector.
![Cut two red primary wires](wiring_images/IMG_9272.JPG)
16. Attach one wire to the single orange lever of the bottom 3-to-9 connector and to one of the common ports of the power relay, preferably 1COM. If you use 2COM, make sure to use 2NC instead of 1NC in the following step.
![Attach the power relay common port to the orange single end port of the bottom 3-to-9 connector](wiring_images/IMG_9274.JPG)
17. Attach the other primary wire from the 1NC (normally closed) port to the last open port on the positive (red) line of the charge controller load wire.
![Connect the power relay to the load of the charge controller](wiring_images/IMG_9275.JPG)
18. Cut and strip a piece of black primary wire to connect the single-end blue lever of the 3-to-9 connector just used to the last free lever port of the negative (black) line of the charge controller load wire.
19. Take a Grove cable and cut it in half, stripping the ends. Connec the cable to the Grove terminal of a stereo jack to grove converter.
![Connect a stripped Grove cable to a stereo jack converter](wiring_images/IMG_9277.JPG)
20. Using the 3-to-9 connector that is not being used for the power relay, connect the black wire of the Grove cable to one of the 3 blue ports, the white wire to one of the yellow ports, and the red wire to one of the orange ports.
![Connect a stereo jack converter to teh 3-to-9 connector](wiring_images/IMG_9278.JPG)
21. Repeat this process two more times until all 9 lever ports are filled. Note in these instructions one of the stereo jack converters is replaced with a screw gate converter in case you forgoe the stereo jacks and go the route of stripping the soil sensor wiring, which is shown in the next step.
22. Connect the soil sensors to the converters. It does not matter which one they are connected to. If you are not using stereo jack converters, strip the sensor sheath and connect the bare wire to the black Grove cable, the brown wire to the red Grove cable, and the yellow wire to the white Grove cable.
![Connect the soil sensors](wiring_images/IMG_9358.JPG)
23. Cut and strip a long Grove cable near one of its ends to maximize the length obtained.
![Cut Grove cable](wiring_images/IMG_9359.JPG)
24. Connect the unclipped end to one of the SDI-12 Grove ports on the Mayfly. Whichever one you connect it to, you need to also adjust the switched power voltage supplied to that port. Take a very fine screw driver and relocate the black jumper so that it connects the ground (center pin) to the 12-volt pin (far right). Be careful not to drop/lose this jumper!
![Connect the cable and relocate the jumper](wiring_images/IMG_9360.JPG)
25. Attach these SDI-12 Grove cables to the 3-to-9 connector that has the soil sensors attached to it, matching the colors accordingly.
26. Connect a Grove screw terminal to the MaxBotix MB7374 by connecting the black wire to GND, the red wire to VCC, and the white wire to D2.
![Connect a screw terminal to the MB7374](wiring_images/IMG_9361.JPG)
27. Attach a Grove cable to the screw terminal of the MB7374.
![Attach a Grove cable](wiring_images/IMG_9362.JPG)
28. Attach the Grove cable of the MB7374 to the D5-6 Grove terminal header on the Mayfly. Adjust the switched power jumper to the 5-volt position.
![Attach the MB7374 Grove cable and adjust the switched power jumper](wiring_images/IMG_9363.JPG)
29. Take a Qwiic cable and connect it to one of the Mayfly's Qwiic ports on the bottom right. It does not matter which one. Connect the other end of the cable to one of the free ends of the ADCs on the soldered development board. It does not matter which ADC it is connected to.
![Connect the ADCs to the Mayfly with a Qwiic cable](wiring_images/IMG_9364.JPG)
30. Take a Grove cable and attach one end to the power relay.
![Add a Grove cable to the power relay](wiring_images/IMG_9365.JPG)
31. Attach the other end of the power relay Grove cable to the D10-11 Grove terminal header on the Mayfly. Adjust the header to the 5-volt position.
![Attach the power relay to the Mayfly and adjust the switched power jumper](wiring_images/IMG_9366.JPG)
32. Separate the blue and clear (shield) cables of both the SL-510 and SL-610 sensors.
![Take the blue and clear wires of the pyrgeometers](wiring_images/IMG_9367.JPG)
33. Add them as pairs to 2 blue levers on the 3-to-9 connector that is attached to the power relay. This is grounding the pyrgeometer heaters.
![Ground the pyrgeometer heaters](wiring_images/IMG_9368.JPG)
34. Separate the green and clear wires from the SP-710.
![Separate the pyranometer heater grounds](wiring_images/IMG_9369.JPG)
35. Add these as a pair to the last open blue lever port, grounding the pyranometer heaters.
![Ground the pyranometer heaters](wiring_images/IMG_9370.JPG)
36. Separate the yellow wires of both the SL-510 and the SL-610.
![Separate the yellow wires of the pyrgeometers](wiring_images/IMG_9371.JPG)
37. Attach these wires to their own orange lever ports of the 3-to-9 connector attached to the power relay.
38. Separate the red wire of the SP-710.
![Separe the red wire of the pyranometers](wiring_images/IMG_9372.JPG)
39. Attach this red wire to the last open orange lever port.
40. Take two screw gate terminals and connect Grove cables to them (one for each so each Grove cable has a free end).
![Two open screw gate terminals](wiring_images/IMG_9373.JPG)
41. Connect the red wire of the ST-110 air temperature sensor to the VCC port of one of the screw gate terminals, then connect the white wire of the SP-710 to the D1 (yellow) port and the black wire of the SP-710 to the D2 (white) port.
![Connect pyranometer thermopile leads and excitation line of air temperature sensor](wiring_images/IMG_9374.JPG)
42. Attach the Grove cable of that screw gate to the inner-most auxiliary analog Grove terminal on the Mayfly. Its switched power jmper should already be at 3.3 volts, and it should stay there.
![Attach the screw gate terminal to the Mayfly](wiring_images/IMG_9375.JPG)
43. Take the other screw gate terminal and attach the yellow wire of the SP-710 to D1 (yellow) and the blue wire of the SP-710 to D2 (white). Take the two red wires from the SL-510 and the SL-610 and attach them to VCC. Take the clear and black wires of the ST-110 and attach them to GND. Note that the clear and black wires of the ST-110 could be attached to the other screw terminal as well. It does not matter. The red wires of the SL-510/610 and the ST-110 could also be attached to either terminal.
![Attach the thermopiles of the pyranometer, the excitiation lines of the pyrgeometers, and the shield and ground of the air temperature sensor](wiring_images/IMG_9377.JPG)
44. Attach the Grove cable to the outer-most auxiliary analog Grove port on the Mayfly, leaving the jumper pin as is.
![Attach the Grove cable to the Mayfly](wiring_images/IMG_9378.JPG)
45. Attach the green wire of the SL-510 to A1 on the board addressed to VIN. Attach the green wire of the SL-610 to A2 of the same board. As well attach the white wire of the ST-110 to A0 to this board.
![Attach the temperature sensors to their ADC](wiring_images/IMG_9382.JPG)
46. Attach the white wire of the SL-510 to A0 of the ADC addressed to SDA. Attach the black wire of the SL-510 to A1 of the same board. Attach the white wire of the SL-610 to A2 and the black wire of the SL-610 to A3 of this board.
![Attach the thermopiles of the pyrgeometers](wiring_images/IMG_9383.JPG)
