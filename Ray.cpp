/*
 *  Ray.cpp
 *  
 *
 *  Created by Donald House on 1/24/14.
 *  Copyright 2014 Clemson University. All rights reserved.
 *
 */

#include "Ray.h"
  
Ray::Ray(){
  for(int i = 0; i < 3; i++)
	sign[i] = 0;
}

Ray::Ray(const Vector3d &rayp, const Vector3d &raydir){
  Set(rayp, raydir);
}

void Ray::Set(const Vector3d &rayp, const Vector3d &raydir){
  SetStart(rayp);
  SetDir(raydir);
}

void Ray::SetStart(const Vector3d &rayp){
  p = rayp;
}

void Ray::SetDir(const Vector3d &raydir){
  u = raydir;
  inv_u = Vector3d(1.0 / u.x, 1.0 / u.y, 1.0 / u.z);
  for(int i = 0; i < 3; i++)
	sign[i] = u[i] < 0;  
}

/*
Ray operator*(const Matrix4x4 &M, const Ray &r){
  Ray rprime;
  Vector4d ph(r.p.x, r.p.y, r.p.z, 1.0);
  Vector4d uh(r.u.x, r.u.y, r.u.z, 0.0);

  ph = M * ph;
  uh = M * uh;

  rprime.Set(Vector3d(ph.x, ph.y, ph.z), Vector3d(uh.x, ph.y, ph.z));

  return rprime;
}
*/