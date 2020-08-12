 int drink1 = 11;
int drink1S = 12;
int drink2 = 9;
int drink2S = 10;

float cali;

int flowSensor1 = 2;
int flowSensor2 = 3;

#define low 1
#define high 0

double dispensedVolume = 0;
char incomingChar;

void setup() {
  // put your setup code here, to run once:
  pinMode(drink1, OUTPUT);
  digitalWrite(drink1, high);
  pinMode(drink2, OUTPUT);
  digitalWrite(drink2, high);
  
  pinMode(drink1S, OUTPUT);
  digitalWrite(drink1S, high);
  pinMode(drink2S, OUTPUT);
  digitalWrite(drink2S, high);
 

  pinMode(flowSensor1, INPUT_PULLUP);
  pinMode(flowSensor2, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.flush();
  

}

void loop() {
  // put your main code here, to run repeatedly:
    //dispensedVolume = 0;
    
    if (Serial.available() > 0)  {
    
    incomingChar = Serial.read();

    switch(incomingChar){

      case 'a':
      cali = 0.8*0.33/2.0;
      dispensedVolume = 0;
      dispense(drink1, drink1S, flowSensor1, 50);
      break;

      case 'b':
      cali = 0.8*0.33/2.0;
      dispensedVolume = 0;
      dispense(drink1, drink1S, flowSensor1, 360);
      break;

      case 'c':
      cali = 0.8*0.33/2.0;
      dispensedVolume = 0;
      dispense(drink2, drink2S, flowSensor2, 50);
      break;

      case 'd':
      cali = 0.8*0.33/2.0;
      dispensedVolume = 0;
      dispense(drink2, drink2S, flowSensor2, 360);
      break;
      
//      case 'e':
//      dispensedVolume = 0;
//      dispense(drink3, flowSensor3, 375);
//      break;
//      
//      case 'f':
//      dispensedVolume = 0;
//      dispense(drink3, flowSensor3, 500);
//      break;

      case 'K':
      bool ack = true;
      while (ack){
      Serial.println('J', 0);
      incomingChar = Serial.read();
      if (incomingChar == 'J'){
        ack = false;
      }
      
      break;
    }
      

  }


}

}

void dispense(int drink, int drinkS, int flowSensor, int volume){
  //digitalWrite(power, low);
  delay(100);
  //digitalWrite(led, low);
  //delay(100);
  
  attachInterrupt(digitalPinToInterrupt(flowSensor), dispensingVolume, FALLING);
  while (dispensedVolume*cali < volume){
    Serial.println(dispensedVolume*cali, 0);
    delay(25); 
    incomingChar= Serial.read();
    
    
    if (incomingChar == 'P')
    {
        digitalWrite(drink, low);
        digitalWrite(drinkS, low);  //turn on the pump
    }
    else if (incomingChar == 'S')
    {
        digitalWrite(drink, high);
        digitalWrite(drinkS, high); //stops the pump
    }
    
    
    if (incomingChar == 'E'){
      break;
    } 
    
    
    else if (incomingChar == 'K'){
      detachInterrupt(digitalPinToInterrupt(flowSensor));
      //digitalWrite(power,high);
      //digitalWrite(led, HIGH);
      digitalWrite(drink, high);
      digitalWrite(drinkS, high);
      return;
    }
  }
  dispenseEnded();
  
}

void dispensedPaused(){
  
}

void dispenseEnded(){
  //digitalWrite(power,high);
  //digitalWrite(led, HIGH);
  digitalWrite(drink1, high);
  digitalWrite(drink2, high);
  digitalWrite(drink1S, high);
  digitalWrite(drink2S, high);
  //digitalWrite(drink3, HIGH);
  detachInterrupt(digitalPinToInterrupt(flowSensor1));
  detachInterrupt(digitalPinToInterrupt(flowSensor2));
  //detachInterrupt(digitalPinToInterrupt(flowSensor3));
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
