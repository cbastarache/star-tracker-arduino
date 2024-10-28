#ifndef __CONTROLLER__H
#define __CONTROLLER__H

#include "axis.h"
#include "svector.h"
#include "star_follower.h"

#define CMD_LENGTH 20
#define DELIMITER ','
#define EOL '\n'

class Controller {

public:
    enum State {
        STOPPED,
        HOMING,
        MOVE_TO,
        MOVE_SPEED,
        FIX,
        TRACK
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
        mode = PARALLEL;

        starFollower = StarFollower();

        state = STOPPED;
        pinMode(enablePin, OUTPUT); 
        enabled = true;
        setEnable(false);

        // testMove();
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
            case TRACK:
                setEnable(true);

                if( checklimits(bearing->getPos(), be1, be2, true) ){
                    bearing->driveSpeed(bearingSpeed);
                } else {
                    // bearing->driveToPos(closerLimit(bearing->getPos(), be1, be2, true));
                    bearing->driveToPos(be2);
                }

                if( checklimits(azimuth->getPos(), az1, az2, false) ){
                    azimuth->driveSpeed(azimuthSpeed);
                } else {
                    // azimuth->driveToPos(closerLimit(azimuth->getPos(), az1, az2, false));
                    azimuth->driveToPos(az2);
                }

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

    //stream variables
    double be1, be2, az1, az2;
    double streamRate;

    byte enablePin;
    bool enabled;

    String tmp_msg[CMD_LENGTH];
    String msg[CMD_LENGTH];
    bool newMsg = false;
    int msg_index = 0;

    State state;
    Mode mode;

    StarFollower starFollower;

    void setEnable(bool en){
        if (en != enabled){
            digitalWrite(enablePin, enabled);
            enabled = en;
        }
    }

    bool checklimits(double x, double l1, double l2, bool continuous){
        if (continuous){
            if ( abs(l2 - l1) > 180 ){
                return x < min(l1, l2) || x > max(l1,l2);
            }
        } 
        
        return x > min(l1, l2) && x < max(l1, l2) ;
    }

    double closerLimit(double pos, double l1, double l2, bool continuous){
        double d1, d2, tmp;

        if (continuous){
            tmp = l1 - pos + 540;
            d1 = ( (int)tmp % 360 - 180) + (tmp - (int)tmp);

            tmp = l2 - pos + 540;
            d2 = ( (int)tmp % 360 - 180) + (tmp - (int)tmp);
        } else {
            d1 = l1 - pos;
            d2 = l2 - pos;
        }

        return d1 < d2 ? l1 : l2;
    }

    void parseCommand(){
        char read_char;
        
        if (Serial.available()) {
            while( Serial.available() ) { 
                read_char = (char)Serial.read();
                if(read_char == DELIMITER){

                    msg_index ++;

                } else if (read_char == EOL) {

                    for (int i = 0; i < CMD_LENGTH; i++){
                        msg[i] = tmp_msg[i]; 
                        tmp_msg[i] = "";
                    }
                    newMsg = true;
                    msg_index = 0;
                } 
                else {
                    tmp_msg[msg_index] += read_char;
                }
            }
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
            else if (msg[0] == "ZERO"){
                handle_zero();
            }
            else if (msg[0] == "STREAM"){
                handle_stream();
            }
            else if (msg[0] == "POS"){
                char out_msg[30], cBe[7], cAz[7];
                dtostrf(bearing->getPos(), 6, 2, cBe);
                dtostrf(azimuth->getPos(), 6, 2, cAz);
                sprintf(out_msg,"{\"Be\":%s,\"Az\":%s}", cBe, cAz);
                Serial.print(out_msg);
            }
            else if (msg[0] == "MODE"){
                int tmp = msg[1].toInt();
                switch (tmp)
                {
                case 0:
                    mode = PARALLEL;
                    azimuth->applyOffset(0);
                    break;
                default:
                    mode = PERPENDICULAR;
                    azimuth->applyOffset(-90);
                    break;
                }
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

    void handle_zero(){

        if (msg[1] == "BE"){
            bearing->setPos(0);
        } else if (msg[1] == "AZ") {
            azimuth->setPos(0);
        }

    }

    void handle_stream(){
        // Move between two positions, interpolating speed to make the move
        //Format: STREAM,bearing1,azimuth1,bearing2,azimuth2,time
        // 0: STREAM
        // 1: bearing1, double for angular position
        // 2: azimuth1, double for angular position
        // 3: bearing2, double for angular position
        // 4: azimuth2, double for angular position
        // 5: time (s), double time delta for the move
        be1 = msg[1].toDouble();
        az1 = msg[2].toDouble();
        be2 = msg[3].toDouble();
        az2 = msg[4].toDouble();
        streamRate = msg[5].toDouble();

        bearingTarget = be1;
        azimuthTarget = az1;

        double tmp = be2 - be1 + 540;
        double distance = ( (int)tmp % 360 - 180) + (tmp - (int)tmp);
        bearingSpeed = (distance) / streamRate;

        azimuthSpeed = (az2 - az1) / streamRate;
        state = TRACK;

        // Serial.println("stream calcs-----");
        // Serial.println(bearingTarget);
        // Serial.println(azimuthTarget);
        // Serial.println(bearingSpeed);
        // Serial.println(azimuthSpeed);
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