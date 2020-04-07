/* cairo.i */
%module cairo
%{
  /* Put header files here or function declarations like below */
  #define CAIRO_HAS_PNG_FUNCTIONS 1
  #include <cairo.h>
  %}

#define CAIRO_HAS_PNG_FUNCTIONS 1
%include "/usr/include/cairo/cairo.h"
