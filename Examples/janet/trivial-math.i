/* trivial-math.i */
%module trivial_math
%{
  /* Put header files here or function declarations like below */
  /* extern int sum_nums (int a, int b); */
  /* extern int diff_nums (int x, int y); */

  /* // FIXME: Atm the output doesn't create the helpers for this... */
  /* extern struct point { int x; int y; }; */
  /* extern struct point * make_point (int x, int y); */
  /* extern int sum_point (struct point *p); */
  #include "trivial-math.c"
  %}

%include "trivial-math.h"

/* extern int sum_nums (int a, int b); */
/* extern int diff_nums (int x, int y); */
/* // struct point { int x; int y; }; */
/* extern struct point * make_point (int x, int y); */
/* extern int sum_point (struct point *p); */
