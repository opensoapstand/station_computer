#include "Adafruit_NeoPixel.h"

#define NUMPIXELS 8                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
int led = 13;

int colourSpec = 0;

double percentage = 0.0;

bool change = true;

// Interval is how long we wait
// add const if this should never change
unsigned long interval=50;
// Tracks the time since last event fired
unsigned long previousMillis=0;

Adafruit_NeoPixel strip(NUMPIXELS, led, NEO_GRB + NEO_KHZ800);

//Adafruit_NeoPixel strip(NUMPIXELS, led, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  //pinMode(led, INPUT_PULLUP);           // set pin to input

  strip.begin();
  //colorWipe(strip.Color(0,0,0),10);
  //strip.setPixelColor(i, colourSpec,colourSpec,colourSpec);
  //strip.show();
}

void loop() {
  //colorWipe(strip.Color(random(255),random(255),random(255)),50);
  //colorWipe(strip.Color(255,255,255),50);

  unsigned long currentMillis = millis();

  if ((unsigned long)(currentMillis - previousMillis) >= interval){
    if (change == true){
      colourSpec = colourSpec - 5; 
      percentage -= 0.02;
      if (colourSpec <= 5)
        change = false;
    }
    else 
    {
      colourSpec = colourSpec + 5; 
      percentage += 0.02;
      if (colourSpec >= 250)
        change = true; 
    }
  
    for(int i = 0; i < NUMPIXELS; i++){
      strip.setPixelColor(i, colourSpec,colourSpec,colourSpec);
      //strip.setPixelColor(i, random(250)*percentage,random(250)*percentage,random(250)*percentage);
      //colorWipe(strip.Color(random(250),random(250),random(250)), 10);
    }

    // Use the snapshot to set track time until next event
    previousMillis = currentMillis;
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
    //delay(100);
    strip.setPixelColor(i+1, c);
    //strip.setPixelColor(i+2, c);
    strip.show();
    //strip.setPixelColor(i, 0, 0 ,0);
    //strip.setPixelColor(i+1, 0, 0, 0);
    //strip.setPixelColor(i+2, 0, 0, 0);
    
    delay(100);
  }

  for(uint16_t i=strip.numPixels(); i>0; i--) {
    //strip.setPixelColor(i, c);
    //delay(1);
    //strip.setPixelColor(i+1, c);
    //strip.setPixelColor(i+2, c);
    //strip.show();
    strip.setPixelColor(i, 0, 0 ,0);
    strip.setPixelColor(i+1, 0, 0, 0);
    //strip.setPixelColor(i+2, 0, 0, 0);
    strip.show();
    delay(100);
    }
}
