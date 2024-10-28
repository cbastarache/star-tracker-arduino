#include "axis.h"
#include "controller.h"

#define BEARING_GR (130.0 / 30.0)
#define AZIMUTH_GR (100.0 / 30.0)

byte enablePin = 8;
int numberOfSteps = 3202; // 1.8 deg steps, 1/16th step

Axis bearing = Axis(numberOfSteps, BEARING_GR, 2, 5);
Axis azimuth = Axis(numberOfSteps, AZIMUTH_GR, 3, 6);

Controller controller = Controller(&bearing, &azimuth, enablePin);

int printStep = 0;

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
  bearing.setLimits(0, 360, 10000, 5000, true);
  azimuth.setLimits(-70, 70, 10000, 5000, false);
  
}

void loop() {

  controller.run();
  // doPrints();

}
