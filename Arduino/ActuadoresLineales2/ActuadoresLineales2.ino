const int pinRelayA = 12;       /*Actuador 1*/
const int pinRelayB = 11;
const int pinRelayC = 10;       /*Actuador 2*/
const int pinRelayD = 9;
const int pinPotenciometerS = A0;/*Potenciometro Velocidad*/
const int pinPotenciometerD = A1;/*Potenciometro Dirección*/

const int EXTEND = 1;
const int STOP = 0;
const int RETRACT = -1;

const int UP = 1;
const int DOWN = -1;

long countSpeed = 2000; 
int speed;
int direction;
int directionAux;
long delayValue = 300;

String inputString = ""; 
boolean stringComplete = false;

void setup(){
   Serial.begin(9600);
   pinMode(pinRelayA, OUTPUT);/*Actuador 1*/
   pinMode(pinRelayB, OUTPUT);
   digitalWrite(pinRelayA, LOW);
   digitalWrite(pinRelayB, LOW);
   pinMode(pinRelayC, OUTPUT);  /*Actuador 2*/
   pinMode(pinRelayD, OUTPUT);
   digitalWrite(pinRelayC, HIGH);
   digitalWrite(pinRelayD, HIGH);
   directionAux = 50;
}

/*Extender, retraer y detener el actuador*/
void actuator(int pinA, int pinB, int type){
  switch(type){
    case EXTEND :
      digitalWrite(pinB, LOW);
      delay(delayValue);
      digitalWrite(pinA, HIGH);
    break;
    case STOP :
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, LOW);
    break;
    case RETRACT :
      digitalWrite(pinA, LOW);
      delay(delayValue);
      digitalWrite(pinB, HIGH);
    break;
  }
}

void ola(int UP_DOWN){
    switch(UP_DOWN){
      case UP :
        actuator(pinRelayA, pinRelayB, EXTEND);
        actuator(pinRelayD, pinRelayC, RETRACT);
        delay(delayValue);
      break;
      
      case DOWN  :
        actuator(pinRelayA, pinRelayB, RETRACT);
        actuator(pinRelayD, pinRelayC, EXTEND);
        delay(delayValue);
      break;
    }
};
/**/
void directionFunction(int velocidad, int EXTEND_RETRACT){
  /*extend - left*/ /*retract - rigth*/
  actuator(pinRelayA, pinRelayB, EXTEND_RETRACT);
  actuator(pinRelayD, pinRelayC, EXTEND_RETRACT);
  delay(delayValue);
}
void loop(){
  /*Leer los potenciomentros*/
  speed = map(analogRead(pinPotenciometerS), 0, 1023, 0, 100);
  direction = map(analogRead(pinPotenciometerD), 0, 1023, 0, 100);
  //Mandar los valores a unreal
  if(inputString.length()>0){
    if(inputString.substring(1, 7) == "getVel" || inputString.substring(0, 6) == "getVel"){
      Serial.println(speed);
    }
    if(inputString.substring(1, 7) == "getDir" || inputString.substring(0, 6) == "getDir"){
       Serial.println(direction);
    }
    inputString = "";
    delay(90);

    if((direction >= directionAux - 2) && (direction <= directionAux + 2)){
     //Leer valor
        if(inputString.substring(1, 7) == "get_UP" || inputString.substring(0, 6) == "get_UP"){
          ola(UP);
        }
        if(inputString.substring(1, 7) == "getDOW" || inputString.substring(0, 6) == "getDOW"){
          ola(DOWN);
        }
        inputString = "";
        delay(90);
    }else if(direction < directionAux){
      directionFunction(5000, RETRACT);
      if(inputString.substring(1, 7) == "getAct" || inputString.substring(0, 6) == "getAct"){
        Serial.println("RETRACT");
      }
    }else if(direction > directionAux){
      directionFunction(5000, EXTEND);
      if(inputString.substring(1, 7) == "getAct" || inputString.substring(0, 6) == "getAct"){
        Serial.println("EXTEND");
      }
    }
  }
  directionAux = direction;
}

void serialEvent() {
  stringComplete = "";
    while (Serial.available()) {
      char inChar = (char)Serial.read();
      inputString += inChar;
      if (inChar == '\n') {
        stringComplete = true;
    }
  }
}

