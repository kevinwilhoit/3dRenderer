#ifndef PointLight_H
#define PointLight_H

#include "Vector.h"
#include "Color.h"
#include "Sphere.h"
#include "Light.h"
#include "Face.h"
#include "Material.h"
#include "Collision.h"

using namespace std;

class PointLight:public Light {
  public:
    PointLight(Vector3d, Color);
    Color shading(Vector3d, Vector3d, Sphere);
		Color shading(Vector3d, Collision);
};

#endif