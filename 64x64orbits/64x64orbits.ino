// orbits on a 64x64 led matrix display
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
float G = 1.5; // gravity strength
float dt = 0.0001*N; // timestep
float v02 = 15.0; // target mean speed squared for balls
float alpha =1.0e-8; // convergence rate to mean speed
float loss = 0.7; // loss factor if a fast ball hits a hard wall

float v2, dpx,dpy,rij2;  
float posx[N], posy[N], oposx[N], oposy[N]; // position in screen coordinates (0->64)
float M[N], Fx[N], Fy[N], px[N], py[N], vx[N], vy[N];
uint16_t col[N];
int analogPin = 3; // for random number

//MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t BLACK = dma_display->color565(0, 0, 0);
uint16_t RED = dma_display->color565(255, 0, 0);
uint16_t ball_col_1 = dma_display->color565(245, 155, 66);
uint16_t ball_col_2 = dma_display->color565(136, 159, 227);
uint16_t ball_col_3 = dma_display->color565(207, 185, 0);

void setup() {
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
    vx[ball] = random(-400,400)/100.0;
    vy[ball] = random(-400,400)/100.0;
    col[ball] = RED;
  }

  col[0] = ball_col_1;
  col[1] = ball_col_2;
  col[2] = ball_col_3;
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

    for (int j = 0; j < N; j++) { // gravity loop
      if (j != ball){
        dpx = px[ball]-px[j];
        dpy = py[ball]-py[j];
        rij2 = dpx*dpx + dpy*dpy;
        if (rij2>0.0004) { // only use gravity when they are not too close
        Fx[ball] = Fx[ball] - G*pow(rij2,-1.5)*dpx;
        Fy[ball] = Fy[ball] - G*pow(rij2,-1.5)*dpy;
        }
      }
    }
    
    // Euler update with mean speed regulation
    px[ball] = px[ball] + vx[ball]*dt;
    py[ball] = py[ball] + vy[ball]*dt;
    vx[ball] = (vx[ball] + Fx[ball]*dt)*regulate(vx[ball]);
    vy[ball] = (vy[ball] + Fy[ball]*dt)*regulate(vy[ball]);
    
    // hard wall check
    if ( px[ball]>=1.0) {vx[ball] = -loss*vx[ball]; px[ball] = 1.0;}
    if ( px[ball]<=0.0) {vx[ball] = -loss*vx[ball]; px[ball] = 0.0;}
    if ( py[ball]>=1.0) {vy[ball] = -loss*vy[ball]; py[ball] = 1.0;}
    if ( py[ball]<=0.0) {vy[ball] = -loss*vy[ball]; py[ball] = 0.0;}
  }

  // display the balls
  for (int ball = 0; ball < N; ball++) {
  posx[ball] = floor((1.0-px[ball])*64);
  posy[ball] = floor((1.0-py[ball])*64);
  dma_display->drawPixel(posy[ball],posx[ball],col[ball]);
  if (posx[ball] != oposx[ball] || posy[ball] != oposy[ball])  dma_display->drawPixel(oposy[ball],oposx[ball],BLACK);
  oposx[ball] = posx[ball];
  oposy[ball] = posy[ball];
  }
}
