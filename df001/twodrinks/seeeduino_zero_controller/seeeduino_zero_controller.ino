#include "Adafruit_NeoPixel.h"

#define NUMPIXELS 19
//int power12 = 50;
//int power24 = 51;
//int led = A0;
//int drink1 = 22;
//int drink2 = 24;
//int drink3 = 26;
//int drink4 = 28;

//int water1 = 23;
//int water2 = 25;
//int water3 = 27;
//int water4 = 29;

//int highVolt = 53;

float cali;

//int flowSensor1 = 2;
//int flowSensor2 = 3;
//int flowSensor3 = 18;
//int flowSensor4 = 19;

int power12 = 0;
int power24 = 1;
int led = 13;

int drink1 = 3;
int water1 = 11;
int flowSensor1 = 5;

int drink2 = 6;
int water2 = 12;
int flowSensor2 = 8;
int pump2 = 9;

Adafruit_NeoPixel strip(NUMPIXELS, led, NEO_GRB + NEO_KHZ800);

volatile double dispensedVolume = 0;
char incomingChar;

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  colorWipe(strip.Color(0,0,0),10);
  strip.show();
  
  pinMode(drink1, OUTPUT);
  digitalWrite(drink1, LOW);
  pinMode(drink2, OUTPUT);
  digitalWrite(drink2, LOW);
//  pinMode(drink3, OUTPUT);
//  digitalWrite(drink3, HIGH);
//  pinMode(drink4, OUTPUT);
//  digitalWrite(drink4, HIGH);

//  pinMode(highVolt, OUTPUT);
//  digitalWrite(highVolt, HIGH);


  pinMode(water1, OUTPUT);
  digitalWrite(water1, LOW);
  pinMode(water2, OUTPUT);
  digitalWrite(water2, LOW);
//  pinMode(water3, OUTPUT);
//  digitalWrite(water3, HIGH);
//  pinMode(water4, OUTPUT);
//  digitalWrite(water4, HIGH);


  //pinMode(led, OUTPUT);
  //digitalWrite(led, HIGH);
  pinMode(power12, OUTPUT);
  digitalWrite(power12, LOW);
  pinMode(power24, OUTPUT);
  digitalWrite(power24, LOW);

  pinMode(flowSensor1, INPUT);
  pinMode(flowSensor2, INPUT);
//  pinMode(flowSensor3, INPUT);
//  pinMode(flowSensor4, INPUT);
  Serial.begin(9600);
  Serial.flush();
  

}

void loop() {
  // put your main code here, to run repeatedly:
      dispensedVolume = 0;

    if (Serial.available() > 0)  {
    incomingChar = Serial.read();

    switch(incomingChar){

      case 'a':
      cali = 13*1.03733/2.9*0.624;
      dispensedVolume = 0;
      //digitalWrite(highVolt, LOW);
      //digitalWrite(power12, LOW);
      dispense(drink1, water1, flowSensor1, 375);
      break;

      case 'b':
      cali = 13*1.03733/2.9*0.624;
      dispensedVolume = 0;
      //digitalWrite(highVolt, LOW);
      //digitalWrite(power12, LOW);
      dispense(drink1, water1, flowSensor1, 500);
      break;

      case 'c':
      cali = 13*1.03733/2.9;
      //cali = 13;
      dispensedVolume = 0;
      //digitalWrite(highVolt, LOW);
      //digitalWrite(power12, LOW);
      //digitalWrite(power24, LOW);
      digitalWrite(pump2, LOW);
      dispense(drink2, water2, flowSensor2, 375);
      break;

      case 'd':
      cali = 13*1.03733/2.9;
      dispensedVolume = 0;
      //digitalWrite(highVolt, LOW);
      //digitalWrite(power12, LOW);
      //digitalWrite(power24, LOW);
      digitalWrite(pump2, LOW);                                                                                                                                                                                                                                                                                                                        
      dispense(drink2, water2, flowSensor2, 500);
      break;
      
//      case 'e':
//      cali = 13*1.03733/2.9;
//
//      dispensedVolume = 0;
//      digitalWrite(highVolt, LOW);
//      dispense(drink3, water3, flowSensor3, 375);
//      break;
//      
//      case 'f':
//      cali = 13*1.03733/2.9;
//      dispensedVolume = 0;
//      digitalWrite(highVolt, LOW);
//      dispense(drink3, water3, flowSensor3, 500);
//      break;
//     
//      case 'g':
//      cali = 13*1.03733/2.9;
//      dispensedVolume = 0;
//      digitalWrite(highVolt, HIGH);
//      dispense(drink4, water4, flowSensor4, 375);
//      break;
//      
//      case 'h':
//      cali = 13*1.03733/2.9;
//      digitalWrite(highVolt, HIGH);
//      dispense(drink4, water4, flowSensor4, 500);
//      break;

      case 'z':
      //dispensedVolume = 0;
//      digitalWrite(highVolt, LOW);
      digitalWrite(water1, HIGH);
      delay(2000);
      digitalWrite(water1, LOW);
      digitalWrite(water2, HIGH);
      delay(2000);
      digitalWrite(water2, LOW);
//      digitalWrite(water3, LOW);
//      delay(2000);
//      digitalWrite(water3, HIGH);
//      digitalWrite(water4, LOW);
//      delay(2000);
//      digitalWrite(water4, HIGH);
      break;


    }
      

  }


}

void dispense(int drink, int water, int flowSensor, int volume){
  //digitalWrite(power, LOW);
  
  delay(100);
  //digitalWrite(led, LOW);
  delay(100);
  digitalWrite(drink, HIGH);
  attachInterrupt(digitalPinToInterrupt(flowSensor), dispensingVolume, FALLING);
  while (dispensedVolume*cali < volume){
    //colorWipe(strip.Color(random(100),random(100),random(100)),30);
    Serial.println(dispensedVolume*cali, 0);
    delay(25); 
    incomingChar= Serial.read();
    if (incomingChar == 'E'){
      break;
    } else if (incomingChar == 'K'){
      detachInterrupt(digitalPinToInterrupt(flowSensor));
//      digitalWrite(power,HIGH);
      //digitalWrite(led, HIGH);
      digitalWrite(drink, LOW);
      return;
    }
  }           
  detachInterrupt(digitalPinToInterrupt(flowSensor));
  digitalWrite(drink, LOW);
  digitalWrite(water, HIGH);
  delay(1000);
  digitalWrite(water, LOW);
  dispenseEnded();
  
}

void dispensedPaused(){
  
}

void dispenseEnded(){
  colorWipe(strip.Color(0,0,0),20);
  //digitalWrite(power12,HIGH);
  //digitalWrite(power24,HIGH);
  //digitalWrite(led, HIGH);
  digitalWrite(drink1, LOW);
  digitalWrite(drink2, LOW);
//  digitalWrite(drink3, HIGH);
//  digitalWrite(drink4, HIGH);
//  digitalWrite(highVolt, HIGH);
  detachInterrupt(digitalPinToInterrupt(flowSensor1));
  detachInterrupt(digitalPinToInterrupt(flowSensor2));
//  detachInterrupt(digitalPinToInterrupt(flowSensor3));
//  detachInterrupt(digitalPinToInterrupt(flowSensor4));
  bool end = false;
  while (!end){
    incomingChar= Serial.read();
    Serial.println(dispensedVolume*cali,0);
    if (incomingChar == 'Z'){
      dispensedVolume = 0;
      break;
    }
    
  }                                                                                                                                                                                              
}


void dispensingVolume(){
  dispensedVolume = (dispensedVolume + 0.2);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    delay(1);
    strip.setPixelColor(i+1, c);
    //strip.setPixelColor(i+2, c);
    strip.show();
    //strip.setPixelColor(i, 0, 0 ,0);
    //strip.setPixelColor(i+1, 0, 0, 0);
    //strip.setPixelColor(i+2, 0, 0, 0);
    
    delay(1);
  }

  for(uint16_t i=strip.numPixels(); i>0; i--) {
    strip.setPixelColor(i, c);
    //delay(1);
    strip.setPixelColor(i+1, c);
    //strip.setPixelColor(i+2, c);
    strip.show();
    //strip.setPixelColor(i, 0, 0 ,0);
    //strip.setPixelColor(i+1, 0, 0, 0);
    //strip.setPixelColor(i+2, 0, 0, 0);
    
    delay(1);
    
    }
}
