#include "swigmod.h"

class JANET : public Language
{
private:
  String *getAccessor (String *s);

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

    Printf (f_init, "static const JanetReg\n");
    Printf (f_init, "cfuns[] = {\n");

    // Emit code for children
    Language::top (n);

    Printf (f_init, "  {NULL, NULL, NULL}\n");
    Printf (f_init, "}\n");
    Printf (f_init, "\nJANET_MODULE_ENTRY (JanetTable *env) {\n");
    // TODO: Allow the module name to be a user input flag.
    Printf (f_init, "  janet_cfuns (env, \"swig\", cfuns);\n");
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
        String *accessor = this->getAccessor (p_type);

        // Pull value out of Janet args and into a local var
        Printf (f_wrappers,
                "  %s %s = %s (argv, %d);\n",
                p_type,
                p_name,
                accessor,
                i);

        p = nextSibling (p);
      }

    // Evaluate the result
    Printf (f_wrappers, "\n  %s result = %s (a1);\n",
            SwigType_str (type, ""), name);

    Printf (f_wrappers, "\n  return janet_wrap_int (result);\n");
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
String *
JANET::getAccessor (String *s)
{
  if (Strcmp (s, "int") == 0) {
    return NewString ("janet_getinteger");
  }

  // FIXME: Come up with an appropriate last error clause
  // return NewString ("janet_unknown");
  return s;
}

extern "C" Language *
swig_janet (void)
{
  return new JANET ();
}
