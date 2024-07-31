#ifndef __CONTROLLER__H
#define __CONTROLLER__H

#include "axis.h"

class Controller {

public:
    enum State {
        STOPPED,
        HOMING,
        MOVE_TO,
        MOVE_SPEED
    };

    enum Mode {
        PARALLEL,
        PERPENDICULAR
    };

    Controller(Axis* be, Axis* az) {  
        bearing = be;
        azimuth = az;
        bearingTarget = 180;
        azimuthTarget = -45;     
    }

    void run() {
        switch(state){
            case MOVE_TO:
                setTargets();
                bearing->run();
                azimuth->run();
                break;

            default:
                break;
        }
    }

    double getBearingTarget() { return bearingTarget; }
    double getAzimuthTarget() { return azimuthTarget; }

    void doPrints() {
        Serial.print("target bearing: ");
        Serial.print(bearingTarget);
    }

private:
    Controller() {}

    Axis *azimuth, *bearing;
    double bearingTarget, azimuthTarget;

    State state = MOVE_TO;
    Mode mode;

    void setTargets() {
        if(bearing->getError() == 0){
            if (bearingTarget == 90){
                bearingTarget = 270;
            }
            else {
                bearingTarget = 90;
            }
        }

        bearing->driveToPos(bearingTarget);
    }
};

#endif