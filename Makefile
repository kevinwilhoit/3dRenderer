CC      = g++
C	= cpp
H	= h

CFLAGS		= -g `Magick++-config --cppflags --cxxflags`
LFLAGS		= -g `Magick++-config --ldflags --libs`

ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = -framework Foundation -framework GLUT -framework OpenGL ${LFLAGS} -lm
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS     = -lglut -lGL -lGLU ${LFLAGS} -lm
  endif
endif

HFILES = Sphere.${H} Vector.${H} Utility.${H} Color.${H} Material.${H} Light.${H} PointLight.${H} ParallelLight.${H} Camera.${H} OBJFile.${H} MTLFile.${H} ImageFile.${H} PolySurf.${H} Face.${H} Line.${H} Group.${H} File.${H} Pixmap.${H} BIHTree.${H} Collision.${H} Object.${H} Ray.${H} AABBox.${H} Matrix.${H} Model.${H}
OFILES = Sphere.o Vector.o Utility.o Color.o Material.o Light.o PointLight.o ParallelLight.o Camera.o File.o OBJFile.o MTLFile.o ImageFile.o PolySurf.o Face.o Line.o Group.o Pixmap.o BIHTree.o Object.o Ray.o AABBox.o Matrix.o Model.o

PROJECT = opengl

${PROJECT}:	${PROJECT}.o ${OFILES}
	${CC} -o ${PROJECT} ${PROJECT}.o ${OFILES} ${LDFLAGS}

${PROJECT}.o: ${PROJECT}.${C} ${HFILES}
	${CC} ${CFLAGS} -c ${PROJECT}.${C}
	
Sphere.o:  Sphere.${C} Sphere.${H} Vector.${H} Utility.${H} Material.${H}
	${CC} ${CFLAGS} -c Sphere.${C}
	
ParallelLight.o:  ParallelLight.${C} ParallelLight.${H} Light.${H} Vector.${H} Utility.${H} Material.${H} Color.${H}
	${CC} ${CFLAGS} -c ParallelLight.${C}

PointLight.o:  PointLight.${C} PointLight.${H} Light.${H} Vector.${H} Utility.${H} Material.${H} Color.${H}
	${CC} ${CFLAGS} -c PointLight.${C}

Light.o:  Light.${C} Light.${H} Vector.${H} Utility.${H} Material.${H} Color.${H}
	${CC} ${CFLAGS} -c Light.${C}
	
Material.o:  Material.${C} Material.${H} Pixmap.${H}
	${CC} ${CFLAGS} -c Material.${C}

File.o:  File.${C} File.${H}
	${CC} ${CFLAGS} -c File.${C}

OBJFile.o:  OBJFile.${C} File.${H} OBJFile.${H} MTLFile.${H} Vector.${H} Utility.${H}
	${CC} ${CFLAGS} -c OBJFile.${C}

MTLFile.o:  MTLFile.${C} File.${H} MTLFile.${H} ImageFile.${H} Color.${H} PolySurf.${H} Pixmap.${H}
	${CC} ${CFLAGS} -c MTLFile.${C}

ImageFile.o:  ImageFile.${C} File.${H} ImageFile.${H} Pixmap.${H} 
	${CC} ${CFLAGS} -c ImageFile.${C}

PolySurf.o:  PolySurf.${C} PolySurf.${H} Pixmap.${H} Vector.${H} Utility.${H}  MakeSpace.${H}
	${CC} ${CFLAGS} -c PolySurf.${C}

Face.o:  Face.${C} Face.${H} Color.${H} Pixmap.${H} Vector.${H} Utility.${H}
	${CC} ${CFLAGS} -c Face.${C}

Line.o:  Line.${C} Line.${H} Vector.${H} Utility.${H} MakeSpace.${H}
	${CC} ${CFLAGS} -c Line.${C}

Group.o:  Group.${C} Group.${H} MakeSpace.${H}
	${CC} ${CFLAGS} -c Group.${C}

Color.o:  Color.${C} Color.${H}
	${CC} ${CFLAGS} -c Color.${C}

Camera.o:  Camera.${C} Camera.${H} Vector.${H} Utility.${H}
	${CC} ${CFLAGS} -c Camera.${C}

Pixmap.o:  Pixmap.${C} Pixmap.${H} Vector.${H} Utility.${H}
	${CC} ${CFLAGS} -c Pixmap.${C}
	
Vector.o:  Vector.${C} Vector.${H} Utility.${H} 
	${CC} ${CFLAGS} -c Vector.${C}

Utility.o:  Utility.${C} Utility.${H}
	${CC} ${CFLAGS} -c Utility.${C}

BIHTree.o:  BIHTree.${C} BIHTree.${H} Vector.${H} Collision.${H} PolySurf.${H}
	${CC} ${CFLAGS} -c BIHTree.${C}
	
Object.o:  Object.${C} Object.${H} AABBox.${H} Collision.${H} Ray.${H} Material.${H} Matrix.${H}
	${CC} ${CFLAGS} -c Object.${C}
	
Ray.o:  Ray.${C} Ray.${H} Matrix.${H}
	${CC} ${CFLAGS} -c Ray.${C}
	
AABBox.o:  AABBox.${C} AABBox.${H} Vector.${H} Collision.${H} Ray.${H}
	${CC} ${CFLAGS} -c AABBox.${C}
	
Matrix.o:  Matrix.${C} Matrix.${H} Vector.${H}
	${CC} ${CFLAGS} -c Matrix.${C}
	
Model.o:  Model.${C} Model.${H} Vector.${H} PolySurf.${H}
	${CC} ${CFLAGS} -c Model.${C}
	
clean:
	rm -f *.o *~ core.* ${PROJECT}
