/*
 *  Face.cpp
 *  
 *
 *  Created by Donald House on 2/18/11.
 *  Copyright 2011 Clemson University. All rights reserved.
 *
 */
#include <cstdlib>
#include <iostream>

#include "Face.h"
#include "MakeSpace.h"
#include "PolySurf.h"

using namespace std;

Material Face::defaultmaterial(0.1, 0.9, 0.0, 1.0);

ostream& operator<<(ostream& os, const FaceVert& fv){
  os << "(" << fv.v << ", " << fv.n << ", " << fv.u << ")";
  return os;
}

Face::Face(int g, int m){
  nverts = maxverts = 0; 
  faceverts = NULL; 
  group = g; 
  material = m;
}

void Face::setGroup(int g){
  group = g;
}

void Face::setMaterial(int m){
  material = m;
}

void Face::addVert(int vv, int vn, int vu){
  if(maxverts == nverts)
	faceverts = makespace <FaceVert> (maxverts, faceverts);

  faceverts[nverts].v = vv;
  faceverts[nverts].n = vn;
  faceverts[nverts].u = vu;
  nverts++;
}

AABBox Face::GetBBox(PolySurf *psurf) const{
  AABBox bbox;
  Vector3d vtx;
  Vector3d *verts = psurf->Vertices();

  bbox.bounds[0] = bbox.bounds[1] = verts[faceverts[0].v];

  for(int i = 1; i < nverts; i++){
	vtx = verts[faceverts[i].v];
	for(int j = 0; j < 3; j++){
	  if(vtx[j] < bbox.bounds[0][j])
		bbox.bounds[0][j] = vtx[j];
	  else if(vtx[j] > bbox.bounds[1][j])
		bbox.bounds[1][j] = vtx[j];
	}
  }

  return bbox;
}

Vector3d Face::GetCentroid(PolySurf *psurf) const{
  Vector3d *verts = psurf->Vertices();
  
  Vector3d centroid = verts[faceverts[0].v];
  for(int i = 1; i < nverts; i++)
	centroid = centroid + verts[faceverts[i].v];
  centroid = centroid / nverts;
  
  return centroid;
}

void Face::norms() {
  Vector3d e01 = faceverts[1].v-faceverts[0].v;
  Vector3d e12 = faceverts[2].v-faceverts[1].v;
  Vector3d e20 = faceverts[0].v-faceverts[2].v;
  
  Vector3d temp = e01%e12;
  temp = temp.normalize();
  normal = temp;
}


Collision Face::RayCollide(const Ray &r, PolySurf *p) const {
  Collision c;

  float denom = r.u * normal;
  if(Abs(denom) < SMALLNUMBER)
	return c;

  // find collision of the ray with the plane of the face
  c.t = (p->Vertices()[faceverts[0].v] - r.p) * normal / denom;

  // if collision at the ray start or backwards on the ray, ignore it
  if(c.t < SMALLNUMBER){
	c.t = INFINITY;
	return c;
  }

  c.x = r.X(c.t);	// location of hit on plane of the face
  c.n = normal;		// normal to the face
  if(material < 0)
	c.m = &defaultmaterial;
  else
	c.m = p->Materials() + material;	// pointer to material for this face

  if(nverts == 3)
	return TriCollide(c, p);
  else if(nverts == 4)
	return QuadCollide(c, p);
  else
	return PolyCollide(c, p);
}

//
// Check for ray collision with a triangle, given the vertices, and collision with
// the plane of the triangle
//
Collision Face::TriCollide(const Collision &c, FaceVert triverts[3], PolySurf *p) const{
  float f, u, v, w;
  Collision ctri(c);

  Vector3d e12 = p->Vertices()[triverts[2].v] - p->Vertices()[triverts[1].v];
  Vector3d e20 = p->Vertices()[triverts[0].v] - p->Vertices()[triverts[2].v];
  
  f = 1.0 / ((e12 % e20) * c.n);	  // 1 over twice area of triangle
  u = f * (e12 % (c.x - p->Vertices()[triverts[1].v])) * c.n;  

  if(u < 0.0 || u > 1.0){
	ctri.t = INFINITY;
	return ctri;
  }
  
  v = f * (e20 % (c.x - p->Vertices()[triverts[2].v])) * c.n;
  w = 1.0 - u - v;

  if(v < 0.0 || w < 0.0){
	ctri.t = INFINITY;
	return ctri;
  }

  if(triverts[0].n != -1)
	ctri.n = (u * p->Normals()[triverts[0].n] + v * p->Normals()[triverts[1].n] + 
			  w * p->Normals()[triverts[2].n]).normalize();

  if(triverts[0].u != -1)
	ctri.uv = u * p->UVs()[triverts[0].u] + v * p->UVs()[triverts[1].u] + 
			w * p->UVs()[triverts[2].u];
  return ctri;
}

//
// Check for ray collision with a triangle stored as a face
//
Collision Face::TriCollide(const Collision &c, PolySurf *p) const{
  return TriCollide(c, faceverts, p);
}

//
// Check for ray collision with a quad, given the vertices, and collision with
// the plane of the quad
//
Collision Face::QuadCollide(const Collision &c, PolySurf *p) const{
  Collision ctri;
  FaceVert verts[3];
  
  for(int i = 0; i < 3; i++)
	verts[i] = faceverts[i];
  ctri = TriCollide(c, verts, p);

  if(ctri.t == INFINITY){
	for(int i = 0; i < 3; i++)
	  verts[i] = faceverts[(i + 2) % 4];
	ctri = TriCollide(c, verts, p);	
  }
  
  return ctri;
}

// not implemented yet, return no collision
Collision Face::PolyCollide(const Collision &c, PolySurf *p) const{
  Collision cpoly(c);
  
  cpoly.t = INFINITY;
  
  return cpoly;
}

ostream& operator<< (ostream& os, const Face& f){
  os << "[face: " << f.nverts;
  for(int i = 0; i < f.nverts; i++)
    os << " (" << f.faceverts[i].v << ", " << f.faceverts[i].n << ", " << f.faceverts[i].u << ")";
  os << ", group: " << f.group << ", material: " << f.material;
  os << "]";
  return os;
}
