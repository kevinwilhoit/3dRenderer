Alex Sheppard, Kevin Wilhoit, Nick Bodtorf
CpSc 405
Project 6 README
Due: 4/22/2014

PROGRAM DESCRIPTION:
   This program renders a teapot from a .obj file in an interactive window using openGL.
   Once running the user can move the camera around, change the view from orthographic
   to perspective, as well as change the viewing mode from wireframe to shaded. 
   They can also press 'r' to open up a new view window with a fully raytraced version 
   of the image. If the user presses 'i', the model will return to its initial state.
   If the user presses q or ESC, the program will quit. 

   Once an image has been raytraced, the user can press 'w' in that view window to write
   the image to the file specified on the command line or also press 'q' or ESC to quit
   both windows.

   To run the program, type:
        ./opengl model.obj [imagefilename]

KNOWN PROBLEMS:
   Whenever the program tries to raytrace a texture mapped image, the raytraced image is distorted
   and incorrect. However, it only does this with textures. Every file runs fine with 
   some minor artifacting in the raytraced image. Also, when raytracing the full teapot,
   the program takes a few minutes to actually produce the image, so you just have to 
   let it run for a while and wait. Also, we could not get different texture mapping to work for 
   more than one group in the scene (i.e. different textures on both the teapot and the planes). 

INCLUDED FILES:
   AABBox.cpp
   AABBox.h
   Big_Brick.png
   BIHTree.cpp
   BIHTree.h
   Camera.cpp
   Camera.h
   Collision.h
   Color.cpp
   Color.h
   cube.mtl
   cube.obj
   Face.cpp
   Face.h
   File.cpp
   File.h
   file.png
   file2.png
   file3.png
   Group.cpp
   Group.h
   happy-face.png
   ImageFile.cpp
   ImageFile.h
   Light.cpp
   Light.h
   Line.cpp
   Line.h
   Makefile
   MakeSpace.h
   Material.cpp
   Material.h
   Matrix.cpp
   Matrix.h
   Model.cpp
   Model.h
   MTLFile.cpp
   MTLFile.h
   Object.cpp
   Object.h
   OBJFile.cpp
   OBJFile.h
   objload.cpp
   objtrace.cpp
   opengl.cpp
   ParallelLight.cpp
   ParallelLight.h
   Pixmap.cpp
   Pixmap.h
   PointLight.cpp
   PointLight.h
   PolySurf.cpp
   PolySurf.h
   Ray.cpp
   Ray.h
   README.txt
   Sphere.cpp
   Sphere.h
   teapot-lite.mtl
   teapot-lit.obj
   teapot.mtl
   teapot.obj
   tetrahedron.mtl
   tetrahedron.obj
   Utility.cpp
   Utility.h
   Vector.cpp
   Vector.h

** All files that were taken from the code posted by professor House
   have the original headers with copyright info included, even if we
   modified the file **
