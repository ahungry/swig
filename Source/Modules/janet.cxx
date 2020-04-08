#include "swigmod.h"

static int in_class = 0;
static int in_constructor = 0;
static int in_destructor = 0;
static int in_special_setter = 0;

class JANET : public Language
{
private:
  String *getAccessor (String *s1, String *s);
  String *getRetvalAccessor (String *s);
  String *getStructOrTypedef (Node *n);
  String *lastFour (String *s);
  int isSetter (String *s);
  int isGetter (String *s);

protected:
  File *f_begin;
  File *f_runtime;
  File *f_header;
  File *f_wrappers;
  File *f_init;
  File *f_inline_class;
  File *f_inline_getter;
  File *f_inline_setter;

public:

  virtual void main (int argc, char *argv[])
  {
    printf ("I'm the Janet module.\n");

    // CLI arg parsing.
    for (int i = 0; i < argc; i++)
      {
        if (argv[i])
          {
            if (strcmp (argv[i], "-test") == 0)
              {
                printf ("you called with the -test flag. It does nothing atm.\n");
                Swig_mark_arg (i);
              }
          }
      }

    // 35.10.4 Config and preprocessor
    SWIG_library_directory ("janet");
    Preprocessor_define ("SWIGJANET 1", 0);
    SWIG_config_file ("janet.swg");
    SWIG_typemap_lang ("janet");
  }

  virtual int top (Node *n)
  {
    printf ("Generating code.\n");

    String *module  = Getattr (n, "name");
    String *outfile = Getattr (n, "outfile");

    // Init I/O
    f_begin = NewFile (outfile, "w", SWIG_output_files ());

    if (!f_begin)
      {
        FileErrorDisplay (outfile);
        SWIG_exit (EXIT_FAILURE);
      }

    f_runtime  = NewString ("");
    f_init     = NewString ("");
    f_header   = NewString ("");
    f_wrappers = NewString ("");

    f_inline_class  = NewString ("");
    f_inline_getter = NewString ("");
    f_inline_setter = NewString ("");

    // Register file targets with the SWIG file handler
    Swig_register_filebyname ("begin"   , f_begin);
    Swig_register_filebyname ("header"  , f_header);
    Swig_register_filebyname ("wrapper" , f_wrappers);
    Swig_register_filebyname ("runtime" , f_runtime);
    Swig_register_filebyname ("init"    , f_init);

    Swig_register_filebyname ("inline_class", f_inline_class);
    Swig_register_filebyname ("inline_getter", f_inline_getter);
    Swig_register_filebyname ("inline_setter", f_inline_setter);

    // Output module init code
    Swig_banner (f_begin);

    // Runtime comes first, then f_wrappers, and lastly f_init.
    Printf (f_runtime, "#include <janet.h>\n\n");

    Printf (f_init, "static const JanetReg\n");
    Printf (f_init, "cfuns[] = {\n");

    // Emit code for children
    Language::top (n);

    Printf (f_init, "  {NULL, NULL, NULL}\n");
    Printf (f_init, "};\n");
    Printf (f_init, "\nJANET_MODULE_ENTRY (JanetTable *env) {\n");
    // TODO: Allow the module name to be a user input flag.
    Printf (f_init, "  janet_cfuns (env, \"%s\", cfuns);\n", module);
    Printf (f_init, "}\n");

    // Write all to the files
    Dump (f_runtime  , f_begin);
    Dump (f_header   , f_begin);
    Dump (f_wrappers , f_begin);
    Wrapper_pretty_print (f_init, f_begin);

    // Cleanup files
    Delete (f_runtime);
    Delete (f_header);
    Delete (f_wrappers);
    Delete (f_init);

    Delete (f_inline_class);
    Delete (f_inline_getter);
    Delete (f_inline_setter);

    // Why is this line in the guide? Function doesn't exist.
    // Close (f_begin);
    Delete (f_begin);

    return SWIG_OK;
  }

  virtual int constructorHandler (Node *n)
  {
    String   *name    = Getattr (n, "sym:name");
    SwigType *type    = Getattr (n, "type");
    ParmList *parms   = Getattr (n, "parms");

    Node     *parent  = Getattr (n, "parentNode");
    String   *strukt  = this->getStructOrTypedef (parent);

    // printf ("In constructorHandler....\n");
    // Printf (f_runtime, "constructorHandler -- : %s / %s / %s", name, type, parms);

    // Create a new factory function
    // Printf (f_inline_class, "\n%s %s * new_%s () {\n", strukt, name, name);
    Printf (f_inline_class, "  %s %s *result;\n\n", strukt, name);
    Printf (f_inline_class, "  if (with_malloc)\n    {\n");
    Printf (f_inline_class, "      result = malloc (sizeof (%s %s));\n", strukt, name);
    Printf (f_inline_class, "    }\n  else\n    {\n");
    Printf (f_inline_class, "      result = NULL;\n", strukt, name);
    Printf (f_inline_class, "    }\n ");
    // Printf (f_inline_class, "  return x;\n");
    // Printf (f_inline_class, "}\n\n");

    // wrapperType = membervar;
    in_constructor = 1;
    Language::constructorHandler(n);
    in_constructor = 0;
    // wrapperType = standard;

    return SWIG_OK;
  }


  virtual int destructorHandler (Node *n)
  {
    String   *name    = Getattr (n, "sym:name");
    SwigType *type    = Getattr (n, "type");
    ParmList *parms   = Getattr (n, "parms");

    Node     *parent  = Getattr (n, "parentNode");
    String   *strukt  = this->getStructOrTypedef (parent);

    // Create a new delete function
    // Printf (f_inline_class, "\nvoid delete_%s (%s %s *x) {\n", name, strukt, name);
    Printf (f_inline_class, "  free (arg_0);\n");
    //Printf (f_inline_class, "}\n\n");

    // printf ("In destructorHandler....\n");
    // Printf (f_runtime, "destructorHandler -- : %s / %s / %s", name, type, parms);

    // wrapperType = membervar;
    Language::destructorHandler(n);
    // wrapperType = standard;

    return SWIG_OK;
  }


  virtual int classHandler (Node *n)
  {
    String   *name    = Getattr (n, "sym:name");
    SwigType *type    = Getattr (n, "type");
    ParmList *parms   = Getattr (n, "parms");

    // printf ("In classHandler....\n");
    // Printf (f_runtime, "classHandler -- : %s / %s / %s", name, type, parms);

    // wrapperType = membervar;
    in_class = 1;

    Language::classHandler(n);

    in_class = 0;
    // wrapperType = standard;

    return SWIG_OK;
  }

  virtual int membervariableHandler (Node *n)
  {
    Node     *parent  = Getattr (n, "parentNode");
    String   *name    = Getattr (n, "sym:name");
    SwigType *type    = Getattr (n, "type");
    String   *typex   = NewString(SwigType_str (type, ""));
    ParmList *parms   = Getattr (n, "parms");

    String   *parentName = Getattr (parent, "sym:name");
    String   *strukt     = this->getStructOrTypedef (parent);

    // Do we need to empty them out so only the last one pre-print is kept?
    f_inline_getter = NewString ("");
    f_inline_setter = NewString ("");

    if (Strcmp (strukt, "union") == 0 ||
        Strcmp (strukt, "struct") == 0)
      {
        in_special_setter = 1;

        // This is working around struct<anonymous> to struct<anonymous> compile complaints
        Printf (f_inline_getter, "\n  %s * result;\n", typex);
        Printf (f_inline_getter, "  result = (%s *)& ((arg_0)->%s);\n", typex, name);

        // Printf (f_inline_setter, "\n  arg_0->%s = arg_1;\n", name);
        Printf (f_inline_setter, "  %s *ptr;\n\n", typex);
        Printf (f_inline_setter, "  ptr = (%s *) &arg_1;\n", typex);
        Printf (f_inline_setter, "  arg_1 = *ptr;\n");
        Printf (f_inline_setter, "\n  arg_0->%s = arg_1;\n", name);
      }
    else
      {
        // Make the getter
        Printf (f_inline_getter, "\n  %s result;\n", typex);
        Printf (f_inline_getter, "  result = arg_0->%s;\n", name);

        // Make the setter
        Printf (f_inline_setter, "\n // %s\n", name);
        Printf (f_inline_setter, "\n  arg_0->%s = arg_1;\n", name);
      }

    Language::membervariableHandler(n);

    return SWIG_OK;
  }

  virtual int constantWrapper (Node *n)
  {
    String   *name    = Getattr (n, "sym:name");
    SwigType *type    = Getattr (n, "type");
    ParmList *parms   = Getattr (n, "parms");
    String   *parmstr = ParmList_str_defaultargs (parms); // to string
    String   *func    = SwigType_str (type, NewStringf ("%s(%s)", name, parmstr));
    String   *action  = Getattr (n, "wrap:action");
    // int       arity   = ParmList_numrequired (parms);
    int       arity   = 0;

    int i;
    Parm *p;

    Printf (f_wrappers, "static Janet\n");
    Printf (f_wrappers, "const_%s_wrapped (int32_t argc, const Janet *argv)\n", name);
    Printf (f_wrappers, "{\n");
    Printf (f_wrappers, "  janet_fixarity (argc, %d);\n\n", arity);

    // Evaluate the result
    Printf (f_wrappers, "\n  %s result = %s;\n",
            SwigType_str (type, ""), name);

    // Need to do dynamic return values
    // https://janet-lang.org/capi/wrapping.html
    Printf (f_wrappers, "\n  return ");

    String *p_retval = this->getRetvalAccessor (type);

    // Printf (f_wrappers, "janet_wrap_<%s>", SwigType_str (type, ""), name);
    if (Strcmp (p_retval, "nil") == 0) {
      Printf (f_wrappers, "janet_wrap_nil ();\n");
    } else {
      // This works fine for non-void, for void it has to be different...
      Printf (f_wrappers, "janet_wrap_%s", p_retval);
      Printf (f_wrappers, " (result);\n");
    }

    Printf (f_wrappers, "}\n\n");

    String *fn_name = NewString (name);
    Replaceall (fn_name, "_", "-");

    Printf (f_init, "{\"const-%s\", const_%s_wrapped, \"Return the constant value.\"},",
            fn_name,
            name);

    return SWIG_OK;
  }

  virtual int functionWrapper (Node *n)
  {
    String   *name    = Getattr (n, "sym:name");
    SwigType *type    = Getattr (n, "type");
    ParmList *parms   = Getattr (n, "parms");
    String   *parmstr = ParmList_str_defaultargs (parms); // to string
    String   *func    = SwigType_str (type, NewStringf ("%s(%s)", name, parmstr));
    String   *action  = Getattr (n, "wrap:action");
    int       arity   = ParmList_numrequired (parms);

    int i;
    Parm *p;

    Printf (f_wrappers, "static Janet\n");
    Printf (f_wrappers, "%s_wrapped (int32_t argc, const Janet *argv)\n", name);
    Printf (f_wrappers, "{\n");

    String *last_type     = NewString ("");
    String *last_accessor = NewString ("");

    if (in_constructor)
      {
        Printf (f_wrappers, "  janet_fixarity (argc, 1);\n\n");
        Printf (f_wrappers, "  int with_malloc = janet_getinteger (argv, 0);\n\n");
      }
    else
      {
        Printf (f_wrappers, "  janet_fixarity (argc, %d);\n\n", arity);

        // TODO: Iterate each item in parm list and use the janet accessor
        for (i = 0, p = parms; i < arity; i++)
          {
            String *p_type   = Getattr (p, "type");
            // String *p_name   = NewStringf ("%s_%d", Getattr (p, "name"), i);
            String *p_name   = NewStringf ("arg_%d", i);
            String *p_typex  = NewString (SwigType_str (p_type, ""));
            // String *accessor = this->getAccessor (p_typex);
            String *accessor = this->getAccessor (p_type, p_typex);

            // FIXME: Quick fix for variadic args (...)
            if (Strcmp (p_typex, "...") == 0)
              {
                p_typex = NewString (last_type);
                accessor = NewString (last_accessor);
              }

            // Pull value out of Janet args and into a local var
            Printf (f_wrappers,
                    "  %s %s = %s (argv, %d);\n",
                    p_typex,
                    p_name,
                    accessor,
                    i);

            last_type     = NewString (p_typex);
            last_accessor = NewString (accessor);

            p = nextSibling (p);
          }
    }

    // Calling the dynamic getter/setter doesn't work well because
    // of struct<anonymous> incompatability with struct<anonymous>
    if (in_class)
      {
        Dump (f_inline_class, f_wrappers);
        f_inline_class = NewString ("");

        if (this->isSetter (name))
          {
            Dump (f_inline_setter, f_wrappers);
            f_inline_setter = NewString ("");
            in_special_setter = 0;
          }

        if (this->isGetter (name))
          {
            Dump (f_inline_getter, f_wrappers);
            f_inline_getter = NewString ("");
          }
      }
    else
      {
        // Evaluate the result

        if (Strcmp (type, "void") == 0) {
          Printf (f_wrappers, "\n  %s (", name);
        } else {
          Printf (f_wrappers, "\n  %s result = %s (",
                  SwigType_str (type, ""), name);
        }

        last_type = NewString ("");

        // Print each argument in the call
        for (i = 0, p = parms; i < arity; i++)
          {
            String *p_type   = Getattr (p, "type");
            // String *p_name   = NewStringf ("%s_%d", Getattr (p, "name"), i);
            String *p_name   = NewStringf ("arg_%d", i);
            String *p_typex  = NewString (SwigType_str (p_type, ""));

            // FIXME: Quick fix for variadic args (...)
            if (Strcmp (p_typex, "...") == 0)
              {
                p_typex = NewString (last_type);
              }

            Printf (f_wrappers, "(%s) %s", p_typex, p_name);

            if (i + 1 < arity) {
              Printf (f_wrappers, ", ");
            }

            last_type = NewString (p_typex);

            p = nextSibling (p);
          }

        // End the call
        Printf (f_wrappers, ");\n",
                SwigType_str (type, ""), name);

      }

    // Need to do dynamic return values
    // https://janet-lang.org/capi/wrapping.html
    Printf (f_wrappers, "\n  return ");

    String *p_retval = this->getRetvalAccessor (type);

    // Printf (f_wrappers, "janet_wrap_<%s>", SwigType_str (type, ""), name);
    if (Strcmp (p_retval, "nil") == 0) {
      Printf (f_wrappers, "janet_wrap_nil ();\n");
    } else {
      // This works fine for non-void, for void it has to be different...
      Printf (f_wrappers, "janet_wrap_%s", p_retval);
      Printf (f_wrappers, " (result);\n");
    }

    Printf (f_wrappers, "}\n\n");

    String *fn_name = NewString (name);
    Replaceall (fn_name, "_", "-");

    Printf (f_init, "{\"%s\", %s_wrapped, \"SWIG generated\"},",
            fn_name,
            name);

    return SWIG_OK;
  }

};

// Helper things - I should probably be using the TYPEMAP features?
// http://www.swig.org/Doc1.3/Extending.html
// See more Janet accessors here:
// https://janet-lang.org/capi/writing-c-functions.html
String *
JANET::getAccessor (String *s1, String *s)
{
  String *start = NewStringWithSize (s1, 2);

  if (Strcmp (s, "int") == 0)
    {
      return NewString ("janet_getinteger");
    }

  if (Strcmp (s, "double") == 0)
    {
      return NewString ("janet_getnumber");
    }

  if (Strcmp (s, "float") == 0)
    {
      return NewString ("janet_getnumber");
    }

  if (Strcmp (s, "char const *") == 0)
    {
      return NewStringf ("(%s) janet_getstring", SwigType_str (s, ""));
    }

  if (Strcmp (start, "p.") == 0)
    {
      return NewStringf ("(%s) janet_getpointer", SwigType_str (s, ""));
    }

  if (Strcmp (start, "s.") == 0)
    {
      return NewStringf ("(%s) janet_getpointer", SwigType_str (s, ""));
    }

  printf ("getAccessor default went to this: \n");
  Printf (stdout, "  ---  %s\n", s1);
  Printf (stdout, "  ---  %s\n", s);

  if (Strcmp (s, "int") == 0 ||
      Strcmp (s, "unsigned int") == 0 ||
      Strcmp (s, "long") == 0)
    {
      return NewString ("janet_getinteger");
    }

  String *maybeCustomStruct = NewStringWithSize (s1, 6);

  if (Strcmp (maybeCustomStruct, "struct") == 0)
    {
      if (in_special_setter)
        {
          return NewStringf ("*(%s *) janet_getpointer", SwigType_str (s, ""));
        }
      else
        {
          return NewStringf ("(%s) janet_getpointer", SwigType_str (s, ""));

        }
    }

  // FIXME: Come up with an appropriate last error clause
  // return NewString ("janet_unknown");
  // return s;
  // return NewStringf ("(%s) janet_getpointer", SwigType_str (s, ""));
  return NewString ("janet_getinteger");
}

// https://janet-lang.org/capi/wrapping.html
String *
JANET::getRetvalAccessor (String *s)
{
  String *start = NewStringWithSize (s, 2);

  if (Strcmp (start, "p.") == 0)
    {
      return NewString ("pointer");
    }

  if (Strcmp (start, "s.") == 0)
    {
      return NewString ("pointer");
    }

  if (Strcmp (s, "double") == 0)
    {
      return NewString ("number");
    }

  if (Strcmp (s, "float") == 0)
    {
      return NewString ("number");
    }

  if (Strcmp (s, "int") == 0 ||
      Strcmp (s, "unsigned int") == 0 ||
      Strcmp (s, "long") == 0)
    {
      return NewString ("integer");
    }

  if (Strcmp (s, "void") == 0)
    {
      return NewString ("nil");
    }

  String *maybeCustomStruct = NewStringWithSize (s, 6);

  if (Strcmp (maybeCustomStruct, "struct") == 0)
    {
      return NewString ("pointer");
    }

  printf ("getRetvalAccessor default went to this: \n");
  Printf (stdout, "  ---  %s\n", s);

  // Default likely means a custom typedef
  // FIXME: Do we need to look up the parent types?
  // Perhaps just int is fine...
  return NewString ("integer");
  // return NewString ("pointer");
}

// Again, more hack job.  I guess we may need to add on 'struct'
// qualifier when it is *not* a typedef, else nothing.
String *
JANET::getStructOrTypedef (Node *n)
{
  String *kind   = Getattr (n, "kind");
  String *tdname = Getattr (n, "tdname");

  if (!tdname || Strcmp (tdname, "") == 0)
    {
      return NewString (kind);
    }

  return NewString ("");
}

String *
JANET::lastFour (String *s)
{
  char *cname = Char (s);
  int len = strlen (cname);

  return NewString (cname + len - 4);
}

int
JANET::isSetter (String *s)
{
  return Strcmp (this->lastFour (s), "_set") == 0;
}

int
JANET::isGetter (String *s)
{
  return Strcmp (this->lastFour (s), "_get") == 0;
}

extern "C" Language *
swig_janet (void)
{
  return new JANET ();
}
