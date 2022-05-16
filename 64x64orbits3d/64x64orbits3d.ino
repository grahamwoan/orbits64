// orbits on a 64x64 led matrix display. In 3-D with z-dimming. ( I think 2-D is better!)
// Graham Woan 2022
// see https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#define MATRIX_HEIGHT 64

#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 14
#define G2_PIN 12
#define B2_PIN 13
#define A_PIN 23
#define B_PIN 19
#define C_PIN 5
#define D_PIN 22
#define E_PIN 32 
#define LAT_PIN 4
#define OE_PIN 15
#define CLK_PIN 21

#define  N 3 // number of balls
float n = 6; // soft wall repulsion index
float G = 4; // gravity strength
float dt = 0.0001*N; // timestep
float v02 = 6.0; // target mean speed squared for balls
float alpha =1.0e-8; // convergence rate to mean speed
float loss = 0.7; // loss factor if a fast ball hits a hard wall

float v2, dpx,dpy,dpz,rij2, dim;  
float posx[N], posy[N], posz[N], oposx[N], oposy[N], oposz[N]; // position in screen coordinates (0->64)
float M[N], Fx[N], Fy[N], Fz[N], px[N], py[N], pz[N], vx[N], vy[N], vz[N];
uint16_t col[N];
int analogPin = 3; // for random number

//MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t BLACK = dma_display->color565(0, 0, 0);
uint16_t RED = dma_display->color565(255, 0, 0);

void setup() {
  Serial.begin(9600); 
  // pin mapping
  HUB75_I2S_CFG::i2s_pins _pins={R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
  HUB75_I2S_CFG mxconfig(64,64,1, _pins);

  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(255); //0-255
  dma_display->clearScreen();

  randomSeed(analogRead(analogPin));
//random initial locations and speeds
  for (int ball = 0; ball < N; ball++) {
    px[ball] = random(100,900)/1000.0;
    py[ball] = random(100,900)/1000.0;
    pz[ball] = random(100,900)/1000.0;
    vx[ball] = random(-400,400)/100.0;
    vy[ball] = random(-400,400)/100.0;
    vz[ball] = random(-400,400)/100.0;
    col[ball] = RED;
  }

}

float regulate(float v) {   // regulate the speeds to a mean speed squared value
  double r,r1,r2;
  r = 1.0 - alpha*pow(v*v-v02,3);
  r1 = max(r,0.9);
  r2 = min(r1,1.1);
  return r2;
}

void loop() {
  for (int ball = 0; ball < N; ball++) {     // soft wall potential
    Fx[ball] = 0.0001*( pow(0.1+px[ball],-n) - pow(1.1-px[ball],-n) );
    Fy[ball] = 0.0001*( pow(0.1+py[ball],-n) - pow(1.1-py[ball],-n) );
    Fz[ball] = 0.0001*( pow(0.1+pz[ball],-n) - pow(1.1-pz[ball],-n) );

    for (int j = 0; j < N; j++) { // gravity loop
      if (j != ball){
        dpx = px[ball]-px[j];
        dpy = py[ball]-py[j];
        dpz = pz[ball]-pz[j];
        rij2 = dpx*dpx + dpy*dpy+ dpz*dpz;
        if (rij2>0.0004) { // only use gravity when they are not too close
        Fx[ball] = Fx[ball] - G*pow(rij2,-1.5)*dpx;
        Fy[ball] = Fy[ball] - G*pow(rij2,-1.5)*dpy;
        Fz[ball] = Fz[ball] - G*pow(rij2,-1.5)*dpz;
        }
      }
    }
    
    // Euler update with mean speed regulation
    px[ball] = px[ball] + vx[ball]*dt;
    py[ball] = py[ball] + vy[ball]*dt;
    pz[ball] = pz[ball] + vz[ball]*dt;
    vx[ball] = (vx[ball] + Fx[ball]*dt)*regulate(vx[ball]);
    vy[ball] = (vy[ball] + Fy[ball]*dt)*regulate(vy[ball]);
    vz[ball] = (vz[ball] + Fz[ball]*dt)*regulate(vz[ball]);
    
    // hard wall check
    if ( px[ball]>=1.0) {vx[ball] = -loss*vx[ball]; px[ball] = 1.0;}
    if ( px[ball]<=0.0) {vx[ball] = -loss*vx[ball]; px[ball] = 0.0;}
    if ( py[ball]>=1.0) {vy[ball] = -loss*vy[ball]; py[ball] = 1.0;}
    if ( py[ball]<=0.0) {vy[ball] = -loss*vy[ball]; py[ball] = 0.0;}
    if ( pz[ball]>=1.0) {vz[ball] = -loss*vz[ball]; pz[ball] = 1.0;}
    if ( pz[ball]<=0.0) {vz[ball] = -loss*vz[ball]; pz[ball] = 0.0;}
  }

  // display the balls
  for (int ball = 0; ball < N; ball++) {
  posx[ball] = floor((1.0-px[ball])*64);
  posy[ball] = floor((1.0-py[ball])*64);
  dim = 0.5/(0.5 + pow(pz[ball],2));
  switch (ball) {
    case 0:
     dma_display->drawPixel(posy[ball],posx[ball],dma_display->color565(int(dim*245), int(dim*155), int(dim*66)));
     break;
    case 1:
     dma_display->drawPixel(posy[ball],posx[ball],dma_display->color565(int(dim*136), int(dim*159), int(dim*227)));
     break;
    case 2:
     dma_display->drawPixel(posy[ball],posx[ball],dma_display->color565(int(dim*207), int(dim*185), 0));
     break;
  }
//  dma_display->drawPixel(posy[ball],posx[ball],col[ball]);
  if (posx[ball] != oposx[ball] || posy[ball] != oposy[ball])  dma_display->drawPixel(oposy[ball],oposx[ball],BLACK);
  oposx[ball] = posx[ball];
  oposy[ball] = posy[ball];
  }
}
