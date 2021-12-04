# ThingsBoard-TTGO-T-Beam-GeoTracker-Telemetry
This project seeks to collect and send any kind of telemetry to the ThingsBoard platform. It's purpose is to geotrace each data point, with the initial work of tracking temperature and humidity changes and compare with the urban aspect of that data set. At least two TTGO T-Beam are required to run this project, one or more Telemetry unities (Which collect the data and send them via LoRa to the Gateway) and the Gateway itself (Which is responsible to gather the LoRa data from multiple Telemetry Units and send it to the ThingsBoard, using the ThingsBoard SDK).

![Animação](https://user-images.githubusercontent.com/22375957/144682160-767af315-0fe9-481e-8302-346752556ded.gif)

# Tutorial on how to upload sketches to the ESP32 TTGO T-Beam Board, and basic usage of the built in GPS and LoRa.

* ## Before you start:
	* CONECT THE LORA ANTENNA TO THE BOARD, DO NOT POWER ON THE BOARD WITHOUT THE LORA ANTENNA OR YOU MIGHT DAMAGE IT!
	* Install the following driver on your computer:
		* Silicon Labs - CP210x: Download [here](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers) 
			> This driver is used by your computer to open a serial comunitation with your board.
	* Environment setup:
		* Arduino IDE:
			* Add the following url in "Additional Board Manager URLs" at File -> Preferences:
				> https://dl.espressif.com/dl/package_esp32_index.json
			
				then click Ok, go to Tools -> Board -> Boards Manager, search and install "ESP32 by Espressif Systems", or follow this [guide](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).
			* After you install the board software, just select on the boards menu "T-Beam", set the upload speed to 115200, leave the rest as is and you are going to be able to upload sketches to your TTGO board.

# At least two TTGO T-Beam boards are required to run this project. One for the gateway and one or more for the Telemetry Unit.

	* A new device shall be created on ThingsBoard for each Telemetry Unit. However, tokens are set on utilities.h of the Gateway code.

	* The Gateway Board must be connected to a network that can be accessed by the ThingsBoard Instance at all time. Otherwise, data will be lost.

	* The Wi-Fi SSID and Password must be defined at utilities.h of the Gateway code.
