/*
 *  Material.cpp
 *  
 *
 *  Created by Donald House on 9/8/08.
 *  Copyright 2008 Clemson University. All rights reserved.
 *
 */

#include "Material.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <cstring>

Material::Material(Color color2, double ambient2, double diffuse2, double specular2) :
  color(color2), ambient(ambient2), diffuse(diffuse2), specular(specular2) {}

Material::Material(char *mname){
  name = NULL;
  setName(mname);
  
  amap = dmap = smap = NULL;
  
  alpha = n = 1.0;
  illum_model = 1;
  
  textureid = -1;

  setProperties(0.2, 0.8, 0, 1);
}

Material::Material(const Color &ambient, const Color &diffuse, const Color &specular, 
		   double spexp){
  name = NULL;
  
  amap = dmap = smap = NULL;
  
  alpha = n = 1.0;
  illum_model = 1;
  setProperties(ambient, diffuse, specular, spexp);
}

void Material::setName(char *mname){
  delete name;
  if(mname == NULL)
    name = NULL;
  else{
    name = new char[strlen(mname) + 1];
    strcpy(name, mname);
  }
}

bool Material::isNamed(char *mname){
  if(name == NULL || mname == NULL)
    return false;
  return strcmp(mname, name) == 0;
}

void Material::setProperties(const Color &ambient, const Color &diffuse, const Color &specular,
			     double spexp){
  a = ambient; 
  d = diffuse; 
  s = specular; 
  exp = spexp;
}

void Material::setProperties(double ambient2, double diffuse2, double specular2,
			     double spexp){
  Color ca, cd, cs;
  ca.set(ambient2, ambient2, ambient2);
  cd.set(diffuse2, diffuse2, diffuse2);
  cs.set(specular2, specular2, specular2);
  setProperties(ca, cd, cs, spexp);
}

void Material::setK(int k, const Color &c){
  switch(k){
  case 0:
    a = c;
    break;
  case 1:
    d = c;
    break;
  case 2:
    s = c;
    break;
  }
}

void Material::setTransmission(const Color &c){
  t = c;
}

void Material::setExp(double spexp){
  exp = spexp;
}

void Material::setAlpha(double alfa){
  alpha = alfa;
}

void Material::setIOR(double ior){
  n = ior;
}

void Material::setIllum(int i){
  illum_model = i;
}

void Material::setMap(int mtype, Pixmap *p){
  switch(mtype){
    case 0:
      amap = p;
      break;
    case 1:
      dmap = p;
      break;
    case 2:
      smap = p;
      break;
  }
}

void Material::createTexture(){
  if(illum_model < 1 || dmap == NULL || textureid != -1)
    return;
  glGenTextures(1, &textureid); // OpenGL ID for this texture
  glBindTexture(GL_TEXTURE_2D, textureid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, dmap->NCols(), dmap->NRows(),		    
		    GL_RGBA, GL_UNSIGNED_BYTE, dmap->Pixels());  
}

ostream& operator<< (ostream& os, const Material& m){
  os << "[material " << m.name << ": a = " << m.a << ", d = " << m.d << 
	", s = " << m.s << ", exp = " << m.exp << "\n";
  os << "     alpha = " << m.alpha << ", t = " << m.t << 
	", illum model = " << m.illum_model << "\n";
  os << "     textures: (" << (m.amap? "ambient": "no ambient") << ", " <<  
			 (m.dmap? "diffuse": "no diffuse") << ", " <<
			 (m.smap? "specular": "no specular") << ")";
  os << "]";
  return os;
}
