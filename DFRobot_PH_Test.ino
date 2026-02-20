#include "DFRobot_PH.h"
#include <EEPROM.h>

#define BAUD_RATE 115200
#define PH_PIN A1
#define FULL_MOVEMENT_TIME 3600
#define SMALL_BURST_MOVEMENT_TIME 1000


const int dirPin = 2;
const int stepPin = 3;
int mem = -1;
volatile bool stopRequested = false; 

float voltage,phValue,temperature = 25;
DFRobot_PH ph;

void setup() {
  Serial.begin(BAUD_RATE);  
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  Serial.begin(115200);  
  ph.begin();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    processCommand(input);
  }
  
}


void moveUp(bool smallStepOnly = false) {
  if (mem == 0) return; 

  int spinTime = FULL_MOVEMENT_TIME;

  digitalWrite(dirPin, HIGH);
  unsigned long startTime = millis();
  mem = 0;
  stopRequested = false; 

  if(smallStepOnly){
    spinTime = SMALL_BURST_MOVEMENT_TIME;
    mem = -1;
  }

  Serial.println("Moving Up to the top...");

  while (millis() - startTime < spinTime) {
    if (stopRequested) {        
      Serial.println("PANIC STOP triggered!");
      break;
    }

    digitalWrite(stepPin, HIGH);
    delayMicroseconds(350);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(350);

    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      processCommand(input);
    }
  }

  Serial.println("Done Moving Up");
}

void moveDown(bool smallStepOnly = false) {
  if (mem == 1) return; 

  int spinTime = FULL_MOVEMENT_TIME;

  digitalWrite(dirPin, LOW);
  unsigned long startTime = millis();
  mem = 1;
  stopRequested = false;

  if(smallStepOnly){
    spinTime = SMALL_BURST_MOVEMENT_TIME;
    mem = -1;
  }

  Serial.println("Moving Down to the bottom...");

  while (millis() - startTime < spinTime) {
    if (stopRequested) {
      Serial.println("PANIC STOP triggered!");
      break;
    }

    digitalWrite(stepPin, HIGH);
    delayMicroseconds(350);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(350);

    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      processCommand(input);
    }
  }

  Serial.println("Done Moving Down");
}


void getPh() {
  voltage = analogRead(PH_PIN)/1024.0*5000;  
  phValue = ph.readPH(voltage,temperature);  
  Serial.print("pH:");
  Serial.println(phValue,2);

  ph.calibration(voltage,temperature);           
}

void panicStop() {
  stopRequested = true;
  Serial.println("Stop request received!");
}


void processCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "U") {
    moveUp();
    return;
  }
  if (cmd == "W") {
    moveUp(true);
    return;
  }

  if (cmd == "D") {
    moveDown();
    return;
  }

  if (cmd == "X") {
    moveDown(true);
    return;
  }

  if (cmd == "V") {
    getPh();
    return;
  }

  if (cmd == "S") {
    panicStop();
    return;
  }

  panicStop();

  Serial.println("{\"status\":\"error\",\"message\":\"Unknown command\"}");  
}