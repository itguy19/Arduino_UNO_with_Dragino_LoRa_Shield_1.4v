# Arduino_UNO_with_Dragino_LoRa_Shield_1.4v
<h2>Used hardware:</h2>
<ul>
  <li>Arduino Uno</li>
  <li>Dragino Lora Shield 1.4v</li>
  <li>I2C CHT8305C temperature and humidity sensor</li>
</ul>

<h2>Used resources / documentation:</h2>
<ul>
  <li>Deprecated and old ino file: https://github.com/dragino/Lora/blob/master/Lora%20Shield/Examples/lora_shield_ttn/lora_shield_ttn.ino</li>
  <li>Newest ino file for TTN v3: https://www.thethingsnetwork.org/forum/t/registering-an-end-device-arduino-dragino-lora-shield/50912/8</li>
  <li>Temperature and humidity sensor docs: https://wiki.dfrobot.com/SKU_SEN0546_I2C_Temperature_and_Humidity_Sensor_Stainless_Steel_Shell#target_4</li>
  <li>Andreas Spiess Youtube Video: https://www.youtube.com/watch?v=duwUwXt-hs8</li>
</ul>

This repo contains the necessary code to collect and send data from an Arduino Uno to TTN (The Things Network). It is equipped with a humidity and temperature sensor. Please note that this script only works for the european Lora frequency (868Mhz), so if you aren't using this in Europe, you have to modify the script a little bit more.


There are 3 versions of this projects. The .ino files uploaded here.
<ul>
  <li>v1 - Works, gets the humidity and temperature and prints the data to the monitor</li>
  <li>v2 - Doesn't work, tries to send the temp & hum data to the TTN</li>
  <li>v3 - Sometimes works, sends a "Hello World!" to the TTN, but it only works sometimes</li>
</ul>

<h2>Credit:</h2>
<ul>
  <li>Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman</li>
  <li>Copyright (c) 2018 Terry Moore, MCCI</li>
  <li>Copyright (c) 2018 Betina Wendel and Thomas Laurenson</li>
  <li>Adapted by 2023 itguy19</li>
</ul>
