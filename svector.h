#ifndef __SVECTOR__H
#define __SVECTOR__H

#include <math.h>

#include "utils.h"

class Vector{
    public:

        Vector(double n_bering, double n_azimuth){
            bearing = n_bering;
            azimuth = n_azimuth;
            updateComponents();
        }

        Vector(double n_x, double n_y, double n_z){
            set(n_x, n_y, n_z);
        }

        void set(double n_x, double n_y, double n_z){
            x = n_x;
            y = n_y;
            z = n_z;
            updateAngles();
        }

        void set(double be, double az){
            bearing = be;
            azimuth = az;
            updateComponents();
        }

        void normalize() {
            double mag = sqrt( pow(x, 2) + pow(y, 2) + pow(z, 2) );
            x = x / mag;
            y = y / mag;
            z = z / mag;
        }

        double X() {return x; }
        double Y() {return y; }
        double Z() {return z; }
        double Be() { return Utils::rad2deg(bearing); }
        double Az() { return Utils::rad2deg(azimuth); }

    private:
        double x, y, z;
        double bearing, azimuth;

        void updateAngles() {
            azimuth = atan2(z, sqrt( pow(x,2) + pow(y, 2) ));
            bearing = atan2(y, x);
        }

        void updateComponents() {
            x = cos(bearing) * cos(azimuth);
            y = sin(bearing) * cos(azimuth);
            z = sin(azimuth);
            normalize();
        }
};

#endif