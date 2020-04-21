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

// BEGIN Non-Swig hand generation stuff
  /* { */
  /*   "iup-set-thunk-callback", IupSetThunkCallback_wrapped, "" */
  /* }, */
  /* { */
  /*   "IupGetAttributeAsString", IupGetAttributeAsString_wrapped, "" */
  /* }, */

typedef struct thunks {
  Ihandle *ih;
  JanetFunction *thunk;
  struct thunks *next;
} thunks_t;

thunks_t * root_thunks = NULL;

void
push_thunk (JanetFunction *f, Ihandle *ih)
{
  fprintf(stderr, "push_thunk was called...addy: %d\n", (int)ih);

  if (NULL == root_thunks)
    {
      root_thunks = malloc (sizeof (thunks_t));
      thunks_t t = { ih, f, NULL };
      memcpy (root_thunks, &t, sizeof (thunks_t));

      return 0;
    }

  thunks_t * node = root_thunks;

  while (NULL != node->next)
    {
      node = node->next;
    }

  thunks_t next = { ih, f, NULL };

  node->next = malloc (sizeof (thunks_t));
  memcpy (node->next, &next, sizeof (thunks_t));
}

JanetFunction *
get_thunk_by_id (Ihandle *ih)
{
  fprintf(stderr, "get_thunk_by_id was called on addy: %d\n", (int)ih);

  thunks_t * node = root_thunks;

  while (NULL != node->next)
    {
      printf ("Looking at a node, ih is %d\n", (int)node->ih);

      if ((int)node->ih == (int)ih)
        {
          break;
        }

      node = node->next;
    }

  if (NULL == node || NULL == node->thunk)
    {
      fprintf(stderr, "CRAP");

      return root_thunks->thunk;
    }

  return node->thunk;
}

static int
call_thunk_N (Ihandle* ih)
{
  int *ptr = &ih;

  printf ("Received Ihandle based call back, handler is: %d \n",
          *ptr);

  janet_call (get_thunk_by_id (ih), 0, NULL);

  return 0;
}

JanetTable * janet_iup_cbs = NULL;

// TODO: Re-implement the thunk storage and calling with something like this.
static int
janet_iup_universal_cb (Ihandle *ih)
{
  JanetFunction *f = janet_unwrap_function (janet_table_get (janet_iup_cbs,
                                                             janet_wrap_pointer (ih)));

  Janet res = janet_call (f, 0, NULL);

  return (int) janet_unwrap_integer (res);
}


static Janet
IupSetThunkCallback_wrapped (int32_t argc, Janet *argv)
{
  janet_fixarity (argc, 3);

  Ihandle * arg_0 = (Ihandle *) janet_getpointer (argv, 0);
  char const * arg_1 = (char const *) janet_getstring (argv, 1);
  Icallback arg_2;

  JanetFunction *f = janet_getfunction (argv, 2);
  // push_thunk (f, arg_0);

  if (NULL == janet_iup_cbs)
    {
      janet_iup_cbs = janet_table (0);
    }

  janet_table_put(janet_iup_cbs, janet_wrap_pointer (arg_0), janet_wrap_function (f));

  // arg_2 = (Icallback) call_thunk_N;
  arg_2 = (Icallback) janet_iup_universal_cb;

  Icallback result = IupSetCallback ((Ihandle *) arg_0, (char const *) arg_1, (Icallback) arg_2);

  return janet_wrap_integer (result);
}

static Janet
IupGetAttributeAsString_wrapped (int32_t argc, Janet *argv)
{
  janet_fixarity (argc, 2);

  Ihandle * arg_0 = (Ihandle *) janet_getpointer (argv, 0);
  char const * arg_1 = (char const *) janet_getstring (argv, 1);

  char * result = IupGetAttribute ((Ihandle *) arg_0, (char const *) arg_1);
  const uint8_t *s = janet_string ((unsigned char *) result, strlen (result));

  return janet_wrap_string (s);
}
// END Non-Swig hand generation stuff


  %}

%include "/usr/include/iup/iup_export.h"
%include "/usr/include/iup/iup.h"
%include "/usr/include/iup/iupdraw.h"

extern int * int_ptr ();
extern char *** char_ptr ();
