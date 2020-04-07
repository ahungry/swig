struct point {
  int x;
  int y;
};

typedef struct pointed {
  int a;
  int b;
} pointed_t;

extern int sum_nums (int a, int b);
extern int diff_nums (int x, int y);
extern struct point * make_point (int x, int y);
extern int sum_point (struct point *p);
