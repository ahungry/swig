/* curl.i */
%module curl
%{
  /* Put header files here or function declarations like below */
  #include <curl/curl.h>
  %}

%include "/usr/include/curl/curl.h"
