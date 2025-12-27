# Example Code for Base Station Internet-Connected Dataloggers

Two examples are given in this folder, and each has associated code for the [Satellite station](../../mayfly_datalogger/telemetry) and [Base Mayfly](../mayfly) that you must use for successful deployment. 

## Telemetering Data with a Campbell Scientific Datalogger as the Internet-Connected Datalogger

The first example is for receiving and parsing data into Campbell Scientific (CS) datalogger tables using a CR800 as the "Internet-connected datalogger". We recognize that the CR800 is no longer being sold by Campbell Scientific, but the code here should be adaptable to any other CS datalogger.
This assumes that datalogger is already connected to a network you've set up in CS LoggerNet. Make sure to use the [push_to_cr800](../mayfly/push_to_cr800) sketch for the Base Mayfly if this is how you will connect to retrieve your data and the [satellite_varCode](../../mayfly_datalogger/telemetry/satellite_varCode) sketch for the satellite station Mayfly. You will also need to verify that the variable names the CRBasic code looks for match all the varCode variables that will be measured on the satellite data loggers.

The following figures show an example of an implementation like this done at the Tony Grove Ranger Station in Logan Canyon in northern Utah. The Base Mayfly datalogger is connected to a CR800 datalogger that is part of the [Logan River Observatory's](https://uwrl.usu.edu/lro/) climate monitoring network.

![LRO Base Station Example](../base_figures/cr800_base.png)

In the figure, the Mayfly datalogger is at the bottom-right of the enclosure, and the CR800 is at the top-right.

## Telemetering Data to HydroServer Using a Mayfly as the Internet-Connected DataLogger

The other example given here is for using a Mayfly as the "Internet-connected datalogger". This is separate from the Base Mayfly that aggregates data from every satellite station using a 900 MHz Xbee radio. The Internet-connected Mayfly datalogger uses a [LTE Bee modem](https://www.envirodiy.org/product/envirodiy-lte-bee/) for publishing data directly to [HydroServer](http://hydroserver.org). The following figure shows an example of how this would look, with a satellite snow sensing station that sends its data to a Base Mayfly over 900 MHz spreadspectrum radio which then pushes that 
data to an Internet-connected datalogger, which also happens to be a Mayfly datalogger in this case.

![mayfly_internet_datalogger_example](../base_figures/mayfly_internet_datalogger_example.png)

If you are using this method to set up your network, you will need to use the [push_to_mayfly_lte](../mayfly/push_to_mayfly_lte) sketch for the Base Mayfly and the [satellite_varUUID](../../mayfly_datalogger/telemetry/satellite_varUUID) sketch for the satellite station.

Make sure to properly set up the LTE Bee before deployment. To do so, you will need to purchase a SIM card. We recommend [Hologram](https://store.hologram.io/). You will need to set up an account before the SIM card will work. Once you receive the SIM card and set up the account, follow the instructions in the following screenshot taken from the first three steps of EnviroDIY's "[Assembling the Mayfly Data Logger Electronics](https://www.envirodiy.org/knowledge-base/building-an-envirodiy-monitoring-station/)" instructions.

![sim_card_instructions](../base_figures/sim_card_instructions.png)
