/* sqlite3.i */
%module sqlite3
%{
  /* Put header files here or function declarations like below */
  #include <sqlite3.h>
  %}

%include "/usr/include/sqlite3.h"
