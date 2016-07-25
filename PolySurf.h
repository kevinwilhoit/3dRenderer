/*
 *  PolySurf.h
 *  
 *
 *  Created by Donald House on 2/18/11.
 *  Copyright 2011 Clemson University. All rights reserved.
 *
 */

#ifndef __H_POLYSURF__
#define __H_POLYSURF__

#include "Object.h"
#include "Vector.h"
#include "Pixmap.h"
#include "Face.h"
#include "Line.h"
#include "Group.h"
#include "Material.h"

class BIHTree;

class PolySurf: public Object{
public:
  int nverts, maxverts;
  Vector3d *verts;

  Vector3d *norms;
  int nuvs, maxuvs;
  Vector2d *uvs;
  int npoints, maxpoints;
  int *points;
  int nfaces, maxfaces;
  Face *faces;
  int nlines, maxlines;
  Line *lines;
  int ngroups, maxgroups;
  Group *groups;
  int nnorms, maxnorms;
  int nmaterials, maxmaterials;
  BIHTree *bihtree;


  Material *materials;
  PolySurf();
  ~PolySurf();
  
	int getfacenum();
  Face *getfaces();
  Vector3d *getverts();
  Vector2d *getuv();
	
  void addVertex(const Vector3d &v);
  int NVertices(){return nverts;}
  Vector3d *Vertices(){return verts;}
  
  void addNormal(const Vector3d &n);
  int NNormals(){return nnorms;}
  Vector3d *Normals(){return norms;}
  
  void addUV(const Vector2d &u);
  int NUVs(){return nuvs;}
  Vector2d *UVs(){return uvs;}

  void addPoint(int p);
  int NPoints(){return npoints;}
  int *Points(){return points;}
  
  int newFace(int g, int m);
  void addFaceVert(int f, int v, int n = -1, int u = -1);
  void setFaceNormal(int f);
  int NFaces(){return nfaces;}
  Face *Faces(){return faces;}
  
  int newLine();
  void addLinePoint(int l, int p);
  int NLines(){return nlines;}
  Line *Lines(){return lines;}
  
  int setGroup(char *gname);
  void addFaceGroup(int f, int g);
  int NGroups(){return ngroups;}
  Group *Groups(){return groups;}
  
  int newMaterial(char *mname);
  int idxMaterial(char *mname);  
  void setMaterialK(int m, int k, const Color &c);
  void setMaterialTransmission(int m, const Color &c);
  void setMaterialAlpha(int m, double alfa);
  void setMaterialExp(int m, double spexp);
  void setMaterialIOR(int m, double ior);
  void setMaterialIllum(int m, int n);
  void setMaterialMap(int m, int mtype, Pixmap *p);
  void addFaceMaterial(int f, int m);
  int NMaterials(){return nmaterials;}
  Material *Materials(){return materials;}
  
  void BuildBIHTree();

  Collision RayCollide(const Ray &r) const;
  
  friend ostream& operator<<(ostream& os, const PolySurf& ps);
};

#endif
