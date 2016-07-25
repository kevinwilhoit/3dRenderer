#include "Light.h"
using namespace std;

Light::Light(Vector3d center2, Vector3d ul2, Color color2) :
  center(center2), ul(ul2), color(color2) {
}


Color Light::shade(int snum, Vector3d x, Vector3d ur, int level, Sphere **spheres, Light **lights) {
  if(level > 8)
	  return Color(0, 0, 0);
	
	Vector3d n = (x-spheres[snum]->center).normalize();
	Vector3d um = ur - (2*(ur*n)*n);
	

	double min = 500000000;
	for(int s=0; s<5; s++) {
	  double xhit = spheres[s]->hit(um, x);
	  if(xhit>0&&xhit<min) {
		  min = xhit;
			snum = s;
		}
	}
  Sphere *sphere = spheres[snum];
  Color tcolor(0, 0, 0);
  Color difspec(0, 0, 0);
	if(min != 500000000 && min > 0) {
	  Color ambient(0, 0, 0);
		ambient.r = spheres[snum]->material.color.r * spheres[snum]->material.ambient;
		ambient.g = spheres[snum]->material.color.g * spheres[snum]->material.ambient;
		ambient.b = spheres[snum]->material.color.b * spheres[snum]->material.ambient;

	  Vector3d lhit = x + (um * min);

		for(int l=0; l<3; l++) {
		  Vector3d lum = (lights[l]->center-lhit).normalize();
		  double lmin = 500000000000;
		  for(int s=0; s<5; s++) {
		    if(s != snum) {
          double lhit = spheres[s]->hit(lum, lhit);
          if(lhit>0&&lhit<lmin) {
            lmin = lhit;
          }
        }
		  }
		  if(lmin == 500000000000) {
        tcolor = lights[l]->shading(um, lhit, *sphere);
        difspec.r += tcolor.r;
        difspec.g += tcolor.g;
        difspec.b += tcolor.b;
      }
    }
		level++;
		tcolor.r = ambient.r + difspec.r;
		tcolor.g = ambient.g + difspec.g;
		tcolor.b = ambient.b + difspec.b;
		Color ncolor = shade(snum, lhit, um, level, spheres, lights); 
		tcolor.r = tcolor.r + ((1/level)*ncolor.r);
	  tcolor.g = tcolor.g + ((1/level)*ncolor.g);
		tcolor.b = tcolor.b + ((1/level)*ncolor.b);
	}

  return tcolor;
}
