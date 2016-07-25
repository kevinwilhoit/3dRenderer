#ifndef Sphere_H
#define Sphere_H

#include "Vector.h"
#include "Material.h"

using namespace std;

class Sphere {
  public:
    Vector3d center;
    double radius;
    Material material;
    
    Sphere(double, double, double, double, Material);
    double hit(Vector3d ur, Vector3d pr);
};

#endif