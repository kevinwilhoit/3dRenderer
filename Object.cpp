/*
 *  Object.cpp
 *  
 *
 *  Created by Donald House on 1/24/14.
 *  Copyright 2014 Clemson University. All rights reserved.
 *
 */

#include "Object.h"

int Object::objcount = 0;

Collision Object::BBoxCollide(const Ray &r) const{
  float tmin, tmax;
  Collision c;
  //Ray rprime = invxform * r;

  c.n = bbox.RayCollide(r, tmin, tmax);
  c.t = tmin;
  c.x = r.X(tmin);
  
  //c.x = xform * Vector4d(c.x, 1.0);
  //c.n = xform * Vector4d(c.n, 0.0);
  
  c.objectid = objid;

  return c;
}
