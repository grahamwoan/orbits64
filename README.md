# orbits64
Displays an approximate n-body gravity simulation on a 64x64 LED matrix, for hypnotic fun. There are many displays out there, and the one I used was from AliExpress, described as a "64x64 indoor RGB hd p3 indoor led module video wall high quality P2.5 P3 P4 P5 P6 P7.62 P8 P10 LED panel full color led display".  The PCB is labelled "P3(2121) 64x64-32S-M5.1".  The silkscreen on my unit incorrectly labels the D pin (pin 18 on the HUB75 connector) as GND, so watch out. 

The display is controlled by a generic ESP32 WROVER module using the excellent drivers at https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA. The pinout definitions I used are in the code, so the standard driver install works fine. 

To get the full effect you need to place a diffusing screen about 2cm in front of the panel, so each of the gravitating bodies looks like a diffuse blob.  I used some 3mm "Matt Frosted Perspex Acrylic Sheet" from ebay. For the best effect, tune the parameters so that motion is not too slow as the trajectory can look stepped.

There is no need for a precision differential equation solver here as orbits don't last long before a body hits a wall, so they are simple fixed Euler steps.  The walls are hard, but have a power-law repulsion force to push slower bodies towards the centre.  Faster bodies that hit the walls experience a loss of energy when they bounce off, to tame them a little.  Overall stability is enforced by 1) turning gravity off when bodies get too close and 2) boosting/slowing bodies slighly over time so their mean kinetic energies are constant.  The ESP32 does not struggle to do these computations in real time, and large numbers of bodies are quite possible.  However, keep an eye on current consumption -- these displays can draw several amps if all the LEDs are turned on simultaneously. 

There is a simple 3D-printable frame to keep the screen the right distance for the LEDs, and a short movie to give some idea of what it looks like in practice.


