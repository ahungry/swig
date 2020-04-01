#include "swigmod.h"

class JANET : public Language
{
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

    Printf (f_wrappers, "\nstatic Janet\n");
    Printf (f_wrappers, "%s_wrapped (int32_t argc, const Janet *argv)\n", name);
    Printf (f_wrappers, "{\n");
    Printf (f_wrappers, "  janet_fixarity (argc, %d);\n\n", arity);

    // TODO: Iterate each item in parm list and use the janet accessor
    Printf (f_wrappers, "  int a1 = janet_getinteger (argv, 0);\n");

    Printf (f_wrappers, "  %s result = %s (a1);\n",
            SwigType_str (type, ""), name);

    Printf (f_wrappers, "\n  return janet_wrap_int (result);\n");
    Printf (f_wrappers, "}\n");

    // Printf (f_wrappers, "functionWrapper   : %s\n", func);
    // Printf (f_wrappers, "           action : %s\n", action);
    // Printf (f_wrappers, "           arity  : %d\n", arity);

    Printf (f_init, "{\"%s\", %s_wrapped, \"SWIG generated\"},",
            Replaceall (name, "_", "-"), name);

    return SWIG_OK;
  }

};

extern "C" Language *
swig_janet (void)
{
  return new JANET ();
}
