/*
 *  AABBox.h
 *  
 *
 *  Created by Donald House on 1/24/14.
 *  Copyright 2014 Clemson University. All rights reserved.
 *
 */

#ifndef _H_AABBOX_
#define _H_AABBOX_

#include "Vector.h"
#include "Collision.h"
#include "Ray.h"

struct AABBox{
  static Material *defaultmaterial;
  static double ndir[2];

  Vector3d bounds[2];	// bounds[0] is min, bounds [1] is max

  AABBox(){
	bounds[0] = Vector3d(INFINITY, INFINITY, INFINITY);
	bounds[1] = Vector3d(-INFINITY, -INFINITY, -INFINITY);
  }
  
  AABBox(const AABBox &bb);
  
  void AddBBox(const AABBox &newbox);

  Vector3d RayCollide(const Ray &r, float &tmin, float &tmax) const;
  
  const AABBox& operator=(const AABBox& bb);	// assignment

  friend ostream& operator<<(ostream& os, const AABBox& b);
};

#endif
