# orbits64
Displaying and approximate n-body gravity simulation on a 64x64 LED matrix. There are many displays out there.  The one I used was from AliExpress and described as a "64x64 indoor RGB hd p3 indoor led module video wall high quality P2.5 P3 P4 P5 P6 P7.62 P8 P10 LED panel full color led display".  The PCB is labelled "P3(2121) 64x64-32S-M5.1".  The silkscreen on my unit incorrectly labelled the D pin (pin 18 on the HUB75 connector) as GND. 

The display is controlled by a generic ESP32 WROVER module using the drivers at https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA. The pinout definitions are in the code. 

To get the full effect you need to place a diffusing screen about 2cm in front of the panel, so each of the gravitating bodies looks like a diffuse blob.  I used some 3mm "Matt Frosted Perspex Acrylic Sheet" from ebay. 

I've also uploaded a simple 3D-printable frame to keep the screen the right distance fro the LEDs.
