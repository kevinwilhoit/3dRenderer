#ifndef Light_H
#define Light_H

#include "Vector.h"
#include "Color.h"
#include "Sphere.h"
#include "Face.h"
#include "Material.h"
#include "Collision.h"

using namespace std;

class Light {
  public:
    Vector3d center;
    Vector3d ul;
    Color color;
    Light(Vector3d, Vector3d, Color);
    static Color shade(int, Vector3d, Vector3d, int, Sphere**, Light**);
    virtual Color shading(Vector3d, Vector3d, Sphere) = 0;
		virtual Color shading(Vector3d, Collision) = 0;
};

#endif