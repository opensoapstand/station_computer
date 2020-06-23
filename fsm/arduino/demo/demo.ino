#include "Adafruit_NeoPixel.h"

#define NUMPIXELS 19
int led = 13;
int colourSpec = 0;

int period = 50;
unsigned long time_now = 0;

bool change = true;

Adafruit_NeoPixel strip(NUMPIXELS, led, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  //colorWipe(strip.Color(0,0,0),10);
  //strip.setPixelColor(i, colourSpec,colourSpec,colourSpec);
  //strip.show();
}

void loop() {
  //colorWipe(strip.Color(random(255),random(255),random(255)),50);
  //colorWipe(strip.Color(255,255,255),50);

  if (millis() >= time_now + period){
    if (change == true){
      colourSpec = colourSpec - 5; 
      if (colourSpec <= 5)
        change = false;
    }
    else 
    {
      colourSpec = colourSpec + 5; 
      if (colourSpec >= 250)
        change = true; 
    }
  
    for(int i = 0; i < NUMPIXELS; i++){
      strip.setPixelColor(i, colourSpec,colourSpec,colourSpec);
    }

    if(time_now >10000*millis())
      time_now = 0; //resets time_now
    else 
      time_now += period;
      
    
    strip.show();
  }
  //delay(25);

  //for(int i = 0; i < NUMPIXELS; i++)
    //strip.setPixelColor(i, 255,255,255);
    
  //strip.show();
  //delay(350);

  
}


void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    delay(100);
    strip.setPixelColor(i+1, c);
    //strip.setPixelColor(i+2, c);
    strip.show();
    //strip.setPixelColor(i, 0, 0 ,0);
    //strip.setPixelColor(i+1, 0, 0, 0);
    //strip.setPixelColor(i+2, 0, 0, 0);
    
    delay(1);
  }

  for(uint16_t i=strip.numPixels(); i>0; i--) {
    //strip.setPixelColor(i, c);
    //delay(1);
    //strip.setPixelColor(i+1, c);
    //strip.setPixelColor(i+2, c);
    //strip.show();
    strip.setPixelColor(i, 0, 0 ,0);
    //strip.setPixelColor(i+1, 0, 0, 0);
    //strip.setPixelColor(i+2, 0, 0, 0);
    strip.show();
    delay(50);
    
    }
}
