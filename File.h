/*
 *  File.h
 *  
 *
 *  Created by Donald House on 2/18/11.
 *  Copyright 2011 Clemson University. All rights reserved.
 *
 */

#ifndef __MYFILE__
#define __MYFILE__

#include <cstdlib>	// standard C utility library
#include <string>

#define MAXLINESIZE  4096

using namespace std;

class File{
protected:
  char *filename;	 // Name of file
  char line[MAXLINESIZE];
  int lineno;

  void stripwhite(char *line);
  
  bool str_pfx(const char *str, const char *pfx);
  
  bool str_sfx(const char *str, const char *sfx);
  
  void errmsg(const char *msg);
  
public:
  File(const char *filename = NULL);
  ~File();

  void setfilename(const char *fname);
  char *getfilename();
  
  virtual int read(const char *fname = NULL) = 0;
};

#endif
