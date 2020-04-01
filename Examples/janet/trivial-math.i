/* trivial-math.i */
%module trivial_math
%{
  /* Put header files here or function declarations like below */
  extern int sum (int a, int b);
  extern int diff (int x, int y);
  %}

extern int sum (int a, int b);
extern int diff (int x, int y);
