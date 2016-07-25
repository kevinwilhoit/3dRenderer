/*
 *  Collision.h
 *  
 *
 *  Created by Donald House on 1/24/14.
 *  Copyright 2014 Clemson University. All rights reserved.
 *
 */

#ifndef _H_COLLISION_
#define _H_COLLISION_

#include "Vector.h"
#include "Material.h"

//
// Collision object holds id of object hit, distance along ray, position of hit,
// surface normal at hit, texture coordinates, and material
//
struct Collision{
  int objectid;
  double t;
  Vector3d x;
  Vector2d uv;
  Vector3d n;
  Material *m;
  
  Collision(): objectid(-1), t(INFINITY), m(NULL)
	{}
  
  friend ostream& operator<<(ostream& os, const Collision& c){
	os << "[collision: " << c.objectid << ", " << c.t << ", " << c.x << ", " << c.uv << ", " << c.n << "]";
	return os;
  }
};

#endif