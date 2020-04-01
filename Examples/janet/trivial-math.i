/* trivial-math.i */
%module trivial_math
%{
  /* Put header files here or function declarations like below */
  extern int sum_nums (int a, int b);
  extern int diff_nums (int x, int y);
  %}

extern int sum_nums (int a, int b);
extern int diff_nums (int x, int y);
