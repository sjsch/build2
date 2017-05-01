// file      : build2/utility.cxx -*- C++ -*-
// copyright : Copyright (c) 2014-2017 Code Synthesis Ltd
// license   : MIT; see accompanying LICENSE file

#include <build2/utility.hxx>

#include <time.h>   // tzset()

#include <cstring>  // strlen(), str[n]cmp()
#include <cstdlib>  // strtol()
#include <iostream> // cerr

#include <build2/variable.hxx>
#include <build2/diagnostics.hxx>

using namespace std;
using namespace butl;

//
// <build2/types.hxx>
//
namespace build2
{
  static const char* const run_phase_[] = {"load", "match", "execute"};

  ostream&
  operator<< (ostream& os, run_phase p)
  {
    return os << run_phase_[static_cast<uint8_t> (p)];
  }
}

namespace std
{
  ostream&
  operator<< (ostream& os, const ::butl::path& p)
  {
    using namespace build2;

    return os << (stream_verb (os) < 2
                  ? diag_relative (p)
                  : p.representation ());
  }

  ostream&
  operator<< (ostream& os, const ::butl::process_path& p)
  {
    using namespace build2;

    if (p.empty ())
      os << "<empty>";
    else
    {
      // @@ Is there a reason not to print as a relative path as it is done
      //    for path (see above)?
      //
      os << p.recall_string ();

      if (!p.effect.empty ())
        os << '@' << p.effect.string (); // Suppress relative().
    }

    return os;
  }
}

namespace build2
{
  //
  // <build2/utility.hxx>
  //

  string&
  trim (string& l)
  {
    /*
    assert (trim (r = "") == "");
    assert (trim (r = " ") == "");
    assert (trim (r = " \t\r") == "");
    assert (trim (r = "a") == "a");
    assert (trim (r = " a") == "a");
    assert (trim (r = "a ") == "a");
    assert (trim (r = " \ta") == "a");
    assert (trim (r = "a \r") == "a");
    assert (trim (r = " a ") == "a");
    assert (trim (r = " \ta \r") == "a");
    */

    size_t i (0), n (l.size ());

    for (char c;
         i != n && ((c = l[i]) == ' ' || c == '\t' || c == '\r');
         ++i) ;

    for (char c;
         n != i && ((c = l[n - 1]) == ' ' || c == '\t' || c == '\r');
         --n) ;

    if (i != 0)
    {
      string s (l, i, n - i);
      l.swap (s);
    }
    else if (n != l.size ())
      l.resize (n);

    return l;
  }

  options ops;
  process_path argv0;

  const standard_version build_version (BUILD2_VERSION_STR);

  void
  check_build_version (const standard_version_constraint& c, const location& l)
  {
    if (!c.satisfies (build_version))
      fail (l) << "incompatible build2 version" <<
        info << "running " << build_version.string () <<
        info << "required " << c.string ();
  }

  dir_path work;
  dir_path home;
  const dir_path* relative_base = &work;

  string
  diag_relative (const path& p, bool cur)
  {
    if (p.string () == "-")
      return "<stdin>";

    const path& b (*relative_base);

    if (p.absolute ())
    {
      if (p == b)
        return cur ? "." + p.separator_string () : string ();

#ifndef _WIN32
      if (!home.empty ())
      {
        if (p == home)
          return "~" + p.separator_string ();
      }
#endif

      path rb (relative (p));

#ifndef _WIN32
      if (!home.empty ())
      {
        if (rb.relative ())
        {
          // See if the original path with the ~/ shortcut is better that the
          // relative to base.
          //
          if (p.sub (home))
          {
            path rh (p.leaf (home));
            if (rb.size () > rh.size () + 2) // 2 for '~/'
              return "~/" + move (rh).representation ();
          }
        }
        else if (rb.sub (home))
          return "~/" + rb.leaf (home).representation ();
      }

#endif

      return move (rb).representation ();
    }

    return p.representation ();
  }

  process_path
  run_search (const char*& args0)
  try
  {
    return process::path_search (args0);
  }
  catch (const process_error& e)
  {
    fail << "unable to execute " << args0 << ": " << e << endf;
  }

  process_path
  run_search (const path& f, bool init, const dir_path& fallback)
  try
  {
    return process::path_search (f, init, fallback);
  }
  catch (const process_error& e)
  {
    fail << "unable to execute " << f << ": " << e << endf;
  }

  process
  run_start (const process_path& pp, const char* args[], bool err)
  {
    assert (args[0] == pp.recall_string ());

    if (verb >= 3)
      print_process (args);

    try
    {
      return process (pp, args, 0, -1, (err ? 2 : 1));
    }
    catch (const process_error& e)
    {
      if (e.child)
      {
        // Note: run_finish() expects this exact message.
        //
        cerr << "unable to execute " << args[0] << ": " << e << endl;
        exit (1);
      }
      else
        fail << "unable to execute " << args[0] << ": " << e << endf;
    }
  }

  bool
  run_finish (const char* args[], bool err, process& pr, const string& l)
  try
  {
    if (pr.wait ())
      return true;

    if (err)
      // Assuming diagnostics has already been issued (to STDERR).
      //
      throw failed ();

    // Even if the user asked to suppress diagnostiscs, one error that we
    // want to let through is the inability to execute the program itself.
    // We cannot reserve a special exit status to signal this so we will
    // just have to compare the output. This particular situation will
    // result in a single error line printed by run_start() above.
    //
    if (l.compare (0, 18, "unable to execute ") == 0)
      fail << l;

    return false;
  }
  catch (const process_error& e)
  {
    fail << "unable to execute " << args[0] << ": " << e << endf;
  }

  const string empty_string;
  const path empty_path;
  const dir_path empty_dir_path;

  void
  append_options (cstrings& args, const lookup& l)
  {
    if (l)
      append_options (args, cast<strings> (l));
  }

  void
  append_options (strings& args, const lookup& l)
  {
    if (l)
      append_options (args, cast<strings> (l));
  }

  void
  hash_options (sha256& csum, const lookup& l)
  {
    if (l)
      hash_options (csum, cast<strings> (l));
  }

  void
  append_options (cstrings& args, const strings& sv)
  {
    if (!sv.empty ())
    {
      args.reserve (args.size () + sv.size ());

      for (const string& s: sv)
        args.push_back (s.c_str ());
    }
  }

  void
  append_options (strings& args, const strings& sv)
  {
    if (!sv.empty ())
    {
      args.reserve (args.size () + sv.size ());

      for (const string& s: sv)
        args.push_back (s);
    }
  }

  void
  hash_options (sha256& csum, const strings& sv)
  {
    for (const string& s: sv)
      csum.append (s);
  }

  bool
  find_option (const char* o, const lookup& l, bool ic)
  {
    return l && find_option (o, cast<strings> (l), ic);
  }

  bool
  find_option (const char* o, const strings& strs, bool ic)
  {
    for (const string& s: strs)
      if (ic ? casecmp (s, o) == 0 : s == o)
        return true;

    return false;
  }

  bool
  find_option (const char* o, const cstrings& cstrs, bool ic)
  {
    for (const char* s: cstrs)
      if (s != nullptr && (ic ? casecmp (s, o) : strcmp (s, o)) == 0)
        return true;

    return false;
  }

  bool
  find_options (initializer_list<const char*> os, const lookup& l, bool ic)
  {
    return l && find_options (os, cast<strings> (l), ic);
  }

  bool
  find_options (initializer_list<const char*> os, const strings& strs, bool ic)
  {
    for (const string& s: strs)
      for (const char* o: os)
        if (ic ? casecmp (s, o) == 0 : s == o)
          return true;

    return false;
  }

  bool
  find_options (initializer_list<const char*> os,
                const cstrings& cstrs,
                bool ic)
  {
    for (const char* s: cstrs)
      if (s != nullptr)
        for (const char* o: os)
          if ((ic ? casecmp (s, o) : strcmp (s, o)) == 0)
            return true;

    return false;
  }

  bool
  find_option_prefix (const char* p, const lookup& l, bool ic)
  {
    return l && find_option_prefix (p, cast<strings> (l), ic);
  }

  bool
  find_option_prefix (const char* p, const strings& strs, bool ic)
  {
    size_t n (strlen (p));

    for (const string& s: strs)
      if ((ic ? casecmp (s, p, n) : s.compare (0, n, p)) == 0)
        return true;

    return false;
  }

  bool
  find_option_prefix (const char* p, const cstrings& cstrs, bool ic)
  {
    size_t n (strlen (p));

    for (const char* s: cstrs)
      if (s != nullptr && (ic ? casecmp (s, p, n) : strncmp (s, p, n)) == 0)
        return true;

    return false;
  }

  bool
  find_option_prefixes (initializer_list<const char*> ps,
                        const lookup& l,
                        bool ic)
  {
    return l && find_option_prefixes (ps, cast<strings> (l), ic);
  }

  bool
  find_option_prefixes (initializer_list<const char*> ps,
                        const strings& strs,
                        bool ic)
  {
    for (const string& s: strs)
      for (const char* p: ps)
        if ((ic
             ? casecmp (s, p, strlen (p))
             : s.compare (0, strlen (p), p)) == 0)
          return true;

    return false;
  }

  bool
  find_option_prefixes (initializer_list<const char*> ps,
                        const cstrings& cstrs,
                        bool ic)
  {
    for (const char* s: cstrs)
      if (s != nullptr)
        for (const char* p: ps)
          if ((ic
               ? casecmp (s, p, strlen (p))
               : strncmp (s, p, strlen (p))) == 0)
            return true;

    return false;
  }

  string
  apply_pattern (const char* s, const string* p)
  {
    if (p == nullptr)
      return s;

    size_t i (p->find ('*'));
    assert (i != string::npos);

    string r (*p, 0, i++);
    r.append (s);
    r.append (*p, i, p->size () - i);
    return r;
  }

  void
  init (const char* a0, uint16_t v)
  {
    // Build system driver process path.
    //
    argv0 = process::path_search (a0, true);

    // Diagnostics verbosity.
    //
    verb = v;

    // Initialize time conversion data that is used by localtime_r().
    //
    tzset ();

    // Figure out work and home directories.
    //
    try
    {
      work = dir_path::current_directory ();
    }
    catch (const system_error& e)
    {
      fail << "invalid current working directory: " << e;
    }

    home = dir_path::home_directory ();
  }
}
