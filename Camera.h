#ifndef Camera_H
#define Camera_H

#include "Vector.h"

using namespace std;

class Camera {
  public:
    Vector3d viewpoint;
    Vector3d viewdir;
    Vector3d vup;
    double flength;
    double aratio;
    double swidth;
    Camera(Vector3d, Vector3d, Vector3d, double, double, double);
    Camera();
};

#endif