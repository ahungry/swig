/* curl.i */
%module curl
%{
  /* Put header files here or function declarations like below */
  #include <curl/curl.h>
  #include <curl/easy.h>

  /* We need a shim function to set opts that are of different types */
  CURL_EXTERN CURLcode curl_easy_setopt_string (CURL *curl, CURLoption option, const char *);

  CURLcode curl_easy_setopt_string (CURL *curl, CURLoption option, const char * s)
  {
    return curl_easy_setopt (curl, option, s);
  }

  CURL_EXTERN CURLcode curl_easy_setopt_pointer (CURL *curl, CURLoption option, struct curl_slist *);

  CURLcode curl_easy_setopt_pointer (CURL *curl, CURLoption option, struct curl_slist * s)
  {
    return curl_easy_setopt (curl, option, s);
  }

// Also, this apparently only works if its NULL initialized and not malloc'ed...
struct curl_slist * new_curl_slist_null () {
  struct curl_slist *x = NULL;

  return x;
}

  %}

%include "/usr/include/curl/curl.h"
%include "/usr/include/curl/easy.h"

CURL_EXTERN CURLcode curl_easy_setopt_string (CURL *curl, CURLoption option, const char * s);
CURL_EXTERN CURLcode curl_easy_setopt_pointer (CURL *curl, CURLoption option, struct curl_slist * s);

// Also, this apparently only works if its NULL initialized and not malloc'ed...
struct curl_slist * new_curl_slist_null () {
  struct curl_slist *x = NULL;

  return x;
}
