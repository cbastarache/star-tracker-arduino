#ifndef __UTILS_H
#define __UTILS_H

class Utils {
    
public:
    static constexpr double pi = 3.141592653589793;
    static double rad2deg(double x) { return x * 180.0 / pi; }
    static double deg2rad(double x) { return x * pi / 180.0; }
    static void comp2sphere(double x, double y, double z, double &be, double &az) {
        be = rad2deg(atan2(y, x));
        az = rad2deg(atan2(z, sqrt(pow(x, 2) + pow(y, 2))));
    } 
    static void sphere2comp(double be, double az, double &x, double &y, double &z) {
        x = cos(be) * cos(az);
        y = sin(be) * cos(az);
        z = sin(az);
    } 
};

#endif