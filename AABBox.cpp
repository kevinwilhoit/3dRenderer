/*
 *  AABBox.cpp
 *  
 *
 *  Created by Donald House on 1/24/14.
 *  Copyright 2014 Clemson University. All rights reserved.
 *
 */

#include "AABBox.h"

Material* AABBox::defaultmaterial = new Material(Color(0.2, 0.2, 0.2), Color(0.8, 0.8, 0.8), Color(0.0, 0.0, 0.0), 1.0);

double AABBox::ndir[2] = {-1.0, 1.0};

AABBox::AABBox(const AABBox &bb){
  bounds[0] = bb.bounds[0];
  bounds[1] = bb.bounds[1];
}

void AABBox::AddBBox(const AABBox &newbox){
  for(int i = 0; i < 3; i++){
	if(bounds[0][i] > newbox.bounds[0][i])
	  bounds[0][i] = newbox.bounds[0][i];
	if(bounds[1][i] < newbox.bounds[1][i])
	  bounds[1][i] = newbox.bounds[1][i];
  }
}

//
// Find ray collision with bounding box. Return normal of hit plane.
// tmin and tmax set to ray parameter on entry and exit
//
Vector3d AABBox::RayCollide(const Ray &r, float &tmin, float &tmax) const{
  Vector3d n, ny, nz;
  float tymin, tzmin;
  float tymax, tzmax;

  // find entry and exit ray distances for x planes
  tmin = (bounds[r.sign[0]].x - r.p.x) * r.inv_u.x;		 // entry on left side
  tmax = (bounds[1 - r.sign[0]].x - r.p.x) * r.inv_u.x;  // exit on right side
  n = Vector3d(ndir[r.sign[0]], 0, 0);

  // find entry and exit ray distances for y planes
  tymin = (bounds[r.sign[1]].y - r.p.y) * r.inv_u.y;	 // entry on bottom side
  tymax = (bounds[1 - r.sign[1]].y - r.p.y) * r.inv_u.y; // exit on top side
  ny = Vector3d(0, ndir[r.sign[1]], 0);

  // if x entry point beyond y exit point 
  // or y entry point beyond x exit point, 
  // there is no collision with the bounding box
  if((tmin > tymax) || (tymin > tmax)){
	tmin = tmax = INFINITY;
	return Vector3d(0, 0, 0);
  }
  
  // keep track of farthest entry point
  if(tymin > tmin){
	tmin = tymin;
	n = ny;
  }
  // keep track of nearest exit point
  if(tymax < tmax)
	tmax = tymax;
  
  // find entry and exit ray distances for z planes
  tzmin = (bounds[r.sign[2]].z - r.p.z) * r.inv_u.z;	  // entry on back side
  tzmax = (bounds[1 - r.sign[2]].z - r.p.z) * r.inv_u.z;  // exit on front side
  nz = Vector3d(0, 0, ndir[r.sign[2]]);

  // if nearest entry point beyond z exit point 
  // or z entry point beyond nearest exit point, 
  // there is no collision with the bounding box
  if((tmin > tzmax) || (tzmin > tmax)){
	tmin = tmax = INFINITY;
	return Vector3d(0, 0, 0);
  }
  
  // keep track of farthest entry point
  if(tzmin > tmin){
	tmin = tzmin;
	n = nz;
  }
  // keep track of nearest exit point
  if(tzmax < tmax)
	tmax = tzmax;

  return n;
}

const AABBox& AABBox::operator=(const AABBox& bb){	// assignment
  bounds[0] = bb.bounds[0];
  bounds[1] = bb.bounds[1];

  return *this;
}

ostream& operator<<(ostream& os, const AABBox& b){
  os << "{" << b.bounds[0] << ", " << b.bounds[1] << "}";
  return os;
}
