#include <String.h>
char *strtok( char *str1, const char *str2 );

const int pinRelayA = 12;       /*Actuador 1: Frontal*/
const int pinRelayB = 11;
const int pinRelayC = 10;       /*Actuador 2: LateralA*/
const int pinRelayD = 9;
const int pinRelayE = 8;        /*Actuador 3: LateralB*/
const int pinRelayF = 7;
const int pinPotenciometerS = A0;/*Potenciometro Velocidad*/
const int pinPotenciometerD = A1;/*Potenciometro Dirección*/

const bool EXTEND = true;
const bool RETRACT = false;
const int STOP = 0; /*Inutil >:v*/

const int UP = 1;
const int DOWN = -1;

long countSpeed = 2000; 
int speed;
int speedAux;
int direction;
int directionAux;
long delayValue = 200;

String inputString = ""; 
boolean stringComplete = false;
char delimiter[] = "_";
char *front;
char *sideA;
char *sideB;

void setup(){
   Serial.begin(9600);
   pinMode(pinRelayA, OUTPUT);/*Actuador 1 Frontal*/
   pinMode(pinRelayB, OUTPUT);
   digitalWrite(pinRelayA, LOW);
   digitalWrite(pinRelayB, LOW);
   pinMode(pinRelayC, OUTPUT);/*Actuador 2 LateralA*/
   pinMode(pinRelayD, OUTPUT);
   digitalWrite(pinRelayC, LOW);
   digitalWrite(pinRelayD, LOW);
   pinMode(pinRelayE, OUTPUT);/*Actuador 3 LateralB*/
   pinMode(pinRelayF, OUTPUT);
   digitalWrite(pinRelayE, LOW);
   digitalWrite(pinRelayF, LOW);
   //directionAux = 50;
   //speedAux = 50;
}

/*Extender, retraer  el actuador*/
void actuator(int pinA, int pinB, bool type){
  if(type){ /*EXTEND*/
    digitalWrite(pinB, LOW);
    delay(delayValue);
    digitalWrite(pinA, HIGH);
  }else{  /*RETRACT*/
    digitalWrite(pinA, LOW);
    delay(delayValue);
    digitalWrite(pinB, HIGH);
  }
}
void actuadorStop(int pinA, int pinB){
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, LOW);
}

void ola(char *front, char *sideA, char *sideB){
    actuator(pinRelayA, pinRelayB, (front == "U")? EXTEND : RETRACT); /*Frontal*/
    actuator(pinRelayC, pinRelayD, (sideA == "U")? EXTEND : RETRACT); /*LateralA*/
    actuator(pinRelayE, pinRelayF, (sideB == "U")? EXTEND : RETRACT); /*LateralB*/
};
/*Movimiento de tres actuadores*/
void directionFunction3(int EXTEND_RETRACT){
  /*extend - left*/ /*retract - rigth*/
  actuator(pinRelayA, pinRelayB, EXTEND);         /*Frontal*/
  actuator(pinRelayD, pinRelayC, EXTEND_RETRACT); /*LateralA*/
  actuator(pinRelayF, pinRelayE, !EXTEND_RETRACT);/*LateralB*/
  delay(delayValue);
}
/*Velocidad de tres actuadores*/
void speedFunction3(int EXTEND_RETRACT){
  actuator(pinRelayA, pinRelayB, EXTEND_RETRACT);         /*Frontal*/
  actuator(pinRelayD, pinRelayC, !EXTEND_RETRACT);        /*LateralA*/
  actuator(pinRelayF, pinRelayE, !EXTEND_RETRACT);        /*LateralB*/
  delay(delayValue);
}

void loop(){
  /*Leer los potenciomentros*/
  speed = map(analogRead(pinPotenciometerS), 0, 1023, 0, 100);
  direction = map(analogRead(pinPotenciometerD), 0, 1023, 0, 100);


  inputString = "STOP ";

  
  //Mandar los valores a unreal
  if(inputString.length()>0){
    /*Pedir Velocidad*/
    if(inputString.substring(1, 7) == "getVel" || inputString.substring(0, 6) == "getVel"){
      Serial.println(speed);
      Serial.println(speedAux);
      /*Velocidad*/
      if((speed >= speedAux - 1) && (speed <= speedAux + 1)){ //Sin velocidad
        actuadorStop(pinRelayA, pinRelayB);
        actuadorStop(pinRelayC, pinRelayD);
        actuadorStop(pinRelayE, pinRelayF);
      }else if(speed < speedAux){                             //Menos velocidad
        speedFunction3(RETRACT);
      }else if(speed > speedAux){                             //Mayor velocidad
        speedFunction3(EXTEND);
      }
      delay(100);
    }
    /*Pedir Dirección*/
    else if(inputString.substring(1, 7) == "getDir" || inputString.substring(0, 6) == "getDir"){
      Serial.println(direction);
      
      if((direction >= directionAux - 1) && (direction <= directionAux + 1)){ //Sin movimiento
        actuadorStop(pinRelayA, pinRelayB);
        actuadorStop(pinRelayC, pinRelayD);
        actuadorStop(pinRelayE, pinRelayF);
      }
     else if(direction < directionAux){                                       /*Movimiento lateral*/
        directionFunction3(RETRACT);
      }
      else if(direction > directionAux){                                      /*Movimiento lateral*/
        directionFunction3(EXTEND);
      }
      delay(100);
    }
    /*Estado del actuador, >:v no está bien*/
    else if(inputString.substring(1, 7) == "getAct" || inputString.substring(0, 6) == "getAct"){
      if(direction < directionAux){
        Serial.println("RETRACT");  
      }else if(direction > directionAux){
        Serial.println("EXTEND");
      }
    }
    /*Ola sin movimiento desde unrela*/
    //Leer valor
    else if(inputString.substring(1, 5) == "get_" || inputString.substring(0, 4) == "get_"){
      char str[50];
      inputString.toCharArray(str, 50);
      
      strtok( str, delimiter );
      front =strtok( NULL, delimiter );
      sideA =strtok( NULL, delimiter );
      sideB =strtok( NULL, delimiter );
      ola(front, sideA, sideB);
      inputString = "";
      delay(100);
    }
    //Terminar el juego y bajar los actuadores
    else if(inputString.substring(1, 5) == "STOP" || inputString.substring(0, 4) == "STOP"){
       actuator(pinRelayA, pinRelayB, RETRACT);
       actuator(pinRelayC, pinRelayD, RETRACT);
       actuator(pinRelayE, pinRelayF, RETRACT);
       delay(4000);
       Serial.println("FIN");
    }
  }
  //Sin mensajes de Unreal
  else{
    Serial.println("Sin menajes");
    actuadorStop(pinRelayA, pinRelayB);
    actuadorStop(pinRelayC, pinRelayD);
    actuadorStop(pinRelayE, pinRelayF);
  }
  inputString = "";
  speedAux = speed;
  directionAux = direction;
}

/*Leer mensaje de Ureal*/
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
