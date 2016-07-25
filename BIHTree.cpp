/*
 *  BIHTree.cpp
 *  
 *
 *  Created by Donald House on 1/24/14.
 *  Copyright 2014 Clemson University. All rights reserved.
 *
 */

#include "BIHTree.h"
#include "AABBox.h"

inline float BIHNode::LeftPlaneCollide(const Ray &r) const{
  return  r.inv_u[axis] * (bleft - r.p[axis]);
}

inline float BIHNode::RightPlaneCollide(const Ray &r) const{
  return r.inv_u[axis] * (bright - r.p[axis]);
}

Collision BIHNode::LeafCollide(const Ray &r, Faceptr faces[], PolySurf *psurf) const{
  Collision cnear, c;

  for(int i = leafstart; i <= leafend; i++){
	c = faces[i]->RayCollide(r, psurf);
	if(c.t < cnear.t)
	  cnear = c;
  }
  
  return cnear;
}

Collision BIHNode::RayCollide(const Ray &r, float tmin, float tmax, Faceptr faces[], PolySurf *psurf) const{

  // if we are at a leaf, then collide with the objects in the leaf
  if(axis == -1)
	return LeafCollide(r, faces, psurf);
  
  Collision cl, cr;
  float tl, tr;
  
  //
  // handle the simplified special cases if only one child node
  //
  if(rightchild == NULL){	// if right missing, look only at the left hand child
	tl = LeftPlaneCollide(r);
	if(r.u[axis] > 0){		  // ray traveling to right
	  if(tl > tmin)
		cl = leftchild->RayCollide(r, tmin, tl, faces, psurf);
	}
	else{					  // ray traveling to left
	  if(tl < tmax)
		cl = leftchild->RayCollide(r, tl, tmax, faces, psurf);
	}
	return cl;
  }
  
  if(leftchild == NULL){	// if left missing, look only at the right hand child
	tr = RightPlaneCollide(r);
	if(r.u[axis] < 0){		  // ray traveling to left
	  if(tr > tmin)
		cr = rightchild->RayCollide(r, tmin, tr, faces, psurf);
	}
	else{					  // ray traveling to right
	  if(tr < tmax)
		cr = rightchild->RayCollide(r, tr, tmax, faces, psurf);
	}
	return cr;	
  }
  
  //
  // both child nodes exist. Need to process all cases
  // find the ray intersections with the left and right dividing planes
  //
  tl = LeftPlaneCollide(r);
  tr = RightPlaneCollide(r);
  
  if(r.u[axis] > 0){  // ray is traveling to the right along the dividing axis
	// if the ray lies completely within the region between the dividing planes
	// then no collision, prune the rest of the tree
	if(tl < tmin + SMALLNUMBER && tr > tmax - SMALLNUMBER)
	  return cl;

	// check for hit on left side if it lies in the ray interval
	if(tl > tmin - SMALLNUMBER)
	  cl = leftchild->RayCollide(r, tmin, Min(tl, tmax), faces, psurf);

	// On the right, only need to check if intersection on the left is 
	// beyond the right dividing plane
	if(tr < tmax + SMALLNUMBER && cl.t > tr - SMALLNUMBER)
	  cr = rightchild->RayCollide(r, tr, Min(tmax, cl.t), faces, psurf);
  }
  else{				  // ray is travleing to the left along the dividing axis
	// if the ray lies completely within the region between the dividing planes
	// then no collision, prune the rest of the tree
	if(tr < tmin + SMALLNUMBER && tl > tmax - SMALLNUMBER)
	  return cr;
	
	// check for hit on right side if it lies in the ray interval
	if(tr > tmin - SMALLNUMBER)
	  cr = rightchild->RayCollide(r, tmin, Min(tr, tmax), faces, psurf);
	
	// On the left, only need to check if any intersection on the right is 
	// beyond the left dividing plane
	if(tl < tmax + SMALLNUMBER && cr.t > tl - SMALLNUMBER) 
	  cl = leftchild->RayCollide(r, tl, Min(tmax, cr.t), faces, psurf);
  }

  // return the collision that is closest to the ray origin
  if(cl.t < cr.t)
	return cl;
  else 
	return cr;
}

//
// Format and output a BIH Node
//
ostream& operator<<(ostream& os, const BIHNode& b){
  if(b.leftchild != NULL)
	os << *(b.leftchild);
  
  if(b.axis < 0)
	os << "[leaf: (" << b.leafstart << ", " << b.leafend << ")]";
  else{
	os << "[" << char('x' + b.axis) << "-axis, (" << b.bleft << " , " << b.bright << ")]";
  }
  os << endl;

  if(b.rightchild != NULL)
	os << *(b.rightchild);

  return os;
}

//
// Construct a BIH Tree from a polygonal surface
//
BIHTree::BIHTree(PolySurf *p, int leafsize): psurf(p), root(NULL){
  nfaces = psurf->NFaces();			// number of faces
  Face *facelist = psurf->Faces();	// point to start of face array in polysurf

  faces = new Faceptr[nfaces];		// allocate array of pointers to faces
  for(int i = 0; i < nfaces; i++)	// make face ptrs point to faces in polysurf
	faces[i] = facelist + i;

  bbox = psurf->GetBBox();	  // bounding box is bounding box of polygonal surface

  BuildBIHTree(leafsize);	  // construct the tree
}

//
// BIH Tree destructor
//
BIHTree::~BIHTree(){
  delete root;
  delete faces;
}

//
// Use quicksort in-place technique to sort the objects to 
// left and right of the midpoint on the splitting axis
//
int BIHTree::SortAndSplitObjs(int start, int end, int axis, float splitpoint) const{
  Face *tmp;
  float mid;
  
  int swaploc = start;
  for(int probe = start; probe <= end; probe++){
	mid = faces[probe]->GetCentroid(psurf)[axis];
	if(mid < splitpoint){
	  tmp = faces[swaploc];
	  faces[swaploc] = faces[probe];
	  faces[probe] = tmp;
	  swaploc++;
	}
  }
  
  return swaploc;
}

//
// Recursively construct a BIH Tree, returning a pointer to the root node
// start and end are indices into the list of faces
//
BIHNode *BIHTree::BuildBIHTree(int start, int end, const AABBox &nodebbox, 
							   int leafsize){
  // nothing, not even a leaf
  if(end < start)
	return NULL;
  
  // create the new node, making it a leaf to begin with
  BIHNode *newnode = new BIHNode;
  newnode->axis = -1;
  newnode->leafstart = start;
  newnode->leafend = end;
  
  // if no more than leafsize faces, then just return the leaf
  if(end - start < leafsize)
	return newnode;
  
  //
  // more than leafsize faces, split space and build children
  //
  
  // determine the splitting axis by finding longest box dimension
  float maxlength = nodebbox.bounds[1][0] - nodebbox.bounds[0][0];	 // max - min
  int tryaxis = 0;
  float len;
  for(int i = 1; i < 3; i++){
	len = nodebbox.bounds[1][i] - nodebbox.bounds[0][i];	  // max - min
	if(len > maxlength){
	  maxlength = len;
	  tryaxis = i;
	}
  }
  
  // sort the list of faces about the midpoint on the axis
  // and return the location of the split in the face list.
  // If the longest axis does not achieve a split, then try the
  // other axes until one is found that splits the faces,
  // or all have been tried
  float splitpoint;
  int splitloc;
  for(int i = 0; i < 3; i++, tryaxis = (tryaxis + 1) % 3){
	// get the midpoint of the axis we are currently trying
	splitpoint = (nodebbox.bounds[0][tryaxis] + nodebbox.bounds[1][tryaxis]) / 2.0;
	// sort faces to each side of midpoint, and return index of split
	splitloc = SortAndSplitObjs(start, end, tryaxis, splitpoint);

	if(start < splitloc && splitloc <= end)  // done if we have a split
	  break;
  }

  // if everything sorted to one side, we failed to split the faces,
  // so this node remains a leaf and we are done
  if(start >= splitloc || splitloc > end)
	return newnode;
  
  // we found a split axis and successfully split the faces
  // record the split information
  newnode->axis = tryaxis;
  newnode->bleft = newnode->bright = splitpoint;
  
  // recursively build children and store split planes for this node
  AABBox thisbox, rightbbox, leftbbox;
  float left, right;

  // find the rightmost point on the left of the split plane
  right = nodebbox.bounds[0][newnode->axis];
  for(int i = start; i <= splitloc - 1; i++){
	thisbox = faces[i]->GetBBox(psurf);
	if(thisbox.bounds[1][newnode->axis] > right)
	  right = thisbox.bounds[1][newnode->axis];
  }
  // create the bounding box to left of split plane, store left plane
  // in this node, and recursively build the left child node
  leftbbox = nodebbox;
  leftbbox.bounds[1][newnode->axis] = newnode->bleft = right;
  newnode->leftchild = BuildBIHTree(start, splitloc - 1, leftbbox, leafsize);
  
  // find the leftmost point on the right of the split plane
  left = nodebbox.bounds[1][newnode->axis];
  for(int i = splitloc; i <= end; i++){
	thisbox = faces[i]->GetBBox(psurf);
	if(thisbox.bounds[0][newnode->axis] < left)
	  left = thisbox.bounds[0][newnode->axis];
  }
  // create the bounding box to right of split plane, store right plane
  // in this node, and recursively build the right child node
  rightbbox = nodebbox;
  rightbbox.bounds[0][newnode->axis] = newnode->bright = left;
  newnode->rightchild = BuildBIHTree(splitloc, end, rightbbox, leafsize);

  // all done, return the new node, which is the root of the subtree
  return newnode;
}

//
// Build a BIH Tree with a maximum of leafsize objects in each leaf
//
void BIHTree::BuildBIHTree(int leafsize){
  // nothing to put in the tree
  if(nfaces == 0)
	return;
  
  // create the new node and build the BIH tree
  root = BuildBIHTree(0, nfaces - 1, bbox, leafsize);
}

//
// Intersect a ray with the BIH Tree and return the collision information
//
Collision BIHTree::RayCollide(const Ray &r) const{
  Vector3d n;
  float tmin, tmax;
  Collision cnear;
  
  // nothing to do if the tree is empty
  if(root != NULL){
	// find the ray entry and exit points of the BIH Tree bounding box
	n = bbox.RayCollide(r, tmin, tmax);
	if(tmin != INFINITY && tmax > SMALLNUMBER){
	  if(tmin < SMALLNUMBER)
		tmin = 0;

	  /*
	  // code to do an exhaustive collision test with all of the faces
	  // to be used in timing tests to measure the speedup gained using the
	  // BIH Tree. Uncomment this and comment the RayCollide() line below
	  // to do the exhaustive testing
	  Collision c;
	  for(int i = 0; i < nfaces; i++){
		c = faces[i]->RayCollide(r, psurf);
		if(c.t < cnear.t)
		  cnear = c;
	  }
	  */

	  // shoot the ray at the root of the tree
	  cnear = root->RayCollide(r, tmin, tmax, faces, psurf);
	}
  }

  return cnear;
}


//
// Format and output a BIH Tree
//
ostream& operator<<(ostream& os, const BIHTree& t){
  os << "BIH Tree: " << t.nfaces << " faces" << '\n';
  os << *(t.root) << endl;

  return os;
}
