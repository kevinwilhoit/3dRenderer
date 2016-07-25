/* 
Nicholas Bodtorf
CpSc 405 HW3

Camera class for storing camera values from text file
*/

#include "Camera.h"

using namespace std;

Camera::Camera(Vector3d viewpoint2, Vector3d viewdir2, Vector3d vup2, double flength2, double aratio2, double swidth2) :
  viewpoint(viewpoint2), viewdir(viewdir2), vup(vup2) {
  
  flength = flength2;
  aratio = aratio2;
  swidth = swidth2;  
}

Camera::Camera() {}
