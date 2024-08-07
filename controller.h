#ifndef __CONTROLLER__H
#define __CONTROLLER__H

#include "axis.h"
#include "command.h"

#define CMD_LENGTH 20

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

    Controller(Axis* be, Axis* az, byte n_enablePin) {  
        bearing = be;
        azimuth = az;
        bearingTarget = 0;
        azimuthTarget = 0;    
        enablePin = n_enablePin;

        state = STOPPED;
        pinMode(enablePin, OUTPUT); 
        enabled = true;
        setEnable(false);
    }

    void run() {

        parseCommand();

        switch(state){
            case STOPPED:
                setEnable(false);
                break;
            case MOVE_TO:
                setEnable(true);
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
        
        Serial.print("\tlast cmd: ");
        Serial.print(msg[0]);
    }

private:
    Controller() {}

    Axis *azimuth, *bearing;
    double bearingTarget, azimuthTarget;

    byte enablePin;
    bool enabled;

    String msg[CMD_LENGTH];
    bool newMsg = false;

    State state;
    Mode mode;

    void setTargets() {
        bearing->driveToPos(bearingTarget);
        azimuth->driveToPos(azimuthTarget);
    }

    void setEnable(bool en){
        if (en != enabled){
            digitalWrite(enablePin, enabled);
            enabled = en;
        }
    }

    void parseCommand(){
        String tmp[CMD_LENGTH];
        char delimiter = ',';
        int index = 0;
        char read_char;
        
        if (Serial.available()) {
            delay(50);
            while( Serial.available() ) { 
                read_char = (char)Serial.read();
                if(read_char == delimiter){
                    index ++;
                } else {
                    tmp[index] += read_char;
                }
            }

            for ( int i = 0; i < CMD_LENGTH; i++){
                msg[i] = tmp[i]; 
            }

            newMsg = true;
        }

        if (newMsg){
            if (msg[0] == "MOVE"){
                handle_move();
            }
            else if (msg[0] == "STOP"){
                handle_stop();
            }
            else if (msg[0] == "TRACK"){
                handle_track();
            }
            else if (msg[0] == "FIX"){
                handle_fix();
            }

            newMsg = false;
        }
    }

    void handle_move(){
        //Format: MOVE,bearing,azimuth
        // 0: MOVE
        // 1: bearing, double for target bearing angle
        // 2: azimuth, double for target azimuth angle

        state = MOVE_TO;
        bearingTarget = msg[1].toDouble();
        azimuthTarget = msg[2].toDouble();
    }

    void handle_stop(){
        //Format: STOP
        // 0: STOP

        state = STOPPED;
    }

    void handle_track(){

    }

    void handle_fix(){

    }

    void testMove() {
        if(bearing->getError() == 0){
            if (bearingTarget == 90){
                bearingTarget = 270;
            }
            else {
                bearingTarget = 90;
            }
        }

        if(azimuth->getError() == 0){
            if (azimuthTarget == 45){
                azimuthTarget = -45;
            }
            else {
                azimuthTarget = 45;
            }
        }

    }
};

#endif