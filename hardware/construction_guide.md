### Construction Guide

There are two principal methods of constructing the mast that will house all the sensors, enclosures, solar panels, etc.  
The first is to use a Campbell Scientific tripod: https://www.campbellsci.com/cm106b

The second is to use the platform equipment listed in the instrumentation costs spreadsheet that specifically does not use the Campbell Scientific equipment. This spreadsheet is found in this directory.  
You will save approximately $1000 by not just using the platform materials listed there, but also using other alternatives to  
Campbell Scientific hardware, such as solar charge controllers, solar panels, grounding kits, etc. 

This document will go over the construction of a mast that does not use Campbell Scientific hardware and how to assemble sensors and enclosures.

It will also cover to construct a central station for aggregating data from multiple snow satellite stations on a Mayfly and communicating that data to  
a Campbell Scientific CR800.

#### Low-cost Platform Setup

1. Take an auger and drill/dig a hole at least 20 inches into the ground near where the mast will be placed but not so close the u-post will be driven into the sensors.
2. If you have not already programmed your Teros 12 sensors, you will need to do that now. Follow the instructions of step 4 in the Hardware Setup Steps in the Mayfly Datalogger Code ![markdown file](https://github.com/CIROH-Snow/snow_sensing/tree/main/code/mayfly_datalogger)
3. Once the sensors are programmed, insert each sensor at their different depths: sensor 'c' at 20", sensor 'b' at 8 inches, and sensor 'a' at 2 inches.
4. Cut about a 3-foot lenght of flexible aluminum conduit that you can run the sensor stereo jacks through and bury the hole.
5. Take the U-post and begin inserting into the location of where the mast will go using a sledge hammer or a fence post pounder.
![Placing u-post](construction_images/IMG_9014.JPG)
6. Before pounding the u-post in too far, take a level and make sure the post is being driven in vertically by checking that it is straight on all sides.
![Leveling the u-post](construction_images/IMG_9015.JPG)
7. Finish driving in the post with enough depth that it feels secure in the ground when pushed around.
![Finishing the u-post](construction_images/IMG_9016.JPG)
8. Add the crossarm bracket to the fence pole at the desired height of the sensors. Note that the soil depth sensor cannot make measurements of distances less than 50 centimeters away and more than 5 meters away, so make sure to locate the crossarm at a height that will be able to capture the range of snow depths you can anticipate for your site. The rounded parts of the u-bolts on the bracket go around the pole and crossarm.
![Placing the crossarm bracket](construction_images/IMG_9129.JPG)
9. Remove the guy-wire bracket from the guy-wire kit.
![Guy-wire bracket](construction_images/IMG_9130.JPG)
10. Use a 7/16" wrench and a 1/2" wrench to loosen all the nuts on the bracket.
![Loosen the guy-wire bracket](construction_images/IMG_9132.JPG)
11. Slide the bracket over the top of the mast and locate it at a height between the top of the mast and the crossarm bracket. Note that the guy wires will be extending from the eye bolts, so orient them so that they do not interfere with the crossarm. At this point you may also consider capping off the ends of the crossarm and the top of the mast with electrical puddy to prevent bees, hornets, and wasps from making a home in them.
![Locating and orienting the guy-wire bracket](construction_images/IMG_9135.JPG)
12. Loosen the nuts between the bracket and the top of the copper bolt so that they are not pressed into the bracket.
![Loosen the copper nuts](construction_imgaes/IMG_9136.JPG)
13. Tighten down the copper bolts until the bracket has sufficiently grabbed onto the mast then tighten the previously loosened nuts to lock the bolts in place. Tighten the eye bolts as well.
14. Place the mast over the u-post making sure to orient the crossarm bracket in the desired direction of the crossarm.
![Placing the mast](construction_images/IMG_9139.JPG)
![Orienting the crossarm bracket](construction_images/IMG_9142.JPG)
15. Begin assembling the three guy wires. Each guy wire will have an end with an open loop that will be secured in the ground with a stake and an end that is attached to a turnbuckle that is hooked onto the guy-wire bracket. Start by cutting the wire into three equal lengths.
16. Assemble a wire by first loosening a wire clamp with a philips screw driver.
![Loosen a wire clamp](construction_images/IMG_9143.JPG)
17. Run the end of one wire through the grooves of the clamp to form a loop. Tighten the clamp down on this loop.
![Create free loop on guy wire](construction_images/IMG_9144.JPG)
18. Decide on where the stake for this wire will go. The wire needs to pull away from the bracket's eye bolts and not induce a torque when pulling on the mast, causing it to twist. Decide approximately the location of the stake and run the wire from that location to the bracket to get the approximate length of wire needed.
19. At that length, make another loop where the turnbuckle will go. Repeat the process of creating a loop and clamping it down, but this time run the loop through one end of the turnbuckle.
![Create the turnbuckle loop](construction_images/IMG_9146.JPG)
20. Loosen the turnbuckle as far as it will go and add the S-hook to the free end of the turnbuckle.
![Loosen the turnbuckle and add the hook](construction_images/IMG_9147.JPG)
21. Attach the S-hook end of the turnbuckle to the eye bolt of the bracket.
![Attach wire to the bracket](construction_images/IMG_9148.JPG)
22. Stake down the ground end of the guy wire using a mallet and pullin the wire tight, but making sure to keep the mast level.
![Stake in guy wire](construction_images/IMG_9149.JPG)
23. Repeat this process for all three wires.
24. Tighten the turnbuckles to impart tension to the wires and level the mast. Note that this may be an interative process of realigning stakes, changing wire lengths, and loosening/tightening turnbuckles to create even and solid tension in all wires while keeping the mast level, and you may need to go back and change things a few times over as you narrow in on the correct dimensions and setup.
![Tensioning the system](construction_images/IMG_9150.JPG)
25. Take the mast grounding clamp and open it up.
![Mast grounding clamp](construction_images/IMG_9153.JPG)
![Opening the mast grounding clamp](construction_images/IMG_9155.JPG)
26. Wrap it around the mast about two feet above the ground and tighten it on with a flat head screw driver.
![Placing the mast grounding clamp](construction_images/IMG_9159.JPG)
27. Dig a shallow trench for the grounding rod near the mast and place the rod inside.
![Grounding rod trench](construction_images/IMG_9220.JPG)
28. Place the grounding rod clamp on the end near the mast and tighten one end of the grounding cable in-between the bolt and the rod.
![Attach the grounding cable to the grounding rod](construction_images/IMG_9236.JPG)
29. Feed the other end into the cable hole of the mast grounding clamp and tighten the bolt to attach the cable.
![Attach the grounding cable to the mast](construction_images/IMG_9238.JPG)
30. Bury the rod.
![Bury the grounding rod](construction_images/IMG_9241.JPG)
31. Prep the enclosures that will be used. In these instructions, we will have an enclosure that will house the data logger, charge controller, and all other sensing peripherals; another enclosure will house a 35-Ah battery. You may elect to use larger batteries in different enclosures or to have smaller batteries in the same enclosure as the data logger. If you use a larger battery, such as the 100-Ah lithium iron phosphate batteries, the same principles can apply, but it is suggested that you do not locate the battery enclosure on the mast, but rather on the ground.
32. For a separate battery enclosure, drill four holes for U-bolts in the center of the enclosure. In this application, a 35-Ah battery is used, which fits better with a vertically oriented enclosure. Drill two holes on the side and place two cable glands. Note that you may also use one cable gland if you can get both wires that will be used through a single gland and tighten the gland all the way.
![Prep the battery enclosure](construction_images/IMG_9242.JPG)
33. Mount the enclosure to the mast at an accessible height using the U-bolts that come with plates. The plates will help distribute the pressure caused by the U-bolts over the enclosure's plastic.
![Mounting the battery enclosure](construction_images/IMG_9250.JPG)
34. Place the battery inside.
![Placing the battery inside the enclosure](construction_images/IMG_9251.JPG)
35. For the data logger enclosure, also drill four holes for the 2 U-bolts that will be used to mount this enclosure in the center. A hole will also be drilled for each of the sensors, the solar panel, and the battery. Select the cable gland sizes that will fit each of these cables and close around them completely. The Teros sensors will need larger glands than the others to accommodate the stereo jack. Test these glands on the cables before drilling holes. Once you decide on the cable gland sizes for each of the these items, begin drilling the holes in the locations shown in the image. If you drill something incorrectly, just use electrical puddy to fill in the hole as seen in the picture. It is suggested that you label what will be fed through each hole. Also attach the two 9-to-3 cable terminal blocks, power relay, soldered ADCs, and buck converter using command strips in the locations shown.
![Data logger enclosure layout](construction_images/IMG_9252.JPG)
36. Begin prepping the air temperature sensor and crossarm by breaking open the radiation shield kit.
37. Take the top piece of the radiation shield which has three prongs sticking out the top.
![Remove top of radiation shield](construction_images/IMG_9160.JPG)
38. Use a hacksaw to remove these three prongs.
![Flattened radiation shield top](construction_images/IMG_9161.JPG)
39. Drill holes down the center of this piece for the U-bolts that came in the radiation shield packet.
![U-bolts for radiation shield](construction_images/IMG_9162.JPG)
40. Slide the crossarm into the U-bolts and tighten the U-bolts to the crossarm.
![Attach the radiation shield top to the crossarm](construction_images/IMG_9136.JPG)
41. Attach the packet's long threaded rods to each of the three threaded holes in the underside of the top piece of the radiation shield.
![Attach the threaded rods](construction_images/IMG_9167.JPG)
42. Slide the next piece over these rods. Use the piece shown in the image.
![Add 2nd layer of radiation shield](construction_images/IMG_9169.JPG)
43. Add another layer of the shield, which will be the same piece as the one just added.
![Add 3rd layer of radiation shield](construction_images/IMG_9170.JPG)
44. Attach the Apogee air temperature sensor to the radiation shield's white sensor mounting rod using the supplied zipties.
![Attach the air temperature sensor](construction_images/IMG_9171.JPG)
45. Place the sensor mount in the center slot.
![Place the sensor mount](construction_images/IMG_9174.JPG)
46. Feed the layers of the radiation shield that have a large opening in the middle over the air temperature sensor wire.
![Begin feeding open layers of radiation shield over sensor wire](construction_images/IMG_9175.JPG)
![Placing open layers](construction_images/IMG_9176.JPG)
![Add all open layers](construction_images/IMG_9176.JPG)
47. The last piece will be a closed layer.
![Add the final layer](construction_images/IMG_9180.JPG)
48. Add the small hex nuts and wing nuts to the threaded rods.
![Add the hex nuts and wing nuts](construction_images/IMG_9184.JPG)
49. Add the finished crossarm to the mast by sliding the end without the air temperature sensor in first.
![Add the crossarm to the mast](construction_images/IMG_9186.JPG)
![Slide the crossarm through the bracket](construction_images/IMG_9189.JPG)
50. Slide the crossarm so that the radiation shield sits close to the mast.
![Put the radiation shield near the mast](construction_images/IMG_9190.JPG)
51. Tighten the crossarm to the bracket.
52. Prep the MaxBotix enclosure by opening it up and drilling a hole large enough for the sensor's threaded end to fit through and a hole for a gland that fits the sensor's wire.
![Drill holes in the MaxBotix enclosure](construction_images/IMG_9192.JPG)
![Add the cable gland](construction_images/IMG_9193.JPG)
53. Feed the MaxBotix gaskit over the cable and snugly wrap it over the threads of the sensor so it rests flat against the sensor. This will create a seal between the sensor and the enclosure.
54. Feed the sensor cable through the large hole and place the MaxBotix nut over the cable so that it sits inside the enclosure.
![Feed the cable through the hole and nut](construction_images/IMG_9194.JPG)
55. Feed the cable through the cable gland.
![Feed cable through gland](construction_images/IMG_9195.JPG)
56. Continue feeding cable until the sensor sits up against the enclosure then tighten the nut onto the sensor.
57. Put the top back on and screw it in.
![Close of the enclosure](construction_images/IMG_9196.JPG)
58. Mount the MaxBotix sensor to the crossarm by threading zipties through the enclosure holes found in the four corners and latches those onto the crossarm.
![Thread the zipties](construction_images/IMG_9202.JPG)
![Attach the enclosure to the crossarm](construction_images/IMG_9204.JPG)
59. Make sure the zipties are tight to limit any movement of the sensor. If you are setting up a site on a slope, you will need to measure the angle of the slope and then apply that same angle to the sensor. Many smart phones have level apps included in them that measure how flat a surface is. Make this adjustment to the sensor before tightening the zipties down. The goal is to have the sensor measure a perpendicular distance to the ground surface. Record this angle as it will be used in the data logger code.
![Tighten the zipties](construction_images/IMG_9205.JPG)
60. Assemble the Apogee AM-130 radiometer mounts.
61. Attach the free rod end to the AM-240 fixture using an allen wrench.
62. Use the supplied U-bolts in the AM-240 kit to attach the mount to the crossarm.
![Attach the radiometer mount to the crossarm](construction_images/IMG_9210.JPG)
63. Level the mount using the bubble.
![Leveled radiometer mount](construction_images/IMG_9212.JPG)
64. Remove the bases to the radiometers that will go in this mount, whether that is the shortwave or longwave sensors. These bases are not compatible with these mounts, and bases are not necessary for them.
![Remove the radiometer bases](construction_images/IMG_9213.JPG)
65. Slide them in to their respective slots, tightening them in with the philips screw at the front of the mount. Make sure the upward and downward sensors are in the approrpriate slots. Repeate the process for the other set of radiometers.
![Add the radiometers](construction_images/IMG_9214.JPG)
66. Get the solar panel ready for mounting to the post. In this implementation, we use a 30-watt panel from SOLPERK, which requires additional work to mount compared to a Campbell Scientific panel. Prep the solar panel by removing the adjustable brace and drilling two holes for the U-bolt mount clamp in the center. Note that the solar panel is metal, and drilling will take extra time and care. Start with small holes and progressively work your way up to the desired size to facilitate drilling.
![Drill holes for U-bolt in solar panel brace](construction_images/IMG_9218.JPG)
67. The wing nuts do not provide a lot of tightness and friction for holding the solar panel in place. You may consider changing out that hardware by drilling larger holes in the sides of the panels and the brace and adding more robust bolts and washers. The package shown below has hex nuts that fit on the inside of the solar panel.
![Enlarge the holes on the side](construction_images/IMG_9217.JPG)
![Hardware for solar panel brace](construction_images/IMG_9216.JPG)
68. Attach the solar panel to the mast using the U-bolt kit. Set the angle to be as perpendicular to midday sun as possible.
![Add the panel to the mast](construction_images/IMG_9219.JPG)
69. Ziptie the sensor cables to the crossarm and the solar panel and air temperature sensors to the mast.
![Ziptie crossarm sensor cables](construction_images/IMG_9349.JPG)
![Ziptie air temperature sensor and solar panel wires to mast](construction_images/IMG_9350.JPG)
70. Coil the wires and attach them to the mast behind the enclosure. Leave enough of a lead on each wire to feed it through its cable gland.
![Coil the wires](construction_images/IMG_9258.JPG)
71. Feed the cables through their respective glands in the enclosure and tighten the glands.
