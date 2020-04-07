/* File : trivial-math.c */
#include <stdlib.h>
#include <stdio.h>

#include "trivial-math.h"

int sum_nums (int a, int b) {
  return a + b;
}

int diff_nums (int x, int y) {
  return x - y;
}

struct point *
make_point (int x, int y)
{
  struct point *p = malloc (sizeof (struct point));

  p->x = x;
  p->y = y;

  return p;
}

int
sum_point (struct point *p)
{
  return p->x + p->y;
}

int
main (int argc, char *argv[])
{
  struct point *p1 = make_point (3, 4);

  printf ("Sum is: %d", sum_point (p1));

  free (p1);


  point_pointed_t ppt;
  ppt.upoint.x = 3;
  ppt.upointed.a = 5;

  printf ("ppt is: %d\n", ppt.upointed.a);

  return 0;
}

some_nums_t
get_snt_ten ()
{
  return TEN;
}
