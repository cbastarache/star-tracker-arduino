#ifndef __AXIS__H
#define __AXIS__H

#include <AccelStepper.h>

class Axis {

public:
    enum Direction { FWD = HIGH, REV = LOW };

    Axis(int n_motorSteps, double n_gearRatio, byte n_pulsePin, byte n_directionPin){
        motorSteps = n_motorSteps;
        gearRatio = n_gearRatio;
        pulsePin = n_pulsePin;
        directionPin = n_directionPin;

        stepSize = 360.f / (motorSteps * gearRatio);

        stepper = AccelStepper(AccelStepper::DRIVER, pulsePin, directionPin);
        stepper.setAcceleration(2000);
    }

    void setLimits(double n_min, double n_max, float n_maxSpeed, float n_accel, bool n_continuous){
        maxLimit = n_max;
        minLimit = n_min;
        continuous = n_continuous;
        configured = true;
        setSpeed(n_maxSpeed);
        stepper.setAcceleration(n_accel);
    }

    void setSpeed(int s) {
        stepper.setMaxSpeed(s);
    }

    void driveToPos(double pos){
        double distance;
        double tmp;

        if (continuous) {
            normalize(pos);
            tmp = pos - position + 540;
            distance = ( (int)tmp % 360 - 180) + (tmp - (int)tmp);
        }
        else {
            checkLimits(pos);
            distance = (pos - position);
        }

        int steps = distance / stepSize;
        stepper.moveTo(stepper.currentPosition() + steps);
        stepper.run();

        position = stepper.currentPosition() * stepSize;
        
        if(continuous){
            normalize(position);
        }
    }

    void driveSpeed(double deg_s){
        double step_s = deg_s / stepSize; // deg/sec * step/deg = step/sec

        position = stepper.currentPosition() * stepSize;

        if (!continuous){
            if (position <= maxLimit && position >= minLimit ){
                stepper.setSpeed(step_s);
            } else {
                stepper.setSpeed(0.0);
            }
        } else {
            stepper.setSpeed(step_s);
        }
        
        stepper.runSpeed();
    }

    void home() {
        setPos(0);
    }

    void setPos(double pos){
        if (continuous) {
            normalize(pos);
        }
        stepper.setCurrentPosition(pos / stepSize);
    }

    double getError() { return abs(stepper.distanceToGo()) * stepSize; }
    double getPos() { return position; }
    AccelStepper* getStepper() { return &stepper; }

    void doPrints() {
        Serial.print("\taxis: ");
        Serial.print(position);
    }

private:
    int motorSteps;
    double gearRatio;
    byte pulsePin;
    byte directionPin;

    double stepSize;
    double maxLimit;
    double minLimit;
    bool continuous = false; 
    bool configured = false;

    double position = 0.f;
    AccelStepper stepper;

    int signOf(double x) {
        if ( x >= 0 ) 
            return 1;
        return -1;
    }

    Direction getDir(double x){
        if ( x >= 0 ) 
            return Direction::FWD;
        return Direction::REV;
    }

    void normalize(double &x){
        while( x < 0 ){
            x += 360.f;
        }
        x = ((int)x % 360 ) + (x - (int)x);
    }

    void checkLimits(double &x){
        if (x < minLimit)
            x = minLimit;
        else if (x > maxLimit)
            x = maxLimit;
    }

};

#endif