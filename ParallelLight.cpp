/* 
Nicholas Bodtorf
CpSc 405 HW3

ParallelLight class for parallel lights
*/

#include "ParallelLight.h"

using namespace std;

ParallelLight::ParallelLight(Vector3d ul2, Color color2) : 
  Light(Vector3d(-100, -100, -100), ul2, color2) {
}

//Computes the Specular and Diffuse color values to be returned
Color ParallelLight::shading(Vector3d ur, Vector3d hit, Sphere sphere) {
  double dred = 0, dgreen = 0, dblue = 0, sred = 0, sgreen = 0, sblue = 0;
  
  Vector3d n = (hit - sphere.center).normalize(); 
  double ctheta = -(ul * n);
  
  if(ctheta>0.0) {
    dred = ctheta * color.r * sphere.material.color.r * sphere.material.diffuse;
    dgreen = ctheta * color.g * sphere.material.color.g * sphere.material.diffuse;
    dblue = ctheta * color.b * sphere.material.color.b * sphere.material.diffuse;
  }

  Vector3d ulp = ul - (2*(ul*n)*n);
  double cphi = ulp * -ur;
  
  if(cphi>0.0 && sphere.material.specular > 0) {
    double specf = pow(cphi, sphere.material.specular);
    sred = specf * color.r * sphere.material.color.r; 
    sgreen = specf * color.g * sphere.material.color.g; 
    sblue = specf * color.b * sphere.material.color.b;
  }
  return Color(dred+sred, dgreen+sgreen, dblue+sblue);
}


Color ParallelLight::shading(Vector3d ur, Collision clsn) {
	Color dcolor(0, 0, 0);
	
	double ctheta = -(ul * clsn.n);
	if(ctheta>0.0)
	  dcolor = ctheta * color * (clsn.m->d);
	
	Vector3d ulp = ul - (2*(ul*clsn.n)*clsn.n);
  double cphi = -ulp * ur;
	

	Color scolor(0, 0, 0);
  if(clsn.m->illum_model == 1) {
    if(cphi>0.0 && ((clsn.m->s.r > 0) || (clsn.m->s.g > 0) || (clsn.m->s.b > 0))) {
      double specfr = pow(cphi, clsn.m->s.r);
      double specfg = pow(cphi, clsn.m->s.g);
      double specfb = pow(cphi, clsn.m->s.b);
      scolor.r = specfr * color.r;
      scolor.g = specfg * color.g;
      scolor.b = specfb * color.b;
    }
  }
	return Color(scolor+dcolor);
}


/*
Color ParallelLight::shading(Vector3d ur, Vector3d hit, Face face, Vector3d *verts, Material mat) {
  int vnum = face.nverts;
	Vector3d v[vnum];
	for(int i=0; i<vnum; i++) {
	  v[i] = Vector3d(verts[face.faceverts[i].v]);
	}

	Color dcolor(0, 0, 0);
	Vector3d n = ((v[1]-v[0])%(v[vnum-1]-v[0])).normalize();
	double ctheta = -(ul * n);
	if(ctheta>0.0)
	  dcolor = ctheta * color * mat.d;
	
	Vector3d ulp = ul - (2*(ul*n)*n);
  double cphi = -ulp * ur;
	

	Color scolor(0, 0, 0);
  if(mat.illum_model == 1) {
    if(cphi>0.0 && ((mat.s.r > 0) || (mat.s.g > 0) || (mat.s.b > 0))) {
      double specfr = pow(cphi, mat.s.r);
      double specfg = pow(cphi, mat.s.g);
      double specfb = pow(cphi, mat.s.b);
      scolor.r = specfr * color.r;
      scolor.g = specfg * color.g;
      scolor.b = specfb * color.b;
    }
  }
	return Color(scolor+dcolor);
}
*/