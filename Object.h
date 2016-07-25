/*
 *  Object.h
 *  
 *
 *  Created by House on 1/24/14.
 *  Copyright 2014 Clemson University. All rights reserved.
 *
 */

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "AABBox.h"
#include "Collision.h"
#include "Ray.h"
#include "Material.h"
#include "Matrix.h"

//
// Basic structure for storing the information about an object
//
class Object{
protected:
  static int objcount;
  int objid;
  AABBox bbox;
  Matrix4x4 xform;
  Matrix4x4 invxform;
  Material *mat;
  
  Object(): mat(NULL){
	objid = ++objcount;
	xform.identity();
	invxform.identity();
  }

public:
  int GetID() const{
	return objid;
  }

  void SetMaterial(Material *m){
	mat = m;
  }
  Material *GetMaterial() const{
	return mat;
  }
  
  void SetTransform(const Matrix4x4 &tform){
	xform = tform;
	invxform = xform.inv();
  }
  Matrix4x4 GetTransform() const{
	return xform;
  }

  AABBox GetBBox() const{
	return bbox;
  }
  
  Collision BBoxCollide(const Ray &r) const;

  virtual Collision RayCollide(const Ray &r) const{
	return BBoxCollide(r);
  }
};

typedef Object* Objptr;

#endif
