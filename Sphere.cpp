/* 
Nicholas Bodtorf
CpSc 405 HW3

Sphere class with relevant attributes and a hit function
*/

#include "Sphere.h"

using namespace std;

Sphere::Sphere(double x, double y, double z, double radius2, Material material2) :
  radius(radius2), material(material2) {
  center = Vector3d(x, y, z);
}

//Determines if the sphere is hit by the ray
double Sphere::hit(Vector3d ur, Vector3d pr) {
  Vector3d c = center;
  Vector3d cp = (c-pr);
  
  double tc = ur*cp;
  Vector3d xc = pr+(tc*ur);
  Vector3d d = xc - c;
  double dist = d.norm();
  if(dist <= radius) {
    double a = sqrt((radius*radius)-(dist*dist));
    double xdist = tc-a;
    return xdist;
  }
  return -1;
}
