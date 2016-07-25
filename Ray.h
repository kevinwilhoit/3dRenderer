/*
 *  Ray.h
 *  
 *
 *  Created by Donald House on 1/24/14.
 *  Copyright 2014 Clemson University. All rights reserved.
 *
 */

#ifndef _H_RAY_
#define _H_RAY_

#include "Matrix.h"

class Ray{
public:				// public for fast access to ray parameters
  Vector3d p;		// ray start
  Vector3d u;		// ray direction. Note, danger. u should never
  Vector3d inv_u;	// be set directly since inv_u and sign need to
  int sign[3];		// be updated to match u

  Ray();
  Ray(const Vector3d &rayp, const Vector3d &raydir);
  
  void Set(const Vector3d &rayp, const Vector3d &raydir);
  void SetStart(const Vector3d &rayp);
  void SetDir(const Vector3d &raydir);
  
  Vector3d Start() const{
	return p;
  }
  Vector3d Dir() const{
	return u;
  }
  
  Vector3d X(float t) const{
	return p + t * u;
  }

  friend ostream& operator<<(ostream& os, const Ray& r){
	os << "{" << r.p << ", " << r.u << "}";
	return os;
  }
/*
  friend Ray operator*(const Matrix4x4 &M, const Ray &r);
	*/
};
#endif
