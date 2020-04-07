/* fuse.i */
%module fuse
%{
  /* Put header files here or function declarations like below */
  #include <fuse.h>
  %}

%include "/usr/include/fuse/fuse.h"
