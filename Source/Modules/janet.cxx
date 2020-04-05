#include "swigmod.h"

class JANET : public Language
{
private:
  String *getAccessor (String *s);
  String *getRetvalAccessor (String *s);

protected:
  File *f_begin;
  File *f_runtime;
  File *f_header;
  File *f_wrappers;
  File *f_init;

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

    // Register file targets with the SWIG file handler
    Swig_register_filebyname ("begin"   , f_begin);
    Swig_register_filebyname ("header"  , f_header);
    Swig_register_filebyname ("wrapper" , f_wrappers);
    Swig_register_filebyname ("runtime" , f_runtime);
    Swig_register_filebyname ("init"    , f_init);

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

    printf ("In constructorHandler....\n");
    // Printf (f_runtime, "constructorHandler -- : %s / %s / %s", name, type, parms);

    // Create a new factory function
    Printf (f_runtime, "\nstruct %s * new_%s () {\n", name, name);
    Printf (f_runtime, "  struct %s *x = malloc (sizeof (struct %s));\n\n", name, name);
    Printf (f_runtime, "  return x;\n");
    Printf (f_runtime, "}\n");

    // wrapperType = membervar;
    Language::constructorHandler(n);
    // wrapperType = standard;

    return SWIG_OK;
  }


  virtual int destructorHandler (Node *n)
  {
    String   *name    = Getattr (n, "sym:name");
    SwigType *type    = Getattr (n, "type");
    ParmList *parms   = Getattr (n, "parms");

    // Create a new delete function
    Printf (f_runtime, "\ndelete_%s (struct %s *x) {\n", name, name);
    Printf (f_runtime, "  free (x);\n");
    Printf (f_runtime, "}\n");

    printf ("In destructorHandler....\n");
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

    printf ("In classHandler....\n");
    // Printf (f_runtime, "classHandler -- : %s / %s / %s", name, type, parms);

    // wrapperType = membervar;
    Language::classHandler(n);
    // wrapperType = standard;

    return SWIG_OK;
  }

  virtual int membervariableHandler (Node *n)
  {
    Node     *parent  = Getattr (n, "parentNode");
    String   *name    = Getattr (n, "sym:name");
    SwigType *type    = Getattr (n, "type");
    // String   *typex   = NewString(SwigType_str (type, ""));
    ParmList *parms   = Getattr (n, "parms");

    String *parentName  = Getattr (parent, "sym:name");
    String *parentType  = Getattr (parent, "type");
    // String *parentTypex = NewString(SwigType_str (parentType, ""));

    // we need to define a get and a set for these...
    // If it was a struct like point->x we end up with point_x_get/1 and
    // also a point_x_set/2 respectively

    // Make the getter
    Printf (f_runtime, "\n%s %s_%s_get (struct %s *x) {\n",
            type, parentName, name, parentName);
    Printf (f_runtime, "  return x->%s;\n", name);
    Printf (f_runtime, "}\n");

    // Make the setter
    Printf (f_runtime, "\nvoid %s_%s_set (struct %s *x, %s v) {\n",
            parentName, name, parentName, type);
    Printf (f_runtime, "  x->%s = v;\n", name);
    Printf (f_runtime, "}\n");

    printf ("In membervariableHandler....\n");
    Printf (f_runtime, "// membervariableHandler -- : %s / %s / %s\n\n",
            name, type, parentName);

    // wrapperType = membervar;
    Language::membervariableHandler(n);
    // wrapperType = standard;

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
    Printf (f_wrappers, "  janet_fixarity (argc, %d);\n\n", arity);

    // TODO: Iterate each item in parm list and use the janet accessor
    for (i = 0, p = parms; i < arity; i++)
      {
        String *p_type   = Getattr (p, "type");
        String *p_name   = Getattr (p, "name");
        String *p_typex  = NewString(SwigType_str (p_type, ""));
        String *accessor = this->getAccessor (p_typex);

        // Pull value out of Janet args and into a local var
        Printf (f_wrappers,
                "  %s %s = %s (argv, %d);\n",
                p_typex,
                p_name,
                accessor,
                i);

        p = nextSibling (p);
      }

    // Evaluate the result
    Printf (f_wrappers, "\n  %s result = %s (",
            SwigType_str (type, ""), name);

    // Print each argument in the call
    for (i = 0, p = parms; i < arity; i++)
      {
        String *p_type   = Getattr (p, "type");
        String *p_name   = Getattr (p, "name");
        String *p_typex  = NewString(SwigType_str (p_type, ""));

        Printf (f_wrappers, "(%s) %s", p_typex, p_name);

        if (i + 1 < arity) {
          Printf (f_wrappers, ", ");
        }

        p = nextSibling (p);
      }

    // End the call
    Printf (f_wrappers, ");\n",
            SwigType_str (type, ""), name);

    // Need to do dynamic return values
    // https://janet-lang.org/capi/wrapping.html
    Printf (f_wrappers, "\n  return ");

    String *p_retval = this->getRetvalAccessor (type);
    // Printf (f_wrappers, "janet_wrap_<%s>", SwigType_str (type, ""), name);
    Printf (f_wrappers, "janet_wrap_%s", p_retval);
    Printf (f_wrappers, " (result);\n");
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
JANET::getAccessor (String *s)
{
  if (Strcmp (s, "int") == 0) {
    return NewString ("janet_getinteger");
  }

  if (Strcmp (s, "double") == 0) {
    return NewString ("janet_getnumber");
  }

  if (Strcmp (s, "float") == 0) {
    return NewString ("janet_getnumber");
  }

  // FIXME: Come up with an appropriate last error clause
  // return NewString ("janet_unknown");
  // return s;
  return NewStringf ("(%s) janet_getpointer", SwigType_str (s, ""));
}

// https://janet-lang.org/capi/wrapping.html
String *
JANET::getRetvalAccessor (String *s)
{
  if (Strcmp (s, "p.") == 0) {
    return NewString ("pointer");
  }

  if (Strcmp (s, "double") == 0) {
    return NewString ("number");
  }

  if (Strcmp (s, "float") == 0) {
    return NewString ("number");
  }

  if (Strcmp (s, "int") == 0) {
    return NewString ("integer");
  }

  // TODO: Handle void type
  return NewString ("pointer");
}

extern "C" Language *
swig_janet (void)
{
  return new JANET ();
}
