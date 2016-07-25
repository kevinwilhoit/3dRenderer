/* 
   Alex Sheppard, Kevin Wilhoit, Nick Bodtorf
   CpSc 405
   Homework #6
   Due: 4/22/14

   opengl.cpp  ---> most of this code was taken from mipmaps.cpp
   
   CP SC 405/605 Computer Graphics    Donald H. House     12/2/08
   
   OpenGL/GLUT Program to View a 3D teapot using textures
   
   Keyboard keypresses have the following effects:
   p            - toggle between orthographic and perspective view
   i            - reinitialize (reset program to initial default state)
   q or Esc     - quit
   t            - toggle between wireframe and shaded viewing
   r            - produce raytraced image in new view window

   Camera and model controls following the mouse:
   model yaw  - left-button, horizontal motion, rotation of the model around the y axis
   model tilt  - left-button, vertical motion, rotation of the model about the x axis
   camera yaw  - middle-button, horizontal motion, rotation of the camera about the y axis
   camera tilt  - middle-button, vertical motion, rotation of the camera about the x axis
   approach  - right-button, vertical or horizontal motion, translation of camera along z axis
   
   usage: ./opengl model.obj [filename.ext]
 */

#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring> 
#include <iostream>
#include <fstream>
#include <Magick++.h>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "Model.h"
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

#define FALSE            0
#define TRUE             1

#define HORIZONTAL       0
#define VERTICAL         1

#define ESC             27  // numeric code for keyboard Esc key

#define WIDTH          800  // initial window dimensions
#define HEIGHT         600

#define ORTHO            0  // projection system codes
#define PERSPECTIVE      1

#define NONE            -1  // used to indicate no mouse button pressed

#define DRAWWIDTH      100  // view volume sizes (note: width and
#define DRAWHEIGHT      75  // height should be in same ratio as window)
#define NEAR            10  // distance of near clipping plane
#define FAR           1000  // distance of far clipping plane

#define MODELWIDTH      30  // dimension of the model
#define DEPTH          -50  // initial z coord. of center of model

#define ROTFACTOR      0.2  // degrees rotation per pixel of mouse movement
#define XLATEFACTOR    0.5  // units of translation per pixel of mouse movement

#define TEXTUREWIDTH    64  // dimensions of texture map image
#define TEXTUREHEIGHT   64

#define AMBIENT_FRACTION 0.2
#define DIFFUSE_FRACTION 0.8
#define SPECULAR_FRACTION 0.3

// colors used for lights, and materials for coordinate axes
const float DIM_PALEBLUE[] = {0.1, 0.1, 0.3, 1};
const float BRIGHT_PALEBLUE[] = {0.5, 0.5, 1, 1};
const float GRAY[] = {0.3, 0.3, 0.3, 1};
const float WHITE[] = {1, 1, 1, 1};
const float DIM_WHITE[] = {0.8, 0.8, 0.8, 1};
const float DIM_RED[] = {0.3, 0, 0, 1};
const float RED[] = {1, 0, 0, 1};
const float DIM_GREEN[] = {0, 0.3, 0, 1};
const float GREEN[] = {0, 1, 0, 1};
const float DIM_BLUE[] = {0, 0, 0.3, 1};
const float BLUE[] = {0, 0, 1, 1};

enum TEXTUREMODE{NOTEXTURE, NEARTEXTURE, LINEARTEXTURE, MIPMAPTEXTURE};

//
// Global variables updated and shared by callback routines
//

//Scene variables
PolySurf *scene;
OBJFile objfile;
string filename = "";

//Camera variable
Camera *camera;

// Window dimensions
static double Width = WIDTH;
static double Height = HEIGHT;

// Viewing parameters
static int Projection;

// Camera position and orientation
static double Pan;
static double Tilt;
static double Approach;

// model orientation
static double ThetaX;
static double ThetaY;

// global variables to track mouse and shift key
static int MouseX;
static int MouseY;
static int Button = NONE;

// global variables to track wireframe/shaded mode, material color, and texturing mode
static int Wireframe;
//static int Color;
static int SmoothShading;
static int TextureMode;
static int ColorMode;

// global variables to hold geometric models
Model Teapot;

// Texture map to be used by program
unsigned int TextureID;
static unsigned char *TextureImage;   // pixmap for texture image

// Raytrace globals
Magick::Image image;
unsigned char *pixmap;
Light **lights;
int lightnum;

//
// Raytracing functions
//
void sceneinit() {
  lightnum = 2;
  lights = new Light*[2];
  lights[0] = new PointLight(Vector3d(1.0, 1.0, 1.0), Color(1.0, 1.0, 0.9));
  lights[1] = new PointLight(Vector3d(0.5, -5.5, 50.0), Color(1.0, 1.0, 0.9));
  
  string tmp = "800x600";
  image = Magick::Image(tmp, "black");
}

Color tmaping(Vector3d Ur, Vector3d P, Collision clsn) {
  Pixmap *map = clsn.m->dmap;
  unsigned char *pmap = map->Pixels();

  int x = clsn.uv.x*map->NRows();
  int y = clsn.uv.y*map->NCols();

  return Color(pmap[(int)(4*(y*map->NRows()+x))], pmap[(int)(4*(y*map->NRows()+x)+1)], 
  pmap[(int)(4*(y*map->NRows()+x)+2)]);
}

void raytrace() {
  double w = 0.5;
  double h = 0.5;
  double pixwidth = w / Width;
  double pixheight = h / Height;
  double px, py, pz, x;

  Vector3d center = camera->viewpoint + (0.5 * camera->viewdir);
  Vector3d ux = (camera->viewdir%camera->vup).normalize();
  Vector3d uy = -camera->viewdir % ux;
  Vector3d uz = -camera->viewdir;
  
  pz = -(-0.5); 
  for(int i=0; i<Height; i++) {
    for(int j=0; j<Width; j++) {
      py = -(h/2)+(pixheight*(i-0.5));
      px = -(w/2)+(pixwidth*(j-0.5));
    
      Vector3d P = center + px * ux + py * uy;
      Vector3d Ur(0, 0, 0);
      if(Projection == ORTHO) {
        Ur = camera->viewdir;
      }
      else {
        Ur = (P-camera->viewpoint).normalize();
      }
      Ray r = Ray(P, Ur);
      Collision clsn = scene->RayCollide(r);
      Color tcolor(0, 0, 0);
      Color acolor(0, 0, 0);
      Color dspec(0, 0, 0);
      
      if(clsn.t > 0 && clsn.t < 500000) {
        if(clsn.m->dmap == NULL) {
          acolor = clsn.m->a;      
          for(int l=0; l<lightnum; l++) {
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

void writeImage() {
  if(filename.length() > 0) {
    image.flip();
    image.write(filename);
  }
}

void handleDisplay() {
  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2i(0,0);
  glDrawPixels(image.columns(), image.rows(), GL_RGBA, GL_UNSIGNED_BYTE, pixmap); //may need to change
  glutSwapBuffers();
}

void handleKeyR(unsigned char key, int x, int y) {
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
      glutHideWindow();
    
    //No Valid Key
    default:
      return;
  }
}

void handleReshapeR(int w, int h) {
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

void tracing() {
  glutInitWindowSize(image.columns(), image.rows());
  glutCreateWindow("3D Raytraced Image Viewer");  
  raytrace();
  glutDisplayFunc(handleDisplay);    // display callback
  glutKeyboardFunc(handleKeyR);    // keyboard callback
  glutReshapeFunc(handleReshapeR); // window resize callback
  glClearColor(0, 0, 0, 0);
  glutMainLoop();
}

//
// Routine to initialize the state of the program to startup defaults
//
void setInitialState(){
  
  // initial camera viewing and shading model controls
  Projection = PERSPECTIVE;
  
  // model initially wireframe with white color, and flat shading
  Wireframe = TRUE;
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  TextureMode = NOTEXTURE;
  ColorMode = GL_MODULATE;
  // if the vertex normal array in polysurf is not empty, then true, otherwise false
  if(scene->nnorms > 0) 
     SmoothShading = TRUE;
  else
     SmoothShading = FALSE;
  
  // initial camera orientation and position
  Pan = 0;
  Tilt = -7;
  Approach = DEPTH;
  camera = new Camera(Vector3d(0,0,0), Vector3d(0,0,-1), Vector3d(0,1,0), 0.5, (4.0/3.0), 800); 
  
  // initial model orientation
  ThetaX = 290;
  ThetaY = 0;
}

//
// Set up the projection matrix to be either orthographic or perspective
//
void updateProjection(){
  
  // initialize the projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  // determine the projection system and drawing coordinates
  if(Projection == ORTHO)
    glOrtho(-DRAWWIDTH/2, DRAWWIDTH/2, -DRAWHEIGHT/2, DRAWHEIGHT/2, NEAR, FAR);
  else{
    // scale drawing coords so center of cube is same size as in ortho
    // if it is at its nominal location
    double scale = fabs((double)NEAR / (double)DEPTH);
    double xmax = scale * DRAWWIDTH / 2;
    double ymax = scale * DRAWHEIGHT / 2;
    glFrustum(-xmax, xmax, -ymax, ymax, NEAR, FAR);
  }
  
  // restore modelview matrix as the one being updated
  glMatrixMode(GL_MODELVIEW);
}

//
// routine to load the scene into the PolySurf variable
//
void load_scene(PolySurf **scene, OBJFile &objfile){

  int err = objfile.read();
  *scene = objfile.getscene();

  if(err || scene == NULL){
    char *filename = objfile.getfilename();
    cerr << "OBJ file " << filename << " has errors" << endl;
    exit(2);
  }
}

//
// routine to draw the current model
//
void drawModel(int wireframe){
  
  if(wireframe){
    glDisable(GL_TEXTURE_2D);
    
    // set drawing color to white, and draw with thicker wireframe lines
    glColor3f(1, 1, 1);
    glLineWidth(2);
  }
  else{ 
    if(TextureMode != NOTEXTURE){
      glEnable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, ColorMode);
  switch(TextureMode){
  case NEARTEXTURE:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    break;
  case LINEARTEXTURE:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    break;
  case MIPMAPTEXTURE:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    break;
      }
    }
  }

  Teapot.Draw(wireframe);
  
  glDisable(GL_TEXTURE_2D);
}

//
// Display callback
//
void doDisplay(){
  // distant light source, parallel rays coming from front upper right
  const float light_position[] =  {1, 1, 1, 0};
  const float light_position2[] =  {0.5, -5.5, 50, 0};
  const float direction[] = {-0.0311766, -0.0104073, -0.99946};
  
  // clear the window to the background color
  glClear(GL_COLOR_BUFFER_BIT);
  
  if(!Wireframe)
    glClear(GL_DEPTH_BUFFER_BIT);  // solid - clear depth buffer
  
  if(!Wireframe && SmoothShading)  // establish shading model, flat or smooth
    glShadeModel(GL_SMOOTH);
  else
    glShadeModel(GL_FLAT);
  
  // light is positioned in camera space so it does not move with object
  glLoadIdentity();
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, WHITE);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
  glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);

  glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
  glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction);
  glLightfv(GL_LIGHT1, GL_AMBIENT, WHITE);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, WHITE);
  glLightfv(GL_LIGHT1, GL_SPECULAR, WHITE);

  // establish camera coordinates
  glRotatef(Tilt, 1, 0, 0);      // tilt - rotate camera about x axis
  glRotatef(Pan, 0, 1, 0);      // pan - rotate camera about y axis
  glTranslatef(0, 0, Approach);     // approach - translate camera along z axis
  
  // rotate the model
  glRotatef(ThetaY, 0, 1, 0);       // rotate model about x axis
  glRotatef(ThetaX, 1, 0, 0);       // rotate model about y axis
  
  // draw the model in wireframe or solid
  drawModel(Wireframe);

  glutSwapBuffers();
}

void get_camera()
{
    float t_mat[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, t_mat);

    Vector3d xdir(0.0, 0.0, 0.0);
    Vector3d ydir(0.0, 0.0, 0.0);
    Vector3d zdir(0.0, 0.0, 0.0);

    xdir.x = t_mat[0];  ydir.x = t_mat[1];  zdir.x = t_mat[2];
    xdir.y = t_mat[4];  ydir.y = t_mat[5];  zdir.y = t_mat[6];
    xdir.z = t_mat[8];  ydir.z = t_mat[9];  zdir.z = t_mat[10];

    const Vector3d vector(-t_mat[12], -t_mat[13], -t_mat[14]);

    const Matrix3x3 temp_mat(xdir.x, xdir.y, xdir.z,
                              ydir.x, ydir.y, ydir.z,
                              zdir.x, zdir.y, zdir.z);

    const Matrix3x3 inv = temp_mat.inv();

    camera->viewpoint = inv * vector;
    camera->viewdir = -zdir;
    camera->vup = ydir;
}

//
// Keyboard callback routine. 
// Set various modes or take actions based on key presses
//
void handleKey(unsigned char key, int x, int y){
  
  switch(key){
      
    case 'p':      // P -- toggle between ortho and perspective
    case 'P':
      Projection = !Projection;
      updateProjection();
      glutPostRedisplay();
      break;
      
    case 'i':      // I -- reinitialize 
    case 'I':
      setInitialState();
      updateProjection();
      glutPostRedisplay();
      break;
  
    case 'q':      // Q or Esc -- exit program
    case 'Q':
    case ESC:
      exit(0);
      
    case 't':      // T -- toggle between wireframe and shaded viewing
    case 'T':
      Wireframe = !Wireframe;
      if(Wireframe){
        TextureMode = NOTEXTURE;
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
      }
      else{
        TextureMode = LINEARTEXTURE;
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
      }
      glutPostRedisplay();
      break;

    case 'r':      // R -- produce raytraced image in new view window
    case 'R':
      get_camera();
      tracing();
      break;
  }
}

//
// Mouse Button Callback
// on button press, record mouse position and which button is pressed
//
void handleButtons(int button, int state, int x, int y){
  
  if(state == GLUT_UP)
    Button = NONE;    // no button pressed
  else{
    MouseY = -y;    // invert y window coordinate to correspond with OpenGL
    MouseX = x;
    
    Button = button;    // store which button pressed
  }
}

//
// Mouse Motion Callback
// when mouse moves with a button down, update appropriate camera parameter
//
void handleMotion(int x, int y){
  int delta;
  
  y = -y;
  int dy = y - MouseY;
  int dx = x - MouseX;
  
  switch(Button){
    case GLUT_LEFT_BUTTON:
      ThetaX -= ROTFACTOR * dy;
      ThetaY += ROTFACTOR * dx;
      glutPostRedisplay();
      break;
    case GLUT_MIDDLE_BUTTON:
      Pan -= ROTFACTOR * dx;
      Tilt += ROTFACTOR * dy;
      glutPostRedisplay();
      break;
    case GLUT_RIGHT_BUTTON:
      delta = (fabs(dx) > fabs(dy)? dx: dy);
      Approach += XLATEFACTOR * delta;
      glutPostRedisplay();
      break;
  }
  
  MouseX = x;
  MouseY = y;
}

//
// Reshape Callback
// Keep viewport the entire screen
//
void doReshape(int width, int height){
  
  glViewport(0, 0, width, height);
  Width = width;
  Height = height;
  
  updateProjection();
}

//
// Initialize OpenGL to establish lighting and colors
// and initialize viewing and model parameters
//
void initialize(){
  
  // initialize modelview matrix to identity
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  // specify window clear (background) color to be black
  glClearColor(0, 0, 0, 0);
  
  // position light and turn it on
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  
  // initialize viewing and model parameters
  setInitialState();
  updateProjection();
  
  // construct the models that will be used for cube and for coordinate arrows
  Teapot.BuildTeapot(scene); // teapot
 
  for(int i = 0; i < scene->nmaterials; i++){
    if(scene->materials[i].dmap != NULL)
      scene->materials[i].createTexture(); 
  }
}

//
// Main program to create window, setup callbacks, and initiate GLUT
//
int main(int argc, char* argv[]){
  int dot;

  //Checks comand line arguments
  if(argc > 3 || argc < 2){
     fprintf(stderr, "usage: ./objtrace model.obj [imagefilename]\n");
     exit(1);
  } 

  //parses the command line arguments and makes sure they were used correctly
  for(int i = 1; i < argc; i++){
     if(i == 1){
  dot = strlen(argv[i]) - 4;
        if(strcmp(&(argv[i][dot]), ".obj") != 0){
     fprintf(stderr, "usage: ./objtrace model.obj [imagefilename]\n");
           exit(1);
        }
  objfile.setfilename(argv[i]);
     } else if(i == 2){
        filename = (string)argv[i];
        dot = filename.length() - 4;
        if(filename[dot] != '.'){
           fprintf(stderr, "usage: ./objtrace model.obj [imagefilename]\n");
           exit(1);
        }
     }
  }
  sceneinit();
  load_scene(&scene, objfile);
  scene->BuildBIHTree();
  for(int i=0; i<scene->getfacenum(); i++) {
    scene->setFaceNormal(i);
  }

  // start up the glut utilities
  glutInit(&argc, argv);
  
  // create the graphics window, giving width, height, and title text
  // and establish double buffering, RGBA color
  // Depth buffering must be available for drawing the shaded model
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("3D OpenGL Image Viewer");
  
  // register callback to draw graphics when window needs updating
  glutDisplayFunc(doDisplay);
  glutReshapeFunc(doReshape);
  glutKeyboardFunc(handleKey);
  glutMouseFunc(handleButtons);
  glutMotionFunc(handleMotion);
  
  initialize();
  
  glutMainLoop();
  
}
