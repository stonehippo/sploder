#SPLODER

A connected button for doing stuff up. With Bluetooth Smart (aka BLE).

## Dependencies

* [FiniteStateMachine](http://playground.arduino.cc/Code/FiniteStateMachine)
* [Adafruit's nRF8001 Bluetooth Low Energy Breakout drivers](https://github.com/adafruit/Adafruit_nRF8001)
* [Modified Firmata code to work with Adafruit's nRF8001 Breakout](https://github.com/adafruit/Adafruit_BLEFirmata)

## Building Sploder

Sploder is currently being developed using [Arduino](http://arduino.cc) 1.0.6, mostly due to incompatibilities between the Arduino 1.5/1.6 branch and the Adafruit BLE libraries.

If you want to hack on Sploder, clone the project into your Arduino sketchbook directory, and make sure you've installed the library dependencies above.


## Design notes

![Sploder circuit schematic](docs/sploder_breadboard.jpg)

![Sploder circuit schematic](docs/sploder_schematic.jpg)

See the Fritzing sketch for the Sploder circuit is available in `docs`. (Sorry for the outsized nrf on the schematic. Adafruit needs to fix some scale issues on some parts in their still-excellent [Fritzing Library](https://github.com/adafruit/Fritzing-Library)).

I am using an Arduino Pro Mini 3.3V for my Sploder implementation, but any Arduino should work just fine. I am powering my Sploder from a 3.7V LiPO battery, but anything that works from 3.7V-12V will probably work.

The Bluetooth LE interface is provided by Adafruit's nrf8001 Bluefruit LE Breakout. You can check out [the guide for the nrf8001 board](https://learn.adafruit.com/getting-started-with-the-nrf8001-bluefruit-le-breakout/) if you want detailed information on how to use it.
