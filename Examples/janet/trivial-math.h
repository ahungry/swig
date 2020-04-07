struct point {
  int x;
  int y;
};

typedef struct pointed {
  int a;
  int b;
} pointed_t;

typedef enum some_nums {
    TEN  = 10,
    TWENTY  = 20
} some_nums_t;

extern int sum_nums (int a, int b);
extern int diff_nums (int x, int y);
extern struct point * make_point (int x, int y);
extern int sum_point (struct point *p);
extern some_nums_t get_snt_ten ();
