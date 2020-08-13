
int power12 = 50;
int power24 = 51;
int led = A0;
int drink1 = 22;
int drink2 = 24;
int drink3 = 26;
int drink4 = 28;

int water1 = 23;
int water2 = 25;
int water3 = 27;
int water4 = 29;

int highVolt = 53;

float cali;

int flowSensor1 = 2;
int flowSensor2 = 3;
int flowSensor3 = 18;
int flowSensor4 = 19;

volatile double dispensedVolume = 0;
char incomingChar;

void setup() {
  // put your setup code here, to run once:
  pinMode(drink1, OUTPUT);
  digitalWrite(drink1, HIGH);
  pinMode(drink2, OUTPUT);
  digitalWrite(drink2, HIGH);
  pinMode(drink3, OUTPUT);
  digitalWrite(drink3, HIGH);
  pinMode(drink4, OUTPUT);
  digitalWrite(drink4, HIGH);

  pinMode(highVolt, OUTPUT);
  digitalWrite(highVolt, HIGH);


  pinMode(water1, OUTPUT);
  digitalWrite(water1, HIGH);
  pinMode(water2, OUTPUT);
  digitalWrite(water2, HIGH);
  pinMode(water3, OUTPUT);
  digitalWrite(water3, HIGH);
  pinMode(water4, OUTPUT);
  digitalWrite(water4, HIGH);


  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  pinMode(power12, OUTPUT);
  digitalWrite(power12, HIGH);
  pinMode(power24, OUTPUT);
  digitalWrite(power24, HIGH);

  pinMode(flowSensor1, INPUT);
  pinMode(flowSensor2, INPUT);
  pinMode(flowSensor3, INPUT);
  pinMode(flowSensor4, INPUT);
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
      digitalWrite(highVolt, LOW);
      dispense(drink1, water1, flowSensor1, 375);
      break;

      case 'b':
      cali = 13*1.03733/2.9*0.624;
      dispensedVolume = 0;
      digitalWrite(highVolt, LOW);
      dispense(drink1, water1, flowSensor1, 500);
      break;

      case 'c':
      cali = 13*1.03733/2.9;
      //cali = 13;
      dispensedVolume = 0;
      digitalWrite(highVolt, LOW);
      dispense(drink2, water2, flowSensor2, 375);
      break;

      case 'd':
      cali = 13*1.03733/2.9;
      dispensedVolume = 0;
      digitalWrite(highVolt, LOW);
      dispense(drink2, water2, flowSensor2, 500);
      break;
      
      case 'e':
      cali = 13*1.03733/2.9;

      dispensedVolume = 0;
      digitalWrite(highVolt, LOW);
      dispense(drink3, water3, flowSensor3, 375);
      break;
      
      case 'f':
      cali = 13*1.03733/2.9;
      dispensedVolume = 0;
      digitalWrite(highVolt, LOW);
      dispense(drink3, water3, flowSensor3, 500);
      break;
     
      case 'g':
      cali = 13*1.03733/2.9;
      dispensedVolume = 0;
      digitalWrite(highVolt, HIGH);
      dispense(drink4, water4, flowSensor4, 375);
      break;
      
      case 'h':
      cali = 13*1.03733/2.9;
      digitalWrite(highVolt, HIGH);
      dispense(drink4, water4, flowSensor4, 500);
      break;

      case 'z':
      //dispensedVolume = 0;
      digitalWrite(highVolt, LOW);
      digitalWrite(water1, LOW);
      delay(2000);
      digitalWrite(water1, HIGH);
      digitalWrite(water2, LOW);
      delay(2000);
      digitalWrite(water2, HIGH);
      digitalWrite(water3, LOW);
      delay(2000);
      digitalWrite(water3, HIGH);
      digitalWrite(water4, LOW);
      delay(2000);
      digitalWrite(water4, HIGH);
      break;


    }
      

  }


}

void dispense(int drink, int water, int flowSensor, int volume){
  //digitalWrite(power, LOW);
  delay(100);
  digitalWrite(led, LOW);
  delay(100);
  digitalWrite(drink, LOW);
  attachInterrupt(digitalPinToInterrupt(flowSensor), dispensingVolume, FALLING);
  while (dispensedVolume*cali < volume){
    Serial.println(dispensedVolume*cali, 0);
    delay(25); 
    incomingChar= Serial.read();
    if (incomingChar == 'E'){
      break;
    } else if (incomingChar == 'K'){
      detachInterrupt(digitalPinToInterrupt(flowSensor));
//      digitalWrite(power,HIGH);
      digitalWrite(led, HIGH);
      digitalWrite(drink, HIGH);
      return;
    }
  }
  detachInterrupt(digitalPinToInterrupt(flowSensor));
  digitalWrite(drink, HIGH);
  digitalWrite(water, LOW);
  delay(600);
  digitalWrite(water, HIGH);
  dispenseEnded();
  
}

void dispensedPaused(){
  
}

void dispenseEnded(){
  digitalWrite(power12,HIGH);
  digitalWrite(power24,HIGH);
  digitalWrite(led, HIGH);
  digitalWrite(drink1, HIGH);
  digitalWrite(drink2, HIGH);
  digitalWrite(drink3, HIGH);
  digitalWrite(drink4, HIGH);
  digitalWrite(highVolt, HIGH);
  detachInterrupt(digitalPinToInterrupt(flowSensor1));
  detachInterrupt(digitalPinToInterrupt(flowSensor2));
  detachInterrupt(digitalPinToInterrupt(flowSensor3));
  detachInterrupt(digitalPinToInterrupt(flowSensor4));
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
