# RGB-matrix-Panel-Zero
Libraries to drive LED Matrix panels from the Arduino MKRZero and the Adafruit Feather M0 boards.

### What's here?
All sorts of stuff. Graphics drivers, adapter schematics, and PCBs.

There are two libraries, a forked Adafruit/RGB-matrix-Panel to work with Feather M0 boards which
will drive panels in 12 bit color, and a simpler, faster driver which works with both the Arduino
and Feather boards but is limited to 8 colors.

### Forked Adafruit RGBmatrixPanel Library
- Adds support for Feather M0 boards.
- Drives 16 and 32 row panels. See LEDpanel library for 64 row panels.
- All changes are via #ifdef ARDUINO_SAMD_FEATHER_M0.
- No change to the interface. Pin assignments in the constructor are ignored.
- Adds an interrupt handler which pushes pixels directly out the hardware I/O port.

This library directly replaces the RGBmatrixPanel.cpp from the Adafruit library. It is
plug-and-play, no other changes are required. Any existing code and all examples at
https://github.com/adafruit/RGB-matrix-Panel/tree/master/examples will now run unmodified
on your Feather M0.

### LEDpanel library
This library supports both Arduino MKRZero and Feather M0 boards. It is strictly a driver that
takes a pointer to a frame buffer and drives the display. Each pixel is one of 8 colors stored
as the lowest 3 bits in a byte. ex: 00000BGR. The total display buffer size is (width * height) bytes.
Panels with 16, 32, and 64 rows are supported.

### Pinouts and connections
The pinout is fixed. The SPI and I2C ports are free and available. On the Feather M0 the LED_BUILTIN
port is used. Don't try to control the LED or it will affect the panel display.

There are schematics for both boards with a bill of materials and Digikey part numbers for connectors
if you wish to build an adapter on a proto board, or you can order a PCB from Oshpark.com. Eagle CAD
files are also supplied if you want to do some customization.

<a href="https://oshpark.com/shared_projects/1QNzmjwa"><img src="https://oshpark.com/assets/badge-5b7ec47045b78aef6eb9d83b3bac6b1920de805e9a0c227658eac6e19a045b9c.png" alt="Order from OSH Park"></img></a>

<p align="center"><img width="388" height="390" src="/images/Feather_M0_RGBmatrixPanel_Connection_Diagram.png"></p>

<p align="center"><img width="388" height="390" src="/images/adapter_in_panel.jpg"></p>

<p align="center"><img width="388" height="390" src="/images/feather_in_panel.jpg"></p>
