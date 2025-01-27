### Wiring Guide

The steps in this guide are meant to be completed after the construction is done. Follow the instructions in the ![Construction Guide markdown file](construction_guide.md) and the ![Soldering Guide markdown file](soldering_guide.md) before continuing here. Note that this setup assumes a Solperk charge controller ([Link](https://www.amazon.com/SOLPERK-Controller-Waterproof-Intelligent-Regulator/dp/B0946MWYPK?ref_=ast_sto_dp)) is being used. Your implementation may look a little different if using a different charge controller, but the principles are the same.

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
23.
