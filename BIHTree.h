/*
 *  BIHTree.h
 *  
 *
 *  Created by Donald House on 1/24/14.
 *  Copyright 2014 Clemson University. All rights reserved.
 *
 */

#ifndef _H_BIHTREE_
#define _H_BIHTREE_

#include "Vector.h"
//#include "Scene.h"
#include "Collision.h"
#include "PolySurf.h"
#include "Face.h"

//
// A single BIH Tree node
//
struct BIHNode{
  int axis;			// -1 is leaf, 0 x-axis, 1 y-axis, 2 z-axis
  
  union{
	int leafstart;	// if a leaf node, store starting index into list of objects
	float bleft;	// if not a leaf node, store right bound of left child
  };
  union{
	int leafend;	// if a leaf node, store index of lastindex into list of objects
	float bright;	// left bound of right child
  };

  BIHNode *leftchild, *rightchild;  // left and right children of this node
  
  BIHNode(): axis(-1), leafstart(-1), leafend(-1), leftchild(NULL), rightchild(NULL) {
  }

  ~BIHNode(){
	if(axis >= 0){
	  delete leftchild;
	  delete rightchild;
	}
  }
  
  inline float LeftPlaneCollide(const Ray &r) const;
  inline float RightPlaneCollide(const Ray &r) const;
  
  Collision LeafCollide(const Ray &r, Faceptr faces[], PolySurf *psurf) const;

  Collision RayCollide(const Ray &r, float tmin, float tmax, Faceptr faces[], PolySurf *psurf) const;

  friend ostream& operator<<(ostream& os, const BIHNode& b);
};

//
// The BIH Tree
//
class BIHTree{
private:
  int nfaces;
  Faceptr *faces;

  AABBox bbox;
  PolySurf *psurf;

  BIHNode *root;
  
  int SortAndSplitObjs(int start, int end, int axis, float splitpoint) const;
  
  BIHNode *BuildBIHTree(int start, int end, const AABBox &bbox, int leafsize);

public:
  BIHTree(PolySurf *p, int leafsize = 1);

  ~BIHTree();
  
  void BuildBIHTree(int leafsize = 1);
  
  Collision RayCollide(const Ray &r) const;
  
  friend ostream& operator<<(ostream& os, const BIHTree& t);
};

#endif
