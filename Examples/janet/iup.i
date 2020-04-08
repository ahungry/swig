/* iup.i */
%module iup
%{
  /* Put header files here or function declarations like below */
  #include <iup.h>

  int * int_ptr ()
  {
    int *x = NULL;

    return x;
  }

  char *** char_ptr ()
  {
    char ***x = NULL;

    return x;
  }
  %}

%include "/usr/include/iup/iup_export.h"
%include "/usr/include/iup/iup.h"

extern int * int_ptr ();
extern char *** char_ptr ();
