int power = 12;
int drink1 = 11;
int drink2 = 10;
int mpos = 9;

//int drink3 = 10;
float cali;

int flowSensor1 = 2;
int flowSensor2 = 3;
//int flowSensor3 = 2;

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
  //pinMode(drink3, OUaTPUT);
  //digitalWrite(drink3, HIGH);

  pinMode(mpos, OUTPUT);
  digitalWrite(mpos, low);
  pinMode(power, OUTPUT);
  //digitalWrite(power, high);

  pinMode(flowSensor1, INPUT);
  pinMode(flowSensor2, INPUT);
  //pinMode(flowSensor3, INPUT);
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
      cali = 13;
      dispensedVolume = 0;
      dispense(drink1, flowSensor1, 375);
      break;

      case 'b':
      cali = 13;
      dispensedVolume = 0;
      dispense(drink1, flowSensor1, 500);
      break;

      case 'c':
      cali = 13*1.03733;
      dispensedVolume = 0;
      dispense(drink2, flowSensor2, 375);
      break;

      case 'd':
      cali = 13*1.03733;
      dispensedVolume = 0;
      dispense(drink2, flowSensor2, 500);
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
      }
      break;
    }
      

  }


}

void dispense(int drink, int flowSensor, int volume){
  digitalWrite(power, low);
  delay(100);
  //digitalWrite(led, low);
  //delay(100);
  digitalWrite(drink, low);
  attachInterrupt(digitalPinToInterrupt(flowSensor), dispensingVolume, FALLING);
  while (dispensedVolume*cali < volume){
    Serial.println(dispensedVolume*cali, 0);
    delay(25); 
    incomingChar= Serial.read();
    if (incomingChar == 'E'){
      break;
    } else if (incomingChar == 'K'){
      detachInterrupt(digitalPinToInterrupt(flowSensor));
      digitalWrite(power,high);
      //digitalWrite(led, HIGH);
      digitalWrite(drink, high);
      return;
    }
  }
  dispenseEnded();
  
}

void dispensedPaused(){
  
}

void dispenseEnded(){
  digitalWrite(power,high);
  //digitalWrite(led, HIGH);
  digitalWrite(drink1, high);
  digitalWrite(drink2, high);
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
