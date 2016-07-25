/*
Nicholas Bodtorf
CpSc 405
HW5

Compile with make
Run with conventions:
./objtrace [modelname.obj] [imagefilename]
Runs with cube.obj, and tetrahedron.obj included files
*/

#include <Magick++.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <cmath>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "Vector.h"
#include "Sphere.h"
#include "Light.h"
#include "ParallelLight.h"
#include "PointLight.h"
#include "Material.h"
#include "Color.h"
#include "Camera.h"
#include "OBJFile.h"
#include "PolySurf.h"
#include "Face.h"
#include "BIHTree.h"
#include "Collision.h"
#include "Ray.h"

using namespace std;
unsigned char *pixmap;
string outputName;
bool ofn = false;
Magick::Image image;
bool perspective = true;

Light **lights;
Camera camera;
OBJFile objfile;
PolySurf *scene;
Face *faces;
Vector3d *verts;
Vector2d *uvarray;

void sceneinit() {
  lights = new Light*[3];
  lights[0] = new PointLight(Vector3d(0.0, 4.5, 1.0), Color(1.0, 1.0, 0.9));
  lights[1] = new PointLight(Vector3d(1.0, -4.5, -2.0), Color(1.0, 1.0, 0.9));
  lights[2] = new PointLight(Vector3d(-1.00, 1.00, 4.0), Color(0.9, 0.9, 1.0));
}
/*
Color tmaping(Vector3d Ur, Vector3d P, double min, int f) {
  int vnum = faces[f].nverts;
	Vector3d v[vnum];
  Vector3d xhit = P+(min*Ur);
	for(int i=0; i<vnum; i++) {
	  v[i] = Vector3d(verts[faces[f].faceverts[i].v]);
	}
	
	Vector3d e01 = v[1]-v[0];
	Vector3d e12 = v[2]-v[1];
	Vector3d e20 = v[0]-v[2];
	
	Vector3d normal = (e01%e12).normalize();
	
	double A = 0.5 * (e01%e12)*normal;
	double Au = 0.5 * (e12%(xhit-v[1]))*normal;
	double Av = 0.5 * (e20%(xhit-v[2]))*normal;

	//Computed B-coords	
	double u = Au/A;
	double vc = Av/A;
	double w = 1-u-vc;
	Pixmap *map = scene->materials[faces[f].material].dmap;
	unsigned char *pmap = map->Pixels();

	Vector2d tcord[vnum];
        for(int i=0; i<vnum; i++) {
	  tcord[i] = Vector2d(uvarray[faces[f].faceverts[i].u]);
	}
	Vector2d ux = u*tcord[0]+vc*tcord[1]+w*tcord[2];

	int newx = ux.x*map->NRows();
	int newy = ux.y*map->NCols();
	
	return Color(pmap[(int)(4*(newy*map->NRows()+newx))], pmap[(int)(4*(newy*map->NRows()+newx)+1)], 
pmap[(int)(4*(newy*map->NRows()+newx)+2)]);
}
*/

Color tmaping(Vector3d Ur, Vector3d P, Collision clsn) {
  Pixmap *map = clsn.m->dmap;
  unsigned char *pmap = map->Pixels();

  int x = clsn.uv.x*map->NRows();
  int y = clsn.uv.y*map->NCols();

  	return Color(pmap[(int)(4*(y*map->NRows()+x))], pmap[(int)(4*(y*map->NRows()+x)+1)], 
pmap[(int)(4*(y*map->NRows()+x)+2)]);
}


void cast() {
  double width = camera.swidth;
	double height = camera.swidth;
	double pixwidth = width / image.columns();
	double pixheight = height / image.rows();
  double px, py, pz, x;

  Vector3d center = camera.viewpoint + (camera.flength * camera.viewdir);
  Vector3d ux = (camera.viewdir%camera.vup).normalize();
  Vector3d uy = -camera.viewdir % ux;
  Vector3d uz = -camera.viewdir;

  
  pz = -(-0.5); 
  for(int i=0; i<image.rows(); i++) {
    for(int j=0; j<image.columns(); j++) {
      py = -(height/2)+(pixheight*(i-0.5));
      px = -(width/2)+(pixwidth*(j-0.5));
    
      Vector3d P = center + px * ux + py * uy;
      Vector3d Ur(0, 0, 0);
      if(perspective == false) {
        Ur = camera.viewdir;
      }
      else {
        Ur = (P-camera.viewpoint).normalize();
      }
      
			Ray r = Ray(P, Ur);
			Collision clsn = scene->RayCollide(r);
			Color tcolor(0, 0, 0);
			Color acolor(0, 0, 0);
			Color dspec(0, 0, 0);
			if(clsn.t > 0 && clsn.t < 500000) {
				if(clsn.m->dmap == NULL) {
					acolor = clsn.m->a;
					for(int l=0; l<3; l++) {
						tcolor = lights[l]->shading(Ur, clsn);
						dspec = dspec + tcolor;
					}
				}
				else {
					acolor = tmaping(Ur, P, clsn);
				}
				tcolor = dspec + acolor;
			}


      image.pixelColor(j, i, Magick::ColorRGB(tcolor.r, tcolor.g, tcolor.b));
			
    }
  }
  pixmap = new unsigned char[4*image.columns()*image.rows()];
  image.write(0, 0, image.columns(), image.rows(), "RGBA", Magick::CharPixel, (void *)pixmap);
}

/*
      Color tcolor(0, 0, 0);
			Color acolor(0, 0, 0);
			Color dspec(0, 0, 0);
      double min = 5000000;
      for(int f=0; f<scene->getfacenum(); f++) {
        double xh = facehit(Ur, P, f);
        if(xh>0&&xh<min) {
          if(hit(Ur, P, xh, f)) {
            min = xh;
						if(scene->materials[faces[f].material].dmap == NULL) {				
							acolor = scene->materials[faces[f].material].a;
							for(int l=0; l<3; l++) {		
						 	  Vector3d lhit = P+(Ur*min);
							  tcolor = lights[l]->shading(Ur, lhit, faces[f], verts, scene->materials[faces[f].material]);
							  dspec = dspec + tcolor;							
						  }
						}
						else {
						  acolor = tmaping(Ur, P, min, f);
						}
          }
        }
      }

*/

void writeImage() {
  if(ofn) {
    image.flip();
    image.write(outputName);
  }
}

void handleDisplay() {
  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2i(0,0);
  glDrawPixels(image.columns(), image.rows(), GL_RGBA, GL_UNSIGNED_BYTE, pixmap); //may need to change
  glutSwapBuffers();
}

void handleKey(unsigned char key, int x, int y) {
  switch(key) {
    
    //Write
    case 'w':
    case 'W':
      writeImage();
      break;
    
    //Quit
    case 'q':
    case 'Q':
    case 27:
      exit(0);
    
    //No Valid Key
    default:
      return;
  }
}

void handleReshape(int w, int h) {
  float zoom = 1.0;
  int nwidth = image.columns();
  int nheight = image.rows();
        
  //Zooms based on the size of the image
  if(w < nwidth || h < nheight) {
    float xzoom = w / (nwidth * 1.0);
    float yzoom = h / (nheight * 1.0);

    if(xzoom < yzoom) zoom = xzoom;
    else zoom = yzoom;
    glPixelZoom(zoom, zoom);
   }
        
  //Transposes the image in order to be centered
  int x = (int)((w - (nwidth * zoom)) / 2);
  int y = (int)((h - (nheight * zoom)) / 2);
  glViewport(x, y, w - x, h - y);
        
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  glMatrixMode(GL_MODELVIEW);
}

void initcamera() {
  ifstream myfile("camera.txt");
  if(myfile.is_open()) {
    myfile >> camera.viewpoint.x;
    myfile >> camera.viewpoint.y;
    myfile >> camera.viewpoint.z;
    
    myfile >> camera.viewdir.x;
    myfile >> camera.viewdir.y;
    myfile >> camera.viewdir.z;
    
    myfile >> camera.vup.x;
    myfile >> camera.vup.y;
    myfile >> camera.vup.z;
    
    myfile >> camera.flength;
    myfile >> camera.aratio;
    myfile >> camera.swidth;
    
		string temp;
    myfile >> temp;
    if(temp.compare("l")==0)
      perspective == false;
    myfile.close();
  }
  else {
   camera = Camera(Vector3d(0, 0, 0.5), Vector3d(0, 0, -1), Vector3d(0, 1, 0), 0.5, 1.25, 0.5);
  }
}

int main(int argc, char* argv[]) {
  string length = "250";
  const char *objfname;
  if(argc > 1) {
    objfname = argv[1];
    if(argc > 2) {
      outputName = argv[2];
			ofn = true;
    }
  }

  /* OBJ File Loading */
  objfile.setfilename(objfname);
  int err = objfile.read();
  scene = objfile.getscene();
  if(err || scene == NULL) {
    char *filename = objfile.getfilename();
    cerr << "OBJ file " << filename << " has errors" << endl;
    exit(2);
  }
  faces = scene->getfaces();
  verts = scene->getverts();
  uvarray = scene->getuv();  
	
  sceneinit();
  initcamera();
  string temp = length+"x"+length;
  image = Magick::Image(temp, "black");
  scene->BuildBIHTree();
  for(int i=0; i<scene->getfacenum(); i++) {
    scene->setFaceNormal(i);
  }

/*
  //  Create graphics window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(image.columns(), image.rows());
  glutCreateWindow("Oh Shoot!");
*/
  cast();
	    image.flip();
      image.write("test.ppm");
/*
  //Callback routines
  glutDisplayFunc(handleDisplay);    // display callback
  glutKeyboardFunc(handleKey);    // keyboard callback
  glutReshapeFunc(handleReshape); // window resize callback
  glClearColor(0, 0, 0, 0);
  glutMainLoop();
*/
 /* Print OBJ Info */
/*
 cout << *scene << endl;
  centroid = scene->Centroid();
  bboxmin = scene->MinBBox();
  bboxmax = scene->MaxBBox();
  cout << "Centroid = " << centroid << ", Bounding Box: (" << bboxmin << ", " << bboxmax << ")" << endl;
*/
  return 0;
}
