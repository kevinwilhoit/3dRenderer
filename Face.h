/*
 *  Face.h
 *  
 *
 *  Created by Donald House on 2/18/11.
 *  Copyright 2011 Clemson University. All rights reserved.
 *
 */

#ifndef __H_FACE__
#define __H_FACE__

#include "Vector.h"
#include "Group.h"
#include "Material.h"
#include "Collision.h"
#include "Ray.h"
#include "AABBox.h"

class PolySurf;

//
// A vertex has indices into vertex location, normal, and uv coordinate tables
// Any index set to -1 means there is no corresponding value
//
struct FaceVert{
  int v, n, u;
  
  friend ostream& operator<<(ostream& os, const FaceVert& fv);
};

class Face{
private:
  static Material defaultmaterial;

  Collision TriCollide(const Collision &c, FaceVert triverts[3], PolySurf *p) const;
  Collision TriCollide(const Collision &c, PolySurf *p) const;
  Collision QuadCollide(const Collision &c, PolySurf *p) const;
  Collision PolyCollide(const Collision &c, PolySurf *p) const;

public:
  int nverts, maxverts;
  FaceVert *faceverts;
  Vector3d normal;
  int group;
  int material;

  Face(int g = -1, int m = -1);
  ~Face(){}   // note, verts not destroyed
  
  void setGroup(int g);
  void setMaterial(int m);
  
  void addVert(int vv, int vn = -1, int vu = -1);
  
  AABBox GetBBox(PolySurf *psurf) const;
  Vector3d GetCentroid(PolySurf *psurf) const;

  Collision RayCollide(const Ray &r, PolySurf *p) const;

  friend ostream& operator<<(ostream& os, const Face& f);

  void norms();
};

typedef Face* Faceptr;

#endif
