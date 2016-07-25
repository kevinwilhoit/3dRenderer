/*
 *  PolySurf.cpp
 *  
 *  Created by Donald House on 2/18/11.
 *  Copyright 2011 Clemson University. All rights reserved.
 *
 */
#include <cstdlib>
#include <iostream>

#include "PolySurf.h"
#include "MakeSpace.h"
#include "BIHTree.h"

using namespace std;

PolySurf::PolySurf(){
  nverts = nnorms = nuvs = nfaces = 0;
  maxverts = maxnorms = maxuvs = maxfaces = 0;
  verts = norms = NULL;
  uvs = NULL;
  faces = NULL;

  npoints = nlines = 0;
  maxpoints = maxlines = 0;
  points = NULL;
  lines = NULL;

  ngroups = nmaterials = 0;
  maxgroups = maxmaterials = 0;
  groups = NULL;
  materials = NULL;
  
  bihtree = NULL;
}

PolySurf::~PolySurf(){
  delete []verts;
  delete []norms;
  delete []uvs;
  delete []points;
  delete []faces;
  delete []lines;
  delete []groups;
  delete []materials;
  delete bihtree;
}

int PolySurf::getfacenum() {
  return nfaces;
}

Face *PolySurf::getfaces() {
  Face *f = faces;
  return f;
}

Vector3d *PolySurf::getverts() {
  Vector3d *v = verts;
  return v;
}

Vector2d *PolySurf::getuv() {
  Vector2d *u = uvs;
  return u;
}

void PolySurf::addVertex(const Vector3d &v){
  if(maxverts == nverts)
    verts = makespace <Vector3d> (maxverts, verts);
  verts[nverts++] = v;
  
  for(int i = 0; i < 3; i++){
	if(v[i] < bbox.bounds[0][i])
	  bbox.bounds[0][i] = v[i];
	if(v[i] > bbox.bounds[1][i])
	  bbox.bounds[1][i] = v[i];
  }  
}

void PolySurf::addNormal(const Vector3d &n){
  if(maxnorms == nnorms)
    norms = makespace <Vector3d> (maxnorms, norms);
  norms[nnorms++] = n;
}

void PolySurf::addUV(const Vector2d &u){
  if(maxuvs == nuvs)
    uvs = makespace <Vector2d> (maxuvs, uvs);
  uvs[nuvs++] = u;
}

void PolySurf::addPoint(int p){
  if(maxpoints == npoints)
    points = makespace <int> (maxpoints, points);
  points[npoints++] = p;
}

int PolySurf::newFace(int g, int m){
  if(maxfaces == nfaces)
    faces = makespace <Face> (maxfaces, faces);
  
  faces[nfaces].setGroup(g);
  faces[nfaces].setMaterial(m);

  return nfaces++;
}

void PolySurf::addFaceVert(int f, int v, int n, int u){
  faces[f].addVert(v, n, u);
}

void PolySurf::setFaceNormal(int f){
  if(faces[f].nverts < 3)
	faces[f].normal = Vector3d(0, 0, 0);
  else{
	Vector3d e0 = verts[faces[f].faceverts[1].v] - verts[faces[f].faceverts[0].v];
	Vector3d e1 = verts[faces[f].faceverts[faces[f].nverts - 1].v] - 
					verts[faces[f].faceverts[0].v];
	faces[f].normal = (e0 % e1).normalize();
  }
}

int PolySurf::newLine(){
  if(maxlines == nlines)
    lines = makespace <Line> (maxlines, lines);
  return nlines++;
}

void PolySurf::addLinePoint(int l, int p){
  lines[l].addPoint(p);  
}

int PolySurf::setGroup(char *gname){
  for(int i = 0; i < ngroups; i++)
    if(groups[i].isNamed(gname))
      return i;

  if(maxgroups == ngroups)
    groups = makespace <Group> (maxgroups, groups);

  groups[ngroups].setName(gname);
  return ngroups++;
}

void PolySurf::addFaceGroup(int f, int g){
  if(g != -1)
    groups[g].addFace(f);
}

int PolySurf::newMaterial(char *mname){
  int idx = idxMaterial(mname);
  if(idx != -1)
    return idx;

  if(maxmaterials == nmaterials)
    materials = makespace <Material> (maxmaterials, materials);
  
  materials[nmaterials].setName(mname);
  return nmaterials++;
}

void PolySurf::setMaterialK(int m, int k, const Color &c){
  materials[m].setK(k, c);
}

void PolySurf::setMaterialTransmission(int m, const Color &c){
  materials[m].setTransmission(c);
}

void PolySurf::setMaterialAlpha(int m, double alfa){
  materials[m].setAlpha(alfa);
}

void PolySurf::setMaterialExp(int m, double spexp){
  materials[m].setExp(spexp);
}

void PolySurf::setMaterialIOR(int m, double ior){
  materials[m].setIOR(ior);
}

void PolySurf::setMaterialIllum(int m, int n){
  materials[m].setIllum(n);
}

void PolySurf::setMaterialMap(int m, int mtype, Pixmap *p){
  materials[m].setMap(mtype, p); 
}

int PolySurf::idxMaterial(char *mname){
  for(int i = 0; i < nmaterials; i++)
    if(materials[i].isNamed(mname))
      return i;
  return -1;
}

void PolySurf::addFaceMaterial(int f, int m){
  faces[f].setMaterial(m);
}

void PolySurf::BuildBIHTree(){
  bihtree = new BIHTree(this);
}

Collision PolySurf::RayCollide(const Ray &r) const{
  Collision c;
  c = BBoxCollide(r);
  if(c.t != INFINITY)
	c = bihtree->RayCollide(r);

  return c;
}

ostream& operator<< (ostream& os, const PolySurf& ps){
  os << "[polysurf: " << '\n';
  os << "bbox " << ps.bbox << '\n';
  os << "verts (" << ps.nverts << ") = ";
  for(int i = 0; i < ps.nverts; i++)
    os << ps.verts[i] << " ";
  os << '\n';
  os << "faces (" << ps.nfaces << ") = " << '\n';
  for(int i = 0; i < ps.nfaces; i++)
    os << i << " : " << ps.faces[i] << '\n';
  os << "lines (" << ps.nlines << ") = " << '\n';
  for(int i = 0; i < ps.nlines; i++)
    os << i << " : " << ps.lines[i] << '\n';
  os << "points (" << ps.npoints << ") = ";
  for(int i = 0; i < ps.npoints; i++)
    os << "(" << i << " : " << ps.points[i] << ") ";
  os << '\n';
  os << "groups (" << ps.ngroups << ") = " << '\n';
  for(int i = 0; i < ps.ngroups; i++)
    os << i << " : " << ps.groups[i] << '\n';
  os << "materials (" << ps.nmaterials << ") = " << '\n';
  for(int i = 0; i < ps.nmaterials; i++)
    os << i << " : " << ps.materials[i] << '\n';
  os << "]";
  return os;
}
