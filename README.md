# RGB-matrix-Panel-Zero
Forked Adafruit/RGB-matrix-Panel enhanced to work with Feather M0 boards.

### What's different from the Adafruit RBG-matrix-Panel library?
- Adds support for Feather M0 boards.
- All changes are via #ifdef ARDUINO_SAMD_FEATHER_M0.
- No change to the interface. Pin assignments in the constructor are ignored.
- Adds an interrupt handler which pushes pixels directly out the hardware I/O port.

### Useage
To download. click the ZIP (download) button, rename the uncompressed folder RGBLEDMatrix. 
Check that the RGBmatrixPanel folder contains RGBmatrixPanel.cpp and RGBmatrixPanel.h

Place the RGBmatrixPanel library folder your arduinosketchfolder/libraries/ folder. 
You may need to create the libraries subfolder if its your first library. 
Restart the IDE.

Your code and all examples at https://github.com/adafruit/RGB-matrix-Panel/tree/master/examples
will now run unmodified on your Feather M0.

Build a wiring adapter on a proto board from the supplied schematic or order a PCB from
Oshpark.com. Connect up to the panel and you are good to go!

<a href="https://oshpark.com/shared_projects/1QNzmjwa"><img src="https://oshpark.com/assets/badge-5b7ec47045b78aef6eb9d83b3bac6b1920de805e9a0c227658eac6e19a045b9c.png" alt="Order from OSH Park"></img></a>
