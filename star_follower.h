#ifndef __STAR_FOLLOWER_H
#define __STAR_FOLLOWER_H

#include "svector.h"

// earths rotation in deg/s
// 1 rotation is 23h 56m 4s
// #define D_THETA (360.0/86164.0)

// test value, rotate 360 in 10 seconds
#define D_THETA (360.0/5.0)

class StarFollower{

public:
    StarFollower(){
        theta = 0;
        polaris = new Vector(0,0,0);
        c = new Vector(0,0,0);
        a = new Vector(0,0,0);
        b = new Vector(0,0,0);
    }

    void setPole(double be, double az){
        polaris->set(Utils::deg2rad(be), Utils::deg2rad(az));
    }

    void initialize(double be, double az) {
        Vector v = Vector(Utils::deg2rad(be), Utils::deg2rad(az));
        theta = 0;
        lastTime = millis();

        d = -( polaris->X()*v.X() + polaris->Y()*v.Y() + polaris->Z()*v.Z() );

        c->set( -d * polaris->X(), 
                -d * polaris->Y(), 
                -d * polaris->Z() );

        r = sqrt( pow(c->X() - v.X(), 2) +  pow(c->Y() - v.Y(), 2) + pow(c->Z() - v.Z(), 2) );

        a->set( v.X() - c->X(),
                v.Y() - c->Y(),
                v.Z() - c->Z());
        a->normalize();

        b->set( a->Y()*polaris->Z() - polaris->Y()*a->Z(),
                a->X()*polaris->Z() - polaris->X()*a->Z(),
                a->X()*polaris->Y() - polaris->X()*a->Y() );
        b->normalize();

        // Serial.print("polaris: ");
        // Serial.print(polaris->X());
        // Serial.print(", ");
        // Serial.print(polaris->Y());
        // Serial.print(", ");
        // Serial.println(polaris->Z());

        // Serial.print("v: ");
        // Serial.print(v.X());
        // Serial.print(", ");
        // Serial.print(v.Y());
        // Serial.print(", ");
        // Serial.println(v.Z());

        // Serial.print("r: ");
        // Serial.println(r);

        // Serial.print("d: ");
        // Serial.println(d);

        // Serial.print("a: ");
        // Serial.print(a->X());
        // Serial.print(", ");
        // Serial.print(a->Y());
        // Serial.print(", ");
        // Serial.println(a->Z());

        // Serial.print("b: ");
        // Serial.print(b->X());
        // Serial.print(", ");
        // Serial.print(b->Y());
        // Serial.print(", ");
        // Serial.println(b->Z());

        // Serial.print("c: ");
        // Serial.print(c->X());
        // Serial.print(", ");
        // Serial.print(c->Y());
        // Serial.print(", ");
        // Serial.println(c->Z());
    }

    void run(double &bearing, double &azimuth){
        long time = millis();
        theta += (time - lastTime) * Utils::deg2rad(D_THETA) / 1000.0;

        double x = r * ( cos(theta) * a->X() + sin(theta) * b->X() ) + c->X();
        double y = r * ( cos(theta) * a->Y() + sin(theta) * b->Y() ) + c->Y();
        double z = r * ( cos(theta) * a->Z() + sin(theta) * b->Z() ) + c->Z();

        Utils::comp2sphere(x, y, z, bearing, azimuth);
        bearing = Utils::rad2deg(bearing);
        azimuth = Utils::rad2deg(azimuth);

        // Serial.println(theta);
        lastTime = time;
    }

private:
    Vector *polaris;
    Vector *star;
    Vector *c, *a, *b;

    double theta;
    double d, r;
    long lastTime;

};

#endif