#ifndef __CONTROLLER__H
#define __CONTROLLER__H

#include "axis.h"
#include "command.h"
#include "svector.h"
#include "star_follower.h"

#define CMD_LENGTH 20

class Controller {

public:
    enum State {
        STOPPED,
        HOMING,
        MOVE_TO,
        MOVE_SPEED,
        FIX
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
        bearingSpeed = 0;
        azimuthSpeed = 0; 
        enablePin = n_enablePin;

        starFollower = StarFollower();

        state = STOPPED;
        pinMode(enablePin, OUTPUT); 
        enabled = true;
        setEnable(false);

        testMove();
    }

    void run() {

        parseCommand();

        switch(state){
            case STOPPED:
                setEnable(false);
                break;
            case MOVE_TO:
                setEnable(true);
                bearing->driveToPos(bearingTarget);
                azimuth->driveToPos(azimuthTarget);
                break;
            case MOVE_SPEED:
                setEnable(true);
                bearing->driveSpeed(bearingSpeed);
                azimuth->driveSpeed(azimuthSpeed);
                break;
            case FIX:
                setEnable(true);
                starFollower.run(bearingTarget, azimuthTarget);
                bearing->driveToPos(bearingTarget);
                azimuth->driveToPos(azimuthTarget);
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

    void testMove() {
        starFollower.setPole(0, 40, true);
        state = FIX;
        starFollower.initialize(50, 25);
    }

private:
    Controller() {}

    Axis *azimuth, *bearing;
    double bearingTarget, azimuthTarget;
    double bearingSpeed, azimuthSpeed;

    byte enablePin;
    bool enabled;

    String msg[CMD_LENGTH];
    bool newMsg = false;

    State state;
    Mode mode;

    StarFollower starFollower;

    void setTargets() {
        
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
            else if (msg[0] == "SET_POLE"){
                handle_set_pole();
            }
            else if (msg[0] == "SPEED"){
                handle_speed();
            }
            else if (msg[0] == "JOG"){
                handle_jog();
            }
            else if (msg[0] == "POS"){
                char out_msg[30], cBe[7], cAz[7];
                dtostrf(bearing->getPos(), 6, 2, cBe);
                dtostrf(azimuth->getPos(), 6, 2, cAz);
                sprintf(out_msg,"{\"Be\":%s,\"Az\":%s}", cBe, cAz);
                Serial.print(out_msg);
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

    void handle_speed(){
        //Format: SPEED,bearing,azimuth
        // 0: SPEED
        // 1: bearing, double for angular speed
        // 2: azimuth, double for angular speed

        state = MOVE_SPEED;
        bearingSpeed = msg[1].toDouble();
        azimuthSpeed = msg[2].toDouble();
    }

    void handle_stop(){
        //Format: STOP
        // 0: STOP

        state = STOPPED;
    }

    void handle_track(){
        
    }

    void handle_set_pole(){
        bool inverted;
        if(msg[1] == "S") inverted = false;
        else inverted = true;

        starFollower.setPole(bearing->getPos(), azimuth->getPos(), inverted);

        // Serial.print("Pole set at ");
        // Serial.print(bearing->getPos());
        // Serial.print(", ");
        // Serial.println(azimuth->getPos());
    }

    void handle_fix(){
        state = FIX;
        starFollower.initialize(bearing->getPos(), azimuth->getPos());
    }

    void handle_jog(){
        double be_inc = msg[1].toDouble();
        double az_inc = msg[2].toDouble();

        switch (state){
        case FIX:
            starFollower.initialize(bearing->getPos() + be_inc, azimuth->getPos() + az_inc);
            break;
        default:
            bearingTarget += be_inc;
            azimuthTarget += az_inc;
            break;
        }
    }

    
};

#endif