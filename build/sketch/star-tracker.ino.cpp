#include <Arduino.h>
#line 1 "C:\\Users\\corey\\Documents\\star-tracker\\star-tracker.ino"
#include "axis.h"
#include "controller.h"

#define BEARING_GR (130.0 / 30.0)
#define AZIMUTH_GR (100.0 / 30.0)

byte enablePin = 8;
int numberOfSteps = 3202; // 1.8 deg steps, 1/16th step

Axis bearing = Axis(numberOfSteps, BEARING_GR, 2, 5);
Axis azimuth = Axis(numberOfSteps, AZIMUTH_GR, 3, 6);

Controller controller = Controller(&bearing, &azimuth);

int printStep = 0;

#line 17 "C:\\Users\\corey\\Documents\\star-tracker\\star-tracker.ino"
void doPrints();
#line 31 "C:\\Users\\corey\\Documents\\star-tracker\\star-tracker.ino"
void setup();
#line 48 "C:\\Users\\corey\\Documents\\star-tracker\\star-tracker.ino"
void loop();
#line 17 "C:\\Users\\corey\\Documents\\star-tracker\\star-tracker.ino"
void doPrints(){
  printStep++;
  if(printStep >= 500){

    controller.doPrints();
    bearing.doPrints();
    azimuth.doPrints();

    Serial.println();

    printStep = 0;
  }
}

void setup() { 

  Serial.begin(9600);
  Serial.println("Starting StepperTest");

  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);
  delay(1000);

  bearing.setLimits(0, 360, 10000, 5000, true);
  azimuth.setLimits(-90, 90, 10000, 5000, false);
  
  delay(1000);
  digitalWrite(enablePin, LOW);

}

void loop() {

  controller.run();
  doPrints();

}

