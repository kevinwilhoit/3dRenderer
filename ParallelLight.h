#ifndef ParallelLight_H
#define ParallelLight_H

#include "Vector.h"
#include "Color.h"
#include "Sphere.h"
#include "Light.h"
#include "Face.h"
#include "Material.h"
#include "Collision.h"

using namespace std;

class ParallelLight: public Light {
  public:
    ParallelLight(Vector3d, Color);
    Color shading(Vector3d, Vector3d, Sphere);
    Color shading(Vector3d, Collision);
		//Color shading(Vector3d, Vector3d, Face, Vector3d*, Material);
};

#endif